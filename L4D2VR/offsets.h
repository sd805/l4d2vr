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
    Offset g_ppClientMode =            { "client.dll", 0x228A48, "8B 04 85 ? ? ? ? 8B 10 8B C8 8B 42 70", 3 };
    Offset CalcViewModelView =         { "client.dll", 0x287270, "55 8B EC 83 EC 48 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 10 8B 10" };
    Offset ClientFireTerrorBullets =   { "client.dll", 0x2F4350, "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 8B 4D 10"};
    Offset WriteUsercmdDeltaToBuffer = { "client.dll", 0x134790, "55 8B EC 83 EC 60 0F 57 C0 8B 55 0C" };
    Offset WriteUsercmd =              { "client.dll", 0x1AAD50, "55 8B EC A1 ? ? ? ? 83 78 30 00 53 8B 5D 10 56 57" };
    Offset g_pppInput =                { "client.dll", 0xA8A22, "8B 0D ? ? ? ? 8B 01 8B 50 58 FF E2", 2 };

    Offset ServerFireTerrorBullets = { "server.dll", 0x3C3FC0, "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 8B 4D 10" };
    Offset ReadUserCmd =             { "server.dll", 0x205100, "55 8B EC 53 8B 5D 10 56 57 8B 7D 0C 53" };
    Offset ProcessUsercmds =         { "server.dll", 0xEF710, "55 8B EC B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 8B 45 0C 8B 55 08" };
    Offset CBaseEntity_entindex =    { "server.dll", 0x25390, "8B 41 28 85 C0 75 01 C3 8B 0D ? ? ? ? 2B 41 58 C1 F8 04 C3 CC CC CC CC CC CC CC CC CC CC CC 55"};

    Offset GetRenderTarget = { "materialsystem.dll", 0x2CD30, "83 79 4C 00" };
};