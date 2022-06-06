#pragma once
#define D3D_DEBUG_INFO
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include "MinHook.h"

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

class Game;
class VR;
class ITexture;
class CViewSetup;
class CUserCmd;
class QAngle;
class Vector;
class edict_t;


template <typename T>
struct Hook {
	T fOriginal;
	LPVOID pTarget;
	bool isEnabled;

	int createHook(LPVOID targetFunc, LPVOID detourFunc)
	{
		if (MH_CreateHook(targetFunc, detourFunc, reinterpret_cast<LPVOID *>(&fOriginal)) != MH_OK)
		{
			char errorString[512];
			sprintf_s(errorString, 512, "Failed to create hook with this signature: %s", typeid(T).name());
			Game::errorMsg(errorString);
			return 1;
		}
		pTarget = targetFunc;
	}

	int enableHook()
	{
		if (MH_EnableHook(pTarget) != MH_OK)
		{
			Game::errorMsg("Failed to enable hook");
			return 1;
		}
		isEnabled = true;
	}

	int disableHook()
	{
		if (MH_DisableHook(pTarget) != MH_OK)
		{
			Game::errorMsg("Failed to disable hook");
			return 1;
		}
		isEnabled = false;
	}
};


// Source Engine functions
typedef ITexture *(__thiscall *tGetRenderTarget)(void *thisptr);
typedef void(__thiscall *tRenderView)(void *thisptr, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);
typedef bool(__thiscall *tCreateMove)(void *thisptr, float flInputSampleTime, CUserCmd *cmd);
typedef void(__thiscall *tEndFrame)(PVOID);
typedef void(__thiscall *tCalcViewModelView)(void *thisptr, void *owner, const Vector &eyePosition, const QAngle &eyeAngles);
typedef int(__cdecl *tFireTerrorBullets)(int playerId, const Vector &vecOrigin, const QAngle &vecAngles, int a4, int a5, int a6, float a7);
typedef float(__thiscall *tProcessUsercmds)(void *thisptr, edict_t *player, void *buf, int numcmds, int totalcmds, int dropped_packets, bool ignore, bool paused);
typedef int(__cdecl *tReadUsercmd)(void *buf, CUserCmd *move, CUserCmd *from);
typedef void(__thiscall *tWriteUsercmdDeltaToBuffer)(void *thisptr, int a1, void *buf, int from, int to, bool isnewcommand);
typedef int(__cdecl *tWriteUsercmd)(void *buf, CUserCmd *to, CUserCmd *from);


class Hooks
{
public:
	static inline Game *m_Game;
	static inline VR *m_VR;

	static inline Hook<tGetRenderTarget> hkGetRenderTarget;
	static inline Hook<tRenderView> hkRenderView;
	static inline Hook<tCreateMove> hkCreateMove;
	static inline Hook<tEndFrame> hkEndFrame;
	static inline Hook<tCalcViewModelView> hkCalcViewModelView;
	static inline Hook<tFireTerrorBullets> hkServerFireTerrorBullets;
	static inline Hook<tFireTerrorBullets> hkClientFireTerrorBullets;
	static inline Hook<tProcessUsercmds> hkProcessUsercmds;
	static inline Hook<tReadUsercmd> hkReadUsercmd;
	static inline Hook<tWriteUsercmdDeltaToBuffer> hkWriteUsercmdDeltaToBuffer;
	static inline Hook<tWriteUsercmd> hkWriteUsercmd;

	Hooks() {};
	Hooks(Game *game);

	~Hooks();

	int initSourceHooks();

	// Detour functions
	static ITexture *__fastcall dGetRenderTarget(void *ecx, void *edx);
	static void __fastcall dRenderView(void *ecx, void *edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);
	static bool __fastcall dCreateMove(void *ecx, void *edx, float flInputSampleTime, CUserCmd *cmd);
	static void __fastcall dEndFrame(void *ecx, void *edx);
	static void __fastcall dCalcViewModelView(void *ecx, void *edx, void *owner, const Vector &eyePosition, const QAngle &eyeAngles);
	static int dServerFireTerrorBullets(int playerId, const Vector &vecOrigin, const QAngle &vecAngles, int a4, int a5, int a6, float a7);
	static int dClientFireTerrorBullets(int playerId, const Vector &vecOrigin, const QAngle &vecAngles, int a4, int a5, int a6, float a7);
	static float __fastcall dProcessUsercmds(void *ecx, void *edx, edict_t *player, void *buf, int numcmds, int totalcmds, int dropped_packets, bool ignore, bool paused);
	static int dReadUsercmd(void *buf, CUserCmd *move, CUserCmd *from);
	static void __fastcall dWriteUsercmdDeltaToBuffer(void *ecx, void *edx, int a1, void *buf, int from, int to, bool isnewcommand);
	static int dWriteUsercmd(void *buf, CUserCmd *to, CUserCmd *from);
};