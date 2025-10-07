#include "game.h"
#include <Windows.h>
#include <iostream>
#include <unordered_map>
#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <mutex>

#include "sdk.h"
#include "vr.h"
#include "hooks.h"
#include "offsets.h"
#include "sigscanner.h"
#include "sdk/ivdebugoverlay.h"

static std::mutex logMutex;
using tCreateInterface = void* (__cdecl*)(const char* name, int* returnCode);

// === Utility: Retry module load with logging ===
static HMODULE GetModuleWithRetry(const char* dllname, std::chrono::milliseconds timeout = std::chrono::seconds(30), int delayMs = 50)
{
    const auto start = std::chrono::steady_clock::now();
    int attempt = 0;

    while (true)
    {
        HMODULE handle = GetModuleHandleA(dllname);
        if (handle)
            return handle;

        ++attempt;
        Game::logMsg("Waiting for module to load: %s (attempt %d)", dllname, attempt);
        Sleep(delayMs);

        if (timeout.count() >= 0 && std::chrono::steady_clock::now() - start >= timeout)
            break;
    }

    Game::errorMsg(("Failed to load module after retrying: " + std::string(dllname)).c_str());
    return nullptr;
}

// === Utility: Safe interface fetch with static cache ===
static void* GetInterfaceSafe(const char* dllname, const char* interfacename)
{
    static std::unordered_map<std::string, void*> cache;

    std::string key = std::string(dllname) + "::" + interfacename;
    auto it = cache.find(key);
    if (it != cache.end())
        return it->second;

    HMODULE mod = GetModuleWithRetry(dllname);
    if (!mod)
        return nullptr;

    auto CreateInterface = reinterpret_cast<tCreateInterface>(GetProcAddress(mod, "CreateInterface"));
    if (!CreateInterface)
    {
        Game::errorMsg(("CreateInterface not found in " + std::string(dllname)).c_str());
        return nullptr;
    }

    int returnCode = 0;
    void* iface = CreateInterface(interfacename, &returnCode);
    if (!iface)
    {
        Game::errorMsg(("Interface not found: " + std::string(interfacename)).c_str());
        return nullptr;
    }

    cache[key] = iface;
    return iface;
}

// === Utility: Attempt interface fetch without error logging ===
static void* TryInterfaceNoError(const char* dllname, const char* interfacename)
{
    HMODULE mod = GetModuleWithRetry(dllname);
    if (!mod)
        return nullptr;

    auto CreateInterface = reinterpret_cast<tCreateInterface>(GetProcAddress(mod, "CreateInterface"));
    if (!CreateInterface)
        return nullptr;

    int returnCode = 0;
    return CreateInterface(interfacename, &returnCode);
}

// === Game Constructor ===
Game::Game()
{
    m_BaseClient = reinterpret_cast<uintptr_t>(GetModuleWithRetry("client.dll"));
    m_BaseEngine = reinterpret_cast<uintptr_t>(GetModuleWithRetry("engine.dll"));
    m_BaseMaterialSystem = reinterpret_cast<uintptr_t>(GetModuleWithRetry("MaterialSystem.dll"));
    m_BaseServer = reinterpret_cast<uintptr_t>(GetModuleWithRetry("server.dll"));
    m_BaseVgui2 = reinterpret_cast<uintptr_t>(GetModuleWithRetry("vgui2.dll"));

    m_ClientEntityList = static_cast<IClientEntityList*>(GetInterfaceSafe("client.dll", "VClientEntityList003"));
    m_EngineTrace = static_cast<IEngineTrace*>(GetInterfaceSafe("engine.dll", "EngineTraceClient003"));
    m_EngineClient = static_cast<IEngineClient*>(GetInterfaceSafe("engine.dll", "VEngineClient013"));
    m_MaterialSystem = static_cast<IMaterialSystem*>(GetInterfaceSafe("MaterialSystem.dll", "VMaterialSystem080"));
    m_ModelInfo = static_cast<IModelInfo*>(GetInterfaceSafe("engine.dll", "VModelInfoClient004"));
    m_ModelRender = static_cast<IModelRender*>(GetInterfaceSafe("engine.dll", "VEngineModel016"));
    m_VguiInput = static_cast<IInput*>(GetInterfaceSafe("vgui2.dll", "VGUI_InputInternal001"));
    m_VguiSurface = static_cast<ISurface*>(GetInterfaceSafe("vguimatsurface.dll", "VGUI_Surface031"));
    m_DebugOverlay = static_cast<IVDebugOverlay*>(TryInterfaceNoError("engine.dll", "VDebugOverlay004"));
    if (!m_DebugOverlay)
        m_DebugOverlay = static_cast<IVDebugOverlay*>(TryInterfaceNoError("engine.dll", "VDebugOverlay003"));

    m_Offsets = new Offsets();
    m_VR = new VR(this);

    ResetAllPlayerVRInfo();

    m_Hooks = new Hooks(this);

    m_Initialized = true;

    Game::logMsg("Game initialized successfully.");
}

