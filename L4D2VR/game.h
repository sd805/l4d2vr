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
class IModelInfo;
class IModelRender;
class IMaterial;
class C_BasePlayer;
struct model_t;

class Game;
class Offsets;
class VR;
class Hooks;

inline Game *g_Game;

struct Player
{
    C_BasePlayer *pPlayer;
    bool isUsingVR;
    QAngle controllerAngle;
    Vector controllerPos;
    bool isMeleeing;
    bool isNewSwing;
    QAngle prevControllerAngle;

    Player()
        : isUsingVR(false),
        controllerAngle({ 0,0,0 }),
        controllerPos({ 0,0,0 }),
        isMeleeing(false),
        isNewSwing(false),
        prevControllerAngle({ 0,0,0 })
    {}
};

class Game 
{
public:
    IClientEntityList *m_ClientEntityList = nullptr;
    IEngineTrace *m_EngineTrace = nullptr;
    IEngineClient *m_EngineClient = nullptr;
    IMaterialSystem *m_MaterialSystem = nullptr;
    IBaseClientDLL *m_BaseClientDll = nullptr;
    IViewRender *m_ClientViewRender = nullptr;
    IViewRender *m_EngineViewRender = nullptr;
    IModelInfo *m_ModelInfo = nullptr;
    IModelRender *m_ModelRender = nullptr;

    uintptr_t m_BaseEngine;
    uintptr_t m_BaseClient;
    uintptr_t m_BaseServer;
    uintptr_t m_BaseMaterialSystem;

    Offsets *m_Offsets = nullptr;
    VR *m_VR = nullptr;
    Hooks *m_Hooks = nullptr;

    bool m_Initialized = false;

    std::array<Player, 24> m_PlayersVRInfo;
    int m_CurrentUsercmdID = -1;
    bool m_PerformingMelee = false;

    model_t *m_ArmsModel = nullptr;
    IMaterial *m_ArmsMaterial = nullptr;
    bool m_CachedArmsModel = false;

    bool m_IsMeleeWeaponActive = false;

    Game();

    void *GetInterface(const char *dllname, const char *interfacename);

    static void errorMsg(const char *msg);

    CBaseEntity *GetClientEntity(int entityIndex);
    char *getNetworkName(uintptr_t *entity);
    void ClientCmd(const char *szCmdString);
    void ClientCmd_Unrestricted(const char *szCmdString);

    typedef void *(__cdecl *tCreateInterface)(const char *name, int *returnCode);
};

