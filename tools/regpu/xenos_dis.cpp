// xenos_dis [--hlsl] <shader.bin>...: disassemble captured Xenos microcode
// (vs_*.bin / ps_*.bin from REGPU_DUMP_SHADERS); --hash prints each file with a hash of its
// translated HLSL (no compile: shaders with the same hash share bytecode); --hlsl translates and
// compiles each one instead and reports the result; --write DIR saves each translation as
// DIR/<file name>.hlsl instead of compiling it (vertex shaders need REGPU_VERTEX_STAGE=1), for
// compiling them with another compiler (DXC); --pack DIR compiles each one with DXC (optimised) into
// DIR/<key>.dxil, the shader pack REGPU_DXIL=1 loads (regpu_shaders beside the game, or
// REGPU_SHADER_PACK). Run it over the shader cache's *.ucode files; dxcompiler.dll and dxil.dll
// must be beside this tool or on the path.
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <unordered_set>
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
    // A folder stands for the *.ucode files in it (a shader cache), in name order.
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
    {
        std::error_code ec;
        const bool outputDir = i > 1 && (!strcmp(argv[i - 1], "--pack") || !strcmp(argv[i - 1], "--write"));
        if (outputDir || !std::filesystem::is_directory(argv[i], ec))
        {
            args.push_back(argv[i]);
            continue;
        }
        std::vector<std::string> found;
        for (const auto& entry : std::filesystem::directory_iterator(argv[i], ec))
            if (entry.path().extension() == ".ucode")
                found.push_back(entry.path().string());
        std::sort(found.begin(), found.end());
        args.insert(args.end(), found.begin(), found.end());
    }
    for (size_t i = 0; i < args.size(); i++)
    {
        if (!strcmp(args[i].c_str(), "--pack") && i + 1 < args.size())
        {
            hlsl = true;
            packDir = args[++i];
            std::filesystem::create_directories(packDir);
            continue;
        }
        if (!strcmp(args[i].c_str(), "--write") && i + 1 < args.size())
        {
            hlsl = true;
            writeDir = args[++i];
            std::filesystem::create_directories(writeDir);
            continue;
        }
        if (!strcmp(args[i].c_str(), "--hlsl"))
        {
            hlsl = true;
            continue;
        }
        if (!strcmp(args[i].c_str(), "--hash"))
        {
            hlsl = hashOnly = true;
            continue;
        }
        if (!strcmp(args[i].c_str(), "--print"))
        {
            print = true;
            continue;
        }
        std::ifstream f(args[i].c_str(), std::ios::binary);
        std::vector<char> bytes{ std::istreambuf_iterator<char>(f), {} };
        const std::string name = args[i].c_str();
        const bool pixel = std::filesystem::path(name).filename().string().find("ps_") != std::string::npos;
        const auto shader = ucode::Parse(reinterpret_cast<const uint32_t*>(bytes.data()), uint32_t(bytes.size() / 4), pixel);
        if (!hlsl)
        {
            printf("=== %s\n%s\n", args[i].c_str(), ucode::Disassemble(shader).c_str());
            continue;
        }
        const TranslatedShader t = TranslateShader(shader);
        if (!t.ok)
        {
            printf("FAIL translate %s: %s\n", args[i].c_str(), t.error.c_str());
            failures++;
            continue;
        }
        if (hashOnly)
        {
            // The renderer's fast path for patched copies must give the same fetch layout.
            TranslatedShader layout;
            const bool dynamic = !pixel && TranslateVertexFetchLayout(shader, layout);
            if (dynamic && (layout.vertexFetchLayout != t.vertexFetchLayout || layout.vertexFetchConstants != t.vertexFetchConstants))
                printf("LAYOUT MISMATCH %s\n", args[i].c_str());
            if (!pixel && !dynamic)
                printf("BAKED FETCH %s\n", args[i].c_str()); // some fetch kept literal fields in the HLSL
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
            printf("%s %016llx canon %016llx\n", args[i].c_str(), (unsigned long long)h, (unsigned long long)canon);
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
            // A patched copy with the same translation. Files already in the pack are rebuilt: an
            // earlier pack may come from another DXC.
            static std::unordered_set<std::string> written;
            if (!written.insert(file).second)
                continue;
            ID3DBlob* code = nullptr;
            std::string errors;
            if (!CompileDxil(t.hlsl, pixel, true, &code, &errors))
            {
                printf("FAIL dxc %s:\n%s\n", args[i].c_str(), errors.c_str());
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
        const HRESULT hr = D3DCompile(t.hlsl.data(), t.hlsl.size(), args[i].c_str(), pixel ? nullptr : vertexStage, nullptr, "main", pixel ? "ps_5_1" : "vs_5_1",
            D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES, 0, &code, &errors);
        if (FAILED(hr))
        {
            printf("FAIL compile %s:\n%s\n", args[i].c_str(), errors ? static_cast<const char*>(errors->GetBufferPointer()) : "?");
            failures++;
        }
        else
            printf("ok   %s (%zu bytes DXBC)\n", args[i].c_str(), code->GetBufferSize());
    }
    if (!packDir.empty())
        printf("%d shaders packed into %s, %d failed\n", packed, packDir.string().c_str(), failures);
    return failures;
}
