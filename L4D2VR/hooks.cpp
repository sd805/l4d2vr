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

	initSourceHooks();

	hkGetRenderTarget.enableHook();
	hkCalcViewModelView.enableHook();
	hkServerFireTerrorBullets.enableHook();
	hkClientFireTerrorBullets.enableHook();
	hkProcessUsercmds.enableHook();
	hkReadUsercmd.enableHook();
	hkWriteUsercmdDeltaToBuffer.enableHook();
	hkWriteUsercmd.enableHook();
	hkAdjustEngineViewport.enableHook();
	hkViewport.enableHook();
	hkGetViewport.enableHook();
}

Hooks::~Hooks()
{
	if (MH_Uninitialize() != MH_OK)
	{
		Game::errorMsg("Failed to uninitialize MinHook");
	}
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

	LPVOID AdjustEngineViewportAddr = (LPVOID)(m_Game->m_Offsets->AdjustEngineViewport.address);
	hkAdjustEngineViewport.createHook(AdjustEngineViewportAddr, &dAdjustEngineViewport);

	LPVOID ViewportAddr = (LPVOID)(m_Game->m_Offsets->Viewport.address);
	hkViewport.createHook(ViewportAddr, &dViewport);

	LPVOID GetViewportAddr = (LPVOID)(m_Game->m_Offsets->GetViewport.address);
	hkGetViewport.createHook(GetViewportAddr, &dGetViewport);

	return 1;
}


ITexture *__fastcall Hooks::dGetRenderTarget(void *ecx, void *edx)
{
	ITexture *result = hkGetRenderTarget.fOriginal(ecx);
	return result;
}

void __fastcall Hooks::dRenderView(void *ecx, void *edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw)
{
	IMatRenderContext *rndrContext = m_Game->m_MaterialSystem->GetRenderContext();

	CViewSetup leftEyeView = setup;
	CViewSetup rightEyeView = setup;

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

		if (m_VR->m_RoomscaleActive)
		{
			Vector setupOriginToHMD = m_VR->m_SetupOriginToHMD;
			setupOriginToHMD.z = 0;
			float distance = VectorLength(setupOriginToHMD);
			if (distance > 1)
			{
				float forwardSpeed = DotProduct2D(setupOriginToHMD, m_VR->m_HmdForward);
				float sideSpeed = DotProduct2D(setupOriginToHMD, m_VR->m_HmdRight);
				to->forwardmove += distance * forwardSpeed;
				to->sidemove += distance * sideSpeed;
			}
		}

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

void Hooks::dAdjustEngineViewport(int &x, int &y, int &width, int &height)
{
	hkAdjustEngineViewport.fOriginal(x, y, width, height);
}

void Hooks::dViewport(void *ecx, void *edx, int x, int y, int width, int height)
{
	
	//hkViewport.fOriginal(ecx, x, y, 1800, 1800);
	hkViewport.fOriginal(ecx, x, y, width, height);
}

void Hooks::dGetViewport(void *ecx, void *edx, int &x, int &y, int &width, int &height)
{
	//int newWidth = 1000;
	//int newHeight = 1000;
	hkGetViewport.fOriginal(ecx, x, y, width, height);
}