#include "vr.h"
#include <Windows.h>
#include "sdk.h"
#include "game.h"
#include "hooks.h"
#include "trace.h"
#include "sdk/ivdebugoverlay.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <thread>
#include <algorithm>
#include <array>
#include <cmath>
#include <d3d9_vr.h>

namespace
{
    struct Matrix3x3
    {
        float m[3][3];
    };

    Matrix3x3 MatrixTranspose(const Matrix3x3& mat)
    {
        Matrix3x3 result{};
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.m[i][j] = mat.m[j][i];
            }
        }
        return result;
    }

    Matrix3x3 MatrixMultiply(const Matrix3x3& a, const Matrix3x3& b)
    {
        Matrix3x3 result{};
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                float value = 0.0f;
                for (int k = 0; k < 3; ++k)
                {
                    value += a.m[i][k] * b.m[k][j];
                }
                result.m[i][j] = value;
            }
        }
        return result;
    }

    Vector MatrixVectorMultiply(const Matrix3x3& mat, const Vector& vec)
    {
        return Vector(
            mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2] * vec.z,
            mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2] * vec.z,
            mat.m[2][0] * vec.x + mat.m[2][1] * vec.y + mat.m[2][2] * vec.z);
    }

    Matrix3x3 BuildSourceRotationMatrix(const QAngle& angles)
    {
        Vector forward, right, up;
        QAngle::AngleVectors(angles, &forward, &right, &up);

        Matrix3x3 result{};
        result.m[0][0] = forward.x;
        result.m[0][1] = right.x;
        result.m[0][2] = up.x;

        result.m[1][0] = forward.y;
        result.m[1][1] = right.y;
        result.m[1][2] = up.y;

        result.m[2][0] = forward.z;
        result.m[2][1] = right.z;
        result.m[2][2] = up.z;

        return result;
    }
}

VR::VR(Game* game)
{
    m_Game = game;

    char errorString[MAX_STR_LEN];

    vr::HmdError error = vr::VRInitError_None;
    m_System = vr::VR_Init(&error, vr::VRApplication_Scene);

    if (error != vr::VRInitError_None)
    {
        snprintf(errorString, MAX_STR_LEN, "VR_Init failed: %s", vr::VR_GetVRInitErrorAsEnglishDescription(error));
        Game::errorMsg(errorString);
        return;
    }

    m_Compositor = vr::VRCompositor();
    if (!m_Compositor)
    {
        Game::errorMsg("Compositor initialization failed.");
        return;
    }

    ConfigureExplicitTiming();

    m_Input = vr::VRInput();
    m_System = vr::OpenVRInternal_ModuleContext().VRSystem();

    m_System->GetRecommendedRenderTargetSize(&m_RenderWidth, &m_RenderHeight);
    
    float l_left = 0.0f, l_right = 0.0f, l_top = 0.0f, l_bottom = 0.0f;
    m_System->GetProjectionRaw(vr::EVREye::Eye_Left, &l_left, &l_right, &l_top, &l_bottom);

    float r_left = 0.0f, r_right = 0.0f, r_top = 0.0f, r_bottom = 0.0f;
    m_System->GetProjectionRaw(vr::EVREye::Eye_Right, &r_left, &r_right, &r_top, &r_bottom);

    float tanHalfFov[2];

    tanHalfFov[0] = std::max({ -l_left, l_right, -r_left, r_right });
    tanHalfFov[1] = std::max({ -l_top, l_bottom, -r_top, r_bottom });
    // For some headsets, the driver provided texture size doesn't match the geometric aspect ratio of the lenses.
    // In this case, we need to adjust the vertical tangent while still rendering to the recommended RT size.
    m_TextureBounds[0].uMin = 0.5f + 0.5f * l_left / tanHalfFov[0];
    m_TextureBounds[0].uMax = 0.5f + 0.5f * l_right / tanHalfFov[0];
    m_TextureBounds[0].vMin = 0.5f - 0.5f * l_bottom / tanHalfFov[1];
    m_TextureBounds[0].vMax = 0.5f - 0.5f * l_top / tanHalfFov[1];

    m_TextureBounds[1].uMin = 0.5f + 0.5f * r_left / tanHalfFov[0];
    m_TextureBounds[1].uMax = 0.5f + 0.5f * r_right / tanHalfFov[0];
    m_TextureBounds[1].vMin = 0.5f - 0.5f * r_bottom / tanHalfFov[1];
    m_TextureBounds[1].vMax = 0.5f - 0.5f * r_top / tanHalfFov[1];

    m_Aspect = tanHalfFov[0] / tanHalfFov[1];
    m_Fov = 2.0f * atan(tanHalfFov[0]) * 360 / (3.14159265358979323846 * 2);

    InstallApplicationManifest("manifest.vrmanifest");
    SetActionManifest("action_manifest.json");

    std::thread configParser(&VR::WaitForConfigUpdate, this);
    configParser.detach();

    while (!g_D3DVR9)
        Sleep(10);

    g_D3DVR9->GetBackBufferData(&m_VKBackBuffer);
    m_Overlay = vr::VROverlay();
    m_Overlay->CreateOverlay("MenuOverlayKey", "MenuOverlay", &m_MainMenuHandle);
    m_Overlay->CreateOverlay("HUDOverlayKey", "HUDOverlay", &m_HUDHandle);
    m_Overlay->SetOverlayInputMethod(m_MainMenuHandle, vr::VROverlayInputMethod_Mouse);
    m_Overlay->SetOverlayInputMethod(m_HUDHandle, vr::VROverlayInputMethod_Mouse);
    m_Overlay->SetOverlayFlag(m_MainMenuHandle, vr::VROverlayFlags_SendVRDiscreteScrollEvents, true);
    m_Overlay->SetOverlayFlag(m_HUDHandle, vr::VROverlayFlags_SendVRDiscreteScrollEvents, true);

    int windowWidth, windowHeight;
    m_Game->m_MaterialSystem->GetRenderContext()->GetWindowSize(windowWidth, windowHeight);

    const vr::HmdVector2_t mouseScaleHUD = { windowWidth, windowHeight };
    m_Overlay->SetOverlayMouseScale(m_HUDHandle, &mouseScaleHUD);

    const vr::HmdVector2_t mouseScaleMenu = { m_RenderWidth, m_RenderHeight };
    m_Overlay->SetOverlayMouseScale(m_MainMenuHandle, &mouseScaleMenu);

    UpdatePosesAndActions();
    FinishFrame();

    m_IsInitialized = true;
    m_IsVREnabled = true;
}

void VR::ConfigureExplicitTiming()
{
    if (!m_Compositor)
        return;

    m_Compositor->SetExplicitTimingMode(
        vr::VRCompositorTimingMode_Explicit_ApplicationPerformsPostPresentHandoff);

    m_CompositorExplicitTiming = true;
}

int VR::SetActionManifest(const char* fileName)
{
    char currentDir[MAX_STR_LEN];
    GetCurrentDirectory(MAX_STR_LEN, currentDir);
    char path[MAX_STR_LEN];
    sprintf_s(path, MAX_STR_LEN, "%s\\VR\\SteamVRActionManifest\\%s", currentDir, fileName);

    if (m_Input->SetActionManifestPath(path) != vr::VRInputError_None)
    {
        Game::errorMsg("SetActionManifestPath failed");
    }

    m_Input->GetActionHandle("/actions/main/in/ActivateVR", &m_ActionActivateVR);
    m_Input->GetActionHandle("/actions/main/in/Jump", &m_ActionJump);
    m_Input->GetActionHandle("/actions/main/in/PrimaryAttack", &m_ActionPrimaryAttack);
    m_Input->GetActionHandle("/actions/main/in/Reload", &m_ActionReload);
    m_Input->GetActionHandle("/actions/main/in/Use", &m_ActionUse);
    m_Input->GetActionHandle("/actions/main/in/Walk", &m_ActionWalk);
    m_Input->GetActionHandle("/actions/main/in/Turn", &m_ActionTurn);
    m_Input->GetActionHandle("/actions/main/in/SecondaryAttack", &m_ActionSecondaryAttack);
    m_Input->GetActionHandle("/actions/main/in/NextItem", &m_ActionNextItem);
    m_Input->GetActionHandle("/actions/main/in/PrevItem", &m_ActionPrevItem);
    m_Input->GetActionHandle("/actions/main/in/ResetPosition", &m_ActionResetPosition);
    m_Input->GetActionHandle("/actions/main/in/Crouch", &m_ActionCrouch);
    m_Input->GetActionHandle("/actions/main/in/Flashlight", &m_ActionFlashlight);
    m_Input->GetActionHandle("/actions/main/in/MenuSelect", &m_MenuSelect);
    m_Input->GetActionHandle("/actions/main/in/MenuBack", &m_MenuBack);
    m_Input->GetActionHandle("/actions/main/in/MenuUp", &m_MenuUp);
    m_Input->GetActionHandle("/actions/main/in/MenuDown", &m_MenuDown);
    m_Input->GetActionHandle("/actions/main/in/MenuLeft", &m_MenuLeft);
    m_Input->GetActionHandle("/actions/main/in/MenuRight", &m_MenuRight);
    m_Input->GetActionHandle("/actions/main/in/Spray", &m_Spray);
    m_Input->GetActionHandle("/actions/main/in/Scoreboard", &m_Scoreboard);
    m_Input->GetActionHandle("/actions/main/in/ShowHUD", &m_ShowHUD);
    m_Input->GetActionHandle("/actions/main/in/Pause", &m_Pause);

    m_Input->GetActionSetHandle("/actions/main", &m_ActionSet);
    m_ActiveActionSet = {};
    m_ActiveActionSet.ulActionSet = m_ActionSet;

    return 0;
}

