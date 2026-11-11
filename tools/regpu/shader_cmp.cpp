// regpu_shadercmp <ucode files or folders>...: draws each pixel shader twice with the same inputs, as
// fxc builds it at run time (ps_5_1, unoptimised: the default path) and as the DXIL shader pack holds
// it (DXC ps_6_0 -O3, HLSL 2018: REGPU_DXIL=1), and compares the two pictures. Inputs: fixed
// pseudo-random constants (pc, pb), short loops, 2D noise textures in every fetch slot, and 16
// interpolators that vary across a 256x256 target (RGBA32F, cleared to -7 so a discard shows).
// A channel differs when |a - b| > 1e-3 * max(1, |a|), or when one side is NaN and the other not.
// --dxc ARG adds a DXC argument (repeatable). --control compares fxc unoptimised with fxc optimised instead (level 3): differences there are
// rounding between two builds of one compiler, the baseline for judging DXC's.
// dxcompiler.dll and dxil.dll must be beside this tool or on the path. Exit code: shaders that differ.
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include <d3d12.h>
#include <d3dcompiler.h>
#include <windows.h>
#include <wrl/client.h>

#include "gpu/dxc.h"
#include "gpu/hlsl.h"
#include "gpu/ucode.h"

using Microsoft::WRL::ComPtr;

namespace
{
constexpr UINT kSize = 256, kTextures = 32, kTextureSize = 64, kRawBytes = 1 << 16;

const char kVertexShader[] = R"(
VSOut main(uint id : SV_VertexID)
{
    VSOut r;
    const float2 uv = float2((id << 1) & 2, id & 2);
    r.pos = float4(uv * float2(2, -2) + float2(-1, 1), 0.5, 1);
    [unroll] for (int k = 0; k < 16; k++)
        r.o[k] = float4(uv.x, uv.y, uv.x * uv.y, 1 - uv.x) * (0.5 + 0.25 * k) - 0.1 * k;
    r.pcoord = uv;
    return r;
}
)";

void Check(HRESULT hr, const char* what)
{
    if (FAILED(hr))
    {
        fprintf(stderr, "%s failed: %08lx\n", what, static_cast<unsigned long>(hr));
        exit(100);
    }
}

uint32_t g_seed = 0x2545F491u;
float Random(float lo, float hi)
{
    g_seed = g_seed * 1664525u + 1013904223u;
    return lo + (hi - lo) * float(g_seed >> 8) / float(1u << 24);
}

uint16_t Half(float f)
{
    uint32_t x;
    memcpy(&x, &f, 4);
    const uint32_t sign = (x >> 16) & 0x8000u;
    const int exponent = int((x >> 23) & 0xFF) - 127 + 15;
    if (exponent <= 0)
        return uint16_t(sign);
    if (exponent >= 31)
        return uint16_t(sign | 0x7C00u);
    return uint16_t(sign | uint32_t(exponent) << 10 | ((x >> 13) & 0x3FFu));
}

struct Gpu
{
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12CommandQueue> queue;
    ComPtr<ID3D12CommandAllocator> allocator;
    ComPtr<ID3D12GraphicsCommandList> list;
    ComPtr<ID3D12Fence> fence;
    uint64_t fenceValue = 0;
    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12DescriptorHeap> srvHeap, rtvHeap;
    ComPtr<ID3D12Resource> vsConstants, psConstants, raw, target, readback, textureUpload;
    std::vector<ComPtr<ID3D12Resource>> textures;

