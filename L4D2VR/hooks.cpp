#include "hooks.h"
#include "game.h"
#include "texture.h"
#include "sdk.h"
#include "vr.h"
#include "QAngle.h"
#include "vector.h"
#include "offsets.h"
#include <iostream>

Hooks::Hooks(Game *game)
{
	if (MH_Initialize() != MH_OK)
	{
		Game::errorMsg("Failed to init MinHook");
	}

	mGame = game;
	mVR = mGame->mVR;

	initDxHooks();
	initSourceHooks();

	hkBeginScene.enableHook();
	hkPresent.enableHook();
	hkGetRenderTarget.enableHook();
	hkCalcViewModelView.enableHook();
	hkServerFireTerrorBullets.enableHook();
	hkClientFireTerrorBullets.enableHook();
	hkProcessUsercmds.enableHook();
	hkReadUsercmd.enableHook();
	hkWriteUsercmdDeltaToBuffer.enableHook();
	hkWriteUsercmd.enableHook();
}

Hooks::~Hooks()
{
	if (MH_Uninitialize() != MH_OK)
	{
		Game::errorMsg("Failed to uninitialize MinHook");
	}
}

int Hooks::initDxHooks()
{
	IDirect3D9 *pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!pD3D)
		return 1;

	D3DPRESENT_PARAMETERS d3dparams = { 0 };
	d3dparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dparams.hDeviceWindow = GetForegroundWindow();
	d3dparams.Windowed = true;
	d3dparams.BackBufferFormat = D3DFMT_UNKNOWN;

	IDirect3DDevice9 *pDevice = nullptr;

	HRESULT result = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dparams.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dparams, &pDevice);

	void **vTable = *reinterpret_cast<void ***>(pDevice);

	LPVOID beginSceneVFunc = reinterpret_cast<LPVOID>(vTable[41]);
	LPVOID endSceneVFunc = reinterpret_cast<LPVOID>(vTable[42]);
	LPVOID createTextureVFunc = reinterpret_cast<LPVOID>(vTable[23]);
	LPVOID clearVFunc = reinterpret_cast<LPVOID>(vTable[43]);
	LPVOID presentVFunc = reinterpret_cast<LPVOID>(vTable[17]);

	hkPresent.createHook(presentVFunc, &dPresent);
	hkBeginScene.createHook(beginSceneVFunc, &dBeginScene);
	hkCreateTexture.createHook(createTextureVFunc, &dCreateTexture);

	pDevice->Release();
	pD3D->Release();
}

int Hooks::initSourceHooks()
{
	LPVOID pGetRenderTargetVFunc = (LPVOID)(mGame->g_materialSystem + offsets::GetRenderTarget);
	hkGetRenderTarget.createHook(pGetRenderTargetVFunc, &dGetRenderTarget);

	LPVOID pRenderViewVFunc = (LPVOID)(mGame->g_client + offsets::RenderView);
	hkRenderView.createHook(pRenderViewVFunc, &dRenderView);

	void **vTable = *reinterpret_cast<void ***>(mGame->MaterialSystem);
	LPVOID pEndFrameVFunc = reinterpret_cast<LPVOID>(vTable[42]);
	hkEndFrame.createHook(pEndFrameVFunc, &dEndFrame);

	LPVOID calcViewModelViewAddr = (LPVOID)(mGame->g_client + offsets::CalcViewModelView);
	hkCalcViewModelView.createHook(calcViewModelViewAddr, &dCalcViewModelView);

	LPVOID serverFireTerrorBulletsAddr = (LPVOID)(mGame->g_server + offsets::ServerFireTerrorBullets);
	hkServerFireTerrorBullets.createHook(serverFireTerrorBulletsAddr, &dServerFireTerrorBullets);

	LPVOID clientFireTerrorBulletsAddr = (LPVOID)(mGame->g_client + offsets::ClientFireTerrorBullets);
	hkClientFireTerrorBullets.createHook(clientFireTerrorBulletsAddr, &dClientFireTerrorBullets);

	LPVOID ProcessUsercmdsAddr = (LPVOID)(mGame->g_server + offsets::ProcessUsercmds);
	hkProcessUsercmds.createHook(ProcessUsercmdsAddr, &dProcessUsercmds);

	LPVOID ReadUserCmdAddr = (LPVOID)(mGame->g_server + offsets::ReadUserCmd);
	hkReadUsercmd.createHook(ReadUserCmdAddr, &dReadUsercmd);

	LPVOID WriteUsercmdDeltaToBufferAddr = (LPVOID)(mGame->g_client + offsets::WriteUsercmdDeltaToBuffer);
	hkWriteUsercmdDeltaToBuffer.createHook(WriteUsercmdDeltaToBufferAddr, &dWriteUsercmdDeltaToBuffer);

	LPVOID WriteUsercmdAddr = (LPVOID)(mGame->g_client + offsets::WriteUsercmd);
	hkWriteUsercmd.createHook(WriteUsercmdAddr, &dWriteUsercmd);

	return 1;
}

