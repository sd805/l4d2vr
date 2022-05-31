#include "game.h"
#include <Windows.h>
#include <iostream>
#include "sdk.h"
#include "vr.h"
#include "hooks.h"
#include "offsets.h"
#include "sigscanner.h"

Game::Game()
{
    while (!GetModuleHandle("client.dll"))
        Sleep(50);
    while (!GetModuleHandle("engine.dll"))
        Sleep(50);
    while (!GetModuleHandle("MaterialSystem.dll"))
        Sleep(50);
    while (!GetModuleHandle("server.dll"))
        Sleep(50);

    g_engine = (uintptr_t)GetModuleHandle("engine.dll");
    if (g_engine == NULL)
    {
        errorMsg("Failed to load engine.dll in dllmain");
    }

    g_client = (uintptr_t)GetModuleHandle("client.dll");
    if (g_client == NULL)
    {
        errorMsg("Failed to load client.dll in dllmain");
    }

    g_materialSystem = (uintptr_t)GetModuleHandle("MaterialSystem.dll");
    if (g_materialSystem == NULL)
    {
        errorMsg("Failed to load MaterialSystem.dll in dllmain");
    }

    g_server = (uintptr_t)GetModuleHandle("server.dll");
    if (g_server == NULL)
    {
        errorMsg("Failed to load server.dll in dllmain");
    }


    ClientEntityList = (IClientEntityList *)GetInterface("client.dll", "VClientEntityList003");
    EngineTrace = (IEngineTrace *)GetInterface("engine.dll", "EngineTraceClient004");
    EngineClient = (IEngineClient *)GetInterface("engine.dll", "VEngineClient013");
    MaterialSystem = (IMaterialSystem *)GetInterface("MaterialSystem.dll", "VMaterialSystem080");
    ClientViewRender = (IViewRender *)GetInterface("client.dll", "VEngineRenderView013");
    EngineViewRender = (IViewRender *)GetInterface("engine.dll", "VEngineRenderView013");

    mOffsets = new Offsets();
    mVR = new VR(this);
    mHooks = new Hooks(this);
}

void *Game::GetInterface(const char *dllname, const char *interfacename)
{
    tCreateInterface CreateInterface = (tCreateInterface)GetProcAddress(GetModuleHandle(dllname), "CreateInterface");

    int returnCode = 0;
    void *createdInterface = CreateInterface(interfacename, &returnCode);

    return createdInterface;
}

void Game::errorMsg(const char *msg)
{
    MessageBox(0, msg, "L4D2VR", MB_ICONERROR | MB_OK);
}

CBaseEntity *Game::GetClientEntity(int entityIndex)
{
    return (CBaseEntity *)(ClientEntityList->GetClientEntity(entityIndex));
}

char *Game::getNetworkName(uintptr_t *entity)
{
    uintptr_t *IClientNetworkableVtable = (uintptr_t *)*(entity + 0x8);
    uintptr_t *GetClientClassPtr = (uintptr_t *)*(IClientNetworkableVtable + 0x8);
    uintptr_t *ClientClassPtr = (uintptr_t *)*(GetClientClassPtr + 0x1);
    char *m_pNetworkName = (char *)*(ClientClassPtr + 0x8);
    int classID = (int)*(ClientClassPtr + 0x10);
    std::cout << "ClassID: " << classID << std::endl;
    return m_pNetworkName;
}

void **Game::getClientModeVTable()
{
    uintptr_t *g_pClientMode = *(uintptr_t **)(mOffsets->g_ppClientMode.address);
    void **clientModeVTable = *reinterpret_cast<void ***>(g_pClientMode);
    return clientModeVTable;
}

void Game::ClientCmd(const char *szCmdString)
{
    EngineClient->ClientCmd(szCmdString);
}

void Game::ClientCmd_Unrestricted(const char *szCmdString)
{
    EngineClient->ClientCmd_Unrestricted(szCmdString);
}


