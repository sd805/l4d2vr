#pragma once

namespace offsets
{
    // client.dll offsets
    static int RenderView = 0x1D6C30; // sig: 55 8B EC 81 EC ? ? ? ? 53 56 57 8B D9
    static int g_pClientMode = 0x7AFB08; // sig: 8B 04 85 ? ? ? ? 8B 10 8B C8 8B 42 70
    static int CalcViewModelView = 0x287270; // sig: 55 8B EC 83 EC 48 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 10 8B 10
    static int ClientFireTerrorBullets = 0x2F4350; // sig: E8 ? ? ? ? 83 C4 1C 80 BE ? ? ? ? ?

    // server.dll offsets
    static int ServerFireTerrorBullets = 0x3C3FC0; // sig: E8 ? ? ? ? D9 86 ? ? ? ? 8B 4D F4

    // materialsystem.dll offsets
    static int GetRenderTarget = 0x2CD30; // sig: 83 79 4C 00
    static int isGameRunning = 0x2Ab8; // sig: (+0x3) E8 ? ? ? ? 83 C4 04 57 8B CE

}