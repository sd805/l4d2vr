// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include "game.h"
#include "hooks.h"
#include "sdk.h"
#include <minidumpapiset.h>

HMODULE dbgcore = 0;

// dgbcore.dll proxy
extern "C" {
    __declspec(dllexport) BOOL MiniDumpReadDumpStream_Hook(PVOID BaseOfDump, ULONG StreamNumber,
        PMINIDUMP_DIRECTORY *Dir,
        PVOID *StreamPointer, ULONG *StreamSize) {
        return ((decltype(MiniDumpReadDumpStream) *)GetProcAddress(dbgcore, "MiniDumpReadDumpStream"))(BaseOfDump, StreamNumber,
            Dir, StreamPointer, StreamSize);
    }

    __declspec(dllexport) BOOL MiniDumpWriteDump_Hook(HANDLE hProcess, DWORD ProcessId, HANDLE hFile,
        MINIDUMP_TYPE DumpType,
        PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
        PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
        PMINIDUMP_CALLBACK_INFORMATION    CallbackParam) {
        return ((decltype(MiniDumpWriteDump) *)GetProcAddress(dbgcore, "MiniDumpWriteDump"))(hProcess, ProcessId, hFile, DumpType,
            ExceptionParam, UserStreamParam, CallbackParam);
    }
}

DWORD WINAPI InitL4D2VR(HMODULE hModule)
{
    wchar_t buffer[MAX_PATH]{ 0 };
    if (GetSystemDirectoryW(buffer, MAX_PATH) != 0) 
    {
        if ((dbgcore = LoadLibraryW((std::wstring{ buffer } + L"\\dbgcore.dll").c_str())) == NULL) 
        {
            Game::errorMsg("Failed to load original dbgcore.dll");
        }

    }

#ifdef _DEBUG
    AllocConsole();
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

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