HRESULT __stdcall Hooks::dBeginScene(IDirect3DDevice9 *pDevice)
{
	return hkBeginScene.fOriginal(pDevice);
}

HRESULT __stdcall Hooks::dClear(IDirect3DDevice9 *pDevice, DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	return hkClear.fOriginal(pDevice, Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT __stdcall Hooks::dEndScene(IDirect3DDevice9 *pDevice)
{
	return hkEndScene.fOriginal(pDevice);
}

HRESULT APIENTRY Hooks::dCreateTexture(IDirect3DDevice9 *pDevice, UINT w, UINT h, UINT levels, DWORD usage, D3DFORMAT format, D3DPOOL pool, IDirect3DTexture9 **tex, HANDLE *shared_handle) {


	HRESULT hr = pDevice->QueryInterface(__uuidof(IDirect3DDevice9On12), (void **)&iD9on12);

	ID3D12Device *d12Device;
	iD9on12->GetD3D12Device(__uuidof(ID3D12Device), (void **)&d12Device);

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	d12Device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void **)&commandQueue);

	HRESULT result = hkCreateTexture.fOriginal(pDevice, w, h, levels, usage, format, pool, tex, shared_handle);

	if (!mVR->d9LeftEyeTexture)
	{
		mVR->d9LeftEyeTexture = *tex;
	}
	else if (!mVR->d9RightEyeTexture)
	{
		mVR->d9RightEyeTexture = *tex;
	}

	return result;
}

