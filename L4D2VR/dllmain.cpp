// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include "game.h"
#include "hooks.h"
#include "vr.h"
#include "sdk.h"

Game *g_pGame;

// testing random hooks
// TestMeleeSwingCollision: game.g_client + 0x30C040
typedef void(__thiscall *TestMeleeSwingCollision)(void *thisptr, Vector const &vec);
TestMeleeSwingCollision pTestMeleeSwingCollision = nullptr;
LPVOID TestMeleeSwingCollisionTarget;
void __fastcall TestMeleeSwingCollisionHook(void *ecx, void *edx, Vector const &vec)
{
    return pTestMeleeSwingCollision(ecx, vec);
}
bool hookTestMeleeSwingCollision(LPVOID tmsAddr)
{
    TestMeleeSwingCollisionTarget = tmsAddr;
    if (MH_CreateHook(TestMeleeSwingCollisionTarget, &TestMeleeSwingCollisionHook, reinterpret_cast<LPVOID *>(&pTestMeleeSwingCollision)) != MH_OK)
    {
        Game::errorMsg("Failed to create TestMeleeSwingCollision hook");
        return 1;
    }

    if (MH_EnableHook(TestMeleeSwingCollisionTarget) != MH_OK)
    {
        Game::errorMsg("Failed to enable TestMeleeSwingCollision hook");
        return 1;
    }

    std::cout << "Hooked TestMeleeSwingCollision" << std::endl;
}


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
        ExitProcess(0);

    Game* game = new Game();
    g_pGame = game;

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


