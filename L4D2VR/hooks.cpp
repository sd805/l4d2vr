#include "hooks.h"
#include "game.h"
#include "texture.h"
#include "sdk.h"
#include "vr.h"
#include "offsets.h"
#include <iostream>

Hooks::Hooks(Game *game)
{
	if (MH_Initialize() != MH_OK)
	{
		Game::errorMsg("Failed to init MinHook");
	}

	m_Game = game;
	m_VR = m_Game->m_VR;

	//initDxHooks();
	initSourceHooks();

	//hkBeginScene.enableHook();
	//hkPresent.enableHook();
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
	LPVOID pGetRenderTargetVFunc = (LPVOID)(m_Game->m_Offsets->GetRenderTarget.address);
	hkGetRenderTarget.createHook(pGetRenderTargetVFunc, &dGetRenderTarget);

	LPVOID pRenderViewVFunc = (LPVOID)(m_Game->m_Offsets->RenderView.address);
	hkRenderView.createHook(pRenderViewVFunc, &dRenderView);

	void **vTable = *reinterpret_cast<void ***>(m_Game->m_MaterialSystem);
	LPVOID pEndFrameVFunc = reinterpret_cast<LPVOID>(vTable[42]);
	hkEndFrame.createHook(pEndFrameVFunc, &dEndFrame);

	LPVOID calcViewModelViewAddr = (LPVOID)(m_Game->m_Offsets->CalcViewModelView.address);
	hkCalcViewModelView.createHook(calcViewModelViewAddr, &dCalcViewModelView);

	LPVOID serverFireTerrorBulletsAddr = (LPVOID)(m_Game->m_Offsets->ServerFireTerrorBullets.address);
	hkServerFireTerrorBullets.createHook(serverFireTerrorBulletsAddr, &dServerFireTerrorBullets);

	LPVOID clientFireTerrorBulletsAddr = (LPVOID)(m_Game->m_Offsets->ClientFireTerrorBullets.address);
	hkClientFireTerrorBullets.createHook(clientFireTerrorBulletsAddr, &dClientFireTerrorBullets);

	LPVOID ProcessUsercmdsAddr = (LPVOID)(m_Game->m_Offsets->ProcessUsercmds.address);
	hkProcessUsercmds.createHook(ProcessUsercmdsAddr, &dProcessUsercmds);

	LPVOID ReadUserCmdAddr = (LPVOID)(m_Game->m_Offsets->ReadUserCmd.address);
	hkReadUsercmd.createHook(ReadUserCmdAddr, &dReadUsercmd);

	LPVOID WriteUsercmdDeltaToBufferAddr = (LPVOID)(m_Game->m_Offsets->WriteUsercmdDeltaToBuffer.address);
	hkWriteUsercmdDeltaToBuffer.createHook(WriteUsercmdDeltaToBufferAddr, &dWriteUsercmdDeltaToBuffer);

	LPVOID WriteUsercmdAddr = (LPVOID)(m_Game->m_Offsets->WriteUsercmd.address);
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

HRESULT APIENTRY Hooks::dCreateTexture(IDirect3DDevice9 *pDevice, UINT w, UINT h, UINT levels, DWORD usage, D3DFORMAT format, D3DPOOL pool, IDirect3DTexture9 **tex, HANDLE *shared_handle) 
{
	HRESULT hr = pDevice->QueryInterface(__uuidof(IDirect3DDevice9On12), (void **)&m_D9on12);

	ID3D12Device *d12Device;
	m_D9on12->GetD3D12Device(__uuidof(ID3D12Device), (void **)&d12Device);

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	d12Device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void **)&m_CommandQueue);

	HRESULT result = hkCreateTexture.fOriginal(pDevice, w, h, levels, usage, format, pool, tex, shared_handle);

	if (!m_VR->m_D9LeftEyeTexture)
	{
		m_VR->m_D9LeftEyeTexture = *tex;
	}
	else if (!m_VR->m_D9RightEyeTexture)
	{
		m_VR->m_D9RightEyeTexture = *tex;
	}

	return result;
}

