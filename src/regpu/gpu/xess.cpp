// Intel XeSS super resolution (XeSS SDK 3.x, Intel Simplified Software License: libxess.dll is
// redistributed unmodified, with its licence beside it). Fed the same frame as DLSS and FSR 3: colour
// at render size, depth and motion (top left of output-sized textures), the output in UAV state.
#include "gpu/xess.h"

#include <xess/xess.h>
#include <xess/xess_d3d12.h>

#include <windows.h>

#include <vector>

#include "log.h"

namespace
{
struct Retired
{
    xess_context_handle_t context;
    int evaluations; // destroyed once this many later evaluations have been recorded (the GPU is done)
};

struct Xess
{
    bool tried = false, available = false, failed = false;
    xess_context_handle_t context = nullptr; // the one in use, initialised for `setup`
    DlssSetup setup{};
    bool initialised = false;
    std::vector<Retired> retired;
} g_xess;

const char* ResultName(xess_result_t r)
{
    switch (r)
    {
    case XESS_RESULT_SUCCESS: return "success";
    case XESS_RESULT_ERROR_UNSUPPORTED_DEVICE: return "unsupported device";
    case XESS_RESULT_ERROR_UNSUPPORTED_DRIVER: return "unsupported driver";
    case XESS_RESULT_ERROR_UNINITIALIZED: return "uninitialised";
    case XESS_RESULT_ERROR_INVALID_ARGUMENT: return "invalid argument";
    case XESS_RESULT_ERROR_DEVICE_OUT_OF_MEMORY: return "out of device memory";
    case XESS_RESULT_ERROR_DEVICE: return "device error";
    case XESS_RESULT_ERROR_NOT_IMPLEMENTED: return "not implemented";
    case XESS_RESULT_ERROR_INVALID_CONTEXT: return "invalid context";
    case XESS_RESULT_ERROR_OPERATION_IN_PROGRESS: return "operation in progress";
    case XESS_RESULT_ERROR_UNSUPPORTED: return "unsupported";
    case XESS_RESULT_ERROR_CANT_LOAD_LIBRARY: return "can't load library";
    default: return r < 0 ? "error" : "warning";
    }
}

// ReGPU's quality steps (settings.h: 0 native .. 4 ultra performance) are XeSS's scales: 1.0, 1.5, 1.7,
// 2.0 and 3.0.
xess_quality_settings_t Quality(uint32_t quality)
{
    static const xess_quality_settings_t kQualities[] = { XESS_QUALITY_SETTING_AA, XESS_QUALITY_SETTING_QUALITY, XESS_QUALITY_SETTING_BALANCED,
        XESS_QUALITY_SETTING_PERFORMANCE, XESS_QUALITY_SETTING_ULTRA_PERFORMANCE };
    return kQualities[quality > 4 ? 1 : quality];
}
} // namespace

bool XessInit(ID3D12Device* device)
{
    if (g_xess.tried)
        return g_xess.available;
    g_xess.tried = true;
    // libxess.dll is delay-loaded: loading it here first means a missing DLL turns XeSS off instead of
    // faulting at its first call (kept loaded; the delay-load then finds it).
    if (!LoadLibraryW(L"libxess.dll"))
    {
        Log("xess: libxess.dll is not beside the game: XeSS is unavailable");
        return false;
    }
    xess_version_t version{};
    if (xessGetVersion(&version) == XESS_RESULT_SUCCESS)
        Log("xess: XeSS %u.%u.%u", unsigned(version.major), unsigned(version.minor), unsigned(version.patch));
    xess_context_handle_t context = nullptr;
    const xess_result_t r = xessD3D12CreateContext(device, &context);
    if (r != XESS_RESULT_SUCCESS || !context)
    {
        Log("xess: not available on this GPU (%s, %d)", ResultName(r), int(r));
        return false;
    }
    if (xessIsOptimalDriver(context) == XESS_RESULT_WARNING_OLD_DRIVER)
        Log("xess: the graphics driver is older than XeSS recommends");
    g_xess.context = context;
    g_xess.available = true;
    Log("xess: ready");
    return true;
}

bool XessEvaluate(ID3D12Device* device, ID3D12GraphicsCommandList* list, const DlssSetup& setup, const DlssFrame& frame)
{
    if (g_xess.failed || !XessInit(device))
        return false;
    for (size_t i = 0; i < g_xess.retired.size();)
    {
        if (--g_xess.retired[i].evaluations <= 0)
        {
            xessDestroyContext(g_xess.retired[i].context);
            g_xess.retired.erase(g_xess.retired.begin() + i);
        }
        else
            ++i;
    }
    // A new size, quality or HDR mode: a fresh context (initialising one in use would need the GPU idle);
    // the old one is destroyed once the frames recorded with it have run.
    if (g_xess.initialised && !(setup == g_xess.setup))
    {
        g_xess.retired.push_back({ g_xess.context, 8 });
        g_xess.context = nullptr;
        g_xess.initialised = false;
        const xess_result_t r = xessD3D12CreateContext(device, &g_xess.context);
        if (r != XESS_RESULT_SUCCESS)
        {
            Log("xess: new context failed (%s)", ResultName(r));
            g_xess.failed = true;
            return false;
        }
    }
    if (!g_xess.initialised)
    {
        xess_d3d12_init_params_t init{};
        init.outputResolution = { setup.outputWidth, setup.outputHeight };
        init.qualitySetting = Quality(setup.quality);
        init.initFlags = XESS_INIT_FLAG_ENABLE_AUTOEXPOSURE | (setup.depthInverted ? XESS_INIT_FLAG_INVERTED_DEPTH : 0) |
            (setup.hdr ? 0 : XESS_INIT_FLAG_LDR_INPUT_COLOR);
        const xess_result_t r = xessD3D12Init(g_xess.context, &init);
        if (r != XESS_RESULT_SUCCESS)
        {
            Log("xess: init failed (%s, %d) for %ux%u -> %ux%u", ResultName(r), int(r), setup.renderWidth, setup.renderHeight, setup.outputWidth,
                setup.outputHeight);
            g_xess.failed = true;
            return false;
        }
        g_xess.setup = setup;
        g_xess.initialised = true;
        Log("xess: %ux%u -> %ux%u, quality %u%s", setup.renderWidth, setup.renderHeight, setup.outputWidth, setup.outputHeight, setup.quality,
            setup.hdr ? ", HDR" : "");
    }
    // Motion as DLSS takes it: values times the scale give render pixels, pointing to last frame.
    xessSetVelocityScale(g_xess.context, frame.motionScaleX, frame.motionScaleY);
    xess_d3d12_execute_params_t exec{};
    exec.pColorTexture = frame.color;
    exec.pVelocityTexture = frame.motion;
    exec.pDepthTexture = frame.depth;
    exec.pOutputTexture = frame.output;
    exec.jitterOffsetX = frame.jitterX;
    exec.jitterOffsetY = frame.jitterY;
    exec.exposureScale = 1.0f;
    exec.resetHistory = frame.reset ? 1 : 0;
    exec.inputWidth = setup.renderWidth;
    exec.inputHeight = setup.renderHeight;
    const xess_result_t r = xessD3D12Execute(g_xess.context, list, &exec);
    if (r != XESS_RESULT_SUCCESS)
    {
        static int logged = 0;
        if (logged++ < 3)
            Log("xess: execute failed (%s, %d)", ResultName(r), int(r));
        return r > 0; // (warnings still recorded the upscale)
    }
    return true;
}
