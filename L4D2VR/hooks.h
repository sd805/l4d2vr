#pragma once
#define D3D_DEBUG_INFO
#include <d3d9.h>
#include <d3dx9.h>
#include <d3d9on12.h>
#include <d3d12.h>
#include <iostream>
#include "MinHook.h"

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3d12.lib")

class Game;
class VR;
class ITexture;
class CViewSetup;
class CUserCmd;
class QAngle;
class Vector;


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

// DirectX functions
typedef HRESULT(__stdcall *tBeginScene)(IDirect3DDevice9 *pDevice); 
typedef HRESULT(__stdcall *tClear)(IDirect3DDevice9 *pDevice, DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
typedef HRESULT(__stdcall *tEndScene)(IDirect3DDevice9 *pDevice);
typedef HRESULT(APIENTRY *tCreateTexture)(IDirect3DDevice9 *, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9 **, HANDLE *);
typedef HRESULT(__stdcall *tPresent)(IDirect3DDevice9 *pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);

// Source Engine functions
typedef ITexture *(__thiscall *tGetRenderTarget)(void *thisptr);
typedef void(__thiscall *tRenderView)(void *thisptr, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);
typedef bool(__thiscall *tCreateMove)(void *thisptr, float flInputSampleTime, CUserCmd *cmd);
typedef void(__thiscall *tEndFrame)(PVOID);
typedef void(__thiscall *tCalcViewModelView)(void *thisptr, void *owner, const Vector &eyePosition, const QAngle &eyeAngles);
typedef int(__cdecl *tFireTerrorBullets)(int playerId, const Vector &vecOrigin, const QAngle &vecAngles, int a4, int a5, int a6, float a7);

class Hooks
{
public:
	static inline Game *mGame;
	static inline VR *mVR;

	static inline Hook<tBeginScene> hkBeginScene;
	static inline Hook<tClear> hkClear;
	static inline Hook<tEndScene> hkEndScene;
	static inline Hook<tCreateTexture> hkCreateTexture;
	static inline Hook<tPresent> hkPresent;

	static inline Hook<tGetRenderTarget> hkGetRenderTarget;
	static inline Hook<tRenderView> hkRenderView;
	static inline Hook<tCreateMove> hkCreateMove;
	static inline Hook<tEndFrame> hkEndFrame;
	static inline Hook<tCalcViewModelView> hkCalcViewModelView;
	static inline Hook<tFireTerrorBullets> hkServerFireTerrorBullets;
	static inline Hook<tFireTerrorBullets> hkClientFireTerrorBullets;

	static inline bool mCreatedTexture;

	static inline IDirect3DDevice9On12 *iD9on12;
	static inline ID3D12CommandQueue *commandQueue;

	static inline bool mInRenderView;

	Hooks() {};
	Hooks(Game *game);

	~Hooks();

	int initDxHooks();

	int initSourceHooks();


	// Detour functions
	static HRESULT __stdcall dBeginScene(IDirect3DDevice9 *pDevice);
	static HRESULT __stdcall dClear(IDirect3DDevice9 *pDevice, DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
	static HRESULT __stdcall dEndScene(IDirect3DDevice9 *pDevice);
	static HRESULT APIENTRY dCreateTexture(IDirect3DDevice9 *pDevice, UINT w, UINT h, UINT levels, DWORD usage, D3DFORMAT format, D3DPOOL pool, IDirect3DTexture9 **tex, HANDLE *shared_handle);
	static HRESULT __stdcall dPresent(IDirect3DDevice9 *pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);

	static ITexture *__fastcall dGetRenderTarget(void *ecx, void *edx);
	static void __fastcall dRenderView(void *ecx, void *edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);
	static bool __fastcall dCreateMove(void *ecx, void *edx, float flInputSampleTime, CUserCmd *cmd);
	static void __fastcall dEndFrame(void *ecx, void *edx);
	static void __fastcall dCalcViewModelView(void *ecx, void *edx, void *owner, const Vector &eyePosition, const QAngle &eyeAngles);
	static int dServerFireTerrorBullets(int playerId, const Vector &vecOrigin, const QAngle &vecAngles, int a4, int a5, int a6, float a7);
	static int dClientFireTerrorBullets(int playerId, const Vector &vecOrigin, const QAngle &vecAngles, int a4, int a5, int a6, float a7);
};