void VR::InstallApplicationManifest(const char* fileName)
{
    char currentDir[MAX_STR_LEN];
    GetCurrentDirectory(MAX_STR_LEN, currentDir);
    char path[MAX_STR_LEN];
    sprintf_s(path, MAX_STR_LEN, "%s\\VR\\%s", currentDir, fileName);

    vr::VRApplications()->AddApplicationManifest(path);
}


void VR::Update()
{
    if (!m_IsInitialized || !m_Game->m_Initialized)
        return;

    struct FrameScope
    {
        VR* owner;
        ~FrameScope()
        {
            if (owner)
                owner->FinishFrame();
        }
    } frameScope{ this };

    bool posesValid = UpdatePosesAndActions();

    if (m_IsVREnabled && g_D3DVR9)
    {
        // Prevents crashing at menu
        if (!m_Game->m_EngineClient->IsInGame())
        {
            IMatRenderContext* rndrContext = m_Game->m_MaterialSystem->GetRenderContext();
            if (!rndrContext)
            {
                HandleMissingRenderContext("VR::Update");
                return;
            }
            rndrContext->SetRenderTarget(NULL);
            m_Game->m_CachedArmsModel = false;
            m_CreatedVRTextures = false; // Have to recreate textures otherwise some workshop maps won't render
        }
    }

    if (!posesValid)
    {
        // Continue using the last known poses so smoothing and aim helpers stay active.
    }

    UpdateTracking();
    SubmitVRTextures();


    if (m_Game->m_VguiSurface->IsCursorVisible())
        ProcessMenuInput();
    else
        ProcessInput();
}

bool VR::GetWalkAxis(float& x, float& y) {
    vr::InputAnalogActionData_t d;
    if (GetAnalogActionData(m_ActionWalk, d)) {  // m_ActionWalk 已在现有代码中使用
        x = d.x; y = d.y;
        return true;
    }
    x = y = 0.0f;
    return false;
}

void VR::CreateVRTextures()
{
    int windowWidth, windowHeight;
    m_Game->m_MaterialSystem->GetRenderContext()->GetWindowSize(windowWidth, windowHeight);

    m_Game->m_MaterialSystem->isGameRunning = false;
    m_Game->m_MaterialSystem->BeginRenderTargetAllocation();
    m_Game->m_MaterialSystem->isGameRunning = true;

    m_CreatingTextureID = Texture_LeftEye;
    m_LeftEyeTexture = m_Game->m_MaterialSystem->CreateNamedRenderTargetTextureEx("leftEye0", m_RenderWidth, m_RenderHeight, RT_SIZE_NO_CHANGE, m_Game->m_MaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED, TEXTUREFLAGS_NOMIP);

    m_CreatingTextureID = Texture_RightEye;
    m_RightEyeTexture = m_Game->m_MaterialSystem->CreateNamedRenderTargetTextureEx("rightEye0", m_RenderWidth, m_RenderHeight, RT_SIZE_NO_CHANGE, m_Game->m_MaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED, TEXTUREFLAGS_NOMIP);

    m_CreatingTextureID = Texture_HUD;
    m_HUDTexture = m_Game->m_MaterialSystem->CreateNamedRenderTargetTextureEx("vrHUD", windowWidth, windowHeight, RT_SIZE_NO_CHANGE, m_Game->m_MaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED, TEXTUREFLAGS_NOMIP);

    m_CreatingTextureID = Texture_Blank;
    m_BlankTexture = m_Game->m_MaterialSystem->CreateNamedRenderTargetTextureEx("blankTexture", 512, 512, RT_SIZE_NO_CHANGE, m_Game->m_MaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED, TEXTUREFLAGS_NOMIP);

    m_CreatingTextureID = Texture_None;

    m_Game->m_MaterialSystem->EndRenderTargetAllocation();

    m_CreatedVRTextures = true;
}

void VR::SubmitVRTextures()
{
    if (!m_Compositor)
        return;

    bool submitted = false;

    // 若这帧没有新内容，就走菜单/Overlay 路径
    if (!m_RenderedNewFrame)
    {
        if (!m_BlankTexture)
            CreateVRTextures();

        if (!vr::VROverlay()->IsOverlayVisible(m_MainMenuHandle))
            RepositionOverlays();

        vr::VROverlay()->SetOverlayTexture(m_MainMenuHandle, &m_VKBackBuffer.m_VRTexture);
        vr::VROverlay()->ShowOverlay(m_MainMenuHandle);
        vr::VROverlay()->HideOverlay(m_HUDHandle);

        if (!m_Game->m_EngineClient->IsInGame())
        {
            m_Compositor->Submit(vr::Eye_Left, &m_VKBlankTexture.m_VRTexture, nullptr, vr::Submit_Default);
            m_Compositor->Submit(vr::Eye_Right, &m_VKBlankTexture.m_VRTexture, nullptr, vr::Submit_Default);
            submitted = true;
        }

        if (submitted && m_CompositorExplicitTiming)
            m_CompositorNeedsHandoff = true;

        return;
    }


    vr::VROverlay()->HideOverlay(m_MainMenuHandle);
    vr::VROverlay()->SetOverlayTexture(m_HUDHandle, &m_VKHUD.m_VRTexture);
    if (m_Game->m_VguiSurface->IsCursorVisible())
    {
        vr::VROverlay()->ShowOverlay(m_HUDHandle);
    }

    if (m_IsThirdPersonCamera)
    {
        vr::HmdMatrix34_t submitPose = BuildThirdPersonSubmitPose();

        vr::VRTextureWithPose_t leftTexture = {};
        leftTexture.handle = m_VKLeftEye.m_VRTexture.handle;
        leftTexture.eType = m_VKLeftEye.m_VRTexture.eType;
        leftTexture.eColorSpace = m_VKLeftEye.m_VRTexture.eColorSpace;
        leftTexture.mDeviceToAbsoluteTracking = submitPose;

        vr::VRTextureWithPose_t rightTexture = {};
        rightTexture.handle = m_VKRightEye.m_VRTexture.handle;
        rightTexture.eType = m_VKRightEye.m_VRTexture.eType;
        rightTexture.eColorSpace = m_VKRightEye.m_VRTexture.eColorSpace;
        rightTexture.mDeviceToAbsoluteTracking = submitPose;

        m_Compositor->Submit(vr::Eye_Left, &leftTexture, &(m_TextureBounds)[0], vr::Submit_TextureWithPose);
        m_Compositor->Submit(vr::Eye_Right, &rightTexture, &(m_TextureBounds)[1], vr::Submit_TextureWithPose);
    }
    else
    {
        m_Compositor->Submit(vr::Eye_Left, &m_VKLeftEye.m_VRTexture, &(m_TextureBounds)[0], vr::Submit_Default);
        m_Compositor->Submit(vr::Eye_Right, &m_VKRightEye.m_VRTexture, &(m_TextureBounds)[1], vr::Submit_Default);
    }
    submitted = true;

    if (submitted && m_CompositorExplicitTiming)
        m_CompositorNeedsHandoff = true;


    m_RenderedNewFrame = false;
}


vr::HmdMatrix34_t VR::BuildThirdPersonSubmitPose() const
{
    const vr::TrackedDevicePose_t& hmdPose = m_Poses[vr::k_unTrackedDeviceIndex_Hmd];
    vr::HmdMatrix34_t basePose = hmdPose.mDeviceToAbsoluteTracking;
    if (!hmdPose.bPoseIsValid)
        return basePose;

    const Matrix3x3 axisTransform{ { { 0.0f, 0.0f, -1.0f },
                                      { -1.0f, 0.0f, 0.0f },
                                      { 0.0f, 1.0f, 0.0f } } };
    const Matrix3x3 axisTransformT = MatrixTranspose(axisTransform);

    const QAngle& thirdPersonAngles = m_ThirdPersonPoseInitialized ? m_ThirdPersonViewAngles : m_SetupAngles;
    const Vector& thirdPersonOrigin = m_ThirdPersonPoseInitialized ? m_ThirdPersonViewOrigin : m_SetupOrigin;

    Matrix3x3 cameraSource = BuildSourceRotationMatrix(thirdPersonAngles);
    Matrix3x3 hmdSource = BuildSourceRotationMatrix(m_HmdAngAbs);

    Matrix3x3 deltaSource = MatrixMultiply(cameraSource, MatrixTranspose(hmdSource));
    Matrix3x3 deltaVR = MatrixMultiply(MatrixMultiply(axisTransformT, deltaSource), axisTransform);

    Matrix3x3 baseRotation{};
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            baseRotation.m[i][j] = basePose.m[i][j];
        }
    }

    Matrix3x3 finalRotation = MatrixMultiply(deltaVR, baseRotation);

    vr::HmdMatrix34_t result = basePose;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            result.m[i][j] = finalRotation.m[i][j];
        }
    }

    if (m_VRScale > 0.0f)
    {
        Vector deltaSourcePos = thirdPersonOrigin - m_HmdPosAbs;
        Vector deltaMeters = deltaSourcePos * (1.0f / m_VRScale);
        Vector deltaVRPos = MatrixVectorMultiply(axisTransformT, deltaMeters);

        result.m[0][3] = basePose.m[0][3] + deltaVRPos.x;
        result.m[1][3] = basePose.m[1][3] + deltaVRPos.y;
        result.m[2][3] = basePose.m[2][3] + deltaVRPos.z;
    }

    return result;
}


