#pragma once
#include "openvr.h"
#include "vector.h"
#include <chrono>

#define MAX_STR_LEN 256

class Game;
class IDirect3DTexture9;
class IDirect3DSurface9;
class ITexture;


struct TrackedDevicePoseData {
	std::string TrackedDeviceName;
	Vector TrackedDevicePos;
	Vector TrackedDeviceVel;
	QAngle TrackedDeviceAng;
	QAngle TrackedDeviceAngVel;
};

struct SharedTextureHolder {
	vr::VRVulkanTextureData_t m_VulkanData;
	vr::Texture_t m_VRTexture;
};


class VR
{
public:
	Game *m_Game = nullptr;

	vr::IVRSystem *m_System = nullptr;
	vr::IVRInput *m_Input = nullptr;

	float m_HorizontalOffsetLeft;
	float m_VerticalOffsetLeft;
	float m_HorizontalOffsetRight;
	float m_VerticalOffsetRight;

	uint32_t m_RenderWidth;
	uint32_t m_RenderHeight;
	float m_Aspect;
	float m_Fov;

	vr::VRTextureBounds_t m_TextureBounds[2];
	vr::TrackedDevicePose_t m_Poses[vr::k_unMaxTrackedDeviceCount];

	Vector m_EyeToHeadTransformPosLeft = { 0,0,0 };
	Vector m_EyeToHeadTransformPosRight = { 0,0,0 };

	Vector m_HmdForward;
	Vector m_HmdRight;
	Vector m_HmdUp;

	Vector m_HmdPosLocalInWorld = { 0,0,0 };

	Vector m_LeftControllerForward;
	Vector m_LeftControllerRight;
	Vector m_LeftControllerUp;

	Vector m_RightControllerForward;
	Vector m_RightControllerRight;
	Vector m_RightControllerUp;
	Vector m_RightControllerForwardCorrected;
	Vector m_RightControllerUpCorrected;

	Vector m_HmdPosAbs = { 0,0,0 };
	Vector m_HmdPosAbsNoOffset = { 0,0,0 };
	QAngle m_HmdAngAbs;

	Vector m_HmdPosCorrectedPrev = { 0,0,0 };
	Vector m_HmdPosLocalPrev = { 0,0,0 };

	Vector m_LeftControllerPosAbs;											
	QAngle m_LeftControllerAngAbs;
	Vector m_RightControllerPosAbs;											
	QAngle m_RightControllerAngAbs;

	float m_Ipd;																	
	float m_EyeZ;

	Vector m_IntendedPositionOffset = { 0,0,0 };

	ITexture *m_LeftEyeTexture;
	ITexture *m_RightEyeTexture;

	IDirect3DSurface9 *m_D9LeftEyeSurface;
	IDirect3DSurface9 *m_D9RightEyeSurface;

	SharedTextureHolder m_VKLeftEye;
	SharedTextureHolder m_VKRightEye;

	bool m_IsVREnabled = false;
	bool m_IsInitialized = false;
	bool m_RenderedNewFrame = false;
	bool m_CreatedVRTextures = false;
	int m_CreatingTextureID = -1;

	bool m_PressedLeftStick = false;
	bool m_ChangedItem = false;
	bool m_ToggledFlashlight = false;
	bool m_PressedTurn = false;

	Vector m_SetupOrigin = { 0,0,0 };

	// action set
	vr::VRActionSetHandle_t m_ActionSet;
	vr::VRActiveActionSet_t m_ActiveActionSet;

	// actions
	vr::VRActionHandle_t m_ActionJump;
	vr::VRActionHandle_t m_ActionPrimaryAttack;
	vr::VRActionHandle_t m_ActionSecondaryAttack;
	vr::VRActionHandle_t m_ActionReload;
	vr::VRActionHandle_t m_ActionWalk;
	vr::VRActionHandle_t m_ActionTurn;
	vr::VRActionHandle_t m_ActionUse;
	vr::VRActionHandle_t m_ActionNextItem;
	vr::VRActionHandle_t m_ActionPrevItem;
	vr::VRActionHandle_t m_ActionResetPosition;
	vr::VRActionHandle_t m_ActionCrouch;
	vr::VRActionHandle_t m_ActionFlashlight;
	vr::VRActionHandle_t m_ActionActivateVR;

	TrackedDevicePoseData m_HmdPose;
	TrackedDevicePoseData m_LeftControllerPose;
	TrackedDevicePoseData m_RightControllerPose;

	float m_RotationOffset = 0;
	std::chrono::steady_clock::time_point m_PrevFrameTime;


	float m_TurnSpeed = 0.3;
	bool m_SnapTurning = false;
	float m_SnapTurnAngle = 45.0;
	float m_VRScale = 43.2;
	float m_IpdScale = 1.0;

	VR() {};
	VR(Game *game);
	int SetActionManifest(const char *fileName);
	void Update();
	void CreateVRTextures();
	void SubmitVRTextures();
	void GetPoses();
	void UpdatePosesAndActions();
	void GetViewParameters();
	void ProcessInput();
	QAngle GetRightControllerAbsAngle();
	Vector GetRightControllerAbsPos();
	Vector GetRecommendedViewmodelAbsPos();
	QAngle GetRecommendedViewmodelAbsAngle();
	void UpdateTracking(Vector viewOrigin);
	Vector GetViewAngle();
	Vector GetViewOriginLeft();
	Vector GetViewOriginRight();
	bool PressedDigitalAction(vr::VRActionHandle_t &actionHandle);
	bool GetAnalogActionData(vr::VRActionHandle_t &actionHandle, vr::InputAnalogActionData_t &analogDataOut);
	void UpdateIntendedPosition();
	void GetPoseData(vr::TrackedDevicePose_t &poseRaw, TrackedDevicePoseData &poseOut);
	void ParseConfigFile();
	void WaitForConfigUpdate();
};