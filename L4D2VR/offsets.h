#pragma once

namespace offsets
{
    // client.dll offsets
    static int RenderView = 0x1D6C30; // 55 8B EC 81 EC ? ? ? ? 53 56 57 8B D9
    static int g_pClientMode = 0x7AFB08; // 8B 04 85 ? ? ? ? 8B 10 8B C8 8B 42 70
    static int CalcViewModelView = 0x287270; // 55 8B EC 83 EC 48 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 10 8B 10
    static int ClientFireTerrorBullets = 0x2F4350; // E8 ? ? ? ? 83 C4 1C 80 BE ? ? ? ? ?
    static int WriteUsercmdDeltaToBuffer = 0x134790; // 55 8B EC 83 EC 60 0F 57 C0 8B 55 0C
    static int WriteUsercmd = 0x1AAD50; // E8 ? ? ? ? 83 C4 0C 38 5E 10

    // server.dll offsets
    static int ServerFireTerrorBullets = 0x3C3FC0; // E8 ? ? ? ? D9 86 ? ? ? ? 8B 4D F4
    static int ReadUserCmd = 0x205100; // E8 ? ? ? ? 8B C6 4F
    static int ProcessUsercmds = 0xEF710; // 55 8B EC B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 8B 45 0C 8B 55 08
    static int CBaseEntity_entindex = 0x25390; // E8 ? ? ? ? 89 47 2C 

    // materialsystem.dll offsets
    static int GetRenderTarget = 0x2CD30; // 83 79 4C 00
    static int isGameRunning = 0x2Ab8; // (+0x3) E8 ? ? ? ? 83 C4 04 57 8B CE

}