void VR::GetPoseData(vr::TrackedDevicePose_t& poseRaw, TrackedDevicePoseData& poseOut)
{
    if (poseRaw.bPoseIsValid)
    {
        vr::HmdMatrix34_t mat = poseRaw.mDeviceToAbsoluteTracking;
        Vector pos;
        Vector vel;
        QAngle ang;
        QAngle angvel;
        pos.x = -mat.m[2][3];
        pos.y = -mat.m[0][3];
        pos.z = mat.m[1][3];
        ang.x = asin(mat.m[1][2]) * (180.0 / 3.141592654);
        ang.y = atan2f(mat.m[0][2], mat.m[2][2]) * (180.0 / 3.141592654);
        ang.z = atan2f(-mat.m[1][0], mat.m[1][1]) * (180.0 / 3.141592654);
        vel.x = -poseRaw.vVelocity.v[2];
        vel.y = -poseRaw.vVelocity.v[0];
        vel.z = poseRaw.vVelocity.v[1];
        angvel.x = -poseRaw.vAngularVelocity.v[2] * (180.0 / 3.141592654);
        angvel.y = -poseRaw.vAngularVelocity.v[0] * (180.0 / 3.141592654);
        angvel.z = poseRaw.vAngularVelocity.v[1] * (180.0 / 3.141592654);

        poseOut.TrackedDevicePos = pos;
        poseOut.TrackedDeviceVel = vel;
        poseOut.TrackedDeviceAng = ang;
        poseOut.TrackedDeviceAngVel = angvel;
    }
}

void VR::RepositionOverlays()
{
    vr::TrackedDevicePose_t hmdPose = m_Poses[vr::k_unTrackedDeviceIndex_Hmd];
    vr::HmdMatrix34_t hmdMat = hmdPose.mDeviceToAbsoluteTracking;
    Vector hmdPosition = { hmdMat.m[0][3], hmdMat.m[1][3], hmdMat.m[2][3] };
    Vector hmdForward = { -hmdMat.m[0][2], 0, -hmdMat.m[2][2] };

    int windowWidth, windowHeight;
    m_Game->m_MaterialSystem->GetRenderContext()->GetWindowSize(windowWidth, windowHeight);

    vr::HmdMatrix34_t menuTransform =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };

    vr::ETrackingUniverseOrigin trackingOrigin = vr::VRCompositor()->GetTrackingSpace();

    // Reposition main menu overlay
    float renderWidth = m_VKBackBuffer.m_VulkanData.m_nWidth;
    float renderHeight = m_VKBackBuffer.m_VulkanData.m_nHeight;

    float widthRatio = windowWidth / renderWidth;
    float heightRatio = windowHeight / renderHeight;
    menuTransform.m[0][0] *= widthRatio;
    menuTransform.m[1][1] *= heightRatio;

    hmdForward[1] = 0;
    VectorNormalize(hmdForward);

    Vector menuDistance = hmdForward * 3;
    Vector menuNewPos = menuDistance + hmdPosition;

    menuTransform.m[0][3] = menuNewPos.x;
    menuTransform.m[1][3] = menuNewPos.y - 0.25;
    menuTransform.m[2][3] = menuNewPos.z;

    float xScale = menuTransform.m[0][0];
    float hmdRotationDegrees = atan2f(hmdMat.m[0][2], hmdMat.m[2][2]);

    menuTransform.m[0][0] *= cos(hmdRotationDegrees);
    menuTransform.m[0][2] = sin(hmdRotationDegrees);
    menuTransform.m[2][0] = -sin(hmdRotationDegrees) * xScale;
    menuTransform.m[2][2] *= cos(hmdRotationDegrees);

    vr::VROverlay()->SetOverlayTransformAbsolute(m_MainMenuHandle, trackingOrigin, &menuTransform);
    vr::VROverlay()->SetOverlayWidthInMeters(m_MainMenuHandle, 1.5 * (1.0 / heightRatio));

    // Reposition HUD overlay
    vr::HmdMatrix34_t hudTransform =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f
    };

    Vector hudDistance = hmdForward * m_HudDistance;
    Vector hudNewPos = hudDistance + hmdPosition;

    hudTransform.m[0][3] = hudNewPos.x;
    hudTransform.m[1][3] = hudNewPos.y - 0.25;
    hudTransform.m[2][3] = hudNewPos.z;

    hudTransform.m[0][0] *= cos(hmdRotationDegrees);
    hudTransform.m[0][2] = sin(hmdRotationDegrees);
    hudTransform.m[2][0] = -sin(hmdRotationDegrees);
    hudTransform.m[2][2] *= cos(hmdRotationDegrees);

    vr::VROverlay()->SetOverlayTransformAbsolute(m_HUDHandle, trackingOrigin, &hudTransform);
    vr::VROverlay()->SetOverlayWidthInMeters(m_HUDHandle, m_HudSize);
}

void VR::GetPoses()
{
    vr::TrackedDevicePose_t hmdPose = m_Poses[vr::k_unTrackedDeviceIndex_Hmd];

    vr::TrackedDeviceIndex_t leftControllerIndex = m_System->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
    vr::TrackedDeviceIndex_t rightControllerIndex = m_System->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);

    if (m_LeftHanded)
        std::swap(leftControllerIndex, rightControllerIndex);

    vr::TrackedDevicePose_t leftControllerPose = m_Poses[leftControllerIndex];
    vr::TrackedDevicePose_t rightControllerPose = m_Poses[rightControllerIndex];

    GetPoseData(hmdPose, m_HmdPose);
    GetPoseData(leftControllerPose, m_LeftControllerPose);
    GetPoseData(rightControllerPose, m_RightControllerPose);
}

bool VR::UpdatePosesAndActions()
{
    if (!m_Compositor)
        return false;

    vr::EVRCompositorError result = m_Compositor->WaitGetPoses(m_Poses, vr::k_unMaxTrackedDeviceCount, NULL, 0);
    bool posesValid = (result == vr::VRCompositorError_None);

    if (posesValid)
    {
        if (m_CompositorExplicitTiming)
            m_CompositorNeedsHandoff = true;
    }
    else
    {
        m_CompositorNeedsHandoff = false;
    }

    m_Input->UpdateActionState(&m_ActiveActionSet, sizeof(vr::VRActiveActionSet_t), 1);
    return posesValid;
}

void VR::GetViewParameters()
{
    vr::HmdMatrix34_t eyeToHeadLeft = m_System->GetEyeToHeadTransform(vr::Eye_Left);
    vr::HmdMatrix34_t eyeToHeadRight = m_System->GetEyeToHeadTransform(vr::Eye_Right);
    m_EyeToHeadTransformPosLeft.x = eyeToHeadLeft.m[0][3];
    m_EyeToHeadTransformPosLeft.y = eyeToHeadLeft.m[1][3];
    m_EyeToHeadTransformPosLeft.z = eyeToHeadLeft.m[2][3];

    m_EyeToHeadTransformPosRight.x = eyeToHeadRight.m[0][3];
    m_EyeToHeadTransformPosRight.y = eyeToHeadRight.m[1][3];
    m_EyeToHeadTransformPosRight.z = eyeToHeadRight.m[2][3];
}

bool VR::PressedDigitalAction(vr::VRActionHandle_t& actionHandle, bool checkIfActionChanged)
{
    vr::InputDigitalActionData_t digitalActionData{};

    if (!GetDigitalActionData(actionHandle, digitalActionData))
        return false;

    if (checkIfActionChanged)
        return digitalActionData.bState && digitalActionData.bChanged;

    return digitalActionData.bState;
}

bool VR::GetDigitalActionData(vr::VRActionHandle_t& actionHandle, vr::InputDigitalActionData_t& digitalDataOut)
{
    vr::EVRInputError result = m_Input->GetDigitalActionData(actionHandle, &digitalDataOut, sizeof(digitalDataOut), vr::k_ulInvalidInputValueHandle);

    return result == vr::VRInputError_None;
}

bool VR::GetAnalogActionData(vr::VRActionHandle_t& actionHandle, vr::InputAnalogActionData_t& analogDataOut)
{
    vr::EVRInputError result = m_Input->GetAnalogActionData(actionHandle, &analogDataOut, sizeof(analogDataOut), vr::k_ulInvalidInputValueHandle);

    if (result == vr::VRInputError_None)
        return true;

    return false;
}

