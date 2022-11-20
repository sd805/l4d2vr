#pragma once
#include <iostream>
#include "MinHook.h"

class Game;
class VR;
class ITexture;
class CViewSetup;
class CUserCmd;
class QAngle;
class Vector;
class edict_t;
class ModelRenderInfo_t;


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
typedef int(__cdecl *tAdjustEngineViewport)(int &x, int &y, int &width, int &height);
typedef void(__thiscall *tViewport)(void *thisptr, int x, int y, int width, int height);
typedef void(__thiscall *tGetViewport)(void *thisptr, int &x, int &y, int &width, int &height);
typedef int(__thiscall *tTestMeleeSwingCollision)(void *thisptr, Vector const &vec);
typedef void(__thiscall *tDoMeleeSwing)(void *thisptr);
typedef void(__thiscall *tStartMeleeSwing)(void *thisptr, void* player, bool a3);
typedef int(__thiscall *tPrimaryAttack)(void *thisptr);
typedef void(__thiscall *tItemPostFrame)(void *thisptr);
typedef int(__thiscall *tGetPrimaryAttackActivity)(void *thisptr, void *meleeInfo);
typedef Vector *(__thiscall *tEyePosition)(void *thisptr, Vector *eyePos);
typedef void(__thiscall *tDrawModelExecute)(void *thisptr, void *state, const ModelRenderInfo_t &info, void *pCustomBoneToWorld);
typedef void(__thiscall *tPushRenderTargetAndViewport)(void *thisptr, ITexture *pTexture, ITexture *pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH);
typedef void(__thiscall *tPopRenderTargetAndViewport)(void *thisptr);
typedef void(__thiscall *tVgui_Paint)(void *thisptr, int mode);
typedef int(__cdecl *tIsSplitScreen)();
typedef DWORD *(__thiscall *tPrePushRenderTarget)(void *thisptr, int a2);


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
	static inline Hook<tAdjustEngineViewport> hkAdjustEngineViewport;
	static inline Hook<tViewport> hkViewport;
	static inline Hook<tGetViewport> hkGetViewport;
	static inline Hook<tTestMeleeSwingCollision> hkTestMeleeSwingCollisionClient;
	static inline Hook<tTestMeleeSwingCollision> hkTestMeleeSwingCollisionServer;
	static inline Hook<tDoMeleeSwing> hkDoMeleeSwingServer;
	static inline Hook<tStartMeleeSwing> hkStartMeleeSwingServer;
	static inline Hook<tPrimaryAttack> hkPrimaryAttackServer;
	static inline Hook<tItemPostFrame> hkItemPostFrameServer;
	static inline Hook<tGetPrimaryAttackActivity> hkGetPrimaryAttackActivity;
	static inline Hook<tEyePosition> hkEyePosition;
	static inline Hook<tDrawModelExecute> hkDrawModelExecute;
	static inline Hook<tPushRenderTargetAndViewport> hkPushRenderTargetAndViewport;
	static inline Hook<tPopRenderTargetAndViewport> hkPopRenderTargetAndViewport;
	static inline Hook<tVgui_Paint> hkVgui_Paint;
	static inline Hook<tIsSplitScreen> hkIsSplitScreen;
	static inline Hook<tPrePushRenderTarget> hkPrePushRenderTarget;

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
	static void dAdjustEngineViewport(int &x, int &y, int &width, int &height);
	static void __fastcall dViewport(void *ecx, void *edx, int x, int y, int width, int height);
	static void __fastcall dGetViewport(void *ecx, void *edx, int &x, int &y, int &width, int &height);
	static int __fastcall dTestMeleeSwingCollisionClient(void *ecx, void *edx, Vector const &vec);
	static int __fastcall dTestMeleeSwingCollisionServer(void *ecx, void *edx, Vector const &vec);
	static void __fastcall dDoMeleeSwingServer(void *ecx, void *edx);
	static void __fastcall dStartMeleeSwingServer(void *ecx, void *edx, void *player, bool a3);
	static int __fastcall dPrimaryAttackServer(void *ecx, void *edx);
	static void __fastcall dItemPostFrameServer(void *ecx, void *edx);
	static int __fastcall dGetPrimaryAttackActivity(void *ecx, void *edx, void* meleeInfo);
	static Vector *__fastcall dEyePosition(void *ecx, void *edx, Vector *eyePos);
	static void __fastcall dDrawModelExecute(void *ecx, void* edx, void *state, const ModelRenderInfo_t &info, void *pCustomBoneToWorld);
	static void __fastcall dPushRenderTargetAndViewport(void *ecx, void *edx, ITexture *pTexture, ITexture *pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH);
	static void __fastcall dPopRenderTargetAndViewport(void *ecx, void *edx);
	static void __fastcall dVGui_Paint(void *ecx, void *edx, int mode);
	static int __fastcall dIsSplitScreen();
	static DWORD *__fastcall dPrePushRenderTarget(void *ecx, void *edx, int a2);

	static inline int m_PushHUDStep;
	static inline bool m_PushedHud;
};