    ComPtr<ID3D12Resource> Buffer(UINT64 size, D3D12_HEAP_TYPE heap, D3D12_RESOURCE_STATES state)
    {
        D3D12_HEAP_PROPERTIES hp{ heap };
        D3D12_RESOURCE_DESC rd{};
        rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        rd.Width = size;
        rd.Height = rd.DepthOrArraySize = rd.MipLevels = 1;
        rd.SampleDesc.Count = 1;
        rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        ComPtr<ID3D12Resource> r;
        Check(device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, state, nullptr, IID_PPV_ARGS(&r)), "CreateCommittedResource(buffer)");
        return r;
    }

    void Flush()
    {
        Check(list->Close(), "Close");
        ID3D12CommandList* lists[] = { list.Get() };
        queue->ExecuteCommandLists(1, lists);
        Check(queue->Signal(fence.Get(), ++fenceValue), "Signal");
        Check(fence->SetEventOnCompletion(fenceValue, nullptr), "SetEventOnCompletion");
        Check(allocator->Reset(), "allocator Reset");
        Check(list->Reset(allocator.Get(), nullptr), "list Reset");
    }

    void Init()
    {
        Check(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)), "D3D12CreateDevice");
        D3D12_COMMAND_QUEUE_DESC qd{};
        Check(device->CreateCommandQueue(&qd, IID_PPV_ARGS(&queue)), "CreateCommandQueue");
        Check(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)), "CreateCommandAllocator");
        Check(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr, IID_PPV_ARGS(&list)), "CreateCommandList");
        Check(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "CreateFence");

        // The renderer's root signature (renderer.cpp) without REGPU_LEAN_FETCH: b0, b1, t0-t1 raw
        // buffers, bindless textures in spaces 1-3 over the same descriptors, eight static samplers.
        D3D12_DESCRIPTOR_RANGE rawRange{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0, 0 };
        D3D12_DESCRIPTOR_RANGE bindless[3]{};
        for (UINT i = 0; i < 3; i++)
            bindless[i] = { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, kTextures, 0, 1 + i, 0 };
        D3D12_ROOT_PARAMETER params[4]{};
        params[0].ParameterType = params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        params[1].Descriptor.ShaderRegister = 1;
        params[2].ParameterType = params[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        params[2].DescriptorTable = { 1, &rawRange };
        params[3].DescriptorTable = { 3, bindless };
        D3D12_STATIC_SAMPLER_DESC samplers[8]{};
        const D3D12_FILTER filters[4] = { D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_FILTER_MIN_MAG_MIP_POINT,
            D3D12_FILTER_MIN_MAG_MIP_POINT };
        const D3D12_TEXTURE_ADDRESS_MODE modes[4] = { D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_WRAP };
        for (UINT i = 0; i < 8; i++)
        {
            samplers[i].Filter = i < 4 ? filters[i] : D3D12_FILTER_ANISOTROPIC;
            samplers[i].MaxAnisotropy = i < 4 ? 0 : 2u << (i - 4);
            samplers[i].AddressU = samplers[i].AddressV = samplers[i].AddressW = i < 4 ? modes[i] : D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            samplers[i].MaxLOD = D3D12_FLOAT32_MAX;
            samplers[i].ShaderRegister = i;
        }
        D3D12_ROOT_SIGNATURE_DESC rs{ 4, params, 8, samplers, D3D12_ROOT_SIGNATURE_FLAG_NONE };
        ComPtr<ID3DBlob> blob, error;
        Check(D3D12SerializeRootSignature(&rs, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error), "D3D12SerializeRootSignature");
        Check(device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)), "CreateRootSignature");

        D3D12_DESCRIPTOR_HEAP_DESC hd{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2 + kTextures, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE };
        Check(device->CreateDescriptorHeap(&hd, IID_PPV_ARGS(&srvHeap)), "CreateDescriptorHeap(srv)");
        D3D12_DESCRIPTOR_HEAP_DESC rh{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1 };
        Check(device->CreateDescriptorHeap(&rh, IID_PPV_ARGS(&rtvHeap)), "CreateDescriptorHeap(rtv)");
        const UINT srvStride = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        auto srv = [&](UINT i) {
            D3D12_CPU_DESCRIPTOR_HANDLE h = srvHeap->GetCPUDescriptorHandleForHeapStart();
            h.ptr += SIZE_T(i) * srvStride;
            return h;
        };

        // Constants: VS zero; PS pseudo-random floats and bools, four-iteration loops, alpha test always
        // passes, no generated parameter, every slot a 64x64 2D texture at its own descriptor.
        vsConstants = Buffer(1 << 16, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
        psConstants = Buffer(1 << 16, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
        void* p = nullptr;
        Check(vsConstants->Map(0, nullptr, &p), "Map(vs)");
        memset(p, 0, 1 << 16);
        Check(psConstants->Map(0, nullptr, &p), "Map(ps)");
        auto* bytes = static_cast<uint8_t*>(p);
        memset(bytes, 0, 1 << 16);
        auto* pc = reinterpret_cast<float*>(bytes);
        for (UINT i = 0; i < 256 * 4; i++)
            pc[i] = Random(-2.0f, 2.0f);
        auto* pb = reinterpret_cast<uint32_t*>(bytes + 4096);
        for (UINT i = 0; i < 8; i++)
            pb[i] = uint32_t(Random(0.0f, 4294967040.0f));
        auto* pl = reinterpret_cast<uint32_t*>(bytes + 4128);
        for (UINT i = 0; i < 32; i++)
            pl[i] = 4u | 1u << 16;
        auto* pinfo = reinterpret_cast<uint32_t*>(bytes + 4256);
        pinfo[0] = 7;
        pinfo[1] = 0xFF;
        auto* pinfo2 = reinterpret_cast<float*>(bytes + 4272);
        pinfo2[0] = 0.5f;
        pinfo2[1] = pinfo2[2] = 1.0f;
        auto* tinfo = reinterpret_cast<uint32_t*>(bytes + 4288);
        auto* tbias = reinterpret_cast<uint32_t*>(bytes + 4800);
        for (UINT i = 0; i < kTextures; i++)
        {
            tinfo[i * 4 + 1] = tinfo[i * 4 + 2] = kTextureSize;
            tinfo[i * 4 + 3] = 1;
            tbias[i * 4 + 1] = i;
        }

        raw = Buffer(kRawBytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
        D3D12_SHADER_RESOURCE_VIEW_DESC rv{};
        rv.Format = DXGI_FORMAT_R32_TYPELESS;
        rv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        rv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        rv.Buffer.NumElements = kRawBytes / 4;
        rv.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
        device->CreateShaderResourceView(raw.Get(), &rv, srv(0));
        device->CreateShaderResourceView(raw.Get(), &rv, srv(1));

        // Noise textures, RGBA16F in [-0.5, 1.5] (past both ends of the unit range).
        const UINT rowPitch = kTextureSize * 8, slice = rowPitch * kTextureSize;
        textureUpload = Buffer(UINT64(slice) * kTextures, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
        Check(textureUpload->Map(0, nullptr, &p), "Map(textures)");
        auto* texels = static_cast<uint16_t*>(p);
        for (UINT i = 0; i < slice / 2 * kTextures; i++)
            texels[i] = Half(Random(-0.5f, 1.5f));
        for (UINT t = 0; t < kTextures; t++)
        {
            D3D12_HEAP_PROPERTIES hp{ D3D12_HEAP_TYPE_DEFAULT };
            D3D12_RESOURCE_DESC td{};
            td.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            td.Width = td.Height = kTextureSize;
            td.DepthOrArraySize = td.MipLevels = 1;
            td.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            td.SampleDesc.Count = 1;
            ComPtr<ID3D12Resource> tex;
            Check(device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &td, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&tex)),
                "CreateCommittedResource(texture)");
            D3D12_TEXTURE_COPY_LOCATION dst{ tex.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX };
            D3D12_TEXTURE_COPY_LOCATION src{ textureUpload.Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT };
            src.PlacedFootprint = { UINT64(slice) * t, { td.Format, kTextureSize, kTextureSize, 1, rowPitch } };
            list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
            D3D12_RESOURCE_BARRIER b{};
            b.Transition = { tex.Get(), 0, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE };
            list->ResourceBarrier(1, &b);
            device->CreateShaderResourceView(tex.Get(), nullptr, srv(2 + t));
            textures.push_back(tex);
        }
        Flush();

        D3D12_HEAP_PROPERTIES hp{ D3D12_HEAP_TYPE_DEFAULT };
        D3D12_RESOURCE_DESC td{};
        td.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        td.Width = td.Height = kSize;
        td.DepthOrArraySize = td.MipLevels = 1;
        td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        td.SampleDesc.Count = 1;
        td.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        Check(device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &td, D3D12_RESOURCE_STATE_RENDER_TARGET, nullptr, IID_PPV_ARGS(&target)),
            "CreateCommittedResource(target)");
        device->CreateRenderTargetView(target.Get(), nullptr, rtvHeap->GetCPUDescriptorHandleForHeapStart());
        readback = Buffer(UINT64(kSize) * kSize * 16, D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST);
    }

    // Draws a full-screen triangle with vs and ps; false when the pipeline can't be built.
    bool Draw(ID3DBlob* vs, ID3DBlob* ps, std::vector<float>& out)
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pd{};
        pd.pRootSignature = rootSignature.Get();
        pd.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
        pd.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
        pd.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        pd.SampleMask = UINT_MAX;
        pd.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        pd.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        pd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pd.NumRenderTargets = 1;
        pd.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
        pd.SampleDesc.Count = 1;
        ComPtr<ID3D12PipelineState> pso;
        if (FAILED(device->CreateGraphicsPipelineState(&pd, IID_PPV_ARGS(&pso))))
            return false;
        const float clear[4] = { -7, -7, -7, -7 };
        const D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtvHeap->GetCPUDescriptorHandleForHeapStart();
        list->ClearRenderTargetView(rtv, clear, 0, nullptr);
        list->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
        const D3D12_VIEWPORT vp{ 0, 0, float(kSize), float(kSize), 0, 1 };
        const D3D12_RECT sc{ 0, 0, LONG(kSize), LONG(kSize) };
        list->RSSetViewports(1, &vp);
        list->RSSetScissorRects(1, &sc);
        list->SetGraphicsRootSignature(rootSignature.Get());
        ID3D12DescriptorHeap* heaps[] = { srvHeap.Get() };
        list->SetDescriptorHeaps(1, heaps);
        list->SetGraphicsRootConstantBufferView(0, vsConstants->GetGPUVirtualAddress());
        list->SetGraphicsRootConstantBufferView(1, psConstants->GetGPUVirtualAddress());
        D3D12_GPU_DESCRIPTOR_HANDLE table = srvHeap->GetGPUDescriptorHandleForHeapStart();
        list->SetGraphicsRootDescriptorTable(2, table);
        table.ptr += 2ull * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        list->SetGraphicsRootDescriptorTable(3, table);
        list->SetPipelineState(pso.Get());
        list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        list->DrawInstanced(3, 1, 0, 0);
        D3D12_RESOURCE_BARRIER b{};
        b.Transition = { target.Get(), 0, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE };
        list->ResourceBarrier(1, &b);
        D3D12_TEXTURE_COPY_LOCATION dst{ readback.Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT };
        dst.PlacedFootprint = { 0, { DXGI_FORMAT_R32G32B32A32_FLOAT, kSize, kSize, 1, kSize * 16 } };
        D3D12_TEXTURE_COPY_LOCATION src{ target.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX };
        list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
        std::swap(b.Transition.StateBefore, b.Transition.StateAfter);
        list->ResourceBarrier(1, &b);
        Flush();
        void* p = nullptr;
        Check(readback->Map(0, nullptr, &p), "Map(readback)");
        out.assign(static_cast<float*>(p), static_cast<float*>(p) + kSize * kSize * 4);
        readback->Unmap(0, nullptr);
        return true;
    }
};

