#include "vr.h"
#include <Windows.h>
#include "sdk.h"
#include "game.h"
#include "hooks.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <thread>

VR::VR(Game *game) {
    mGame = game;

    char g_errorString[MAX_STR_LEN];

    vr::HmdError error = vr::VRInitError_None;
    g_pSystem = vr::VR_Init(&error, vr::VRApplication_Scene);
    if (error != vr::VRInitError_None) {
        snprintf(g_errorString, MAX_STR_LEN, "VR_Init failed: %s", vr::VR_GetVRInitErrorAsEnglishDescription(error));
        Game::errorMsg(g_errorString);
        return;
    }

    vr::EVRInitError peError = vr::VRInitError_None;

    if (!vr::VRCompositor())
    {
        std::cout << "Compositor initialization failed." << std::endl;
        return;
    }

    g_pInput = vr::VRInput();


    g_pSystem = vr::OpenVRInternal_ModuleContext().VRSystem();

    g_pSystem->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);

    float l_left = 0.0f, l_right = 0.0f, l_top = 0.0f, l_bottom = 0.0f;
    g_pSystem->GetProjectionRaw(vr::EVREye::Eye_Left, &l_left, &l_right, &l_top, &l_bottom);

    float r_left = 0.0f, r_right = 0.0f, r_top = 0.0f, r_bottom = 0.0f;
    g_pSystem->GetProjectionRaw(vr::EVREye::Eye_Right, &r_left, &r_right, &r_top, &r_bottom);

    float tanHalfFov[2];

    tanHalfFov[0] = max(-l_left, l_right, -r_left, r_right);
    tanHalfFov[1] = max(-l_top, l_bottom, -r_top, r_bottom);

    m_sTextureBounds[0].uMin = 0.5f + 0.5f * l_left / tanHalfFov[0];
    m_sTextureBounds[0].uMax = 0.5f + 0.5f * l_right / tanHalfFov[0];
    m_sTextureBounds[0].vMin = 0.5f - 0.5f * l_bottom / tanHalfFov[1];
    m_sTextureBounds[0].vMax = 0.5f - 0.5f * l_top / tanHalfFov[1];

    m_sTextureBounds[1].uMin = 0.5f + 0.5f * r_left / tanHalfFov[0];
    m_sTextureBounds[1].uMax = 0.5f + 0.5f * r_right / tanHalfFov[0];
    m_sTextureBounds[1].vMin = 0.5f - 0.5f * r_bottom / tanHalfFov[1];
    m_sTextureBounds[1].vMax = 0.5f - 0.5f * r_top / tanHalfFov[1];

    m_nRenderWidth = m_nRenderWidth / max(m_sTextureBounds[0].uMax - m_sTextureBounds[0].uMin, m_sTextureBounds[1].uMax - m_sTextureBounds[1].uMin);
    m_nRenderHeight = m_nRenderHeight / max(m_sTextureBounds[0].vMax - m_sTextureBounds[0].vMin, m_sTextureBounds[1].vMax - m_sTextureBounds[1].vMin);

    m_nAspect = tanHalfFov[0] / tanHalfFov[1];
    m_nFov = 2.0f * atan(tanHalfFov[0]) * 360 / (3.14159265358979323846 * 2);

    SetActionManifest("action_manifest.json");

    QAngle::AngleVectors(ZeroAngle, &VR_playspace_forward, &VR_playspace_right, &VR_playspace_up);

    std::thread configParser(&VR::WaitForConfigUpdate, this);
    configParser.detach();
}



