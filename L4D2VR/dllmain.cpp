// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include <string>
#include "game.h"
#include "hooks.h"
#include "vr.h"
#include "sdk.h"

DWORD WINAPI InitL4D2VR(HMODULE hModule)
{
#ifdef _DEBUG
    AllocConsole();
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

    // Make sure -insecure is used
    LPWSTR *szArglist;
    int nArgs;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    bool insecureEnabled = false;
    for (int i = 0; i < nArgs; ++i)
    {
        if (wcscmp(szArglist[i], L"-insecure") == 0)
            insecureEnabled = true;
    }
    LocalFree(szArglist);

    if (!insecureEnabled)
    {
        std::wstring warningMessage =
            L"L4D2VR was launched without the -insecure parameter.\n"
            L"That launch option prevents VAC from starting.\n\n"
            L"Without it, VAC may run as normal and could detect unsupported modifications,"
            L" increasing your ban risk.\n\n"
            L"Press OK to continue anyway or Cancel to exit.";
        std::wstring warningTitle = L"L4D2VR Warning";
        int result = MessageBoxW(
            NULL,
            warningMessage.c_str(),
            warningTitle.c_str(),
            MB_ICONWARNING | MB_OKCANCEL | MB_DEFBUTTON2);

        if (result != IDOK)
            ExitProcess(0);
    }

    g_Game = new Game();

    return 0;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitL4D2VR, hModule, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


