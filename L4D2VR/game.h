#pragma once
#include <cstdint>

class IClientEntityList;
class IEngineTrace;
class IEngineClient;
class IMaterialSystem;
class IBaseClientDLL;
class IViewRender;
class IViewRender;
class CBaseEntity;

class VR;
class Hooks;


class Game 
{
public:
    IClientEntityList *ClientEntityList;
    IEngineTrace *EngineTrace;
    IEngineClient *EngineClient;
    IMaterialSystem *MaterialSystem;
    IBaseClientDLL *BaseClientDll;
    IViewRender *ClientViewRender;
    IViewRender *EngineViewRender;

    uintptr_t g_engine;
    uintptr_t g_client;
    uintptr_t g_server;
    uintptr_t g_materialSystem;
    uintptr_t g_LocalPlayer;

    VR *mVR;
    Hooks *mHooks;
    
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

