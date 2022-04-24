#include <d3d9on12.h>

class D3DFE_PROCESSVERTICES;
class IDirect3DShaderValidator9;

using PSGPERRORID = UINT;

HMODULE GetD3D9Module() {
    static HMODULE hModule = []() -> HMODULE {
        constexpr UINT MaxPath = 4096;

        wchar_t dllPath[MaxPath];

        GetSystemDirectoryW(dllPath, MaxPath);
        wcscat_s(dllPath, L"\\d3d9.dll");

        return LoadLibraryW(dllPath);
    }();

    return hModule;
}

template <typename ProcType, typename... Args>
auto RunD3D9Proc(const char *procName, Args... args) {
    static auto proc = reinterpret_cast<ProcType *>(
        GetProcAddress(GetD3D9Module(), procName));

    return proc(args...);
}

extern "C" {
    IDirect3D9 *WINAPI Direct3DCreate9On12(UINT SDKVersion, D3D9ON12_ARGS *pOverrideList, UINT NumOverrideEntries) {
        return RunD3D9Proc<decltype(Direct3DCreate9On12)>("Direct3DCreate9On12", SDKVersion, pOverrideList, NumOverrideEntries);
    }

    HRESULT WINAPI Direct3DCreate9On12Ex(UINT SDKVersion, D3D9ON12_ARGS *pOverrideList, UINT NumOverrideEntries, IDirect3D9Ex **ppDirect3D9Ex) {
        return RunD3D9Proc<decltype(Direct3DCreate9On12Ex)>("Direct3DCreate9On12Ex", SDKVersion, pOverrideList, NumOverrideEntries, ppDirect3D9Ex);
    }

    IDirect3D9 *WINAPI Direct3DCreate9(UINT nSDKVersion) {
        D3D9ON12_ARGS arg = { };
        arg.Enable9On12 = TRUE;

        return RunD3D9Proc<decltype(Direct3DCreate9On12)>("Direct3DCreate9On12", nSDKVersion, &arg, 1);
    }

    HRESULT WINAPI Direct3DCreate9Ex(UINT nSDKVersion, IDirect3D9Ex **ppDirect3D9Ex) {
        D3D9ON12_ARGS arg = { };
        arg.Enable9On12 = TRUE;

        return RunD3D9Proc<decltype(Direct3DCreate9On12Ex)>("Direct3DCreate9On12Ex", nSDKVersion, &arg, 1, ppDirect3D9Ex);
    }

    int WINAPI D3DPERF_BeginEvent(D3DCOLOR col, LPCWSTR wszName) {
        return RunD3D9Proc<decltype(D3DPERF_BeginEvent)>("D3DPERF_BeginEvent", col, wszName);
    }

    int WINAPI D3DPERF_EndEvent(void) {
        return RunD3D9Proc<decltype(D3DPERF_EndEvent)>("D3DPERF_EndEvent");
    }

    void WINAPI D3DPERF_SetMarker(D3DCOLOR col, LPCWSTR wszName) {
        RunD3D9Proc<decltype(D3DPERF_SetMarker)>("D3DPERF_SetMarker", col, wszName);
    }

    void WINAPI D3DPERF_SetRegion(D3DCOLOR col, LPCWSTR wszName) {
        RunD3D9Proc<decltype(D3DPERF_SetRegion)>("D3DPERF_SetRegion", col, wszName);
    }

    BOOL WINAPI D3DPERF_QueryRepeatFrame(void) {
        return RunD3D9Proc<decltype(D3DPERF_QueryRepeatFrame)>("D3DPERF_QueryRepeatFrame");
    }

    void WINAPI D3DPERF_SetOptions(DWORD dwOptions) {
        RunD3D9Proc<decltype(D3DPERF_SetOptions)>("D3DPERF_SetOptions", dwOptions);
    }

    DWORD WINAPI D3DPERF_GetStatus(void) {
        return RunD3D9Proc<decltype(D3DPERF_GetStatus)>("D3DPERF_SetOptions");
    }

    void WINAPI DebugSetMute(void) {
        RunD3D9Proc<decltype(DebugSetMute)>("DebugSetMute");
    }

    int WINAPI DebugSetLevel(void) {
        return RunD3D9Proc<decltype(DebugSetLevel)>("DebugSetLevel");
    }

    void WINAPI PSGPError(D3DFE_PROCESSVERTICES *a, PSGPERRORID b, UINT c) {
        RunD3D9Proc<decltype(PSGPError)>("PSGPError", a, b, c);
    }

    void WINAPI PSGPSampleTexture(D3DFE_PROCESSVERTICES *a, UINT b, float(*const c)[4], UINT d, float(*const e)[4]) {
        RunD3D9Proc<decltype(PSGPSampleTexture)>("PSGPError", a, b, c, d, e);
    }

    IDirect3DShaderValidator9 *WINAPI Direct3DShaderValidatorCreate9(void) {
        return RunD3D9Proc<decltype(Direct3DShaderValidatorCreate9)>("Direct3DShaderValidatorCreate9");
    }

    int WINAPI Direct3D9EnableMaximizedWindowedModeShim(UINT a) {
        return RunD3D9Proc<decltype(Direct3D9EnableMaximizedWindowedModeShim)>("Direct3D9EnableMaximizedWindowedModeShim", a);
    }
}