void VR::ProcessMenuInput()
{
    vr::VROverlayHandle_t currentOverlay = m_Game->m_EngineClient->IsInGame() ? m_HUDHandle : m_MainMenuHandle;

    // Check if left or right hand controller is pointing at the overlay
    const bool isHoveringOverlay = CheckOverlayIntersectionForController(currentOverlay, vr::TrackedControllerRole_LeftHand) ||
        CheckOverlayIntersectionForController(currentOverlay, vr::TrackedControllerRole_RightHand);

    // Overlays can't process action inputs if the laser is active, so
    // only activate laser if a controller is pointing at the overlay
    if (isHoveringOverlay)
    {
        vr::VROverlay()->SetOverlayFlag(currentOverlay, vr::VROverlayFlags_MakeOverlaysInteractiveIfVisible, true);

        int windowWidth, windowHeight;
        m_Game->m_MaterialSystem->GetRenderContext()->GetWindowSize(windowWidth, windowHeight);

        vr::VREvent_t vrEvent;
        while (vr::VROverlay()->PollNextOverlayEvent(currentOverlay, &vrEvent, sizeof(vrEvent)))
        {
            switch (vrEvent.eventType)
            {
            case vr::VREvent_MouseMove:
            {
                float laserX = vrEvent.data.mouse.x;
                float laserY = vrEvent.data.mouse.y;

                if (currentOverlay == m_HUDHandle)
                {
                    laserY = -laserY + windowHeight;
                }
                else // main menu (uses render sized texture)
                {
                    laserX = (laserX / m_RenderWidth) * windowWidth;
                    laserY = ((-laserY + m_RenderHeight) / m_RenderHeight) * windowHeight;
                }

                m_Game->m_VguiInput->SetCursorPos(laserX, laserY);
                break;
            }

            case vr::VREvent_MouseButtonDown:
                // Don't allow holding down the mouse down in the pause menu. The resume button can be clicked before
                // the MouseButtonUp event is polled, which causes issues with the overlay.
                if (currentOverlay == m_MainMenuHandle)
                    m_Game->m_VguiInput->InternalMousePressed(ButtonCode_t::MOUSE_LEFT);
                break;

            case vr::VREvent_MouseButtonUp:
                if (currentOverlay == m_HUDHandle)
                    m_Game->m_VguiInput->InternalMousePressed(ButtonCode_t::MOUSE_LEFT);
                m_Game->m_VguiInput->InternalMouseReleased(ButtonCode_t::MOUSE_LEFT);
                break;

            case vr::VREvent_ScrollDiscrete:
                m_Game->m_VguiInput->InternalMouseWheeled((int)vrEvent.data.scroll.ydelta);
                break;
            }
        }
    }
    else
    {
        vr::VROverlay()->SetOverlayFlag(currentOverlay, vr::VROverlayFlags_MakeOverlaysInteractiveIfVisible, false);

        if (PressedDigitalAction(m_MenuSelect, true))
        {
            m_Game->m_VguiInput->InternalKeyCodeTyped(ButtonCode_t::KEY_SPACE);
            m_Game->m_VguiInput->InternalKeyCodePressed(ButtonCode_t::KEY_SPACE);
            m_Game->m_VguiInput->InternalKeyCodeReleased(ButtonCode_t::KEY_SPACE);
        }
        if (PressedDigitalAction(m_MenuBack, true) || PressedDigitalAction(m_Pause, true))
        {
            m_Game->m_VguiInput->InternalKeyCodeTyped(ButtonCode_t::KEY_ESCAPE);
            m_Game->m_VguiInput->InternalKeyCodePressed(ButtonCode_t::KEY_ESCAPE);
            m_Game->m_VguiInput->InternalKeyCodeReleased(ButtonCode_t::KEY_ESCAPE);
        }
        if (PressedDigitalAction(m_MenuUp, true))
        {
            m_Game->m_VguiInput->InternalKeyCodeTyped(ButtonCode_t::KEY_UP);
            m_Game->m_VguiInput->InternalKeyCodePressed(ButtonCode_t::KEY_UP);
            m_Game->m_VguiInput->InternalKeyCodeReleased(ButtonCode_t::KEY_UP);
        }
        if (PressedDigitalAction(m_MenuDown, true))
        {
            m_Game->m_VguiInput->InternalKeyCodeTyped(ButtonCode_t::KEY_DOWN);
            m_Game->m_VguiInput->InternalKeyCodePressed(ButtonCode_t::KEY_DOWN);
            m_Game->m_VguiInput->InternalKeyCodeReleased(ButtonCode_t::KEY_DOWN);
        }
        if (PressedDigitalAction(m_MenuLeft, true))
        {
            m_Game->m_VguiInput->InternalKeyCodeTyped(ButtonCode_t::KEY_LEFT);
            m_Game->m_VguiInput->InternalKeyCodePressed(ButtonCode_t::KEY_LEFT);
            m_Game->m_VguiInput->InternalKeyCodeReleased(ButtonCode_t::KEY_LEFT);
        }
        if (PressedDigitalAction(m_MenuRight, true))
        {
            m_Game->m_VguiInput->InternalKeyCodeTyped(ButtonCode_t::KEY_RIGHT);
            m_Game->m_VguiInput->InternalKeyCodePressed(ButtonCode_t::KEY_RIGHT);
            m_Game->m_VguiInput->InternalKeyCodeReleased(ButtonCode_t::KEY_RIGHT);
        }
    }
}

void VR::ProcessInput()
{
    if (!m_IsVREnabled)
        return;

    vr::VROverlay()->SetOverlayFlag(m_HUDHandle, vr::VROverlayFlags_MakeOverlaysInteractiveIfVisible, false);

    typedef std::chrono::duration<float, std::milli> duration;
    auto currentTime = std::chrono::steady_clock::now();
    duration elapsed = currentTime - m_PrevFrameTime;
    float deltaTime = elapsed.count();
    m_PrevFrameTime = currentTime;

    vr::InputAnalogActionData_t analogActionData;

    if (GetAnalogActionData(m_ActionTurn, analogActionData))
    {
        if (m_SnapTurning)
        {
            if (!m_PressedTurn && analogActionData.x > 0.5)
            {
                m_RotationOffset -= m_SnapTurnAngle;
                m_PressedTurn = true;
            }
            else if (!m_PressedTurn && analogActionData.x < -0.5)
            {
                m_RotationOffset += m_SnapTurnAngle;
                m_PressedTurn = true;
            }
            else if (analogActionData.x < 0.3 && analogActionData.x > -0.3)
                m_PressedTurn = false;
        }
        // Smooth turning
        else
        {
            float deadzone = 0.2;
            // smoother turning
            float xNormalized = (abs(analogActionData.x) - deadzone) / (1 - deadzone);
            if (analogActionData.x > deadzone)
            {
                m_RotationOffset -= m_TurnSpeed * deltaTime * xNormalized;
            }
            if (analogActionData.x < -deadzone)
            {
                m_RotationOffset += m_TurnSpeed * deltaTime * xNormalized;
            }
        }

        // Wrap from 0 to 360
        m_RotationOffset -= 360 * std::floor(m_RotationOffset / 360);
    }

    // TODO: Instead of ClientCmding, override Usercmd in CreateMove
#if 0
    if (GetAnalogActionData(m_ActionWalk, analogActionData))
    {
        bool pushingStickX = true;
        bool pushingStickY = true;
        if (analogActionData.y > 0.5)
        {
            m_Game->ClientCmd_Unrestricted("-back");
            m_Game->ClientCmd_Unrestricted("+forward");
        }
        else if (analogActionData.y < -0.5)
        {
            m_Game->ClientCmd_Unrestricted("-forward");
            m_Game->ClientCmd_Unrestricted("+back");
        }
        else
        {
            m_Game->ClientCmd_Unrestricted("-back");
            m_Game->ClientCmd_Unrestricted("-forward");
            pushingStickY = false;
        }

        if (analogActionData.x > 0.5)
        {
            m_Game->ClientCmd_Unrestricted("-moveleft");
            m_Game->ClientCmd_Unrestricted("+moveright");
        }
        else if (analogActionData.x < -0.5)
        {
            m_Game->ClientCmd_Unrestricted("-moveright");
            m_Game->ClientCmd_Unrestricted("+moveleft");
        }
        else
        {
            m_Game->ClientCmd_Unrestricted("-moveright");
            m_Game->ClientCmd_Unrestricted("-moveleft");
            pushingStickX = false;
        }

        m_PushingThumbstick = pushingStickX || pushingStickY;
    }
    else
    {
        m_Game->ClientCmd_Unrestricted("-forward");
        m_Game->ClientCmd_Unrestricted("-back");
        m_Game->ClientCmd_Unrestricted("-moveleft");
        m_Game->ClientCmd_Unrestricted("-moveright");
    }
#else
    // Movement via console commands disabled; handled in Hooks::dCreateMove via CUserCmd.
#endif

    if (PressedDigitalAction(m_ActionPrimaryAttack))
    {
        m_Game->ClientCmd_Unrestricted("+attack");
    }
    else
    {
        m_Game->ClientCmd_Unrestricted("-attack");
    }

    if (PressedDigitalAction(m_ActionJump))
    {
        m_Game->ClientCmd_Unrestricted("+jump");
    }
    else
    {
        m_Game->ClientCmd_Unrestricted("-jump");
    }

    if (PressedDigitalAction(m_ActionUse))
    {
        m_Game->ClientCmd_Unrestricted("+use");
    }
    else
    {
        m_Game->ClientCmd_Unrestricted("-use");
    }

    vr::InputDigitalActionData_t crouchActionData{};
    bool crouchDataValid = GetDigitalActionData(m_ActionCrouch, crouchActionData);
    bool crouchButtonDown = crouchDataValid && crouchActionData.bState;
    bool crouchJustPressed = crouchDataValid && crouchActionData.bState && crouchActionData.bChanged;

    vr::InputDigitalActionData_t resetActionData{};
    bool resetDataValid = GetDigitalActionData(m_ActionResetPosition, resetActionData);
    bool resetJustPressed = resetDataValid && resetActionData.bState && resetActionData.bChanged;

    vr::InputDigitalActionData_t reloadActionData{};
    bool reloadDataValid = GetDigitalActionData(m_ActionReload, reloadActionData);
    bool reloadButtonDown = reloadDataValid && reloadActionData.bState;
    bool reloadJustPressed = reloadDataValid && reloadActionData.bState && reloadActionData.bChanged;

    vr::InputDigitalActionData_t flashlightActionData{};
    bool flashlightDataValid = GetDigitalActionData(m_ActionFlashlight, flashlightActionData);
    bool flashlightJustPressed = flashlightDataValid && flashlightActionData.bState && flashlightActionData.bChanged;

    if (resetJustPressed)
    {
        if (crouchButtonDown)
        {
            m_Game->ClientCmd_Unrestricted("vote no");
        }
        else
        {
            m_CrouchToggleActive = !m_CrouchToggleActive;
            ResetPosition();
        }
    }

    if (!m_VoiceRecordActive && ((crouchButtonDown && reloadJustPressed) || (reloadButtonDown && crouchJustPressed)))
    {
        m_Game->ClientCmd_Unrestricted("+voicerecord");
        m_VoiceRecordActive = true;
    }

    if (m_VoiceRecordActive && (!crouchButtonDown || !reloadButtonDown || !crouchDataValid || !reloadDataValid))
    {
        m_Game->ClientCmd_Unrestricted("-voicerecord");
        m_VoiceRecordActive = false;
    }

    if (!crouchButtonDown && reloadButtonDown)
    {
        m_Game->ClientCmd_Unrestricted("+reload");
    }
    else
    {
        m_Game->ClientCmd_Unrestricted("-reload");
    }

    bool secondaryAttackActive = PressedDigitalAction(m_ActionSecondaryAttack);
    if (secondaryAttackActive)
    {
        m_Game->ClientCmd_Unrestricted("+attack2");
    }
    else
    {
        m_Game->ClientCmd_Unrestricted("-attack2");
    }

    if (secondaryAttackActive || !m_RequireSecondaryAttackForItemSwitch)
    {
        if (PressedDigitalAction(m_ActionPrevItem, true))
        {
            m_Game->ClientCmd_Unrestricted("invprev");
        }
        else if (PressedDigitalAction(m_ActionNextItem, true))
        {
            m_Game->ClientCmd_Unrestricted("invnext");
        }
    }

    bool crouchActive = crouchButtonDown || m_CrouchToggleActive;
    if (crouchActive)
    {
        m_Game->ClientCmd_Unrestricted("+duck");
    }
    else
    {
        m_Game->ClientCmd_Unrestricted("-duck");
    }

    if (flashlightJustPressed)
    {
        if (crouchButtonDown)
            m_Game->ClientCmd_Unrestricted("vote yes");
        else
            m_Game->ClientCmd_Unrestricted("impulse 100");
    }

    if (PressedDigitalAction(m_Spray, true))
    {
        m_Game->ClientCmd_Unrestricted("impulse 201");
    }

    bool isControllerVertical = m_RightControllerAngAbs.x > 60 || m_RightControllerAngAbs.x < -45;
    if ((PressedDigitalAction(m_ShowHUD) || PressedDigitalAction(m_Scoreboard) || isControllerVertical || m_HudAlwaysVisible)
        && m_RenderedHud)
    {
        if (!vr::VROverlay()->IsOverlayVisible(m_HUDHandle) || m_HudAlwaysVisible)
            RepositionOverlays();

        if (PressedDigitalAction(m_Scoreboard))
            m_Game->ClientCmd_Unrestricted("+showscores");
        else
            m_Game->ClientCmd_Unrestricted("-showscores");

        vr::VROverlay()->ShowOverlay(m_HUDHandle);
    }
    else
    {
        vr::VROverlay()->HideOverlay(m_HUDHandle);
    }
    m_RenderedHud = false;

    if (PressedDigitalAction(m_Pause, true))
    {
        m_Game->ClientCmd_Unrestricted("gameui_activate");
        RepositionOverlays();
    }
}