HRESULT __stdcall Hooks::dPresent(IDirect3DDevice9 *pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	if (!mCreatedTexture && mVR->isVREnabled)
	{
		IMatRenderContext *rndrContext = mGame->MaterialSystem->GetRenderContext();
		rndrContext->GetRenderTargetDimensions(mVR->frameBufferWidth, mVR->frameBufferHeight);

		hkCreateTexture.enableHook();

		bool *m_GameRunning = (bool *)((uintptr_t)mGame->MaterialSystem + offsets::isGameRunning);
		*m_GameRunning = false;
		mGame->MaterialSystem->BeginRenderTargetAllocation();
		*m_GameRunning = true;

		uint32_t recommendedWidth, recommendedHeight;
		recommendedWidth = mVR->frameBufferWidth / 2;
		recommendedHeight = mVR->frameBufferHeight;

		mVR->leftEyeTexture = mGame->MaterialSystem->CreateNamedRenderTargetTextureEx("leftEye", recommendedWidth, recommendedHeight, RT_SIZE_LITERAL, mGame->MaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED, TEXTUREFLAGS_NOMIP);
		mVR->rightEyeTexture = mGame->MaterialSystem->CreateNamedRenderTargetTextureEx("rightEye", recommendedWidth, recommendedHeight, RT_SIZE_LITERAL, mGame->MaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED, TEXTUREFLAGS_NOMIP);
		*m_GameRunning = false;
		//MaterialSystem->EndRenderTargetAllocation(); // Freezes game and leaks memory
		*m_GameRunning = true;

		hkCreateTexture.disableHook();
		mCreatedTexture = true;
	}
	if (mVR->isVREnabled)
	{
		// Prevents crashing at menu
		if (!mGame->EngineClient->IsInGame())
		{
			IMatRenderContext *rndrContext = mGame->MaterialSystem->GetRenderContext();
			rndrContext->SetRenderTarget(NULL);
		}


		ID3D12Resource *finalD12Res;
		ID3D12Resource *finalD12Res2;
		iD9on12->UnwrapUnderlyingResource(mVR->d9LeftEyeTexture, commandQueue, __uuidof(ID3D12Resource), (void **)&finalD12Res);


		vr::VRTextureBounds_t bounds;
		bounds.uMin = 0.0f;
		bounds.uMax = 1.0f;
		bounds.vMin = 0.0f;
		bounds.vMax = 1.0f;

		vr::TrackedDevicePose_t *m_rTrackedDevicePose = nullptr;

		vr::D3D12TextureData_t d3d12LeftEyeTexture = { finalD12Res, commandQueue, 0 };
		vr::Texture_t leftEyeTexture = { (void *)&d3d12LeftEyeTexture, vr::TextureType_DirectX12, vr::ColorSpace_Gamma };

		bounds.uMin = 0.0f + mVR->g_horizontalOffsetLeft * 0.25f;
		bounds.uMax = 1.0f + mVR->g_horizontalOffsetLeft * 0.25f;
		bounds.vMin = 0.0f - mVR->g_verticalOffsetLeft * 0.5f;
		bounds.vMax = 1.0f - mVR->g_verticalOffsetLeft * 0.5f;
		vr::EVRCompositorError compResult1 = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture, &(mVR->m_sTextureBounds)[0], vr::Submit_Default);

		iD9on12->ReturnUnderlyingResource(mVR->d9LeftEyeTexture, 0, nullptr, nullptr); 


		iD9on12->UnwrapUnderlyingResource(mVR->d9RightEyeTexture, commandQueue, __uuidof(ID3D12Resource), (void **)&finalD12Res2);
		vr::D3D12TextureData_t d3d12RightEyeTexture = { finalD12Res2, commandQueue, 0 };
		vr::Texture_t rightEyeTexture = { (void *)&d3d12RightEyeTexture, vr::TextureType_DirectX12, vr::ColorSpace_Gamma };

		bounds.uMin = 0.0f + mVR->g_horizontalOffsetRight * 0.25f;
		bounds.uMax = 1.0f + mVR->g_horizontalOffsetRight * 0.25f;
		bounds.vMin = 0.0f - mVR->g_verticalOffsetRight * 0.5f;
		bounds.vMax = 1.0f - mVR->g_verticalOffsetRight * 0.5f;
		vr::EVRCompositorError compResult2 = vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture, &(mVR->m_sTextureBounds)[1], vr::Submit_Default);

		iD9on12->ReturnUnderlyingResource(mVR->d9RightEyeTexture, 0, nullptr, nullptr);
	}

	if (mVR->isInitialized)
	{
		mVR->UpdatePosesAndActions();
		mVR->UpdateTracking(mVR->setupOrigin);
	}
	
	return hkPresent.fOriginal(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

ITexture *__fastcall Hooks::dGetRenderTarget(void *ecx, void *edx)
{
	ITexture *result = hkGetRenderTarget.fOriginal(ecx);
	return result;
}

void __fastcall Hooks::dRenderView(void *ecx, void *edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw)
{
	IMatRenderContext *rndrContext = mGame->MaterialSystem->GetRenderContext();

	int clearflags = 0;

	CViewSetup leftEyeView = setup;
	CViewSetup rightEyeView = setup;

	int width_VR = mVR->frameBufferWidth / 2;
	int height_VR = mVR->frameBufferHeight;

	// Left eye CViewSetup
	leftEyeView.x = 0;
	leftEyeView.width = width_VR;
	leftEyeView.height = height_VR;
	leftEyeView.fov = mVR->m_nFov;
	leftEyeView.fovViewmodel = mVR->m_nFov;
	leftEyeView.m_flAspectRatio = mVR->m_nAspect;
	leftEyeView.zNear = 6;
	leftEyeView.zNearViewmodel = 6;
	leftEyeView.origin = mVR->GetViewOriginLeft();
	leftEyeView.angles = mVR->GetViewAngle();

	mVR->setupOrigin = setup.origin;

	Vector hmdAngle = mVR->GetViewAngle();
	QAngle inGameAngle(hmdAngle.x, hmdAngle.y, hmdAngle.z);
	mGame->EngineClient->SetViewAngles(inGameAngle);

	rndrContext->SetRenderTarget(mVR->leftEyeTexture);
	hkRenderView.fOriginal(ecx, leftEyeView, hudViewSetup, clearflags, whatToDraw);

	// Right eye CViewSetup
	rightEyeView.x = 0;
	rightEyeView.width = width_VR;
	rightEyeView.height = height_VR;
	rightEyeView.fov = mVR->m_nFov;
	rightEyeView.fovViewmodel = mVR->m_nFov;
	rightEyeView.m_flAspectRatio = mVR->m_nAspect;
	rightEyeView.zNear = 6;
	rightEyeView.zNearViewmodel = 6;
	rightEyeView.origin = mVR->GetViewOriginRight();
	rightEyeView.angles = mVR->GetViewAngle();

	rndrContext->SetRenderTarget(mVR->rightEyeTexture);
	hkRenderView.fOriginal(ecx, rightEyeView, hudViewSetup, clearflags, whatToDraw);
}

bool __fastcall Hooks::dCreateMove(void *ecx, void *edx, float flInputSampleTime, CUserCmd *cmd)
{
	return hkCreateMove.fOriginal(ecx, flInputSampleTime, cmd);
}

void __fastcall Hooks::dEndFrame(void *ecx, void *edx)
{
	return hkEndFrame.fOriginal(ecx);
}

void __fastcall Hooks::dCalcViewModelView(void *ecx, void *edx, void *owner, const Vector &eyePosition, const QAngle &eyeAngles)
{
	Vector vecNewOrigin = eyePosition;
	QAngle vecNewAngles = eyeAngles;

	if (mVR->isVREnabled)
	{
		vecNewOrigin = mVR->GetRecommendedViewmodelAbsPos();
		vecNewAngles = mVR->GetRecommendedViewmodelAbsAngle();
	}

	return hkCalcViewModelView.fOriginal(ecx, owner, vecNewOrigin, vecNewAngles);
}

int Hooks::dServerFireTerrorBullets(int playerId, const Vector &vecOrigin, const QAngle &vecAngles, int a4, int a5, int a6, float a7)
{
	Vector vecNewOrigin = vecOrigin;
	QAngle vecNewAngles = vecAngles;

	// Server host
	if (mVR->isVREnabled && playerId == mGame->EngineClient->GetLocalPlayer())
	{
		vecNewOrigin = mVR->GetRecommendedViewmodelAbsPos();
		vecNewAngles = mVR->GetRecommendedViewmodelAbsAngle();
	}
	// Clients
	else if (mGame->playersVRInfo[playerId].isUsingVR)
	{
		vecNewAngles = mGame->playersVRInfo[playerId].controllerAngles;
	}

	return hkServerFireTerrorBullets.fOriginal(playerId, vecNewOrigin, vecNewAngles, a4, a5, a6, a7);
}

int Hooks::dClientFireTerrorBullets(int playerId, const Vector &vecOrigin, const QAngle &vecAngles, int a4, int a5, int a6, float a7)
{
	Vector vecNewOrigin = vecOrigin;
	QAngle vecNewAngles = vecAngles;
	
	if (mVR->isVREnabled && playerId == mGame->EngineClient->GetLocalPlayer())
	{
		vecNewOrigin = mVR->GetRecommendedViewmodelAbsPos();
		vecNewAngles = mVR->GetRecommendedViewmodelAbsAngle();
	}

	return hkClientFireTerrorBullets.fOriginal(playerId, vecNewOrigin, vecNewAngles, a4, a5, a6, a7);
}

float __fastcall Hooks::dProcessUsercmds(void *ecx, void *edx, edict_t *player, void *buf, int numcmds, int totalcmds, int dropped_packets, bool ignore, bool paused)
{
	// Function pointer for CBaseEntity::entindex
	typedef int(__thiscall *tEntindex)(void *thisptr);
	tEntindex oEntindex = (tEntindex)(mGame->g_server + offsets::CBaseEntity_entindex);

	IServerUnknown * pUnknown = player->m_pUnk;
	CBasePlayer *pPlayer = (CBasePlayer*)pUnknown->GetBaseEntity();

	int index = oEntindex(pPlayer);
	mGame->currentUsercmdID = index;

	return hkProcessUsercmds.fOriginal(ecx, player, buf, numcmds, totalcmds, dropped_packets, ignore, paused);
}

int Hooks::dReadUsercmd(void *buf, CUserCmd *move, CUserCmd *from)
{
	hkReadUsercmd.fOriginal(buf, move, from);

	int i = mGame->currentUsercmdID;
	if (move->viewangles.z == -1.0) // Signal for VR CUserCmd
	{
		mGame->playersVRInfo[i].isUsingVR = true;
		mGame->playersVRInfo[i].controllerAngles.x = (float)move->mousedx / 10;
		mGame->playersVRInfo[i].controllerAngles.y = (float)move->mousedy / 10;
	}
	else
	{
		mGame->playersVRInfo[i].isUsingVR = false;
	}
	return 1;
}

void __fastcall Hooks::dWriteUsercmdDeltaToBuffer(void *ecx, void *edx, int a1, void *buf, int from, int to, bool isnewcommand) 
{
	return hkWriteUsercmdDeltaToBuffer.fOriginal(ecx, a1, buf, from, to, isnewcommand);
}

int Hooks::dWriteUsercmd(void *buf, CUserCmd *to, CUserCmd *from)
{
	if (mVR->isVREnabled)
	{
		CInput *g_pInput = *(CInput **)(mGame->g_client + offsets::g_ppInput);
		CVerifiedUserCmd *pVerifiedCommands = *(CVerifiedUserCmd **)((uintptr_t)g_pInput + 0xF0);
		CVerifiedUserCmd *pVerified = &pVerifiedCommands[(to->command_number) % 150];

		// Signal to the server that this CUserCmd has VR info
		to->viewangles.z = -1;

		QAngle controllerAngles = mVR->GetRecommendedViewmodelAbsAngle();
		to->mousedx = controllerAngles.x * 10; // Strip off 2nd decimal to save bits.
		to->mousedy = controllerAngles.y * 10;

		// Must recalculate checksum for the edited CUserCmd or gunshots will sound
		// terrible in multiplayer.
		pVerified->m_cmd = *to;
		pVerified->m_crc = to->GetChecksum();
	}
	return hkWriteUsercmd.fOriginal(buf, to, from);
}