// === Fallback Interface ===
void* Game::GetInterface(const char* dllname, const char* interfacename)
{
    Game::logMsg("Fallback GetInterface called for %s::%s", dllname, interfacename);
    return GetInterfaceSafe(dllname, interfacename);
}

// === Thread-safe Log Message with Timestamp ===
void Game::logMsg(const char* fmt, ...)
{
    std::lock_guard<std::mutex> lock(logMutex);

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char timebuf[20] = {};
    std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));

    printf("[%s] ", timebuf);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");

    FILE* file = fopen("vrmod_log.txt", "a");
    if (file)
    {
        fprintf(file, "[%s] ", timebuf);
        va_list args2;
        va_start(args2, fmt);
        vfprintf(file, fmt, args2);
        va_end(args2);
        fprintf(file, "\n");
        fclose(file);
    }
}

// === Error Message ===
void Game::errorMsg(const char* msg)
{
    logMsg("[ERROR] %s", msg);
    MessageBoxA(nullptr, msg, "L4D2VR Error", MB_ICONERROR | MB_OK);
}

bool Game::IsValidPlayerIndex(int index) const
{
    return index >= 0 && index < static_cast<int>(m_PlayersVRInfo.size());
}

void Game::ResetAllPlayerVRInfo()
{
    m_PlayersVRInfo.fill(Player{});
}

// === Entity Access ===
CBaseEntity* Game::GetClientEntity(int entityIndex)
{
    if (!m_ClientEntityList)
        return nullptr;

    return static_cast<CBaseEntity*>(m_ClientEntityList->GetClientEntity(entityIndex));
}

// === Network Name Utility ===
char* Game::getNetworkName(uintptr_t* entity)
{
    if (!entity)
        return nullptr;

    uintptr_t* vtable = reinterpret_cast<uintptr_t*>(*(entity + 0x8));
    if (!vtable)
        return nullptr;

    uintptr_t* getClientClassFn = reinterpret_cast<uintptr_t*>(*(vtable + 0x8));
    if (!getClientClassFn)
        return nullptr;

    uintptr_t* clientClass = reinterpret_cast<uintptr_t*>(*(getClientClassFn + 0x1));
    if (!clientClass)
        return nullptr;

    char* name = reinterpret_cast<char*>(*(clientClass + 0x8));
    int classID = static_cast<int>(*(clientClass + 0x10));

    Game::logMsg("[NetworkClass] ID: %d, Name: %s", classID, name ? name : "nullptr");
    return name;
}

// === Commands ===
void Game::ClientCmd(const char* szCmdString)
{
    if (m_EngineClient)
        m_EngineClient->ClientCmd(szCmdString);
}

void Game::ClientCmd_Unrestricted(const char* szCmdString)
{
    if (m_EngineClient)
        m_EngineClient->ClientCmd_Unrestricted(szCmdString);
}