VMatrix VR::VMatrixFromHmdMatrix(const vr::HmdMatrix34_t& hmdMat)
{
    // VMatrix has a different implicit coordinate system than HmdMatrix34_t, but this function does not convert between them
    VMatrix vMat(
        hmdMat.m[0][0], hmdMat.m[1][0], hmdMat.m[2][0], 0.0f,
        hmdMat.m[0][1], hmdMat.m[1][1], hmdMat.m[2][1], 0.0f,
        hmdMat.m[0][2], hmdMat.m[1][2], hmdMat.m[2][2], 0.0f,
        hmdMat.m[0][3], hmdMat.m[1][3], hmdMat.m[2][3], 1.0f
    );

    return vMat;
}

vr::HmdMatrix34_t VR::VMatrixToHmdMatrix(const VMatrix& vMat)
{
    vr::HmdMatrix34_t hmdMat = { 0 };

    hmdMat.m[0][0] = vMat.m[0][0];
    hmdMat.m[1][0] = vMat.m[0][1];
    hmdMat.m[2][0] = vMat.m[0][2];

    hmdMat.m[0][1] = vMat.m[1][0];
    hmdMat.m[1][1] = vMat.m[1][1];
    hmdMat.m[2][1] = vMat.m[1][2];

    hmdMat.m[0][2] = vMat.m[2][0];
    hmdMat.m[1][2] = vMat.m[2][1];
    hmdMat.m[2][2] = vMat.m[2][2];

    hmdMat.m[0][3] = vMat.m[3][0];
    hmdMat.m[1][3] = vMat.m[3][1];
    hmdMat.m[2][3] = vMat.m[3][2];

    return hmdMat;
}

vr::HmdMatrix34_t VR::GetControllerTipMatrix(vr::ETrackedControllerRole controllerRole)
{
    vr::VRInputValueHandle_t inputValue = vr::k_ulInvalidInputValueHandle;

    if (controllerRole == vr::TrackedControllerRole_RightHand)
    {
        m_Input->GetInputSourceHandle("/user/hand/right", &inputValue);
    }
    else if (controllerRole == vr::TrackedControllerRole_LeftHand)
    {
        m_Input->GetInputSourceHandle("/user/hand/left", &inputValue);
    }

    if (inputValue != vr::k_ulInvalidInputValueHandle)
    {
        char buffer[vr::k_unMaxPropertyStringSize];

        m_System->GetStringTrackedDeviceProperty(vr::VRSystem()->GetTrackedDeviceIndexForControllerRole(controllerRole), vr::Prop_RenderModelName_String,
            buffer, vr::k_unMaxPropertyStringSize);

        vr::RenderModel_ControllerMode_State_t controllerState = { 0 };
        vr::RenderModel_ComponentState_t componentState = { 0 };

        if (vr::VRRenderModels()->GetComponentStateForDevicePath(buffer, vr::k_pch_Controller_Component_Tip, inputValue, &controllerState, &componentState))
        {
            return componentState.mTrackingToComponentLocal;
        }
    }

    // Not a hand controller role or tip lookup failed, return identity
    const vr::HmdMatrix34_t identity =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f
    };

    return identity;
}

bool VR::CheckOverlayIntersectionForController(vr::VROverlayHandle_t overlayHandle, vr::ETrackedControllerRole controllerRole)
{
    vr::TrackedDeviceIndex_t deviceIndex = m_System->GetTrackedDeviceIndexForControllerRole(controllerRole);

    if (deviceIndex == vr::k_unTrackedDeviceIndexInvalid)
        return false;

    vr::TrackedDevicePose_t& controllerPose = m_Poses[deviceIndex];

    if (!controllerPose.bPoseIsValid)
        return false;

    VMatrix controllerVMatrix = VMatrixFromHmdMatrix(controllerPose.mDeviceToAbsoluteTracking);
    VMatrix tipVMatrix = VMatrixFromHmdMatrix(GetControllerTipMatrix(controllerRole));
    tipVMatrix.MatrixMul(controllerVMatrix, controllerVMatrix);

    vr::VROverlayIntersectionParams_t  params = { 0 };
    vr::VROverlayIntersectionResults_t results = { 0 };

    params.eOrigin = vr::VRCompositor()->GetTrackingSpace();
    params.vSource = { controllerVMatrix.m[3][0],  controllerVMatrix.m[3][1],  controllerVMatrix.m[3][2] };
    params.vDirection = { -controllerVMatrix.m[2][0], -controllerVMatrix.m[2][1], -controllerVMatrix.m[2][2] };

    return m_Overlay->ComputeOverlayIntersection(overlayHandle, &params, &results);
}

QAngle VR::GetRightControllerAbsAngle()
{
    return m_RightControllerAngAbs;
}

Vector VR::GetRightControllerAbsPos()
{
    return m_RightControllerPosAbs;
}

Vector VR::GetRecommendedViewmodelAbsPos()
{
    Vector viewmodelPos = GetRightControllerAbsPos();
    viewmodelPos -= m_ViewmodelForward * m_ViewmodelPosOffset.x;
    viewmodelPos -= m_ViewmodelRight * m_ViewmodelPosOffset.y;
    viewmodelPos -= m_ViewmodelUp * m_ViewmodelPosOffset.z;

    return viewmodelPos;
}

