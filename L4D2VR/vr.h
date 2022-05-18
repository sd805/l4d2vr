#pragma once
#include "openvr.h"
#include "QAngle.h"
#include "vector.h"
#include <chrono>

#define MAX_STR_LEN     256

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
	Game *mGame;

	vr::IVRSystem *g_pSystem = NULL;
	vr::IVRInput *g_pInput = NULL;
	float g_horizontalFOVLeft;
	float g_aspectRatioLeft;
	float g_horizontalOffsetLeft;
	float g_verticalOffsetLeft;
	float g_horizontalFOVRight;
	float g_aspectRatioRight;
	float g_horizontalOffsetRight;
	float g_verticalOffsetRight;

	uint32_t m_nRenderWidth;
	uint32_t m_nRenderHeight;
	float m_nAspect;
	float m_nFov;

	vr::VRTextureBounds_t m_sTextureBounds[2];
	vr::TrackedDevicePose_t g_poses[vr::k_unMaxTrackedDeviceCount];
	TrackedDevicePoseData TrackedDevicesPoses[vr::k_unMaxTrackedDeviceCount];

	Vector eyeToHeadTransformPosLeft;
	Vector eyeToHeadTransformPosRight;

	Vector VR_hmd_forward;
	Vector VR_hmd_right;
	Vector VR_hmd_up;

	Vector VR_hmd_pos_local_in_world;

	Vector Player_forward;
	Vector Player_right;
	Vector Player_up;

	Vector VR_playspace_forward;
	Vector VR_playspace_right;
	Vector VR_playspace_up;
	QAngle ZeroAngle = QAngle(0, 0, 0);

	Vector VR_controller_left_forward;
	Vector VR_controller_left_right;
	Vector VR_controller_left_up;

	Vector VR_controller_right_forward;
	Vector VR_controller_right_right;
	Vector VR_controller_right_up;
	Vector VR_controller_right_forward_corrected;
	Vector VR_controller_right_up_corrected;

	Vector VR_hmd_pos_abs;														
	Vector VR_hmd_pos_abs_no_offset;
	QAngle VR_hmd_ang_abs;

	Vector VR_controller_left_pos_abs;											
	QAngle VR_controller_left_ang_abs;
	Vector VR_controller_right_pos_abs;											
	QAngle VR_controller_right_ang_abs;
	QAngle VR_controller_right_ang_test;

	float VR_scale = 52.49;

	int frameBufferWidth;
	int frameBufferHeight;

	float ipd;																	
	float eyeZ;

	Vector intendedPositionOffset = Vector(0, 0, 0);

	float ipd_scale = 45.2;

	ITexture *leftEyeTexture;
	ITexture *rightEyeTexture;

	IDirect3DTexture9 *d9LeftEyeTexture;
	IDirect3DTexture9 *d9RightEyeTexture;

	bool isVREnabled;

	bool mPressedLeftStick = false;
	bool mChangedItem = false;
	bool mToggledFlashlight = false;

	Vector setupOrigin;

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

	float mRotationOffset;
	std::chrono::steady_clock::time_point mPrevFrameTime;

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

};