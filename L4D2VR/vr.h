#pragma once
#include "openvr.h"
#include "vector.h"
#include <chrono>

#define MAX_STR_LEN 256

class Game;
class C_BasePlayer;
class C_WeaponCSBase;
class IDirect3DTexture9;
class IDirect3DSurface9;
class ITexture;


struct TrackedDevicePoseData 
{
	std::string TrackedDeviceName;
	Vector TrackedDevicePos;
	Vector TrackedDeviceVel;
	QAngle TrackedDeviceAng;
	QAngle TrackedDeviceAngVel;
};

struct SharedTextureHolder 
{
	vr::VRVulkanTextureData_t m_VulkanData;
	vr::Texture_t m_VRTexture;
};


class VR
{
public:
	Game *m_Game = nullptr;

        vr::IVRSystem *m_System = nullptr;
        vr::IVRInput *m_Input = nullptr;
        vr::IVROverlay *m_Overlay = nullptr;
        vr::IVRCompositor *m_Compositor = nullptr;

	vr::VROverlayHandle_t m_MainMenuHandle;
	vr::VROverlayHandle_t m_HUDHandle;

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

	Vector m_ViewmodelForward;
	Vector m_ViewmodelRight;
	Vector m_ViewmodelUp;

        Vector m_HmdPosAbs = { 0,0,0 };
        Vector m_HmdPosAbsPrev = { 0,0,0 };
        QAngle m_HmdAngAbs;

        Vector m_HmdPosCorrectedPrev = { 0,0,0 };
        Vector m_HmdPosLocalPrev = { 0,0,0 };
        Vector m_HmdPosSmoothed = { 0,0,0 };
        QAngle m_HmdAngSmoothed = { 0,0,0 };

        Vector m_SetupOrigin = { 0,0,0 };
        QAngle m_SetupAngles = { 0,0,0 };
        Vector m_SetupOriginPrev = { 0,0,0 };
        Vector m_CameraAnchor = { 0,0,0 };
        Vector m_SetupOriginToHMD = { 0,0,0 };

        float m_HeightOffset = 0.0;
        bool m_RoomscaleActive = false;
        bool m_IsThirdPersonCamera = false;
        bool m_ObserverThirdPerson = false;
        int m_ThirdPersonHoldFrames = 0;
        Vector m_ThirdPersonViewOrigin = { 0,0,0 };
        QAngle m_ThirdPersonViewAngles = { 0,0,0 };
        bool m_ThirdPersonPoseInitialized = false;
        float m_ThirdPersonCameraSmoothing = 0.5f;

        Vector m_LeftControllerPosAbs;
        QAngle m_LeftControllerAngAbs;
        Vector m_RightControllerPosAbs;
        QAngle m_RightControllerAngAbs;

        Vector m_ViewmodelPosOffset;
        QAngle m_ViewmodelAngOffset;

        Vector m_AimLineStart = { 0,0,0 };
        Vector m_AimLineEnd = { 0,0,0 };
        Vector m_LastAimDirection = { 0,0,0 };
        bool m_HasAimLine = false;
        float m_AimLineThickness = 2.0f;
        bool m_AimLineEnabled = true;
        float m_AimLinePersistence = 0.02f;
        int m_AimLineColorR = 0;
        int m_AimLineColorG = 255;
        int m_AimLineColorB = 0;
        int m_AimLineColorA = 192;

	float m_Ipd;																	
	float m_EyeZ;

	Vector m_IntendedPositionOffset = { 0,0,0 };

	enum TextureID
	{
		Texture_None = -1,
		Texture_LeftEye,
		Texture_RightEye,
		Texture_HUD,
		Texture_Blank
	};

	ITexture *m_LeftEyeTexture;
	ITexture *m_RightEyeTexture;
	ITexture *m_HUDTexture;
	ITexture *m_BlankTexture = nullptr;

	IDirect3DSurface9 *m_D9LeftEyeSurface;
	IDirect3DSurface9 *m_D9RightEyeSurface;
	IDirect3DSurface9 *m_D9HUDSurface;
	IDirect3DSurface9 *m_D9BlankSurface;

	SharedTextureHolder m_VKLeftEye;
	SharedTextureHolder m_VKRightEye;
	SharedTextureHolder m_VKBackBuffer;
	SharedTextureHolder m_VKHUD;
	SharedTextureHolder m_VKBlankTexture;

        bool m_IsVREnabled = false;
        bool m_IsInitialized = false;
        bool m_RenderedNewFrame = false;
        bool m_RenderedHud = false;
        bool m_CreatedVRTextures = false;
        bool m_CompositorExplicitTiming = false;
        bool m_CompositorNeedsHandoff = false;
	TextureID m_CreatingTextureID = Texture_None;