QAngle VR::GetRecommendedViewmodelAbsAngle()
{
    QAngle result{};

    QAngle::VectorAngles(m_ViewmodelForward, m_ViewmodelUp, result);

    return result;
}

void VR::HandleMissingRenderContext(const char* location)
{
    const char* ctx = location ? location : "unknown";
    LOG("[VR] Missing IMatRenderContext in %s. Disabling VR rendering for this frame.", ctx);
    m_CreatedVRTextures = false;
    m_RenderedNewFrame = false;
    m_RenderedHud = false;
}

void VR::FinishFrame()
{
    if (!m_Compositor || !m_CompositorExplicitTiming)
        return;

    if (!m_CompositorNeedsHandoff)
        return;

    m_Compositor->PostPresentHandoff();
    m_CompositorNeedsHandoff = false;
}

void VR::UpdateTracking()
{
    GetPoses();

    int playerIndex = m_Game->m_EngineClient->GetLocalPlayer();
    C_BasePlayer* localPlayer = (C_BasePlayer*)m_Game->GetClientEntity(playerIndex);
    if (!localPlayer)
    {
        m_ObserverThirdPerson = false;
        m_IsThirdPersonCamera = false;
        m_ThirdPersonHoldFrames = 0;
        return;
    }

    bool thirdPerson = false;
    if (m_Game->m_EngineClient->IsInGame())
    {
        int observerMode = localPlayer->m_iObserverMode;
        if (observerMode != 0 && observerMode != 4)
        {
            thirdPerson = true;
        }
    }
    m_ObserverThirdPerson = thirdPerson;
    if (!thirdPerson && m_IsThirdPersonCamera)
    {
        // Allow the render hook to reevaluate third-person state next frame.
        m_IsThirdPersonCamera = false;
    }

    m_Game->m_IsMeleeWeaponActive = localPlayer->IsMeleeWeaponActive();

    // HMD tracking
    QAngle hmdAngLocal = m_HmdPose.TrackedDeviceAng;
    Vector hmdPosLocal = m_HmdPose.TrackedDevicePos;

    Vector deltaPosition = hmdPosLocal - m_HmdPosLocalPrev;
    Vector hmdPosCorrected = m_HmdPosCorrectedPrev + deltaPosition;

    VectorPivotXY(hmdPosCorrected, m_HmdPosCorrectedPrev, m_RotationOffset);

    hmdAngLocal.y += m_RotationOffset;
    // Wrap angle from -180 to 180
    hmdAngLocal.y -= 360 * std::floor((hmdAngLocal.y + 180) / 360);

    Vector hmdPosSmoothed = hmdPosCorrected;
    QAngle hmdAngSmoothed = hmdAngLocal;
    float smoothingStrength = std::clamp(m_HeadSmoothing, 0.0f, 0.99f);

    if (!m_HmdSmoothingInitialized)
    {
        m_HmdPosSmoothed = hmdPosCorrected;
        m_HmdAngSmoothed = hmdAngLocal;
        m_HmdSmoothingInitialized = true;
    }

    if (smoothingStrength > 0.0f)
    {
        float lerpFactor = 1.0f - smoothingStrength;

        m_HmdPosSmoothed.x += (hmdPosCorrected.x - m_HmdPosSmoothed.x) * lerpFactor;
        m_HmdPosSmoothed.y += (hmdPosCorrected.y - m_HmdPosSmoothed.y) * lerpFactor;
        m_HmdPosSmoothed.z += (hmdPosCorrected.z - m_HmdPosSmoothed.z) * lerpFactor;

        auto smoothAngle = [&](float target, float& current)
            {
                float diff = target - current;
                diff -= 360.0f * std::floor((diff + 180.0f) / 360.0f);
                current += diff * lerpFactor;
            };

        smoothAngle(hmdAngLocal.x, m_HmdAngSmoothed.x);
        smoothAngle(hmdAngLocal.y, m_HmdAngSmoothed.y);
        smoothAngle(hmdAngLocal.z, m_HmdAngSmoothed.z);
    }
    else
    {
        m_HmdPosSmoothed = hmdPosCorrected;
        m_HmdAngSmoothed = hmdAngLocal;
    }

    auto wrapAngle = [](float angle)
        {
            return angle - 360.0f * std::floor((angle + 180.0f) / 360.0f);
        };

    m_HmdAngSmoothed.x = wrapAngle(m_HmdAngSmoothed.x);
    m_HmdAngSmoothed.y = wrapAngle(m_HmdAngSmoothed.y);
    m_HmdAngSmoothed.z = wrapAngle(m_HmdAngSmoothed.z);

    hmdPosSmoothed = m_HmdPosSmoothed;
    hmdAngSmoothed = m_HmdAngSmoothed;

    m_HmdPosCorrectedPrev = hmdPosCorrected;
    m_HmdPosLocalPrev = hmdPosLocal;

    QAngle::AngleVectors(hmdAngSmoothed, &m_HmdForward, &m_HmdRight, &m_HmdUp);

    m_HmdPosLocalInWorld = hmdPosSmoothed * m_VRScale;

    // Roomscale setup
    Vector cameraMovingDirection = m_SetupOrigin - m_SetupOriginPrev;
    Vector cameraToPlayer = m_HmdPosAbsPrev - m_SetupOriginPrev;
    cameraMovingDirection.z = 0;
    cameraToPlayer.z = 0;
    float cameraFollowing = DotProduct(cameraMovingDirection, cameraToPlayer);
    float cameraDistance = VectorLength(cameraToPlayer);

    if (localPlayer->m_hGroundEntity != -1 && localPlayer->m_vecVelocity.IsZero())
        m_RoomscaleActive = true;

    // TODO: Get roomscale to work while using thumbstick
    if ((cameraFollowing < 0 && cameraDistance > 1) || (m_PushingThumbstick))
        m_RoomscaleActive = false;

    if (!m_RoomscaleActive)
        m_CameraAnchor += m_SetupOrigin - m_SetupOriginPrev;

    m_CameraAnchor.z = m_SetupOrigin.z + m_HeightOffset;

    m_HmdPosAbs = m_CameraAnchor - Vector(0, 0, 64) + m_HmdPosLocalInWorld;

    // Check if camera is clipping inside wall
    CGameTrace trace;
    Ray_t ray;
    CTraceFilterSkipNPCsAndPlayers tracefilter((IHandleEntity*)localPlayer, 0);

    Vector extendedHmdPos = m_HmdPosAbs - m_SetupOrigin;
    VectorNormalize(extendedHmdPos);
    extendedHmdPos = m_HmdPosAbs + (extendedHmdPos * 10);
    ray.Init(m_SetupOrigin, extendedHmdPos);

    m_Game->m_EngineTrace->TraceRay(ray, STANDARD_TRACE_MASK, &tracefilter, &trace);
    if (trace.fraction < 1 && trace.fraction > 0)
    {
        Vector distanceInsideWall = trace.endpos - extendedHmdPos;
        m_CameraAnchor += distanceInsideWall;
        m_HmdPosAbs = m_CameraAnchor - Vector(0, 0, 64) + m_HmdPosLocalInWorld;
    }

    // Reset camera if it somehow gets too far
    m_SetupOriginToHMD = m_HmdPosAbs - m_SetupOrigin;
    if (VectorLength(m_SetupOriginToHMD) > 150)
        ResetPosition();

    m_HmdAngAbs = hmdAngSmoothed;

    m_HmdPosAbsPrev = m_HmdPosAbs;
    m_SetupOriginPrev = m_SetupOrigin;

    GetViewParameters();
    m_Ipd = m_EyeToHeadTransformPosRight.x * 2;
    m_EyeZ = m_EyeToHeadTransformPosRight.z;

    // Hand tracking
    Vector leftControllerPosLocal = m_LeftControllerPose.TrackedDevicePos;
    QAngle leftControllerAngLocal = m_LeftControllerPose.TrackedDeviceAng;

    Vector rightControllerPosLocal = m_RightControllerPose.TrackedDevicePos;
    QAngle rightControllerAngLocal = m_RightControllerPose.TrackedDeviceAng;

    Vector hmdToController = rightControllerPosLocal - hmdPosLocal;
    Vector rightControllerPosCorrected = hmdPosSmoothed + hmdToController;

    // When using stick turning, pivot the controllers around the HMD
    VectorPivotXY(rightControllerPosCorrected, hmdPosSmoothed, m_RotationOffset);

    Vector rightControllerPosLocalInWorld = rightControllerPosCorrected * m_VRScale;

    m_RightControllerPosAbs = m_CameraAnchor - Vector(0, 0, 64) + rightControllerPosLocalInWorld;

    rightControllerAngLocal.y += m_RotationOffset;
    // Wrap angle from -180 to 180
    rightControllerAngLocal.y -= 360 * std::floor((rightControllerAngLocal.y + 180) / 360);

    QAngle::AngleVectors(leftControllerAngLocal, &m_LeftControllerForward, &m_LeftControllerRight, &m_LeftControllerUp);
    QAngle::AngleVectors(rightControllerAngLocal, &m_RightControllerForward, &m_RightControllerRight, &m_RightControllerUp);

    // Adjust controller angle 45 degrees downward
    m_LeftControllerForward = VectorRotate(m_LeftControllerForward, m_LeftControllerRight, -45.0);
    m_LeftControllerUp = VectorRotate(m_LeftControllerUp, m_LeftControllerRight, -45.0);

    m_RightControllerForward = VectorRotate(m_RightControllerForward, m_RightControllerRight, -45.0);
    m_RightControllerUp = VectorRotate(m_RightControllerUp, m_RightControllerRight, -45.0);

    UpdateAimingLaser(localPlayer);

    // controller angles
    QAngle::VectorAngles(m_LeftControllerForward, m_LeftControllerUp, m_LeftControllerAngAbs);
    QAngle::VectorAngles(m_RightControllerForward, m_RightControllerUp, m_RightControllerAngAbs);

    PositionAngle viewmodelOffset = localPlayer->GetViewmodelOffset();

    m_ViewmodelPosOffset = viewmodelOffset.position;
    m_ViewmodelAngOffset = viewmodelOffset.angle;

    m_ViewmodelForward = m_RightControllerForward;
    m_ViewmodelUp = m_RightControllerUp;
    m_ViewmodelRight = m_RightControllerRight;

    // Viewmodel yaw offset
    m_ViewmodelForward = VectorRotate(m_ViewmodelForward, m_ViewmodelUp, m_ViewmodelAngOffset.y);
    m_ViewmodelRight = VectorRotate(m_ViewmodelRight, m_ViewmodelUp, m_ViewmodelAngOffset.y);

    // Viewmodel pitch offset
    m_ViewmodelForward = VectorRotate(m_ViewmodelForward, m_ViewmodelRight, m_ViewmodelAngOffset.x);
    m_ViewmodelUp = VectorRotate(m_ViewmodelUp, m_ViewmodelRight, m_ViewmodelAngOffset.x);

    // Viewmodel roll offset
    m_ViewmodelRight = VectorRotate(m_ViewmodelRight, m_ViewmodelForward, m_ViewmodelAngOffset.z);
    m_ViewmodelUp = VectorRotate(m_ViewmodelUp, m_ViewmodelForward, m_ViewmodelAngOffset.z);
}

