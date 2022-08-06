#pragma once
#include "sigscanner.h"
#include "game.h"


struct Offset
{
    std::string moduleName;
    int offset;
    int address;
    std::string signature;
    int sigOffset;

    Offset(std::string moduleName, int currentOffset, std::string signature, int sigOffset = 0)
    {
        this->moduleName = moduleName;
        this->offset = currentOffset;
        this->signature = signature;
        this->sigOffset = sigOffset;

        int newOffset = SigScanner::VerifyOffset(moduleName, currentOffset, signature, sigOffset);
        if (newOffset > 0)
        {
            this->offset = newOffset;
        }
            
        if (newOffset == -1)
        {
            Game::errorMsg(("Signature not found: " + signature).c_str());
            return;
        }

        this->address = (uintptr_t)GetModuleHandle(moduleName.c_str()) + this->offset;
    }
};

class Offsets
{
public:
    Offset RenderView =                { "client.dll", 0x1D6C30, "55 8B EC 81 EC ? ? ? ? 53 56 57 8B D9" };
    Offset g_pClientMode =             { "client.dll", 0x228351, "89 04 B5 ? ? ? ? E8", 3 };
    Offset CalcViewModelView =         { "client.dll", 0x287270, "55 8B EC 83 EC 48 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 10 8B 10" };
    Offset ClientFireTerrorBullets =   { "client.dll", 0x2F4350, "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 8B 4D 10"};
    Offset WriteUsercmdDeltaToBuffer = { "client.dll", 0x134790, "55 8B EC 83 EC 60 0F 57 C0 8B 55 0C" };
    Offset WriteUsercmd =              { "client.dll", 0x1AAD50, "55 8B EC A1 ? ? ? ? 83 78 30 00 53 8B 5D 10 56 57" };
    Offset g_pppInput =                { "client.dll", 0xA8A22, "8B 0D ? ? ? ? 8B 01 8B 50 58 FF E2", 2 };
    Offset AdjustEngineViewport =      { "client.dll", 0x31A890, "55 8B EC 8B 0D ? ? ? ? 85 C9 74 17" };
    Offset TestMeleeSwingClient =      { "client.dll", 0x30C040, "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 53 56 8B 75 08 57 8B D9 E8 ? ? ? ? 8B" };
    Offset GetMeleeWeaponInfoClient =  { "client.dll", 0x30B570, "8B 81 ? ? ? ? 50 B9 ? ? ? ? E8 ? ? ? ? C3" };

    Offset ServerFireTerrorBullets =   { "server.dll", 0x3C3FC0, "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 8B 4D 10" };
    Offset ReadUserCmd =               { "server.dll", 0x205100, "55 8B EC 53 8B 5D 10 56 57 8B 7D 0C 53" };
    Offset ProcessUsercmds =           { "server.dll", 0xEF710, "55 8B EC B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 8B 45 0C 8B 55 08" };
    Offset CBaseEntity_entindex =      { "server.dll", 0x25390, "8B 41 28 85 C0 75 01 C3 8B 0D ? ? ? ? 2B 41 58 C1 F8 04 C3 CC CC CC CC CC CC CC CC CC CC CC 55"};
    Offset TestMeleeSwingServer =      { "server.dll", 0x3E79E0, "24 FF D2 5B 5F 5E C3", 20};
    Offset DoMeleeSwingServer =        { "server.dll", 0x3E84C0, "55 8B EC 83 EC 3C 53 56 8B F1 E8 ? ? ? ? 8B D8 85" };
    Offset StartMeleeSwingServer =     { "server.dll", 0x3E8780, "55 8B EC 53 56 8B F1 8B 86 ? ? ? ? 50 B9 ? ? ? ? E8 ? ? ? ? 8B" };
    Offset PrimaryAttackServer =       { "server.dll", 0x3E8AB0, "56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 0F 84 ? ? ? ? 8B 87 ? ? ? ? 83 F8 FF" };
    Offset ItemPostFrameServer =       { "server.dll", 0x3E8BA0, "56 57 8B F1 E8 ? ? ? ? 8B CE E8 ? ? ? ? 8B F8 85 FF 0F 84 ? ? ? ? 53" };
    Offset GetPrimaryAttackActivity =  { "server.dll", 0x3E7630, "55 8B EC 53 8B 5D 08 56 57 8B BB ? ? ? ?" };
    Offset GetActiveWeapon =           { "server.dll", 0x464F0, "55 8B EC 8B 45 0C 56 8B 75 08 50 56 E8 ? ? ? ? 84 C0 74 47 8B", -64 };
    Offset GetMeleeWeaponInfo =        { "server.dll", 0x3E67D0, "8B 81 ? ? ? ? 50 B9 ? ? ? ? E8 ? ? ? ? C3" };
    Offset EyePosition =               { "server.dll", 0x6D610, "55 8B EC 56 8B F1 8B 86 ? ? ? ? C1 E8 0B A8 01 74 05 E8 ? ? ? ? 8B 45 08 F3" };

    Offset GetRenderTarget =           { "materialsystem.dll", 0x2CD30, "83 79 4C 00" };
    Offset Viewport =                  { "materialsystem.dll", 0x2E010, "55 8B EC 83 EC 28 8B C1" };
    Offset GetViewport =               { "materialsystem.dll", 0x2D240, "55 8B EC 8B 41 4C 8B 49 40 8D 04 C0 83 7C 81 ? ?" };

    Offset DrawModelExecute =          { "engine.dll", 0xE05E0, "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 8B 45 10 56 8B 75 08 57 8B" };
};