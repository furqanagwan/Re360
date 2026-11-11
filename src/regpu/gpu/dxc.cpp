// DXC, loaded at run time like d3dcompiler_47.dll (see d3dcompiler.cpp): from beside the game executable
// first, so a game can ship the Windows SDK's dxcompiler.dll and dxil.dll with it.
#include "gpu/dxc.h"

#include <windows.h>
#include <d3dcompiler.h>
#include <dxcapi.h>
#include <wrl/client.h>

#include <cstring>
#include <vector>

#include "log.h"

using Microsoft::WRL::ComPtr;

namespace
{
std::filesystem::path ExecutableDir()
{
    wchar_t path[MAX_PATH];
    const DWORD n = GetModuleFileNameW(nullptr, path, MAX_PATH);
    return n && n < MAX_PATH ? std::filesystem::path(std::wstring(path, n)).parent_path() : std::filesystem::path();
}

DxcCreateInstanceProc CreateInstance()
{
    static const DxcCreateInstanceProc proc = [] {
        // dxil.dll first, by full path, from beside the executable or its D3D12 folder (where the
        // DXIL shader path ships it): dxcompiler.dll then signs with the copy already loaded, not
        // one the DLL search happens to find.
        const std::filesystem::path dir = ExecutableDir();
        for (const auto& candidate : { dir / L"dxil.dll", dir / L"D3D12" / L"dxil.dll" })
            if (LoadLibraryExW(candidate.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH))
                break;
        const std::filesystem::path local = dir / L"dxcompiler.dll";
        HMODULE m = LoadLibraryExW(local.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
        Log("dxc: %s", m ? "using dxcompiler.dll beside the executable" : "no dxcompiler.dll beside the executable, trying the system's");
        if (!m)
            m = LoadLibraryW(L"dxcompiler.dll");
        if (!m)
        {
            Log("dxc: dxcompiler.dll not found: shaders stay DXBC (fxc)");
            return DxcCreateInstanceProc(nullptr);
        }
        return reinterpret_cast<DxcCreateInstanceProc>(GetProcAddress(m, "DxcCreateInstance"));
    }();
    return proc;
}

// A DXIL container signed by dxil.dll carries its digest in bytes 4-19; DXC leaves them zero without it.
bool IsSigned(const void* data, size_t size)
{
    if (size < 20 || memcmp(data, "DXBC", 4) != 0)
        return false;
    const auto* digest = static_cast<const uint8_t*>(data) + 4;
    for (int i = 0; i < 16; i++)
        if (digest[i])
            return true;
    return false;
}
} // namespace

uint64_t ShaderSourceKey(const std::string& hlsl, bool pixel)
{
    uint64_t h = pixel ? 1 : 2;
    for (const char c : hlsl)
        h = (h ^ uint8_t(c)) * 0x100000001B3ull;
    return h;
}

bool DxcAvailable()
{
    return CreateInstance() != nullptr;
}

bool UseDxil()
{
    static const bool on = getenv("REGPU_DXIL") && atoi(getenv("REGPU_DXIL")) != 0 && DxcAvailable();
    return on;
}

bool CompileDxil(const std::string& hlsl, bool pixel, bool optimize, ID3DBlob** code, std::string* errors)
{
    *code = nullptr;
    const DxcCreateInstanceProc create = CreateInstance();
    if (!create)
    {
        *errors = "dxcompiler.dll not found";
        return false;
    }
    // One compiler per thread: IDxcCompiler3 instances are not shared between threads.
    thread_local ComPtr<IDxcCompiler3> compiler;
    if (!compiler && FAILED(create(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))))
    {
        *errors = "DxcCreateInstance(CLSID_DxcCompiler) failed";
        return false;
    }
    std::vector<LPCWSTR> args = { L"-E", L"main", L"-T", pixel ? L"ps_6_0" : L"vs_6_0", L"-HV", L"2018", optimize ? L"-O3" : L"-O1" };
    if (!pixel)
    {
        args.push_back(L"-D");
        args.push_back(L"REGPU_VERTEX_STAGE=1");
    }
    const DxcBuffer source{ hlsl.data(), hlsl.size(), DXC_CP_UTF8 };
    ComPtr<IDxcResult> result;
    if (FAILED(compiler->Compile(&source, args.data(), UINT32(args.size()), nullptr, IID_PPV_ARGS(&result))))
    {
        *errors = "IDxcCompiler3::Compile failed";
        return false;
    }
    HRESULT status = E_FAIL;
    result->GetStatus(&status);
    if (FAILED(status))
    {
        ComPtr<IDxcBlobUtf8> message;
        result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&message), nullptr);
        *errors = message && message->GetStringLength() ? message->GetStringPointer() : "compile failed";
        return false;
    }
    ComPtr<IDxcBlob> object;
    if (FAILED(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&object), nullptr)) || !object)
    {
        *errors = "no DXIL object";
        return false;
    }
    if (!IsSigned(object->GetBufferPointer(), object->GetBufferSize()))
    {
        *errors = "the DXIL is unsigned (dxil.dll not found beside dxcompiler.dll)";
        return false;
    }
    if (FAILED(D3DCreateBlob(object->GetBufferSize(), code)))
    {
        *errors = "D3DCreateBlob failed";
        return false;
    }
    memcpy((*code)->GetBufferPointer(), object->GetBufferPointer(), object->GetBufferSize());
    return true;
}

const std::filesystem::path& ShaderPackDir()
{
    static const std::filesystem::path dir = getenv("REGPU_SHADER_PACK") ? std::filesystem::path(getenv("REGPU_SHADER_PACK"))
                                                                          : ExecutableDir() / L"regpu_shaders";
    return dir;
}

std::filesystem::path ShaderPackPath(uint64_t key)
{
    char name[40];
    snprintf(name, sizeof(name), "%016llx.dxil", (unsigned long long)key);
    return ShaderPackDir() / name;
}