void VR::UpdateAimingLaser(C_BasePlayer* localPlayer)
{
    if (!m_Game->m_DebugOverlay)
        return;

    C_WeaponCSBase* activeWeapon = nullptr;
    if (localPlayer)
        activeWeapon = static_cast<C_WeaponCSBase*>(localPlayer->GetActiveWeapon());

    if (!ShouldShowAimLine(activeWeapon))
    {
        m_LastAimDirection = Vector{ 0.0f, 0.0f, 0.0f };
        m_HasAimLine = false;
        return;
    }

    Vector direction = m_RightControllerForward;
    if (direction.IsZero())
    {
        if (m_LastAimDirection.IsZero())
        {
            if (!m_HasAimLine)
                return;

            DrawAimLine(m_AimLineStart, m_AimLineEnd);
            return;
        }

        direction = m_LastAimDirection;
    }
    else
    {
        m_LastAimDirection = direction;
    }
    VectorNormalize(direction);

    Vector origin = m_RightControllerPosAbs + direction * 2.0f;
    const float maxDistance = 8192.0f;
    Vector target = origin + direction * maxDistance;

    m_AimLineStart = origin;
    m_AimLineEnd = target;
    m_HasAimLine = true;

    DrawAimLine(origin, target);
}

bool VR::ShouldShowAimLine(C_WeaponCSBase* weapon) const
{
    if (!m_AimLineEnabled || !weapon)
        return false;

    switch (weapon->GetWeaponID())
    {
    case C_WeaponCSBase::PISTOL:
    case C_WeaponCSBase::MAGNUM:
    case C_WeaponCSBase::UZI:
    case C_WeaponCSBase::PUMPSHOTGUN:
    case C_WeaponCSBase::AUTOSHOTGUN:
    case C_WeaponCSBase::M16A1:
    case C_WeaponCSBase::HUNTING_RIFLE:
    case C_WeaponCSBase::MAC10:
    case C_WeaponCSBase::SHOTGUN_CHROME:
    case C_WeaponCSBase::SCAR:
    case C_WeaponCSBase::SNIPER_MILITARY:
    case C_WeaponCSBase::SPAS:
    case C_WeaponCSBase::AK47:
    case C_WeaponCSBase::MP5:
    case C_WeaponCSBase::SG552:
    case C_WeaponCSBase::AWP:
    case C_WeaponCSBase::SCOUT:
    case C_WeaponCSBase::GRENADE_LAUNCHER:
    case C_WeaponCSBase::M60:
        return true;
    default:
        return false;
    }
}

void VR::DrawAimLine(const Vector& start, const Vector& end)
{
    if (!m_Game->m_DebugOverlay)
        return;

    if (!m_AimLineEnabled)
        return;

    const float duration = std::max(m_AimLinePersistence, 0.0f);
    m_Game->m_DebugOverlay->AddLineOverlay(start, end, m_AimLineColorR, m_AimLineColorG, m_AimLineColorB, false, duration);

    const float thickness = std::max(m_AimLineThickness, 0.0f);
    if (thickness <= 0.0f)
        return;

    Vector forward = end - start;
    if (forward.IsZero())
        return;

    VectorNormalize(forward);

    Vector referenceUp = m_RightControllerUp;
    if (referenceUp.IsZero())
        referenceUp = Vector(0.0f, 0.0f, 1.0f);

    Vector basis1 = CrossProduct(forward, referenceUp);
    if (basis1.IsZero())
        basis1 = CrossProduct(forward, Vector(0.0f, 1.0f, 0.0f));
    if (basis1.IsZero())
        basis1 = CrossProduct(forward, Vector(1.0f, 0.0f, 0.0f));
    if (basis1.IsZero())
        return;

    VectorNormalize(basis1);
    Vector basis2 = CrossProduct(forward, basis1);
    if (basis2.IsZero())
        return;
    VectorNormalize(basis2);

    const int segments = 16;
    const float radius = thickness * 0.5f;
    const float twoPi = 6.28318530718f;

    for (int i = 0; i < segments; ++i)
    {
        const float angle0 = twoPi * static_cast<float>(i) / static_cast<float>(segments);
        const float angle1 = twoPi * static_cast<float>(i + 1) / static_cast<float>(segments);

        const float cos0 = std::cos(angle0);
        const float sin0 = std::sin(angle0);
        const float cos1 = std::cos(angle1);
        const float sin1 = std::sin(angle1);

        Vector offset0 = (basis1 * cos0 + basis2 * sin0) * radius;
        Vector offset1 = (basis1 * cos1 + basis2 * sin1) * radius;

        Vector start0 = start + offset0;
        Vector start1 = start + offset1;
        Vector end0 = end + offset0;
        Vector end1 = end + offset1;

        m_Game->m_DebugOverlay->AddTriangleOverlay(start0, start1, end1, m_AimLineColorR, m_AimLineColorG, m_AimLineColorB, m_AimLineColorA, false, duration);
        m_Game->m_DebugOverlay->AddTriangleOverlay(start0, end1, end0, m_AimLineColorR, m_AimLineColorG, m_AimLineColorB, m_AimLineColorA, false, duration);
    }
}

bool VR::UpdateThirdPersonViewState(const Vector& cameraOrigin, const Vector& cameraAnglesVec)
{
    const QAngle cameraAngles(cameraAnglesVec.x, cameraAnglesVec.y, cameraAnglesVec.z);
    if (!m_IsVREnabled)
    {
        m_IsThirdPersonCamera = false;
        m_ObserverThirdPerson = false;
        m_ThirdPersonHoldFrames = 0;
        m_ThirdPersonPoseInitialized = false;
        m_ThirdPersonViewOrigin = cameraOrigin;
        m_ThirdPersonViewAngles = cameraAngles;
        return false;
    }

    bool thirdPersonByObserver = m_ObserverThirdPerson;
    bool thirdPersonByDistance = false;

    if (!m_RoomscaleActive)
    {
        Vector delta = cameraOrigin - m_HmdPosAbs;
        const float horizontalDistance = std::sqrt((delta.x * delta.x) + (delta.y * delta.y));
        const float verticalDistance = std::fabs(delta.z);

        constexpr float horizontalThreshold = 36.0f;
        constexpr float verticalThreshold = 24.0f;

        if (horizontalDistance > horizontalThreshold || verticalDistance > verticalThreshold)
        {
            m_ThirdPersonHoldFrames = 6;
            thirdPersonByDistance = true;
        }
        else if (m_ThirdPersonHoldFrames > 0)
        {
            --m_ThirdPersonHoldFrames;
            thirdPersonByDistance = true;
        }
    }
    else
    {
        m_ThirdPersonHoldFrames = 0;
    }

    const bool thirdPerson = thirdPersonByObserver || thirdPersonByDistance;
    m_IsThirdPersonCamera = thirdPerson;
    if (!thirdPerson)
    {
        m_ObserverThirdPerson = false;
        m_ThirdPersonPoseInitialized = false;
        m_ThirdPersonViewOrigin = cameraOrigin;
        m_ThirdPersonViewAngles = cameraAngles;
        return thirdPerson;
    }

    float smoothingStrength = std::clamp(m_ThirdPersonCameraSmoothing, 0.0f, 0.99f);
    if (!m_ThirdPersonPoseInitialized)
    {
        m_ThirdPersonViewOrigin = cameraOrigin;
        m_ThirdPersonViewAngles = cameraAngles;
        m_ThirdPersonPoseInitialized = true;
    }

    if (smoothingStrength > 0.0f)
    {
        const float lerpFactor = 1.0f - smoothingStrength;

        m_ThirdPersonViewOrigin.x += (cameraOrigin.x - m_ThirdPersonViewOrigin.x) * lerpFactor;
        m_ThirdPersonViewOrigin.y += (cameraOrigin.y - m_ThirdPersonViewOrigin.y) * lerpFactor;
        m_ThirdPersonViewOrigin.z += (cameraOrigin.z - m_ThirdPersonViewOrigin.z) * lerpFactor;

        auto smoothAngle = [&](float target, float& current)
            {
                float diff = target - current;
                diff -= 360.0f * std::floor((diff + 180.0f) / 360.0f);
                current += diff * lerpFactor;
            };

        smoothAngle(cameraAngles.x, m_ThirdPersonViewAngles.x);
        smoothAngle(cameraAngles.y, m_ThirdPersonViewAngles.y);
        smoothAngle(cameraAngles.z, m_ThirdPersonViewAngles.z);
    }
    else
    {
        m_ThirdPersonViewOrigin = cameraOrigin;
        m_ThirdPersonViewAngles = cameraAngles;
    }

    auto wrapAngle = [](float angle)
        {
            return angle - 360.0f * std::floor((angle + 180.0f) / 360.0f);
        };

    m_ThirdPersonViewAngles.x = wrapAngle(m_ThirdPersonViewAngles.x);
    m_ThirdPersonViewAngles.y = wrapAngle(m_ThirdPersonViewAngles.y);
    m_ThirdPersonViewAngles.z = wrapAngle(m_ThirdPersonViewAngles.z);

    return thirdPerson;
}

