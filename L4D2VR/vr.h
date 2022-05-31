#pragma once
#include "openvr.h"
#include "vector.h"
#include <chrono>

#define MAX_STR_LEN 256

class Game;
class IDirect3DTexture9;
class ITexture;


struct TrackedDevicePoseData {
	std::string TrackedDeviceName;
	Vector TrackedDevicePos;
	Vector TrackedDeviceVel;
	QAngle TrackedDeviceAng;
	QAngle TrackedDeviceAngVel;
};


class VR
{
public:
	Game *m_Game;

	vr::IVRSystem *m_System = NULL;
	vr::IVRInput *m_Input = NULL;
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

	Vector m_EyeToHeadTransformPosLeft;
	Vector m_EyeToHeadTransformPosRight;

	Vector m_HmdForward;
	Vector m_HmdRight;
	Vector m_HmdUp;

	Vector m_HmdPosLocalInWorld;

	Vector m_LeftControllerForward;
	Vector m_LeftControllerRight;
	Vector m_LeftControllerUp;

	Vector m_RightControllerForward;
	Vector m_RightControllerRight;
	Vector m_RightControllerUp;
	Vector m_RightControllerForwardCorrected;
	Vector m_RightControllerUpCorrected;

	Vector m_HmdPosAbs;														
	Vector m_HmdPosAbsNoOffset;
	QAngle m_HmdAngAbs;

	Vector m_HmdPosCorrectedPrev;
	Vector m_HmdPosLocalPrev;

	Vector m_LeftControllerPosAbs;											
	QAngle m_LeftControllerAngAbs;
	Vector m_RightControllerPosAbs;											
	QAngle m_RightControllerAngAbs;

	int m_FrameBufferWidth;
	int m_FrameBufferHeight;

	float m_Ipd;																	
	float m_EyeZ;

	Vector m_IntendedPositionOffset = Vector(0, 0, 0);

	ITexture *m_LeftEyeTexture;
	ITexture *m_RightEyeTexture;

	IDirect3DTexture9 *m_D9LeftEyeTexture;
	IDirect3DTexture9 *m_D9RightEyeTexture;

	bool m_IsVREnabled;
	bool m_IsInitialized;

	bool m_PressedLeftStick = false;
	bool m_ChangedItem = false;
	bool m_ToggledFlashlight = false;
	bool m_PressedTurn = false;

	Vector m_SetupOrigin;

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

	int m_LeftControllerIndex;
	int m_RightControllerIndex;

	TrackedDevicePoseData m_HmdPose;
	TrackedDevicePoseData m_LeftControllerPose;
	TrackedDevicePoseData m_RightControllerPose;

	float m_RotationOffset;
	std::chrono::steady_clock::time_point m_PrevFrameTime;

	float m_TurnSpeed = 0.3;
	bool m_SnapTurning = false;
	float m_SnapTurnAngle = 45.0;
	float m_VRScale = 43.2;
	float m_IpdScale = 1.0;

	VR() {};
	VR(Game *game);
	int SetActionManifest(const char *fileName);
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