        bool m_PressedTurn = false;
        bool m_PushingThumbstick = false;
        bool m_CrouchToggleActive = false;
        bool m_VoiceRecordActive = false;

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
	vr::VRActionHandle_t m_MenuSelect;
	vr::VRActionHandle_t m_MenuBack;
	vr::VRActionHandle_t m_MenuUp;
	vr::VRActionHandle_t m_MenuDown;
	vr::VRActionHandle_t m_MenuLeft;
	vr::VRActionHandle_t m_MenuRight;
	vr::VRActionHandle_t m_Spray; 
	vr::VRActionHandle_t m_Scoreboard;
	vr::VRActionHandle_t m_ShowHUD;
	vr::VRActionHandle_t m_Pause;

	TrackedDevicePoseData m_HmdPose;
	TrackedDevicePoseData m_LeftControllerPose;
	TrackedDevicePoseData m_RightControllerPose;

	float m_RotationOffset = 0;
	std::chrono::steady_clock::time_point m_PrevFrameTime;

	float m_TurnSpeed = 0.3;
	bool m_SnapTurning = false;
	float m_SnapTurnAngle = 45.0;
	bool m_LeftHanded = false;
        float m_VRScale = 43.2;
        float m_IpdScale = 1.0;
        bool m_HideArms = false;
        float m_HudDistance = 1.3;
        float m_HudSize = 1.1;
        bool m_HudAlwaysVisible = false;
        float m_HeadSmoothing = 0.0f;
        bool m_HmdSmoothingInitialized = false;

        bool m_ForceNonVRServerMovement = false;
        bool m_RequireSecondaryAttackForItemSwitch = true;

        VR() {};
	VR(Game *game);
	int SetActionManifest(const char *fileName);
	void InstallApplicationManifest(const char *fileName);
        void Update();
        void CreateVRTextures();
        void HandleMissingRenderContext(const char* location);
        void SubmitVRTextures();
        void RepositionOverlays();
        void GetPoses();
        bool UpdatePosesAndActions();
        void GetViewParameters();
        void ProcessMenuInput();
        void ProcessInput();
	VMatrix VMatrixFromHmdMatrix(const vr::HmdMatrix34_t &hmdMat);
        vr::HmdMatrix34_t VMatrixToHmdMatrix(const VMatrix &vMat);
        vr::HmdMatrix34_t GetControllerTipMatrix(vr::ETrackedControllerRole controllerRole);
        vr::HmdMatrix34_t BuildThirdPersonSubmitPose() const;
	bool CheckOverlayIntersectionForController(vr::VROverlayHandle_t overlayHandle, vr::ETrackedControllerRole controllerRole);
	QAngle GetRightControllerAbsAngle();
	Vector GetRightControllerAbsPos();
	Vector GetRecommendedViewmodelAbsPos();
	QAngle GetRecommendedViewmodelAbsAngle();
        void UpdateTracking();
        bool UpdateThirdPersonViewState(const Vector& cameraOrigin, const Vector& cameraAngles);
        Vector GetViewAngle();
        Vector GetViewOriginLeft();
        Vector GetViewOriginRight();
        Vector GetThirdPersonViewOrigin() const { return m_ThirdPersonViewOrigin; }
        QAngle GetThirdPersonViewAngles() const { return m_ThirdPersonViewAngles; }
        bool IsThirdPersonCameraActive() const { return m_IsThirdPersonCamera; }
        bool PressedDigitalAction(vr::VRActionHandle_t &actionHandle, bool checkIfActionChanged = false);
        bool GetDigitalActionData(vr::VRActionHandle_t& actionHandle, vr::InputDigitalActionData_t& digitalDataOut);
        bool GetAnalogActionData(vr::VRActionHandle_t &actionHandle, vr::InputAnalogActionData_t &analogDataOut);
        void ResetPosition();
        void GetPoseData(vr::TrackedDevicePose_t &poseRaw, TrackedDevicePoseData &poseOut);
        void ParseConfigFile();
        void WaitForConfigUpdate();
        bool GetWalkAxis(float &x, float &y);
        bool m_EncodeVRUsercmd = true;
        void UpdateAimingLaser(C_BasePlayer* localPlayer);
        bool ShouldShowAimLine(C_WeaponCSBase* weapon) const;
        void DrawAimLine(const Vector& start, const Vector& end);
        void FinishFrame();
        void ConfigureExplicitTiming();
};