Vector VR::GetViewAngle()
{
    return Vector(m_HmdAngAbs.x, m_HmdAngAbs.y, m_HmdAngAbs.z);
}

Vector VR::GetViewOriginLeft()
{
    Vector viewOriginLeft;

    if (m_IsThirdPersonCamera)
    {
        return m_ThirdPersonViewOrigin;
    }

    viewOriginLeft = m_HmdPosAbs + (m_HmdForward * (-(m_EyeZ * m_VRScale)));
    viewOriginLeft = viewOriginLeft + (m_HmdRight * (-((m_Ipd * m_IpdScale * m_VRScale) / 2)));

    return viewOriginLeft;
}

Vector VR::GetViewOriginRight()
{
    Vector viewOriginRight;

    if (m_IsThirdPersonCamera)
    {
        return m_ThirdPersonViewOrigin;
    }

    viewOriginRight = m_HmdPosAbs + (m_HmdForward * (-(m_EyeZ * m_VRScale)));
    viewOriginRight = viewOriginRight + (m_HmdRight * (m_Ipd * m_IpdScale * m_VRScale) / 2);

    return viewOriginRight;
}


void VR::ResetPosition()
{
    m_CameraAnchor += m_SetupOrigin - m_HmdPosAbs;
    m_HeightOffset += m_SetupOrigin.z - m_HmdPosAbs.z;
}

void VR::ParseConfigFile()
{
    std::ifstream configStream("VR\\config.txt");
    if (!configStream) {
        // 找不到就保持构造时的默认值
        return;
    }

    // 简单的 trim
    auto ltrim = [](std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            [](unsigned char ch) { return !std::isspace(ch); }));
        };
    auto rtrim = [](std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
            [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
        };
    auto trim = [&](std::string& s) { ltrim(s); rtrim(s); };

    std::unordered_map<std::string, std::string> userConfig;
    std::string line;
    while (std::getline(configStream, line))
    {
        // 去掉注释
        size_t cut = std::string::npos;
        size_t p1 = line.find("//");
        size_t p2 = line.find('#');
        size_t p3 = line.find(';');
        if (p1 != std::string::npos) cut = p1;
        if (p2 != std::string::npos) cut = (cut == std::string::npos) ? p2 : std::min(cut, p2);
        if (p3 != std::string::npos) cut = (cut == std::string::npos) ? p3 : std::min(cut, p3);
        if (cut != std::string::npos) line.erase(cut);

        trim(line);
        if (line.empty()) continue;

        // 解析 key=value
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        trim(key); trim(value);
        if (!key.empty())
            userConfig[key] = value;
    }

    // 小工具：带默认值的安全读取
    auto getBool = [&](const char* k, bool defVal)->bool {
        auto it = userConfig.find(k);
        if (it == userConfig.end()) return defVal;
        std::string v = it->second;
        // 转小写
        std::transform(v.begin(), v.end(), v.begin(), [](unsigned char c) { return std::tolower(c); });
        if (v == "1" || v == "true" || v == "on" || v == "yes") return true;
        if (v == "0" || v == "false" || v == "off" || v == "no")  return false;
        return defVal;
        };
    auto getFloat = [&](const char* k, float defVal)->float {
        auto it = userConfig.find(k);
        if (it == userConfig.end() || it->second.empty()) return defVal;
        try { return std::stof(it->second); }
        catch (...) { return defVal; }
        };
    auto getInt = [&](const char* k, int defVal)->int {
        auto it = userConfig.find(k);
        if (it == userConfig.end() || it->second.empty()) return defVal;
        try { return std::stoi(it->second); }
        catch (...) { return defVal; }
        };
    auto getColor = [&](const char* k, int defR, int defG, int defB, int defA)->std::array<int, 4> {
        std::array<int, 4> defaults{ defR, defG, defB, defA };
        auto it = userConfig.find(k);
        if (it == userConfig.end())
            return defaults;

        std::array<int, 4> color = defaults;
        std::stringstream ss(it->second);
        std::string token;
        int index = 0;
        while (std::getline(ss, token, ',') && index < 4)
        {
            trim(token);
            if (!token.empty())
            {
                try
                {
                    color[index] = std::clamp(std::stoi(token), 0, 255);
                }
                catch (...)
                {
                    color[index] = defaults[index];
                }
            }
            ++index;
        }

        for (int& component : color)
        {
            component = std::clamp(component, 0, 255);
        }

        return color;
        };

    // 用当前成员的值作为默认值（构造时已初始化）
    m_SnapTurning = getBool("SnapTurning", m_SnapTurning);
    m_SnapTurnAngle = getFloat("SnapTurnAngle", m_SnapTurnAngle);
    m_TurnSpeed = getFloat("TurnSpeed", m_TurnSpeed);
    m_LeftHanded = getBool("LeftHanded", m_LeftHanded);
    m_VRScale = getFloat("VRScale", m_VRScale);
    m_IpdScale = getFloat("IPDScale", m_IpdScale);
    m_HideArms = getBool("HideArms", m_HideArms);
    m_HudDistance = getFloat("HudDistance", m_HudDistance);
    m_HudSize = getFloat("HudSize", m_HudSize);
    m_HudAlwaysVisible = getBool("HudAlwaysVisible", m_HudAlwaysVisible);
    m_HeadSmoothing = std::clamp(getFloat("HeadSmoothing", m_HeadSmoothing), 0.0f, 0.99f);
    m_ThirdPersonCameraSmoothing = std::clamp(getFloat("ThirdPersonCameraSmoothing", m_ThirdPersonCameraSmoothing), 0.0f, 0.99f);
    m_AimLineThickness = std::max(0.0f, getFloat("AimLineThickness", m_AimLineThickness));
    m_AimLineEnabled = getBool("AimLineEnabled", m_AimLineEnabled);
    auto aimColor = getColor("AimLineColor", m_AimLineColorR, m_AimLineColorG, m_AimLineColorB, m_AimLineColorA);
    m_AimLineColorR = aimColor[0];
    m_AimLineColorG = aimColor[1];
    m_AimLineColorB = aimColor[2];
    m_AimLineColorA = aimColor[3];
    m_AimLinePersistence = std::max(0.0f, getFloat("AimLinePersistence", m_AimLinePersistence));
    m_ForceNonVRServerMovement = getBool("ForceNonVRServerMovement", m_ForceNonVRServerMovement);
    m_RequireSecondaryAttackForItemSwitch = getBool("RequireSecondaryAttackForItemSwitch", m_RequireSecondaryAttackForItemSwitch);
}

void VR::WaitForConfigUpdate()
{
    char currentDir[MAX_STR_LEN];
    GetCurrentDirectory(MAX_STR_LEN, currentDir);
    char configDir[MAX_STR_LEN];
    sprintf_s(configDir, MAX_STR_LEN, "%s\\VR\\", currentDir);
    HANDLE fileChangeHandle = FindFirstChangeNotificationA(configDir, false, FILE_NOTIFY_CHANGE_LAST_WRITE);

    std::filesystem::file_time_type configLastModified;
    while (1)
    {
        try
        {
            // Windows only notifies of change within a directory, so extra check here for just config.txt
            auto configModifiedTime = std::filesystem::last_write_time("VR\\config.txt");
            if (configModifiedTime != configLastModified)
            {
                configLastModified = configModifiedTime;
                ParseConfigFile();
            }
        }
        catch (const std::invalid_argument& e)
        {
            m_Game->errorMsg("Failed to parse config.txt");
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            m_Game->errorMsg("config.txt not found.");
            return;
        }

        FindNextChangeNotification(fileChangeHandle);
        WaitForSingleObject(fileChangeHandle, INFINITE);
        Sleep(100); // Sometimes the thread tries to read config.txt before it's finished writing
    }
}
