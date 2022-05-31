#pragma once
#include <cstdint>
#include <array>
#include "vector.h"

class IClientEntityList;
class IEngineTrace;
class IEngineClient;
class IMaterialSystem;
class IBaseClientDLL;
class IViewRender;
class IViewRender;
class CBaseEntity;

class Offsets;
class VR;
class Hooks;


struct Player
{
    bool isUsingVR;
    QAngle controllerAngle;
    Vector controllerPos;
};

class Game 
{
public:
    IClientEntityList *m_ClientEntityList;
    IEngineTrace *m_EngineTrace;
    IEngineClient *m_EngineClient;
    IMaterialSystem *m_MaterialSystem;
    IBaseClientDLL *m_BaseClientDll;
    IViewRender *m_ClientViewRender;
    IViewRender *m_EngineViewRender;

    uintptr_t m_BaseEngine;
    uintptr_t m_BaseClient;
    uintptr_t m_BaseServer;
    uintptr_t m_BaseMaterialSystem;

    Offsets *m_Offsets;
    VR *m_VR;
    Hooks *m_Hooks;

    std::array<Player, 24> m_PlayersVRInfo;
    int m_CurrentUsercmdID;

    Game();

    void *GetInterface(const char *dllname, const char *interfacename);

    static void errorMsg(const char *msg);

    CBaseEntity *GetClientEntity(int entityIndex);
    char *getNetworkName(uintptr_t *entity);
    void **getClientModeVTable();
    void ClientCmd(const char *szCmdString);
    void ClientCmd_Unrestricted(const char *szCmdString);

    typedef void *(__cdecl *tCreateInterface)(const char *name, int *returnCode);
};