int VR::SetActionManifest(const char *fileName) {

    char currentDir[MAX_STR_LEN];
    GetCurrentDirectory(MAX_STR_LEN, currentDir);
    char path[MAX_STR_LEN];
    sprintf_s(path, MAX_STR_LEN, "%s\\VR\\SteamVRActionManifest\\%s", currentDir, fileName);

    g_pInput = vr::VRInput();
    if (g_pInput->SetActionManifestPath(path) != vr::VRInputError_None) {
        Game::errorMsg("SetActionManifestPath failed");
    }

    g_pInput->GetActionHandle("/actions/main/in/ActivateVR", &m_ActionActivateVR);
    g_pInput->GetActionHandle("/actions/main/in/Jump", &m_ActionJump);
    g_pInput->GetActionHandle("/actions/main/in/PrimaryAttack", &m_ActionPrimaryAttack);
    g_pInput->GetActionHandle("/actions/main/in/Reload", &m_ActionReload);
    g_pInput->GetActionHandle("/actions/main/in/Use", &m_ActionUse);
    g_pInput->GetActionHandle("/actions/main/in/Walk", &m_ActionWalk);
    g_pInput->GetActionHandle("/actions/main/in/Turn", &m_ActionTurn);
    g_pInput->GetActionHandle("/actions/main/in/SecondaryAttack", &m_ActionSecondaryAttack);
    g_pInput->GetActionHandle("/actions/main/in/NextItem", &m_ActionNextItem);
    g_pInput->GetActionHandle("/actions/main/in/PrevItem", &m_ActionPrevItem);
    g_pInput->GetActionHandle("/actions/main/in/ResetPosition", &m_ActionResetPosition);
    g_pInput->GetActionHandle("/actions/main/in/Crouch", &m_ActionCrouch);
    g_pInput->GetActionHandle("/actions/main/in/Flashlight", &m_ActionFlashlight);

    g_pInput->GetActionSetHandle("/actions/main", &m_ActionSet);
    m_ActiveActionSet.ulActionSet = m_ActionSet;

    // Get pose index for each controller
    vr::VRInputValueHandle_t leftControllerHandle{};
    vr::VRInputValueHandle_t rightControllerHandle{};
    g_pInput->GetInputSourceHandle("/user/hand/left", &leftControllerHandle);
    g_pInput->GetInputSourceHandle("/user/hand/right", &rightControllerHandle);

    vr::InputOriginInfo_t leftOriginInfo{};
    vr::InputOriginInfo_t rightOriginInfo{};
    g_pInput->GetOriginTrackedDeviceInfo(leftControllerHandle, &leftOriginInfo, sizeof(leftOriginInfo));
    g_pInput->GetOriginTrackedDeviceInfo(rightControllerHandle, &rightOriginInfo, sizeof(rightOriginInfo));

    m_LeftControllerIndex = leftOriginInfo.trackedDeviceIndex;
    m_RightControllerIndex = rightOriginInfo.trackedDeviceIndex;


    return 0;
}

