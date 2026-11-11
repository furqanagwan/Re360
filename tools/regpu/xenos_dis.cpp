// xenos_dis [--hlsl] <shader.bin>...: disassemble captured Xenos microcode
// (vs_*.bin / ps_*.bin from REGPU_DUMP_SHADERS); --hash prints each file with a hash of its
// translated HLSL (no compile: shaders with the same hash share bytecode); --hlsl translates and
// compiles each one instead and reports the result; --write DIR saves each translation as
// DIR/<file name>.hlsl instead of compiling it (vertex shaders need REGPU_VERTEX_STAGE=1), for
// compiling them with another compiler (DXC); --pack DIR compiles each one with DXC (optimised) into
// DIR/<key>.dxil, the shader pack REGPU_DXIL=1 loads (regpu_shaders beside the game, or
// REGPU_SHADER_PACK). Run it over the shader cache's *.ucode files; dxcompiler.dll and dxil.dll
// must be beside this tool or on the path.
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include <d3dcompiler.h>
#include <windows.h>

#include "gpu/dxc.h"
#include "gpu/hlsl.h"
#include "gpu/ucode.h"

int main(int argc, char** argv)
{
    bool hlsl = false, print = false, hashOnly = false;
    std::filesystem::path writeDir, packDir;
    int failures = 0, packed = 0;
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "--pack") && i + 1 < argc)
        {
            hlsl = true;
            packDir = argv[++i];
            std::filesystem::create_directories(packDir);
            continue;
        }
        if (!strcmp(argv[i], "--write") && i + 1 < argc)
        {
            hlsl = true;
            writeDir = argv[++i];
            std::filesystem::create_directories(writeDir);
            continue;
        }
        if (!strcmp(argv[i], "--hlsl"))
        {
            hlsl = true;
            continue;
        }
        if (!strcmp(argv[i], "--hash"))
        {
            hlsl = hashOnly = true;
            continue;
        }
        if (!strcmp(argv[i], "--print"))
        {
            print = true;
            continue;
        }
        std::ifstream f(argv[i], std::ios::binary);
        std::vector<char> bytes{ std::istreambuf_iterator<char>(f), {} };
        const std::string name = argv[i];
        const bool pixel = name.find("ps_") != std::string::npos;
        const auto shader = ucode::Parse(reinterpret_cast<const uint32_t*>(bytes.data()), uint32_t(bytes.size() / 4), pixel);
        if (!hlsl)
        {
            printf("=== %s\n%s\n", argv[i], ucode::Disassemble(shader).c_str());
            continue;
        }
        const TranslatedShader t = TranslateShader(shader);
        if (!t.ok)
        {
            printf("FAIL translate %s: %s\n", argv[i], t.error.c_str());
            failures++;
            continue;
        }
        if (hashOnly)
        {
            // The renderer's fast path for patched copies must give the same fetch layout.
            TranslatedShader layout;
            const bool dynamic = !pixel && TranslateVertexFetchLayout(shader, layout);
            if (dynamic && (layout.vertexFetchLayout != t.vertexFetchLayout || layout.vertexFetchConstants != t.vertexFetchConstants))
                printf("LAYOUT MISMATCH %s\n", argv[i]);
            if (!pixel && !dynamic)
                printf("BAKED FETCH %s\n", argv[i]); // some fetch kept literal fields in the HLSL
            uint64_t h = 0xCBF29CE484222325ull;
            for (char c : t.hlsl)
                h = (h ^ uint8_t(c)) * 0x100000001B3ull;
            // The renderer's canonical key (CanonicalKey): microcode with the patched vertex
            // fetch fields masked. Shaders with equal keys must have equal HLSL hashes.
            std::vector<uint32_t> w = shader.words;
            for (const ucode::Cf& cf : shader.cf)
            {
                if (cf.IsExec())
                    for (uint32_t k = 0; k < cf.count(); k++)
                    {
                        const uint32_t index = cf.address() + k;
                        if (!((cf.sequence() >> (2 * k)) & 1) || (index + 1) * 3 > w.size() || (w[index * 3] & 0x1F) != 0)
                            continue;
                        w[index * 3] &= ~0x1FF00000u;
                        w[index * 3 + 1] &= 0x80000000u;
                        w[index * 3 + 2] &= 0x80000000u;
                    }
                if (cf.IsEnd())
                    break;
            }
            uint64_t canon = 0xCBF29CE484222325ull;
            for (uint32_t x : w)
                for (int b = 0; b < 4; b++)
                    canon = (canon ^ ((x >> (8 * b)) & 0xFF)) * 0x100000001B3ull;
            printf("%s %016llx canon %016llx\n", argv[i], (unsigned long long)h, (unsigned long long)canon);
            if (print)
                printf("%s\n", t.hlsl.c_str());
            continue;
        }
        if (print)
            printf("%s\n", t.hlsl.c_str());
        if (!packDir.empty())
        {
            char file[40];
            snprintf(file, sizeof(file), "%016llx.dxil", (unsigned long long)ShaderSourceKey(t.hlsl, pixel));
            const auto path = packDir / file;
            if (std::filesystem::exists(path))
                continue; // a patched copy with the same translation
            ID3DBlob* code = nullptr;
            std::string errors;
            if (!CompileDxil(t.hlsl, pixel, true, &code, &errors))
            {
                printf("FAIL dxc %s:\n%s\n", argv[i], errors.c_str());
                failures++;
                continue;
            }
            std::ofstream(path, std::ios::binary).write(static_cast<const char*>(code->GetBufferPointer()), std::streamsize(code->GetBufferSize()));
            code->Release();
            packed++;
            continue;
        }
        if (!writeDir.empty())
        {
            std::ofstream(writeDir / (std::filesystem::path(name).filename().string() + ".hlsl"), std::ios::binary) << t.hlsl;
            continue;
        }
        ID3DBlob* code = nullptr;
        ID3DBlob* errors = nullptr;
        const D3D_SHADER_MACRO vertexStage[] = { { "REGPU_VERTEX_STAGE", "1" }, { nullptr, nullptr } };
        const HRESULT hr = D3DCompile(t.hlsl.data(), t.hlsl.size(), argv[i], pixel ? nullptr : vertexStage, nullptr, "main", pixel ? "ps_5_1" : "vs_5_1",
            D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES, 0, &code, &errors);
        if (FAILED(hr))
        {
            printf("FAIL compile %s:\n%s\n", argv[i], errors ? static_cast<const char*>(errors->GetBufferPointer()) : "?");
            failures++;
        }
        else
            printf("ok   %s (%zu bytes DXBC)\n", argv[i], code->GetBufferSize());
    }
    if (!packDir.empty())
        printf("%d shaders packed into %s, %d failed\n", packed, packDir.string().c_str(), failures);
    return failures;
}