ComPtr<ID3DBlob> Fxc(const std::string& hlsl, bool pixel, std::string& errors, bool optimize = false)
{
    const D3D_SHADER_MACRO vertexStage[] = { { "REGPU_VERTEX_STAGE", "1" }, { nullptr, nullptr } };
    ComPtr<ID3DBlob> code, messages;
    if (FAILED(D3DCompile(hlsl.data(), hlsl.size(), nullptr, pixel ? nullptr : vertexStage, nullptr, "main", pixel ? "ps_5_1" : "vs_5_1",
            (optimize ? D3DCOMPILE_OPTIMIZATION_LEVEL3 : D3DCOMPILE_SKIP_OPTIMIZATION) | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES, 0, &code,
            &messages)))
    {
        errors = messages ? static_cast<const char*>(messages->GetBufferPointer()) : "?";
        return nullptr;
    }
    return code;
}

std::vector<std::wstring> g_dxcArgs;

ComPtr<ID3DBlob> Dxil(const std::string& hlsl, bool pixel, std::string& errors)
{
    ComPtr<ID3DBlob> code;
    return CompileDxil(hlsl, pixel, true, &code, &errors, g_dxcArgs) ? code : nullptr;
}
} // namespace

int main(int argc, char** argv)
{
    std::vector<std::filesystem::path> files;
    bool control = false;
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "--dxc") && i + 1 < argc)
        {
            const std::string a = argv[++i];
            g_dxcArgs.push_back(std::wstring(a.begin(), a.end()));
            continue;
        }
        if (!strcmp(argv[i], "--control"))
        {
            control = true;
            continue;
        }
        std::error_code ec;
        if (std::filesystem::is_directory(argv[i], ec))
        {
            for (const auto& entry : std::filesystem::directory_iterator(argv[i], ec))
                if (entry.path().extension() == ".ucode" && entry.path().filename().string().rfind("ps_", 0) == 0)
                    files.push_back(entry.path());
        }
        else
            files.push_back(argv[i]);
    }
    std::sort(files.begin(), files.end());
    if (files.empty())
    {
        fprintf(stderr, "usage: regpu_shadercmp <ps_*.ucode files or shader cache folders>...\n");
        return 100;
    }

    Gpu gpu;
    gpu.Init();
    const std::string vsSource = std::string(HlslPreamble()) + kVertexShader;
    std::string errors;
    const ComPtr<ID3DBlob> vsFxc = Fxc(vsSource, false, errors), vsDxil = control ? Fxc(vsSource, false, errors, true) : Dxil(vsSource, false, errors);
    if (!vsFxc || !vsDxil)
    {
        fprintf(stderr, "the test vertex shader did not compile:\n%s\n", errors.c_str());
        return 100;
    }

    int same = 0, close = 0, differ = 0, skipped = 0;
    std::vector<uint64_t> seen;
    std::vector<float> a, b;
    for (const auto& file : files)
    {
        std::ifstream f(file, std::ios::binary);
        std::vector<char> bytes{ std::istreambuf_iterator<char>(f), {} };
        const auto shader = ucode::Parse(reinterpret_cast<const uint32_t*>(bytes.data()), uint32_t(bytes.size() / 4), true);
        const TranslatedShader t = TranslateShader(shader);
        const std::string name = file.filename().string();
        if (!t.ok)
        {
            printf("skip %s: not translated (%s)\n", name.c_str(), t.error.c_str());
            skipped++;
            continue;
        }
        const uint64_t key = ShaderSourceKey(t.hlsl, true);
        if (std::find(seen.begin(), seen.end(), key) != seen.end())
            continue; // a patched copy with the same translation
        seen.push_back(key);
        const ComPtr<ID3DBlob> psFxc = Fxc(t.hlsl, true, errors);
        const ComPtr<ID3DBlob> psDxil = !psFxc ? nullptr : control ? Fxc(t.hlsl, true, errors, true) : Dxil(t.hlsl, true, errors);
        if (!psFxc || !psDxil || !gpu.Draw(vsFxc.Get(), psFxc.Get(), a) || !gpu.Draw(vsDxil.Get(), psDxil.Get(), b))
        {
            printf("skip %s: did not compile or build (%s)\n", name.c_str(), errors.c_str());
            skipped++;
            continue;
        }
        size_t exact = 0, bad = 0, nanMismatch = 0, worstAt = 0;
        float worst = 0;
        for (size_t i = 0; i < a.size(); i++)
        {
            const bool na = std::isnan(a[i]), nb = std::isnan(b[i]);
            if (na || nb)
            {
                exact += na && nb;
                nanMismatch += na != nb;
                continue;
            }
            const float d = std::fabs(a[i] - b[i]);
            if (d == 0)
            {
                exact++;
                continue;
            }
            if (d > 1e-3f * (std::max)(1.0f, std::fabs(a[i])))
                bad++;
            if (d > worst)
            {
                worst = d;
                worstAt = i;
            }
        }
        if (!bad && !nanMismatch)
        {
            exact == a.size() ? same++ : close++;
            continue;
        }
        differ++;
        const size_t px = worstAt / 4;
        printf("DIFF %s (%016llx): %zu of %zu channels differ, %zu NaN on one side only; worst %g at (%zu,%zu).%c: fxc %g dxil %g\n", name.c_str(),
            (unsigned long long)key, bad, a.size(), nanMismatch, worst, px % kSize, px / kSize, "rgba"[worstAt % 4], a[worstAt], b[worstAt]);
    }
    printf("%zu pixel shaders: %d identical, %d within tolerance, %d differ, %d skipped\n", seen.size(), same, close, differ, skipped);
    return differ;
}