void VR::GetPoseData(vr::TrackedDevicePose_t &poseRaw, TrackedDevicePoseData &poseOut)
{
    if (poseRaw.bPoseIsValid) {
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

void VR::GetPoses() 
{
    vr::InputPoseActionData_t poseActionData;
    vr::TrackedDevicePose_t pose;

    vr::TrackedDevicePose_t hmdPose = g_poses[vr::k_unTrackedDeviceIndex_Hmd];
    vr::TrackedDevicePose_t leftControllerPose = g_poses[m_LeftControllerIndex];
    vr::TrackedDevicePose_t rightControllerPose = g_poses[m_RightControllerIndex];

    GetPoseData(hmdPose, m_HmdPose);
    GetPoseData(leftControllerPose, m_LeftControllerPose);
    GetPoseData(rightControllerPose, m_RightControllerPose);
}

void VR::UpdatePosesAndActions() 
{
    vr::VRCompositor()->WaitGetPoses(g_poses, vr::k_unMaxTrackedDeviceCount, NULL, 0);
    g_pInput->UpdateActionState(&m_ActiveActionSet, sizeof(vr::VRActiveActionSet_t), 1);
}

void VR::GetViewParameters() 
{
    vr::HmdMatrix34_t eyeToHeadLeft = g_pSystem->GetEyeToHeadTransform(vr::Eye_Left);
    vr::HmdMatrix34_t eyeToHeadRight = g_pSystem->GetEyeToHeadTransform(vr::Eye_Right);
    eyeToHeadTransformPosLeft.x = eyeToHeadLeft.m[0][3];
    eyeToHeadTransformPosLeft.y = eyeToHeadLeft.m[1][3];
    eyeToHeadTransformPosLeft.z = eyeToHeadLeft.m[2][3];

    eyeToHeadTransformPosRight.x = eyeToHeadRight.m[0][3];
    eyeToHeadTransformPosRight.y = eyeToHeadRight.m[1][3];
    eyeToHeadTransformPosRight.z = eyeToHeadRight.m[2][3];
}


bool VR::PressedDigitalAction(vr::VRActionHandle_t &actionHandle)
{
    vr::InputDigitalActionData_t digitalActionData;
    vr::EVRInputError result = g_pInput->GetDigitalActionData(actionHandle, &digitalActionData, sizeof(digitalActionData), vr::k_ulInvalidInputValueHandle);
    
    if (result == vr::VRInputError_None)
        return digitalActionData.bState;

    return false;
}

bool VR::GetAnalogActionData(vr::VRActionHandle_t &actionHandle, vr::InputAnalogActionData_t &analogDataOut)
{
    vr::EVRInputError result = g_pInput->GetAnalogActionData(actionHandle, &analogDataOut, sizeof(analogDataOut), vr::k_ulInvalidInputValueHandle);

    if (result == vr::VRInputError_None)
        return true;

    return false;
}

void VR::ProcessInput()
{
    vr::InputAnalogActionData_t analogActionData;

    if (GetAsyncKeyState(VK_F6) || PressedDigitalAction(m_ActionActivateVR))
    {
        if (!isVREnabled)
        {
            mGame->mHooks->hkRenderView.enableHook();
            isVREnabled = true;
        }
    }

    if (!isVREnabled)
        return;

    typedef std::chrono::duration<float, std::milli> duration;
    auto currentTime = std::chrono::steady_clock::now();
    duration elapsed = currentTime - mPrevFrameTime;
    float deltaTime = elapsed.count();
    mPrevFrameTime = currentTime;

    if (GetAnalogActionData(m_ActionTurn, analogActionData))
    {
        if (mSnapTurning)
        {
            if (!mPressedTurn && analogActionData.x > 0.5)
            {
                mRotationOffset -= mSnapTurnAngle;
                mPressedTurn = true;
            }
            else if (!mPressedTurn && analogActionData.x < -0.5)
            {
                mRotationOffset += mSnapTurnAngle;
                mPressedTurn = true;
            }
            else if (analogActionData.x < 0.3 && analogActionData.x > -0.3)
                mPressedTurn = false;
        }
        // Smooth turning
        else
        {
            if (analogActionData.x > 0.2)
            {
                mRotationOffset -= mTurnSpeed * deltaTime * analogActionData.x;
            }
            if (analogActionData.x < -0.2)
            {
                mRotationOffset += mTurnSpeed * deltaTime * (-analogActionData.x);
            }
        }

        // Wrap from 0 to 360
        mRotationOffset -= 360 * std::floor(mRotationOffset / 360);
    }

    if (GetAnalogActionData(m_ActionWalk, analogActionData))		
    {
        if (analogActionData.y > 0.5)	
        {
            mGame->ClientCmd_Unrestricted("-back");
            mGame->ClientCmd_Unrestricted("+forward");
        }
        else if (analogActionData.y < -0.5)		
        {
            mGame->ClientCmd_Unrestricted("-forward");
            mGame->ClientCmd_Unrestricted("+back");
        }
        else
        {
            mGame->ClientCmd_Unrestricted("-back");
            mGame->ClientCmd_Unrestricted("-forward");
        }

        if (analogActionData.x > 0.5)		
        {
            mGame->ClientCmd_Unrestricted("-moveleft");
            mGame->ClientCmd_Unrestricted("+moveright");
        }
        else if (analogActionData.x < -0.5)		
        {
            mGame->ClientCmd_Unrestricted("-moveright");
            mGame->ClientCmd_Unrestricted("+moveleft");
        }
        else
        {
            mGame->ClientCmd_Unrestricted("-moveright");
            mGame->ClientCmd_Unrestricted("-moveleft");
        }
    }
    else
    {
        mGame->ClientCmd_Unrestricted("-forward");
        mGame->ClientCmd_Unrestricted("-back");
        mGame->ClientCmd_Unrestricted("-moveleft");
        mGame->ClientCmd_Unrestricted("-moveright");
    }

    if (PressedDigitalAction(m_ActionPrimaryAttack))
    {
        mGame->ClientCmd_Unrestricted("+attack");
    }
    else
    {
        mGame->ClientCmd_Unrestricted("-attack");
    }

    if (PressedDigitalAction(m_ActionJump))
    {
        mGame->ClientCmd_Unrestricted("+jump");
    }
    else
    {
        mGame->ClientCmd_Unrestricted("-jump");
    }

    if (PressedDigitalAction(m_ActionUse))
    {
        mGame->ClientCmd_Unrestricted("+use");
    }
    else
    {
        mGame->ClientCmd_Unrestricted("-use");
    }

    if (PressedDigitalAction(m_ActionReload))
    {
        mGame->ClientCmd_Unrestricted("+reload");
    }
    else
    {
        mGame->ClientCmd_Unrestricted("-reload");
    }

    if (PressedDigitalAction(m_ActionSecondaryAttack))
    {
        mGame->ClientCmd_Unrestricted("+attack2");
    }
    else
    {
        mGame->ClientCmd_Unrestricted("-attack2");
    }

    if (PressedDigitalAction(m_ActionPrevItem))
    {
        if (!mChangedItem)
        {
            mGame->ClientCmd_Unrestricted("invprev");
            mChangedItem = true;
        }
    }
    else if (PressedDigitalAction(m_ActionNextItem))
    {
        if (!mChangedItem)
        {
            mGame->ClientCmd_Unrestricted("invnext");
            mChangedItem = true;
        }
    }
    else
    {
        mChangedItem = false;
    }

    if (PressedDigitalAction(m_ActionResetPosition))
    {
        if (!mPressedLeftStick)
        {
            UpdateIntendedPosition();
            mPressedLeftStick = true;
        }
    }
    else
    {
        mPressedLeftStick = false;
    }

    if (PressedDigitalAction(m_ActionCrouch))
    {
        mGame->ClientCmd_Unrestricted("+duck");
    }
    else
    {
        mGame->ClientCmd_Unrestricted("-duck");
    }

    if (PressedDigitalAction(m_ActionFlashlight))
    {
        if (!mToggledFlashlight)
        {
            mGame->ClientCmd_Unrestricted("impulse 100");
            mToggledFlashlight = true;
        }
    }
    else
    {
        mToggledFlashlight = false;
    }

}

QAngle VR::GetRightControllerAbsAngle()
{
    return VR_controller_right_ang_abs;
}

Vector VR::GetRightControllerAbsPos()
{
    return VR_controller_right_pos_abs;
}


Vector VR::GetRecommendedViewmodelAbsPos()
{
    Vector result{};
    result = GetRightControllerAbsPos() - (VR_controller_right_forward_corrected * 20) -(VR_controller_right_right * 3);
    return result;
}

QAngle VR::GetRecommendedViewmodelAbsAngle()
{
    QAngle result{};
    QAngle::VectorAngles(VR_controller_right_forward_corrected, -VR_controller_right_up_corrected, result);
    return result;
}

void VR::UpdateTracking(Vector viewOrigin)
{
    GetPoses();

    CBaseEntity *localPlayer = mGame->GetClientEntity(1);
    if (!localPlayer)
        return;

    // HMD tracking
    QAngle VR_hmd_ang_local = m_HmdPose.TrackedDeviceAng;	
    Vector VR_hmd_pos_local = m_HmdPose.TrackedDevicePos;	

    Vector deltaPosition = VR_hmd_pos_local - VR_hmd_pos_local_prev;
    Vector VR_hmd_pos_corrected = VR_hmd_pos_corrected_prev + deltaPosition;

    VectorPivotXY(VR_hmd_pos_corrected, VR_hmd_pos_corrected_prev, mRotationOffset);

    VR_hmd_pos_corrected_prev = VR_hmd_pos_corrected;
    VR_hmd_pos_local_prev = VR_hmd_pos_local;

    VR_hmd_ang_local = VR_hmd_ang_local; 
    VR_hmd_ang_local.y += mRotationOffset;
    // Wrap angle from -180 to 180
    VR_hmd_ang_local.y -= 360 * std::floor((VR_hmd_ang_local.y + 180) / 360);

    QAngle::AngleVectors(VR_hmd_ang_local, &VR_hmd_forward, &VR_hmd_right, &VR_hmd_up);			

    QAngle::AngleVectors(ZeroAngle, &Player_forward, &Player_right, &Player_up);					

    VR_hmd_pos_local_in_world = VR_hmd_pos_corrected * VR_scale;

    Vector eyePos = viewOrigin;

    VR_hmd_pos_abs_no_offset = eyePos - Vector(0, 0, 64 * 52.49 / 39.37012415030996) + VR_hmd_pos_local_in_world;
    VR_hmd_pos_abs = VR_hmd_pos_abs_no_offset + intendedPositionOffset;	

    VR_hmd_ang_abs = VR_hmd_ang_local;

    GetViewParameters();
    ipd = eyeToHeadTransformPosRight.x * 2;
    eyeZ = eyeToHeadTransformPosRight.z;

    // Hand tracking
    Vector VR_controller_left_pos_local = m_LeftControllerPose.TrackedDevicePos;											
    QAngle VR_controller_left_ang_local = m_LeftControllerPose.TrackedDeviceAng;

    Vector VR_controller_right_pos_local = m_RightControllerPose.TrackedDevicePos;
    QAngle VR_controller_right_ang_local = m_RightControllerPose.TrackedDeviceAng;

    Vector hmdToController = VR_controller_right_pos_local - VR_hmd_pos_local;
    Vector VR_controller_right_pos_corrected = VR_hmd_pos_corrected + hmdToController;

    // When using stick turning, pivot the controllers around the HMD
    VectorPivotXY(VR_controller_right_pos_corrected, VR_hmd_pos_corrected, mRotationOffset);

    VR_controller_right_ang_local.y += mRotationOffset;
    // Wrap angle from -180 to 180
    VR_controller_right_ang_local.y -= 360 * std::floor((VR_controller_right_ang_local.y + 180) / 360);

    QAngle::AngleVectors(VR_controller_left_ang_local, &VR_controller_left_forward, &VR_controller_left_right, &VR_controller_left_up);			
    QAngle::AngleVectors(VR_controller_right_ang_local, &VR_controller_right_forward, &VR_controller_right_right, &VR_controller_right_up);			

    Vector VR_controller_left_pos_local_in_world = VR_controller_left_pos_local * VR_scale;
    Vector VR_controller_right_pos_local_in_world = VR_controller_right_pos_corrected * VR_scale;


    VR_controller_left_pos_abs = eyePos - Vector(0, 0, 64 * 52.49 / 39.37012415030996) + VR_controller_left_pos_local_in_world + intendedPositionOffset;		
    VR_controller_right_pos_abs = eyePos - Vector(0, 0, 64 * 52.49 / 39.37012415030996) + VR_controller_right_pos_local_in_world + intendedPositionOffset;	

    // controller angles
    VR_controller_left_ang_abs = VR_controller_left_ang_local;
    VR_controller_right_ang_abs = VR_controller_right_ang_local;

    // adjust controller angle 45 degrees down from its up vector
    Vector down = -VR_controller_right_up;
    VR_controller_right_forward_corrected = (VR_controller_right_forward + down) / 2;
    VectorNormalize(VR_controller_right_forward_corrected);
    CrossProduct(VR_controller_right_forward_corrected, VR_controller_right_right, VR_controller_right_up_corrected);

    // Process action input
    ProcessInput();

}

Vector VR::GetViewAngle()
{
    Vector result = { VR_hmd_ang_abs.x, VR_hmd_ang_abs.y, VR_hmd_ang_abs.z };
    return result;
}

Vector VR::GetViewOriginLeft()
{

    Vector view_temp_origin;

    view_temp_origin = VR_hmd_pos_abs + (VR_hmd_forward * (-(eyeZ * VR_scale)));
    view_temp_origin = view_temp_origin + (VR_hmd_right * (-((ipd * ipd_scale) / 2)));


    return view_temp_origin;
}

Vector VR::GetViewOriginRight()
{

    Vector view_temp_origin;

    view_temp_origin = VR_hmd_pos_abs + (VR_hmd_forward * (-(eyeZ * VR_scale)));
    view_temp_origin = view_temp_origin + (VR_hmd_right * (ipd * ipd_scale) / 2);

    return view_temp_origin;
}


void VR::UpdateIntendedPosition()
{
    CBasePlayer *localPlayer = (CBasePlayer *)(mGame->ClientEntityList->GetClientEntity(1));
    intendedPositionOffset = localPlayer->EyePosition() - VR_hmd_pos_abs_no_offset;
}

void VR::ParseConfigFile()
{
    std::ifstream configStream("VR\\config.txt");
    std::unordered_map<std::string, std::string> userConfig;

    std::string line;
    while (std::getline(configStream, line))
    {
        std::istringstream sLine(line);
        std::string key;
        if (std::getline(sLine, key, '='))
        {
            std::string value;
            if (std::getline(sLine, value))
                userConfig[key] = value;
        }
    }

    mSnapTurning = userConfig["SnapTurning"] == "true";
    mSnapTurnAngle = std::stof(userConfig["SnapTurnAngle"]);
    ipd_scale = std::stof(userConfig["IpdScale"]);
    mTurnSpeed = std::stof(userConfig["TurnSpeed"]);
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
        catch (const std::invalid_argument &e)
        {
            mGame->errorMsg("Failed to parse config.txt");
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            mGame->errorMsg("config.txt not found.");
            return;
        }
        
        FindNextChangeNotification(fileChangeHandle);
        WaitForSingleObject(fileChangeHandle, INFINITE);
    }
}