HRESULT __stdcall Hooks::dPresent(IDirect3DDevice9 *pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	if (!m_CreatedTexture && m_VR->m_IsVREnabled)
	{
		CreateVRTextures();
	}

	if (m_VR->m_IsVREnabled)
	{
		// Prevents crashing at menu
		if (!m_Game->m_EngineClient->IsInGame())
		{
			IMatRenderContext *rndrContext = m_Game->m_MaterialSystem->GetRenderContext();
			rndrContext->SetRenderTarget(NULL);
		}
		SubmitVRTextures();
	}

	if (m_VR->m_IsInitialized)
	{
		m_VR->UpdatePosesAndActions();
		m_VR->UpdateTracking(m_VR->m_SetupOrigin);
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
	IMatRenderContext *rndrContext = m_Game->m_MaterialSystem->GetRenderContext();

	int clearflags = 0;

	CViewSetup leftEyeView = setup;
	CViewSetup rightEyeView = setup;

	int width_VR = m_VR->m_FrameBufferWidth / 2;
	int height_VR = m_VR->m_FrameBufferHeight;

	// Left eye CViewSetup
	leftEyeView.x = 0;
	leftEyeView.width = m_VR->m_RenderWidth;
	leftEyeView.height = m_VR->m_RenderHeight;
	leftEyeView.fov = m_VR->m_Fov;
	leftEyeView.fovViewmodel = m_VR->m_Fov;
	leftEyeView.m_flAspectRatio = m_VR->m_Aspect;
	leftEyeView.zNear = 6;
	leftEyeView.zNearViewmodel = 6;
	leftEyeView.origin = m_VR->GetViewOriginLeft();
	leftEyeView.angles = m_VR->GetViewAngle();

	m_VR->m_SetupOrigin = setup.origin;

	Vector hmdAngle = m_VR->GetViewAngle();
	QAngle inGameAngle(hmdAngle.x, hmdAngle.y, hmdAngle.z);
	m_Game->m_EngineClient->SetViewAngles(inGameAngle);

	rndrContext->SetRenderTarget(m_VR->m_LeftEyeTexture);
	hkRenderView.fOriginal(ecx, leftEyeView, hudViewSetup, nClearFlags, whatToDraw);

	// Right eye CViewSetup
	rightEyeView.x = 0;
	rightEyeView.width = m_VR->m_RenderWidth;
	rightEyeView.height = m_VR->m_RenderHeight;
	rightEyeView.fov = m_VR->m_Fov;
	rightEyeView.fovViewmodel = m_VR->m_Fov;
	rightEyeView.m_flAspectRatio = m_VR->m_Aspect;
	rightEyeView.zNear = 6;
	rightEyeView.zNearViewmodel = 6;
	rightEyeView.origin = m_VR->GetViewOriginRight();
	rightEyeView.angles = m_VR->GetViewAngle();

	rndrContext->SetRenderTarget(m_VR->m_RightEyeTexture);
	hkRenderView.fOriginal(ecx, rightEyeView, hudViewSetup, nClearFlags, whatToDraw);

	m_VR->m_RenderedNewFrame = true;
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

	if (m_VR->m_IsVREnabled)
	{
		vecNewOrigin = m_VR->GetRecommendedViewmodelAbsPos();
		vecNewAngles = m_VR->GetRecommendedViewmodelAbsAngle();
	}

	return hkCalcViewModelView.fOriginal(ecx, owner, vecNewOrigin, vecNewAngles);
}

int Hooks::dServerFireTerrorBullets(int playerId, const Vector &vecOrigin, const QAngle &vecAngles, int a4, int a5, int a6, float a7)
{
	Vector vecNewOrigin = vecOrigin;
	QAngle vecNewAngles = vecAngles;

	// Server host
	if (m_VR->m_IsVREnabled && playerId == m_Game->m_EngineClient->GetLocalPlayer())
	{
		vecNewOrigin = m_VR->GetRecommendedViewmodelAbsPos();
		vecNewAngles = m_VR->GetRecommendedViewmodelAbsAngle();
	}
	// Clients
	else if (m_Game->m_PlayersVRInfo[playerId].isUsingVR)
	{
		vecNewOrigin = m_Game->m_PlayersVRInfo[playerId].controllerPos;
		vecNewAngles = m_Game->m_PlayersVRInfo[playerId].controllerAngle;
	}

	return hkServerFireTerrorBullets.fOriginal(playerId, vecNewOrigin, vecNewAngles, a4, a5, a6, a7);
}

int Hooks::dClientFireTerrorBullets(int playerId, const Vector &vecOrigin, const QAngle &vecAngles, int a4, int a5, int a6, float a7)
{
	Vector vecNewOrigin = vecOrigin;
	QAngle vecNewAngles = vecAngles;
	
	if (m_VR->m_IsVREnabled && playerId == m_Game->m_EngineClient->GetLocalPlayer())
	{
		vecNewOrigin = m_VR->GetRecommendedViewmodelAbsPos();
		vecNewAngles = m_VR->GetRecommendedViewmodelAbsAngle();
	}

	return hkClientFireTerrorBullets.fOriginal(playerId, vecNewOrigin, vecNewAngles, a4, a5, a6, a7);
}

float __fastcall Hooks::dProcessUsercmds(void *ecx, void *edx, edict_t *player, void *buf, int numcmds, int totalcmds, int dropped_packets, bool ignore, bool paused)
{
	// Function pointer for CBaseEntity::entindex
	typedef int(__thiscall *tEntindex)(void *thisptr);
	tEntindex oEntindex = (tEntindex)(m_Game->m_Offsets->CBaseEntity_entindex.address);

	IServerUnknown * pUnknown = player->m_pUnk;
	CBasePlayer *pPlayer = (CBasePlayer*)pUnknown->GetBaseEntity();

	int index = oEntindex(pPlayer);
	m_Game->m_CurrentUsercmdID = index;

	return hkProcessUsercmds.fOriginal(ecx, player, buf, numcmds, totalcmds, dropped_packets, ignore, paused);
}

int Hooks::dReadUsercmd(void *buf, CUserCmd *move, CUserCmd *from)
{
	hkReadUsercmd.fOriginal(buf, move, from);

	int i = m_Game->m_CurrentUsercmdID;
	if (move->tick_count < 0) // Signal for VR CUserCmd
	{
		move->tick_count *= -1;

		m_Game->m_PlayersVRInfo[i].isUsingVR = true;
		m_Game->m_PlayersVRInfo[i].controllerAngle.x = (float)move->mousedx / 10;
		m_Game->m_PlayersVRInfo[i].controllerAngle.y = (float)move->mousedy / 10;
		m_Game->m_PlayersVRInfo[i].controllerPos.x = move->viewangles.z;
		m_Game->m_PlayersVRInfo[i].controllerPos.y = move->upmove;

		// Decode viewangles.x
		int decodedZInt = (move->viewangles.x / 10000);
		float decodedAngle = abs((float)(move->viewangles.x - (decodedZInt * 10000)) / 10);
		decodedAngle -= 360;
		float decodedZ = (float)decodedZInt / 10;

		m_Game->m_PlayersVRInfo[i].controllerPos.z = decodedZ;

		move->viewangles.x = decodedAngle;
		move->viewangles.z = 0;
		move->upmove = 0;
	}
	else
	{
		m_Game->m_PlayersVRInfo[i].isUsingVR = false;
	}
	return 1;
}

void __fastcall Hooks::dWriteUsercmdDeltaToBuffer(void *ecx, void *edx, int a1, void *buf, int from, int to, bool isnewcommand) 
{
	return hkWriteUsercmdDeltaToBuffer.fOriginal(ecx, a1, buf, from, to, isnewcommand);
}

int Hooks::dWriteUsercmd(void *buf, CUserCmd *to, CUserCmd *from)
{
	if (m_VR->m_IsVREnabled)
	{
		CInput *m_Input = **(CInput ***)(m_Game->m_Offsets->g_pppInput.address);
		CVerifiedUserCmd *pVerifiedCommands = *(CVerifiedUserCmd **)((uintptr_t)m_Input + 0xF0);
		CVerifiedUserCmd *pVerified = &pVerifiedCommands[(to->command_number) % 150];

		// Signal to the server that this CUserCmd has VR info
		to->tick_count *= -1;

		QAngle controllerAngles = m_VR->GetRecommendedViewmodelAbsAngle();
		to->mousedx = controllerAngles.x * 10; // Strip off 2nd decimal to save bits.
		to->mousedy = controllerAngles.y * 10;

		Vector controllerPos = m_VR->GetRecommendedViewmodelAbsPos();
		to->viewangles.z = controllerPos.x;
		to->upmove = controllerPos.y;

		// Space in CUserCmd is tight, so encode viewangle.x and controllerPos.z together.
		// Encoding will overflow if controllerPos.z goes beyond +-21474.8
		float xAngle = to->viewangles.x;
		int encodedAngle = (xAngle + 360) * 10;
		int encoding = (int)(controllerPos.z * 10) * 10000;
		encoding += encoding < 0 ? -encodedAngle : encodedAngle;
		to->viewangles.x = encoding;

		hkWriteUsercmd.fOriginal(buf, to, from);

		to->viewangles.x = xAngle;
		to->tick_count *= -1;
		to->viewangles.z = 0;
		to->upmove = 0;

		// Must recalculate checksum for the edited CUserCmd or gunshots will sound
		// terrible in multiplayer.
		pVerified->m_cmd = *to;
		pVerified->m_crc = to->GetChecksum();
		return 1;

	}
	return hkWriteUsercmd.fOriginal(buf, to, from);
}

void Hooks::CreateVRTextures()
{
	IMatRenderContext *rndrContext = m_Game->m_MaterialSystem->GetRenderContext();
	rndrContext->GetRenderTargetDimensions(m_VR->m_FrameBufferWidth, m_VR->m_FrameBufferHeight);

	hkCreateTexture.enableHook();

	m_Game->m_MaterialSystem->isGameRunning = false;
	m_Game->m_MaterialSystem->BeginRenderTargetAllocation();
	m_Game->m_MaterialSystem->isGameRunning = true;

	uint32_t recommendedWidth, recommendedHeight;
	recommendedWidth = m_VR->m_FrameBufferWidth / 2;
	recommendedHeight = m_VR->m_FrameBufferHeight;

	m_VR->m_LeftEyeTexture = m_Game->m_MaterialSystem->CreateNamedRenderTargetTextureEx("leftEye", recommendedWidth, recommendedHeight, RT_SIZE_LITERAL, m_Game->m_MaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED, TEXTUREFLAGS_NOMIP);
	m_VR->m_RightEyeTexture = m_Game->m_MaterialSystem->CreateNamedRenderTargetTextureEx("rightEye", recommendedWidth, recommendedHeight, RT_SIZE_LITERAL, m_Game->m_MaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED, TEXTUREFLAGS_NOMIP);
	m_Game->m_MaterialSystem->isGameRunning = false;
	//MaterialSystem->EndRenderTargetAllocation(); // Freezes game and leaks memory
	m_Game->m_MaterialSystem->isGameRunning = true;

	hkCreateTexture.disableHook();
	m_CreatedTexture = true;
}

void Hooks::SubmitVRTextures()
{
	ID3D12Resource *leftEyeD12Res;
	ID3D12Resource *rightEyeD12Res;
	m_D9on12->UnwrapUnderlyingResource(m_VR->m_D9LeftEyeTexture, m_CommandQueue, __uuidof(ID3D12Resource), (void **)&leftEyeD12Res);


	vr::VRTextureBounds_t bounds;
	bounds.uMin = 0.0f;
	bounds.uMax = 1.0f;
	bounds.vMin = 0.0f;
	bounds.vMax = 1.0f;

	vr::D3D12TextureData_t d3d12LeftEyeTexture = { leftEyeD12Res, m_CommandQueue, 0 };
	vr::Texture_t leftEyeTexture = { (void *)&d3d12LeftEyeTexture, vr::TextureType_DirectX12, vr::ColorSpace_Gamma };

	bounds.uMin = 0.0f + m_VR->m_HorizontalOffsetLeft * 0.25f;
	bounds.uMax = 1.0f + m_VR->m_HorizontalOffsetLeft * 0.25f;
	bounds.vMin = 0.0f - m_VR->m_VerticalOffsetLeft * 0.5f;
	bounds.vMax = 1.0f - m_VR->m_VerticalOffsetLeft * 0.5f;
	vr::EVRCompositorError leftEyeError = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture, &(m_VR->m_TextureBounds)[0], vr::Submit_Default);

	m_D9on12->ReturnUnderlyingResource(m_VR->m_D9LeftEyeTexture, 0, nullptr, nullptr);


	m_D9on12->UnwrapUnderlyingResource(m_VR->m_D9RightEyeTexture, m_CommandQueue, __uuidof(ID3D12Resource), (void **)&rightEyeD12Res);
	vr::D3D12TextureData_t d3d12RightEyeTexture = { rightEyeD12Res, m_CommandQueue, 0 };
	vr::Texture_t rightEyeTexture = { (void *)&d3d12RightEyeTexture, vr::TextureType_DirectX12, vr::ColorSpace_Gamma };

	bounds.uMin = 0.0f + m_VR->m_HorizontalOffsetRight * 0.25f;
	bounds.uMax = 1.0f + m_VR->m_HorizontalOffsetRight * 0.25f;
	bounds.vMin = 0.0f - m_VR->m_VerticalOffsetRight * 0.5f;
	bounds.vMax = 1.0f - m_VR->m_VerticalOffsetRight * 0.5f;
	vr::EVRCompositorError rightEyeError = vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture, &(m_VR->m_TextureBounds)[1], vr::Submit_Default);

	m_D9on12->ReturnUnderlyingResource(m_VR->m_D9RightEyeTexture, 0, nullptr, nullptr);
}