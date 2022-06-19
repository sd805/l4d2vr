//========= Copyright Valve Corporation, All rights reserved. ============//

#pragma once

#include <math.h>
#include "usercmd.h"
#include "material.h"

class IClientEntityList
{
public:
    // Get IClientNetworkable interface for specified entity
    virtual void *GetClientNetworkable(int entnum) = 0;
    virtual void *GetClientNetworkableFromHandle(int hEnt) = 0;
    virtual void *GetClientUnknownFromHandle(int hEnt) = 0;

    // NOTE: This function is only a convenience wrapper.
    // It returns GetClientNetworkable( entnum )->GetIClientEntity().
    virtual void *GetClientEntity(int entnum) = 0;
    virtual void *GetClientEntityFromHandle(int hEnt) = 0;

    // Returns number of entities currently in use
    virtual int					NumberOfEntities(bool bIncludeNonNetworkable) = 0;

    // Returns highest index actually used
    virtual int					GetHighestEntityIndex(void) = 0;

    // Sizes entity list to specified size
    virtual void				SetMaxEntities(int maxents) = 0;
    virtual int					GetMaxEntities() = 0;
};



typedef struct player_info_s
{
	char pad_0x00[0x10];
	char            name[128];
	int                userID;
	char            guid[33];
	unsigned long    friendsID;
	char            friendsName[128];
	bool            fakeplayer;
	bool            ishltv;
	unsigned int    customFiles[4];
	unsigned char    filesDownloaded;
	char pad_big[0x200];
} player_info_t;


class IClientEngineTools
{
public:
	virtual void *fn0() = 0;
	virtual void *fn1() = 0;
	virtual void *fn2() = 0;
	virtual void *fn3() = 0;
	virtual void *fn4() = 0;
	virtual void *fn5() = 0;
	virtual void *fn6() = 0;
	virtual void *fn7() = 0;
	virtual void *fn8() = 0;
	virtual void *fn9() = 0;
	virtual void *fn10() = 0;
	virtual void *fn11() = 0;
	virtual void *fn12() = 0;
	virtual bool IsThirdPersonCamera() = 0;
};

class IEngineClient
{
public:
	virtual void *fn0() = 0;
	virtual void *fn1() = 0;
	virtual void *fn2() = 0;
	virtual void *fn3() = 0;
	virtual void *fn4() = 0;
	virtual void *fn5() = 0;
	virtual void *fn6() = 0;
	virtual void *ClientCmd(const char *szCmdString) = 0;
	virtual bool GetPlayerInfo(int ent_num, player_info_t *pinfo) = 0;
	virtual int GetPlayerForUserID(int userID) = 0;
	virtual void *fn10() = 0;
	virtual bool Con_IsVisible() = 0;
	virtual int GetLocalPlayer(void) = 0;
	virtual int fn13() = 0;
	virtual int fn14() = 0;
	virtual int fn15() = 0;
	virtual int fn16() = 0;
	virtual int fn17() = 0;
	virtual int fn18() = 0;
	virtual QAngle* GetViewAngles(QAngle &angle) = 0;
	virtual QAngle* SetViewAngles(QAngle &angle) = 0;
	virtual void *sub_1005D230() = 0;
	virtual void *sub_1005D280() = 0;
	virtual void *sub_1005D2A0() = 0;
	virtual void *sub_1005D2C0() = 0;
	virtual void *sub_1005D2D0() = 0;
	virtual bool IsInGame() = 0;
	virtual void *sub_1005F150() = 0;
	virtual void *sub_1005D2F0() = 0;
	virtual void *sub_1005D300() = 0;
	virtual void *sub_1005D350() = 0;
	virtual void *sub_1005D3B0() = 0;
	virtual void *sub_1005D430() = 0;
	virtual void *sub_1005D460() = 0;
	virtual void *sub_1005D5A0() = 0;
	virtual void *sub_1005D4F0() = 0;
	virtual void *sub_1005D5B0() = 0;
	virtual void *sub_1005D5C0() = 0;
	virtual void *sub_1005D5D0() = 0;
	virtual void *sub_1005D5E0() = 0;
	virtual void *sub_1005D600() = 0;
	virtual void *sub_1005D620() = 0;
	virtual void *sub_1005D640() = 0;
	virtual void *sub_1005D710() = 0;
	virtual void *sub_1005D720() = 0;
	virtual void *sub_1005D790() = 0;
	virtual void *sub_1005D7C0() = 0;
	virtual void *sub_1005D840() = 0;
	virtual void *sub_1005D880() = 0;
	virtual void *sub_1005D890() = 0;
	virtual void *sub_1005D8A0() = 0;
	virtual void *sub_1005D8D0() = 0;
	virtual void *sub_1005F160() = 0;
	virtual void *sub_1005F190() = 0;
	virtual void *sub_1005DA90() = 0;
	virtual void *sub_1005DAA0() = 0;
	virtual void *sub_1005DAB0() = 0;
	virtual void *sub_1005DAC0() = 0;
	virtual void *sub_1005DAE0() = 0;
	virtual void *sub_1005DB40() = 0;
	virtual void *sub_1005DB70() = 0;
	virtual void *sub_1005DBA0() = 0;
	virtual void *sub_1005DBC0() = 0;
	virtual void *sub_1005DBE0() = 0;
	virtual void *sub_1005DC10() = 0;
	virtual void *sub_1005DC40() = 0;
	virtual void *sub_1005DC60() = 0;
	virtual void *sub_1005DC80() = 0;
	virtual void *sub_1005DCA0() = 0;
	virtual void *sub_1005D7E0() = 0;
	virtual void *sub_1005DCC0() = 0;
	virtual void *sub_1005DCE0() = 0;
	virtual void *sub_1005DD00() = 0;
	virtual void *sub_1005DD20() = 0;
	virtual void *sub_1005DD40() = 0;
	virtual void *sub_100606A0() = 0;
	virtual void *sub_10060670() = 0;
	virtual void *sub_1005DAD0() = 0;
	virtual void *sub_1005D7B0() = 0;
	virtual void *sub_1005DD50() = 0;
	virtual void *sub_1005DD60() = 0;
	virtual void *sub_1005DD70() = 0;
	virtual void *sub_1005DDE0() = 0;
	virtual void *sub_1005DDF0() = 0;
	virtual void *sub_1005DE00() = 0;
	virtual void *sub_1005DE10() = 0;
	virtual void *sub_1005DE20() = 0;
	virtual void *sub_1005DD80() = 0;
	virtual void *sub_1005DD90() = 0;
	virtual void *sub_1005DDA0() = 0;
	virtual void *sub_1005DE30() = 0;
	virtual void *sub_1005D900() = 0;
	virtual void *sub_1005D910() = 0;
	virtual void *sub_1005D9B0() = 0;
	virtual void *sub_1005DE40() = 0;
	virtual void *sub_1005F1C0() = 0;
	virtual void *sub_1005DE80() = 0;
	virtual void *sub_1005DE90() = 0;
	virtual void *sub_1005DEB0() = 0;
	virtual void *sub_1005CB30() = 0;
	virtual void *sub_1005CB40() = 0;
	virtual void *sub_1005DF00() = 0;
	virtual void *sub_1005DF20() = 0;
	virtual void *sub_1005CF70() = 0;
	virtual void *sub_1005D000() = 0;
	virtual void *sub_1005DED0() = 0;
	virtual void *sub_1005CBB0() = 0;
	virtual void *ClientCmd_Unrestricted(const char *szCmdString) = 0;
};



enum StereoEye_t
{
	STEREO_EYE_MONO = 0,
	STEREO_EYE_LEFT = 1,
	STEREO_EYE_RIGHT = 2,
	STEREO_EYE_MAX = 3,
};

class VMatrix;


class CViewSetup
{
public:
	int32_t x; //0x0000
	int32_t m_nUnscaledX; //0x0004
	int32_t y; //0x0008
	int32_t m_nUnscaledY; //0x000C
	int32_t width; //0x0010
	int32_t m_nUnscaledWidth; //0x0014
	int32_t height; //0x0018
	int32_t m_nUnscaledHeight; //0x001C
	char pad_0020[20]; //0x0020
	float fov; //0x0034
	float fovViewmodel; //0x0038
	Vector origin; //0x003C
	Vector angles; //0x0048
	float zNear; //0x0054
	float zFar; //0x0058
	float zNearViewmodel; //0x005C
	float zFarViewmodel; //0x0060
	float m_flAspectRatio; //0x0064
	char pad_0068[1660]; //0x0068
}; //Size: 0x06E4
static_assert(sizeof(CViewSetup) == 0x6E4);

class IBaseClientDLL
{
	virtual int				Connect(void* appSystemFactory, void *pGlobals) = 0;

	virtual int				Disconnect(void) = 0;

	// run other init code here
	virtual int				Init(void* appSystemFactory, void *pGlobals) = 0;

	virtual void			PostInit() = 0;

	// Called once when the client DLL is being unloaded
	virtual void			Shutdown(void) = 0;

	// Called at the start of each level change
	virtual void			LevelInitPreEntity(char const *pMapName) = 0;
	// Called at the start of a new level, after the entities have been received and created
	virtual void			LevelInitPostEntity() = 0;

	virtual void			LevelFastReload(void) = 0;

	// Called at the end of a level
	virtual void			LevelShutdown(void) = 0;

	// Request a pointer to the list of client datatable classes
	virtual void *GetAllClasses(void) = 0;

	// Called once per level to re-initialize any hud element drawing stuff
	virtual int				HudVidInit(void) = 0;
	// Called by the engine when gathering user input
	virtual void			HudProcessInput(bool bActive) = 0;
	// Called oncer per frame to allow the hud elements to think
	virtual void			HudUpdate(bool bActive) = 0;
	// Reset the hud elements to their initial states
	virtual void			HudReset(void) = 0;
	// Display a hud text message
	virtual void			HudText(const char *message) = 0;

	// Mouse Input Interfaces
	// Activate the mouse (hides the cursor and locks it to the center of the screen)
	virtual void			IN_ActivateMouse(void) = 0;
	// Deactivates the mouse (shows the cursor and unlocks it)
	virtual void			IN_DeactivateMouse(void) = 0;
	// This is only called during extra sound updates and just accumulates mouse x, y offets and recenters the mouse.
	//  This call is used to try to prevent the mouse from appearing out of the side of a windowed version of the engine if 
	//  rendering or other processing is taking too long
	virtual void			IN_Accumulate(void) = 0;
	// Reset all key and mouse states to their initial, unpressed state
	virtual void			IN_ClearStates(void) = 0;
	// If key is found by name, returns whether it's being held down in isdown, otherwise function returns false
	virtual bool			IN_IsKeyDown(const char *name, bool &isdown) = 0;
	// Notify the client that the mouse was wheeled while in game - called prior to executing any bound commands.
	virtual void			IN_OnMouseWheeled(int nDelta) = 0;
	// Raw keyboard signal, if the client .dll returns 1, the engine processes the key as usual, otherwise,
	//  if the client .dll returns 0, the key is swallowed.
	virtual int				IN_KeyEvent(void) = 0;

	// This function is called once per tick to create the player CUserCmd (used for prediction/physics simulation of the player)
	// Because the mouse can be sampled at greater than the tick interval, there is a separate input_sample_frametime, which
	//  specifies how much additional mouse / keyboard simulation to perform.
	virtual void			CreateMove(
		int sequence_number,			// sequence_number of this cmd
		float input_sample_frametime,	// Frametime for mouse input sampling
		bool active) = 0;				// True if the player is active (not paused)

// If the game is running faster than the tick_interval framerate, then we do extra mouse sampling to avoid jittery input
//  This code path is much like the normal move creation code, except no move is created
	virtual void			ExtraMouseSample(float frametime, bool active) = 0;

	// Encode the delta (changes) between the CUserCmd in slot from vs the one in slot to.  The game code will have
	//  matching logic to read the delta.
	virtual bool			WriteUsercmdDeltaToBuffer(void) = 0;
	// Demos need to be able to encode/decode CUserCmds to memory buffers, so these functions wrap that
	virtual void			EncodeUserCmdToBuffer(void) = 0;
	virtual void			DecodeUserCmdFromBuffer(void) = 0;

	// Set up and render one or more views (e.g., rear view window, etc.).  This called into RenderView below
	virtual void			View_Render(void) = 0;

	// Allow engine to expressly render a view (e.g., during timerefresh)
	// See IVRenderView.h, PushViewFlags_t for nFlags values
	virtual void			RenderView(const CViewSetup &view, int nClearFlags, int whatToDraw) = 0;
};

class IViewRender
{
public:
	// SETUP
	// Initialize view renderer
	virtual void		Init(void) = 0;

	// Clear any systems between levels
	virtual void		LevelInit(void) = 0;
	virtual void		LevelShutdown(void) = 0;

	// Shutdown
	virtual void		Shutdown(void) = 0;

	// RENDERING
	// Called right before simulation. It must setup the view model origins and angles here so 
	// the correct attachment points can be used during simulation.	
	virtual void		OnRenderStart() = 0;

	// Called to render the entire scene
	virtual	void		Render(void) = 0;

	// Called to render just a particular setup ( for timerefresh and envmap creation )
	virtual void		RenderView(const CViewSetup &view, int nClearFlags, int whatToDraw) = 0;

	// What are we currently rendering? Returns a combination of DF_ flags.
	virtual int GetDrawFlags() = 0;

	// MISC
	// Start and stop pitch drifting logic
	virtual void		StartPitchDrift(void) = 0;
	virtual void		StopPitchDrift(void) = 0;

	// This can only be called during rendering (while within RenderView).
	virtual VPlane *GetFrustum() = 0;

	virtual bool		ShouldDrawBrushModels(void) = 0;

	virtual const CViewSetup *GetPlayerViewSetup(void) const = 0;
	virtual const CViewSetup *GetViewSetup(void) const = 0;

	virtual void		DisableVis(void) = 0;

	virtual int			BuildWorldListsNumber() const = 0;

	virtual void		SetCheapWaterStartDistance(float flCheapWaterStartDistance) = 0;
	virtual void		SetCheapWaterEndDistance(float flCheapWaterEndDistance) = 0;

	virtual void		GetWaterLODParams(float &flCheapWaterStartDistance, float &flCheapWaterEndDistance) = 0;

	virtual void		DriftPitch(void) = 0;

	virtual void		SetScreenOverlayMaterial(void) = 0;
	virtual void *GetScreenOverlayMaterial() = 0;

	virtual void		WriteSaveGameScreenshot(const char *pFilename) = 0;
	virtual void		WriteSaveGameScreenshotOfSize(const char *pFilename, int width, int height, bool bCreatePowerOf2Padded = false, bool bWriteVTF = false) = 0;

	virtual void		WriteReplayScreenshot(void) = 0;
	virtual void		UpdateReplayScreenshotCache() = 0;

	// Draws another rendering over the top of the screen
	virtual void		QueueOverlayRenderView(const CViewSetup &view, int nClearFlags, int whatToDraw) = 0;

	// Returns znear and zfar
	virtual float		GetZNear() = 0;
	virtual float		GetZFar() = 0;

	virtual void		GetScreenFadeDistances(float *min, float *max) = 0;

	virtual void *GetCurrentlyDrawingEntity() = 0;
	virtual void		SetCurrentlyDrawingEntity(void) = 0;

	virtual bool		UpdateShadowDepthTexture(void) = 0;

	virtual void		FreezeFrame(float flFreezeTime) = 0;

	virtual void *GetReplayScreenshotSystem() = 0;
};

class IClientMode
{
	// Misc.
	public:

		virtual			~IClientMode() {}

		// Called before the HUD is initialized.
		virtual void	InitViewport() = 0;

		// One time init when .dll is first loaded.
		virtual void	Init() = 0;

		// Called when vgui is shutting down.
		virtual void	VGui_Shutdown() = 0;

		// One time call when dll is shutting down
		virtual void	Shutdown() = 0;

		// Called when switching from one IClientMode to another.
		// This can re-layout the view and such.
		// Note that Enable and Disable are called when the DLL initializes and shuts down.
		virtual void	Enable() = 0;

		// Called when it's about to go into another client mode.
		virtual void	Disable() = 0;

		// Called when initializing or when the view changes.
		// This should move the viewport into the correct position.
		virtual void	Layout() = 0;

		// Gets at the viewport, if there is one...
		virtual void *GetViewport() = 0;

		// Gets at the viewports vgui panel animation controller, if there is one...
		virtual void *GetViewportAnimationController() = 0;

		// called every time shared client dll/engine data gets changed,
		// and gives the cdll a chance to modify the data.
		virtual void	ProcessInput(bool bActive) = 0;

		// The mode can choose to draw/not draw entities.
		virtual bool	ShouldDrawDetailObjects() = 0;
		virtual bool	ShouldDrawEntity(void *pEnt) = 0;
		virtual bool	ShouldDrawLocalPlayer(void *pPlayer) = 0;
		virtual bool	ShouldDrawParticles() = 0;

		// The mode can choose to not draw fog
		virtual bool	ShouldDrawFog(void) = 0;

		virtual void	OverrideView(void *pSetup) = 0;
		virtual int		KeyInput(int down, int keynum, const char *pszCurrentBinding) = 0;
		virtual void	StartMessageMode(int iMessageModeType) = 0;
		virtual void *GetMessagePanel() = 0;
		virtual void	OverrideMouseInput(float *x, float *y) = 0;
		virtual bool	CreateMove(float flInputSampleTime, void *cmd) = 0;

		virtual void	LevelInit(const char *newmap) = 0;
		virtual void	LevelShutdown(void) = 0;

		// Certain modes hide the view model
		virtual bool	ShouldDrawViewModel(void) = 0;
		virtual bool	ShouldDrawCrosshair(void) = 0;

		// Let mode override viewport for engine
		virtual void	AdjustEngineViewport(int &x, int &y, int &width, int &height) = 0;

		// Called before rendering a view.
		virtual void	PreRender(void *pSetup) = 0;

		// Called after everything is rendered.
		virtual void	PostRender(void) = 0;

		virtual void	PostRenderVGui() = 0;

		virtual void	ActivateInGameVGuiContext(void *pPanel) = 0;
		virtual void	DeactivateInGameVGuiContext() = 0;
		virtual float	GetViewModelFOV(void) = 0;

		virtual bool	CanRecordDemo(char *errorMsg, int length) const = 0;

		virtual void	ComputeVguiResConditions(void *pkvConditions) = 0;

		//=============================================================================
		// HPE_BEGIN:
		// [menglish] Save server information shown to the client in a persistent place
		//=============================================================================

		virtual wchar_t *GetServerName() = 0;
		virtual void SetServerName(wchar_t *name) = 0;
		virtual wchar_t *GetMapName() = 0;
		virtual void SetMapName(wchar_t *name) = 0;

		//=============================================================================
		// HPE_END
		//=============================================================================

		virtual bool	DoPostScreenSpaceEffects(const void *pSetup) = 0;

		virtual void	DisplayReplayMessage(const char *pLocalizeName, float flDuration, bool bUrgent,
											  const char *pSound, bool bDlg) = 0;

		// Updates.
		public:

			// Called every frame.
			virtual void	Update() = 0;

			// Returns true if VR mode should black out everything around the UI
			virtual bool	ShouldBlackoutAroundHUD() = 0;

			// Returns true if VR mode should black out everything around the UI
			virtual int ShouldOverrideHeadtrackControl() = 0;

			virtual bool	IsInfoPanelAllowed() = 0;
			virtual void	InfoPanelDisplayed() = 0;
			virtual bool	IsHTMLInfoPanelAllowed() = 0;
};

#define  AssertMsg( _exp, _msg, ... )						((void)0)

template <class T>
class CBaseAutoPtr
{
public:
	CBaseAutoPtr() : m_pObject(0) {}
	CBaseAutoPtr(T *pFrom) : m_pObject(pFrom) {}

	operator const void *() const { return m_pObject; }
	operator void *() { return m_pObject; }

	operator const T *() const { return m_pObject; }
	operator const T *() { return m_pObject; }
	operator T *() { return m_pObject; }

	int			operator=(int i) { AssertMsg(i == 0, "Only NULL allowed on integer assign"); m_pObject = 0; return 0; }
	T *operator=(T *p) { m_pObject = p; return p; }

	bool        operator !() const { return (!m_pObject); }
	bool        operator!=(int i) const { AssertMsg(i == 0, "Only NULL allowed on integer compare"); return (m_pObject != NULL); }
	bool		operator==(const void *p) const { return (m_pObject == p); }
	bool		operator!=(const void *p) const { return (m_pObject != p); }
	bool		operator==(T *p) const { return operator==((void *)p); }
	bool		operator!=(T *p) const { return operator!=((void *)p); }
	bool		operator==(const CBaseAutoPtr<T> &p) const { return operator==((const void *)p); }
	bool		operator!=(const CBaseAutoPtr<T> &p) const { return operator!=((const void *)p); }

	T *operator->() { return m_pObject; }
	T &operator *() { return *m_pObject; }
	T **operator &() { return &m_pObject; }

	const T *operator->() const { return m_pObject; }
	const T &operator *() const { return *m_pObject; }
	T *const *operator &() const { return &m_pObject; }

protected:
	CBaseAutoPtr(const CBaseAutoPtr<T> &from) : m_pObject(from.m_pObject) {}
	void operator=(const CBaseAutoPtr<T> &from) { m_pObject = from.m_pObject; }

	T *m_pObject;
};

template <class T>
class CRefPtr : public CBaseAutoPtr<T>
{
	typedef CBaseAutoPtr<T> BaseClass;
public:
	CRefPtr() {}
	CRefPtr(T *pInit) : BaseClass(pInit) {}
	CRefPtr(const CRefPtr<T> &from) : BaseClass(from) {}
	~CRefPtr() { if (BaseClass::m_pObject) BaseClass::m_pObject->Release(); }

	void operator=(const CRefPtr<T> &from) { BaseClass::operator=(from); }

	int operator=(int i) { return BaseClass::operator=(i); }
	T *operator=(T *p) { return BaseClass::operator=(p); }

	operator bool() const { return !BaseClass::operator!(); }
	operator bool() { return !BaseClass::operator!(); }

	void SafeRelease() { if (BaseClass::m_pObject) BaseClass::m_pObject->Release(); BaseClass::m_pObject = 0; }
	void AssignAddRef(T *pFrom) { SafeRelease(); if (pFrom) pFrom->AddRef(); BaseClass::m_pObject = pFrom; }
	void AddRefAssignTo(T *&pTo) { ::SafeRelease(pTo); if (BaseClass::m_pObject) BaseClass::m_pObject->AddRef(); pTo = BaseClass::m_pObject; }
};

class IMaterial
{
public:
	// Get the name of the material.  This is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	virtual const char *GetName() const = 0;
	virtual const char *GetTextureGroupName() const = 0;

	// Get the preferred size/bitDepth of a preview image of a material.
	// This is the sort of image that you would use for a thumbnail view
	// of a material, or in WorldCraft until it uses materials to render.
	// separate this for the tools maybe
	virtual void * GetPreviewImageProperties(int *width, int *height,
							ImageFormat *imageFormat, bool *isTranslucent) const = 0;

	// Get a preview image at the specified width/height and bitDepth.
	// Will do resampling if necessary.(not yet!!! :) )
	// Will do color format conversion. (works now.)
	virtual void* GetPreviewImage(unsigned char *data,
												 int width, int height,
												 ImageFormat imageFormat) const = 0;
	// 
	virtual int				GetMappingWidth() = 0;
	virtual int				GetMappingHeight() = 0;

	virtual int				GetNumAnimationFrames() = 0;

	// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
	virtual bool			InMaterialPage(void) = 0;
	virtual	void			GetMaterialOffset(float *pOffset) = 0;
	virtual void			GetMaterialScale(float *pScale) = 0;
	virtual IMaterial *GetMaterialPage(void) = 0;

	// find a vmt variable.
	// This is how game code affects how a material is rendered.
	// The game code must know about the params that are used by
	// the shader for the material that it is trying to affect.
	virtual void *FindVar(const char *varName, bool *found, bool complain = true) = 0;

	// The user never allocates or deallocates materials.  Reference counting is
	// used instead.  Garbage collection is done upon a call to 
	// IMaterialSystem::UncacheUnusedMaterials.
	virtual void			IncrementReferenceCount(void) = 0;
	virtual void			DecrementReferenceCount(void) = 0;

	inline void AddRef() { IncrementReferenceCount(); }
	inline void Release() { DecrementReferenceCount(); }

	// Each material is assigned a number that groups it with like materials
	// for sorting in the application.
	virtual int 			GetEnumerationID(void) const = 0;

	virtual void			GetLowResColorSample(float s, float t, float *color) const = 0;

	// This computes the state snapshots for this material
	virtual void			RecomputeStateSnapshots() = 0;

	// Are we translucent?
	virtual bool			IsTranslucent() = 0;

	// Are we alphatested?
	virtual bool			IsAlphaTested() = 0;

	// Are we vertex lit?
	virtual bool			IsVertexLit() = 0;

	// Gets the vertex format
	virtual void *	GetVertexFormat() const = 0;

	// returns true if this material uses a material proxy
	virtual bool			HasProxy(void) const = 0;

	virtual bool			UsesEnvCubemap(void) = 0;

	virtual bool			NeedsTangentSpace(void) = 0;

	virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;

	// returns true if the shader doesn't do skinning itself and requires
	// the data that is sent to it to be preskinned.
	virtual bool			NeedsSoftwareSkinning(void) = 0;

	// Apply constant color or alpha modulation
	virtual void			AlphaModulate(float alpha) = 0;
	virtual void			ColorModulate(float r, float g, float b) = 0;

	// Material Var flags...
	virtual void			SetMaterialVarFlag(void) = 0;
	virtual bool			GetMaterialVarFlag(void) const = 0;

	// Gets material reflectivity
	virtual void			GetReflectivity(Vector &reflect) = 0;

	// Gets material property flags
	virtual bool			GetPropertyFlag(void) = 0;

	// Is the material visible from both sides?
	virtual bool			IsTwoSided() = 0;

	// Sets the shader associated with the material
	virtual void			SetShader(const char *pShaderName) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetNumPasses(void) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetTextureMemoryBytes(void) = 0;

	// Meant to be used with materials created using CreateMaterial
	// It updates the materials to reflect the current values stored in the material vars
	virtual void			Refresh() = 0;

	// GR - returns true is material uses lightmap alpha for blending
	virtual bool			NeedsLightmapBlendAlpha(void) = 0;

	// returns true if the shader doesn't do lighting itself and requires
	// the data that is sent to it to be prelighted
	virtual bool			NeedsSoftwareLighting(void) = 0;

	// Gets at the shader parameters
	virtual int				ShaderParamCount() const = 0;
	virtual void **GetShaderParams(void) = 0;

	// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
	// the material can't be found.
	virtual bool			IsErrorMaterial() const = 0;

	virtual void			SetUseFixedFunctionBakedLighting(bool bEnable) = 0;

	// Gets the current alpha modulation
	virtual float			GetAlphaModulation() = 0;
	virtual void			GetColorModulation(float *r, float *g, float *b) = 0;

	// Gets the morph format
	virtual void	GetMorphFormat() const = 0;

	// fast find that stores the index of the found var in the string table in local cache
	virtual void *FindVarFast(char const *pVarName, unsigned int *pToken) = 0;

	// Sets new VMT shader parameters for the material
	virtual void			SetShaderAndParams(void) = 0;
	virtual const char *GetShaderName() const = 0;

	virtual void			DeleteIfUnreferenced() = 0;

	virtual bool			IsSpriteCard() = 0;

	virtual void			CallBindProxy(void *proxyData) = 0;

	virtual IMaterial *CheckProxyReplacement(void *proxyData) = 0;

	virtual void			RefreshPreservingMaterialVars() = 0;

	virtual bool			WasReloadedFromWhitelist() = 0;

	virtual bool			IsPrecached() const = 0;
};

class IRefCounted
{
public:
	virtual int AddRef() = 0;
	virtual int Release() = 0;
};

class IMatRenderContext : public IRefCounted
{
public:
	virtual void				BeginRender() = 0;
	virtual void				EndRender() = 0;

	virtual void				Flush(bool flushHardware = false) = 0;

	virtual void				BindLocalCubemap(ITexture *pTexture) = 0;

	// pass in an ITexture (that is build with "rendertarget" "1") or
	// pass in NULL for the regular backbuffer.
	virtual void				SetRenderTarget(ITexture *pTexture) = 0;
	virtual ITexture *GetRenderTarget(void) = 0;

	virtual void				GetRenderTargetDimensions(int &width, int &height) const = 0;

	// Bind a material is current for rendering.
	virtual void				Bind(IMaterial *material, void *proxyData = 0) = 0;
	// Bind a lightmap page current for rendering.  You only have to 
	// do this for materials that require lightmaps.
	virtual void				BindLightmapPage(int lightmapPageID) = 0;

	// inputs are between 0 and 1
	virtual void				DepthRange(float zNear, float zFar) = 0;

	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) = 0;

	// read to a unsigned char rgb image.
	virtual void				ReadPixels(int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat) = 0;

	// Sets lighting
	virtual void				SetAmbientLight(float r, float g, float b) = 0;
	virtual void				SetLight(void) = 0;

	// The faces of the cube are specified in the same order as cubemap textures
	virtual void				SetAmbientLightCube(Vector4D cube[6]) = 0;

	// Blit the backbuffer to the framebuffer texture
	virtual void				CopyRenderTargetToTexture(ITexture *pTexture) = 0;

	// Set the current texture that is a copy of the framebuffer.
	virtual void				SetFrameBufferCopyTexture(ITexture *pTexture, int textureIndex = 0) = 0;
	virtual ITexture *GetFrameBufferCopyTexture(int textureIndex) = 0;

	//
	// end vertex array api
	//

	// matrix api
	virtual void				MatrixMode(void) = 0;
	virtual void				PushMatrix(void) = 0;
	virtual void				PopMatrix(void) = 0;
	virtual void				LoadMatrix(VMatrix const &matrix) = 0;
	virtual void				LoadMatrix(matrix3x4_t const &matrix) = 0;
	virtual void				MultMatrix(VMatrix const &matrix) = 0;
	virtual void				MultMatrix(matrix3x4_t const &matrix) = 0;
	virtual void				MultMatrixLocal(VMatrix const &matrix) = 0;
	virtual void				MultMatrixLocal(matrix3x4_t const &matrix) = 0;
	virtual void				GetMatrix(void) = 0;
	virtual void				GetMatrix(void*, void*) = 0;
	virtual void				LoadIdentity(void) = 0;
	virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) = 0;
	virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) = 0;
	virtual void				PickMatrix(int x, int y, int width, int height) = 0;
	virtual void				Rotate(float angle, float x, float y, float z) = 0;
	virtual void				Translate(float x, float y, float z) = 0;
	virtual void				Scale(float x, float y, float z) = 0;
	// end matrix api

	// Sets/gets the viewport
	virtual void				Viewport(int x, int y, int width, int height) = 0;
	virtual void				GetViewport(int &x, int &y, int &width, int &height) const = 0;

	// The cull mode
	virtual void				CullMode(void) = 0;

	// end matrix api

	// This could easily be extended to a general user clip plane
	virtual void				SetHeightClipMode(void) = 0;
	// garymcthack : fog z is always used for heightclipz for now.
	virtual void				SetHeightClipZ(float z) = 0;

	// Fog methods...
	virtual void				FogMode(void) = 0;
	virtual void				FogStart(float fStart) = 0;
	virtual void				FogEnd(float fEnd) = 0;
	virtual void				SetFogZ(float fogZ) = 0;
	virtual void	GetFogMode(void) = 0;

	virtual void				FogColor3f(float r, float g, float b) = 0;
	virtual void				FogColor3fv(float const *rgb) = 0;
	virtual void				FogColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void				FogColor3ubv(unsigned char const *rgb) = 0;

	virtual void				GetFogColor(unsigned char *rgb) = 0;

	// Sets the number of bones for skinning
	virtual void				SetNumBoneWeights(int numBones) = 0;

	// Creates/destroys Mesh
	virtual void *CreateStaticMesh(void) = 0;
	virtual void DestroyStaticMesh(void) = 0;

	// Gets the dynamic mesh associated with the currently bound material
	// note that you've got to render the mesh before calling this function 
	// a second time. Clients should *not* call DestroyStaticMesh on the mesh 
	// returned by this call.
	// Use buffered = false if you want to not have the mesh be buffered,
	// but use it instead in the following pattern:
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		Draw partial
	//		Draw partial
	//		Draw partial
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		etc
	// Use Vertex or Index Override to supply a static vertex or index buffer
	// to use in place of the dynamic buffers.
	//
	// If you pass in a material in pAutoBind, it will automatically bind the
	// material. This can be helpful since you must bind the material you're
	// going to use BEFORE calling GetDynamicMesh.
	virtual void *GetDynamicMesh(void) = 0;

	// ------------ New Vertex/Index Buffer interface ----------------------------
	// Do we need support for bForceTempMesh and bSoftwareVertexShader?
	// I don't think we use bSoftwareVertexShader anymore. .need to look into bForceTempMesh.
	virtual void *CreateStaticVertexBuffer(void) = 0;
	virtual void *CreateStaticIndexBuffer(void) = 0;
	virtual void DestroyVertexBuffer(void *) = 0;
	virtual void DestroyIndexBuffer(void *) = 0;
	// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
	virtual void *GetDynamicVertexBuffer(void) = 0;
	virtual void *GetDynamicIndexBuffer(void) = 0;
	virtual void BindVertexBuffer(void) = 0;
	virtual void BindIndexBuffer(void) = 0;
	virtual void Draw(void) = 0;
	// ------------ End ----------------------------

	// Selection mode methods
	virtual int  SelectionMode(bool selectionMode) = 0;
	virtual void SelectionBuffer(unsigned int *pBuffer, int size) = 0;
	virtual void ClearSelectionNames() = 0;
	virtual void LoadSelectionName(int name) = 0;
	virtual void PushSelectionName(int name) = 0;
	virtual void PopSelectionName() = 0;

	// Sets the Clear Color for ClearBuffer....
	virtual void		ClearColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void		ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) = 0;

	// Allows us to override the depth buffer setting of a material
	virtual void	OverrideDepthEnable(bool bEnable, bool bDepthEnable) = 0;

	// FIXME: This is a hack required for NVidia/XBox, can they fix in drivers?
	virtual void	DrawScreenSpaceQuad(IMaterial *pMaterial) = 0;

	// For debugging and building recording files. This will stuff a token into the recording file,
	// then someone doing a playback can watch for the token.
	virtual void	SyncToken(const char *pToken) = 0;

	// FIXME: REMOVE THIS FUNCTION!
	// The only reason why it's not gone is because we're a week from ship when I found the bug in it
	// and everything's tuned to use it.
	// It's returning values which are 2x too big (it's returning sphere diameter x2)
	// Use ComputePixelDiameterOfSphere below in all new code instead.
	virtual float	ComputePixelWidthOfSphere(const Vector &origin, float flRadius) = 0;

	//
	// Occlusion query support
	//

	// Allocate and delete query objects.
	virtual void CreateOcclusionQueryObject(void) = 0;
	virtual void DestroyOcclusionQueryObject(void) = 0;

	// Bracket drawing with begin and end so that we can get counts next frame.
	virtual void BeginOcclusionQueryDrawing(void) = 0;
	virtual void EndOcclusionQueryDrawing(void) = 0;

	// Get the number of pixels rendered between begin and end on an earlier frame.
	// Calling this in the same frame is a huge perf hit!
	virtual int OcclusionQuery_GetNumPixelsRendered(void) = 0;

	virtual void SetFlashlightMode(bool bEnable) = 0;

	virtual void SetFlashlightState(void) = 0;

	// Gets the current height clip mode
	virtual void GetHeightClipMode() = 0;

	// This returns the diameter of the sphere in pixels based on 
	// the current model, view, + projection matrices and viewport.
	virtual float	ComputePixelDiameterOfSphere(const Vector &vecAbsOrigin, float flRadius) = 0;

	// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
	// planes (which are specified in world space) to generate projection-space user clip planes
	// Occasionally (for the particle system in hl2, for example), we want to override that
	// behavior and explictly specify a ViewProj transform for user clip planes
	virtual void	EnableUserClipTransformOverride(bool bEnable) = 0;
	virtual void	UserClipTransform(const VMatrix &worldToView) = 0;

	virtual bool GetFlashlightMode() const = 0;

	// Used to make the handle think it's never had a successful query before
	virtual void ResetOcclusionQueryObject(void) = 0;

	// FIXME: Remove
	virtual void Unused3() {}

	// Creates/destroys morph data associated w/ a particular material
	virtual void *CreateMorph(void) = 0;
	virtual void DestroyMorph(void) = 0;

	// Binds the morph data for use in rendering
	virtual void BindMorph(void) = 0;

	// Sets flexweights for rendering
	virtual void SetFlexWeights(void) = 0;

	// FIXME: Remove
	virtual void Unused4() {};
	virtual void Unused5() {};
	virtual void Unused6() {};
	virtual void Unused7() {};
	virtual void Unused8() {};

	// Read w/ stretch to a host-memory buffer
	virtual void ReadPixelsAndStretch(Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *pBuffer, ImageFormat dstFormat, int nDstStride) = 0;

	// Gets the window size
	virtual void GetWindowSize(int &width, int &height) const = 0;

	// This function performs a texture map from one texture map to the render destination, doing
	// all the necessary pixel/texel coordinate fix ups. fractional values can be used for the
	// src_texture coordinates to get linear sampling - integer values should produce 1:1 mappings
	// for non-scaled operations.
	virtual void DrawScreenSpaceRectangle(
		IMaterial *pMaterial,
		int destx, int desty,
		int width, int height,
		float src_texture_x0, float src_texture_y0,			// which texel you want to appear at
		// destx/y
		float src_texture_x1, float src_texture_y1,			// which texel you want to appear at
		// destx+width-1, desty+height-1
		int src_texture_width, int src_texture_height,		// needed for fixup
		void *pClientRenderable = NULL,
		int nXDice = 1,
		int nYDice = 1) = 0;

	virtual void LoadBoneMatrix(int boneIndex, const matrix3x4_t &matrix) = 0;

	// This version will push the current rendertarget + current viewport onto the stack
	virtual void PushRenderTargetAndViewport() = 0;

	// This version will push a new rendertarget + a maximal viewport for that rendertarget onto the stack
	virtual void PushRenderTargetAndViewport(ITexture *pTexture) = 0;

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture *pTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture *pTexture, ITexture *pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;

	// This will pop a rendertarget + viewport
	virtual void PopRenderTargetAndViewport(void) = 0;

	// Binds a particular texture as the current lightmap
	virtual void BindLightmapTexture(ITexture *pLightmapTexture) = 0;

	// Blit a subrect of the current render target to another texture
	virtual void CopyRenderTargetToTextureEx(ITexture *pTexture, int nRenderTargetID, Rect_t *pSrcRect, Rect_t *pDstRect = NULL) = 0;
	virtual void CopyTextureToRenderTargetEx(int nRenderTargetID, ITexture *pTexture, Rect_t *pSrcRect, Rect_t *pDstRect = NULL) = 0;

	// Special off-center perspective matrix for DoF, MSAA jitter and poster rendering
	virtual void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) = 0;

	// Rendering parameters control special drawing modes withing the material system, shader
	// system, shaders, and engine. renderparm.h has their definitions.
	virtual void SetFloatRenderingParameter(int parm_number, float value) = 0;
	virtual void SetIntRenderingParameter(int parm_number, int value) = 0;
	virtual void SetVectorRenderingParameter(int parm_number, Vector const &value) = 0;

	// stencil buffer operations.
	virtual void SetStencilEnable(bool onoff) = 0;
	virtual void SetStencilFailOperation(void) = 0;
	virtual void SetStencilZFailOperation(void) = 0;
	virtual void SetStencilPassOperation(void) = 0;
	virtual void SetStencilCompareFunction(void) = 0;
	virtual void SetStencilReferenceValue(int ref) = 0;
	virtual void SetStencilTestMask(uint32 msk) = 0;
	virtual void SetStencilWriteMask(uint32 msk) = 0;
	virtual void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax,int value) = 0;

	virtual void SetRenderTargetEx(int nRenderTargetID, ITexture *pTexture) = 0;

	// rendering clip planes, beware that only the most recently pushed plane will actually be used in a sizeable chunk of hardware configurations
	// and that changes to the clip planes mid-frame while UsingFastClipping() is true will result unresolvable depth inconsistencies
	virtual void PushCustomClipPlane(const float *pPlane) = 0;
	virtual void PopCustomClipPlane(void) = 0;

	// Returns the number of vertices + indices we can render using the dynamic mesh
	// Passing true in the second parameter will return the max # of vertices + indices
	// we can use before a flush is provoked and may return different values 
	// if called multiple times in succession. 
	// Passing false into the second parameter will return
	// the maximum possible vertices + indices that can be rendered in a single batch
	virtual void GetMaxToRender(void) = 0;

	// Returns the max possible vertices + indices to render in a single draw call
	virtual int GetMaxVerticesToRender(IMaterial *pMaterial) = 0;
	virtual int GetMaxIndicesToRender() = 0;
	virtual void DisableAllLocalLights() = 0;
	virtual int CompareMaterialCombos(IMaterial *pMaterial1, IMaterial *pMaterial2, int lightMapID1, int lightMapID2) = 0;

	virtual void *GetFlexMesh() = 0;

	virtual void SetFlashlightStateEx(void) = 0;

	// Returns the currently bound local cubemap
	virtual ITexture *GetLocalCubemap() = 0;

	// This is a version of clear buffers which will only clear the buffer at pixels which pass the stencil test
	virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) = 0;

	//enables/disables all entered clipping planes, returns the input from the last time it was called.
	virtual bool EnableClipping(bool bEnable) = 0;

	//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
	virtual void GetFogDistances(float *fStart, float *fEnd, float *fFogZ) = 0;

	// Hooks for firing PIX events from outside the Material System...
	virtual void BeginPIXEvent(unsigned long color, const char *szName) = 0;
	virtual void EndPIXEvent() = 0;
	virtual void SetPIXMarker(unsigned long color, const char *szName) = 0;

	// Batch API
	// from changelist 166623:
	// - replaced obtuse material system batch usage with an explicit and easier to thread API
	virtual void BeginBatch(void) = 0;
	virtual void BindBatch(void) = 0;
	virtual void DrawBatch(int firstIndex, int numIndices) = 0;
	virtual void EndBatch() = 0;

	// Raw access to the call queue, which can be NULL if not in a queued mode
	virtual void *GetCallQueue() = 0;

	// Returns the world-space camera position
	virtual void GetWorldSpaceCameraPosition(Vector *pCameraPos) = 0;
	virtual void GetWorldSpaceCameraVectors(Vector *pVecForward, Vector *pVecRight, Vector *pVecUp) = 0;

	// Tone mapping
	virtual void				ResetToneMappingScale(float monoscale) = 0; 			// set scale to monoscale instantly with no chasing
	virtual void				SetGoalToneMappingScale(float monoscale) = 0; 			// set scale to monoscale instantly with no chasing

	// call TurnOnToneMapping before drawing the 3d scene to get the proper interpolated brightness
	// value set.
	virtual void				TurnOnToneMapping() = 0;

	// Set a linear vector color scale for all 3D rendering.
	// A value of [1.0f, 1.0f, 1.0f] should match non-tone-mapped rendering.
	virtual void				SetToneMappingScaleLinear(const Vector &scale) = 0;

	virtual Vector				GetToneMappingScaleLinear(void) = 0;
	virtual void				SetShadowDepthBiasFactors(float fSlopeScaleDepthBias, float fDepthBias) = 0;

	// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
	virtual void				PerformFullScreenStencilOperation(void) = 0;

	// Sets lighting origin for the current model (needed to convert directional lights to points)
	virtual void				SetLightingOrigin(Vector vLightingOrigin) = 0;

	// Set scissor rect for rendering
	virtual void				SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor) = 0;

	// Methods used to build the morph accumulator that is read from when HW morph<ing is enabled.
	virtual void				BeginMorphAccumulation() = 0;
	virtual void				EndMorphAccumulation() = 0;
	virtual void				AccumulateMorph(void) = 0;

	virtual void				PushDeformation(void) = 0;
	virtual void				PopDeformation() = 0;
	virtual int					GetNumActiveDeformations() const = 0;

	virtual bool				GetMorphAccumulatorTexCoord(void) = 0;

	// Version of get dynamic mesh that specifies a specific vertex format
	virtual void *GetDynamicMeshEx(void) = 0;

	virtual void				FogMaxDensity(float flMaxDensity) = 0;

#if defined( _X360 )
	//Seems best to expose GPR allocation to scene rendering code. 128 total to split between vertex/pixel shaders (pixel will be set to 128 - vertex). Minimum value of 16. More GPR's = more threads.
	virtual void				PushVertexShaderGPRAllocation(int iVertexShaderCount = 64) = 0;
	virtual void				PopVertexShaderGPRAllocation(void) = 0;
#endif

	virtual IMaterial *GetCurrentMaterial() = 0;
	virtual int  GetCurrentNumBones() const = 0;
	virtual void *GetCurrentProxy() = 0;

	// Color correction related methods..
	// Client cannot call IColorCorrectionSystem directly because it is not thread-safe
	// FIXME: Make IColorCorrectionSystem threadsafe?
	virtual void EnableColorCorrection(bool bEnable) = 0;
	virtual void AddLookup(const char *pName) = 0;
	virtual bool RemoveLookup(void) = 0;
	virtual void LockLookup(void) = 0;
	virtual void LoadLookup(void) = 0;
	virtual void UnlockLookup(void) = 0;
	virtual void SetLookupWeight(void) = 0;
	virtual void ResetLookupWeights() = 0;
	virtual void SetResetable(void) = 0;

	//There are some cases where it's simply not reasonable to update the full screen depth texture (mostly on PC).
	//Use this to mark it as invalid and use a dummy texture for depth reads.
	virtual void SetFullScreenDepthTextureValidityFlag(bool bIsValid) = 0;

	// A special path used to tick the front buffer while loading on the 360
	virtual void SetNonInteractivePacifierTexture(ITexture *pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) = 0;
	virtual void SetNonInteractiveTempFullscreenBuffer(void) = 0;
	virtual void EnableNonInteractiveMode(void) = 0;
	virtual void RefreshFrontBufferNonInteractive() = 0;
	// Allocates temp render data. Renderdata goes out of scope at frame end in multicore
	// Renderdata goes out of scope after refcount goes to zero in singlecore.
	// Locking/unlocking increases + decreases refcount
	virtual void *LockRenderData(int nSizeInBytes) = 0;
	virtual void			UnlockRenderData(void *pData) = 0;

	// Typed version. If specified, pSrcData is copied into the locked memory.
	template< class E > E *LockRenderDataTyped(int nCount, const E *pSrcData = NULL);

	// Temp render data gets immediately freed after it's all unlocked in single core.
	// This prevents it from being freed
	virtual void			AddRefRenderData() = 0;
	virtual void			ReleaseRenderData() = 0;

	// Returns whether a pointer is render data. NOTE: passing NULL returns true
	virtual bool			IsRenderData(const void *pData) const = 0;
	virtual void			PrintfVA(char *fmt, va_list vargs) = 0;
	virtual void			Printf(void) = 0;
	virtual float			Knob(char *knobname, float *setvalue = NULL) = 0;
	// Allows us to override the alpha write setting of a material
	virtual void OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable) = 0;
	virtual void OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable) = 0;

	virtual void ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth) = 0;

	// Create a texture from the specified src render target, then call pRecipient->OnAsyncCreateComplete from the main thread.
	// The texture will be created using the destination format, and will optionally have mipmaps generated.
	// In case of error, the provided callback function will be called with the error texture.
	virtual void AsyncCreateTextureFromRenderTarget(void) = 0;
};

class CMatRenderContextPtr : public CRefPtr<IMatRenderContext>
{
	typedef CRefPtr<IMatRenderContext> BaseClass;
public:
	CMatRenderContextPtr() {}
	CMatRenderContextPtr(IMatRenderContext *pInit) : BaseClass(pInit) { if (BaseClass::m_pObject) BaseClass::m_pObject->BeginRender(); }
	CMatRenderContextPtr(IMaterialSystem *pFrom) : BaseClass(pFrom->GetRenderContext()) { if (BaseClass::m_pObject) BaseClass::m_pObject->BeginRender(); }
	~CMatRenderContextPtr() { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); }

	IMatRenderContext *operator=(IMatRenderContext *p) { if (p) p->BeginRender(); return BaseClass::operator=(p); }

	void SafeRelease() { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); BaseClass::SafeRelease(); }
	void AssignAddRef(IMatRenderContext *pFrom) { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); BaseClass::AssignAddRef(pFrom); BaseClass::m_pObject->BeginRender(); }

	void GetFrom(IMaterialSystem *pFrom) { AssignAddRef(pFrom->GetRenderContext()); }


private:
	CMatRenderContextPtr(const CMatRenderContextPtr &from);
	void operator=(const CMatRenderContextPtr &from);

};

class CCollisionProperty
{
public:
	unsigned char _0x0[0x8];
	Vector	m_vecMins; // 0x8
	Vector	m_vecMaxs; // 0x14
	int    	m_usSolidFlags; // 0x20
	int    	m_nSolidType; // 0x22
	int    	m_triggerBloat; // 0x23
	unsigned char _0x27[0x3];
	int    	m_nSurroundType; // 0x2a
	Vector	m_vecSpecifiedSurroundingMins; // 0x2c
	Vector	m_vecSpecifiedSurroundingMaxs; // 0x38
};

class CGlowProperty
{
public:
	unsigned char _0x0[0x4];
	int    	m_iGlowType; // 0x4
	int    	m_nGlowRange; // 0x8
	int    	m_nGlowRangeMin; // 0xc
	int    	m_glowColorOverride; // 0x10
	int    	m_bFlashing; // 0x14
};

class CBaseEntity
{
public:
	unsigned char _0x0[0x14];
	int    	m_hScriptUseTarget; // 0x14
	unsigned char _0x18[0x1c];
	int    	m_Gender; // 0x34
	unsigned char _0x38[0x2c];
	int    	m_clrRender; // 0x64
	int    	m_cellbits; // 0x68
	unsigned char _0x6c[0x4];
	int    	m_cellX; // 0x70
	int    	m_cellY; // 0x74
	int    	m_cellZ; // 0x78
	unsigned char _0x7c[0x48];
	Vector	m_playerAngles; // 0xc4
	unsigned char _0xd0c[0x18];
	int    	m_fEffects; // 0xe0
	int    	m_iTeamNum; // 0xe4
	unsigned char _0xe8[0x30];
	Vector	m_angRotation; // 0x118
	Vector	m_vecOrigin; // 0x124
	unsigned char _0x130[0x4];
	int    	moveparent; // 0x134
	int    	m_hOwnerEntity; // 0x138
	unsigned char _0x13c[0x4];
	int    	m_nModelIndex; // 0x140
	int    	m_nRenderFX; // 0x142
	int    	m_nRenderMode; // 0x143
	unsigned char _0x147[0x1];
	int    	m_flAnimTime; // 0x148
	unsigned char _0x14c[0x4];
	int    	m_flSimulationTime; // 0x150
	unsigned char _0x154[0x4];
	float  	m_flCreateTime; // 0x158
	unsigned char _0x15c[0x8c];
	float  	m_fadeMinDist; // 0x1e8
	float  	m_fadeMaxDist; // 0x1ec
	float  	m_flFadeScale; // 0x1f0
	unsigned char _0x1f4[0x4];
	int    	m_iParentAttachment; // 0x1f8
	unsigned char _0x1fc[0x20];
	CCollisionProperty	m_Collision; // 0x21c
	unsigned char _0x260[0x18];
	CGlowProperty	m_Glow; // 0x278
	unsigned char _0x290[0x3c];
	int    	m_bGlowBackfaceMult; // 0x2cc
	float  	m_flElasticity; // 0x2d0
	float  	m_flShadowCastDistance; // 0x2d4
	unsigned char _0x2d8[0xa8];
	int    	m_CollisionGroup; // 0x380
	unsigned char _0x384[0x261];
	int    	m_bSimulatedEveryTick; // 0x5e5
	int    	m_bAnimatedEveryTick; // 0x5e6
	int    	m_bAlternateSorting; // 0x5e7
	int    	m_nMinCPULevel; // 0x5e8
	int    	m_nMaxCPULevel; // 0x5e9
	int    	m_nMinGPULevel; // 0x5ea
	int    	m_nMaxGPULevel; // 0x5eb
	int    	m_iTextureFrameIndex; // 0x5ec
	unsigned char _0x5f0[0x8];
	int    	m_hEffectEntity; // 0x5f8
};


class C_BaseCombatWeapon
{
public:
	virtual void *sub_10019D90() = 0;
	virtual void *sub_100420F0() = 0;
	virtual void *sub_10042110() = 0;
	virtual void *sub_100194D0() = 0;
	virtual void *sub_100194E0() = 0;
	virtual void *sub_100194F0() = 0;
	virtual void *sub_10019500() = 0;
	virtual void *sub_10019510() = 0;
	virtual void *sub_10019520() = 0;
	virtual void *sub_100414B0() = 0;
	virtual void *sub_10042070() = 0;
	virtual void *C_BaseEntity__GetAbsOrigin() = 0;
	virtual void *C_BaseEntity__GetAbsAngles() = 0;
	virtual void *sub_1002BAD0() = 0;
	virtual void *sub_10039EE0() = 0;
	virtual void *sub_10043480() = 0;
	virtual void *sub_10043270() = 0;
	virtual void *sub_100158F0() = 0;
	virtual void *sub_100158E0() = 0;
	virtual void *sub_1001CC10() = 0;
	virtual void *sub_1001A1E0() = 0;
	virtual void *sub_100194A0() = 0;
	virtual void *sub_1001B830() = 0;
	virtual void *sub_1001A1F0() = 0;
	virtual void *sub_1001A200() = 0;
	virtual void *C_BaseEntity__TraceAttack() = 0;
	virtual void *sub_1001A210() = 0;
	virtual void *sub_10016190() = 0;
	virtual void *sub_1001A2D0() = 0;
	virtual void *sub_1001A2F0() = 0;
	virtual void *sub_1001A300() = 0;
	virtual void *sub_100172C0() = 0;
	virtual void *nullsub_170() = 0;
	virtual void *sub_10017380() = 0;
	virtual void *j_nullsub_171() = 0;
	virtual void *nullsub_172() = 0;
	virtual void *sub_1001A010() = 0;
	virtual void *sub_10019FA0() = 0;
	virtual void *C_BaseEntity__KeyValue() = 0;
	virtual void *C_BaseEntity__GetKeyValue() = 0;
	virtual void *nullsub_173() = 0;
	virtual void *sub_100487C0() = 0;
	virtual void *sub_10019890() = 0;
	virtual void *unknown_0() = 0;
	virtual void *sub_10031930() = 0;
	virtual void *sub_10019530() = 0;
	virtual void *sub_10030B50() = 0;
	virtual void *sub_10030BE0() = 0;
	virtual void *sub_10043470() = 0;
	virtual void *sub_100430D0() = 0;
	virtual void *sub_1004CD20() = 0;
	virtual void *sub_10040BB0() = 0;
	virtual void *nullsub_174() = 0;
	virtual void *unknown_1() = 0;
	virtual void *sub_100427B0() = 0;
	virtual void *sub_10043280() = 0;
	virtual void *sub_100474F0() = 0;
	virtual void *sub_10040C00() = 0;
	virtual void *sub_10040BA0() = 0;
	virtual void *sub_10019950() = 0;
	virtual void *sub_10044FE0() = 0;
	virtual void *sub_10047500() = 0;
	virtual void *sub_1001A150() = 0;
	virtual void *sub_1001A120() = 0;
	virtual void *sub_1002E490() = 0;
	virtual void *sub_1002D580() = 0;
	virtual void *sub_100421C0() = 0;
	virtual void *sub_100495A0() = 0;
	virtual void *sub_100495E0() = 0;
	virtual void *sub_10019830() = 0;
	virtual void *sub_10019850() = 0;
	virtual void *sub_1001BEE0() = 0;
	virtual void *nullsub_175() = 0;
	virtual void *sub_10019810() = 0;
	virtual void *sub_100197F0() = 0;
	virtual void *sub_10040D20() = 0;
	virtual void *sub_10040DF0() = 0;
	virtual void *sub_10040E30() = 0;
	virtual void *sub_10042B10() = 0;
	virtual void *unknown_2() = 0;
	virtual void *unknown_3() = 0;
	virtual void *sub_10042B30() = 0;
	virtual void *sub_10042B40() = 0;
	virtual void *sub_10042B80() = 0;
	virtual void *sub_100195B0() = 0;
	virtual void *sub_100195C0() = 0;
	virtual void *sub_100195D0() = 0;
	virtual void *sub_100195E0() = 0;
	virtual void *sub_100195F0() = 0;
	virtual void *sub_10019600() = 0;
	virtual void *sub_100420C0() = 0;
	virtual void *sub_100445A0() = 0;
	virtual void *sub_1004D170() = 0;
	virtual void *C_BaseAnimating__Simulate() = 0;
	virtual void *sub_10019610() = 0;
	virtual void *sub_100319C0() = 0;
	virtual void *sub_10042780() = 0;
	virtual void *sub_10042520() = 0;
	virtual void *sub_10041120() = 0;
	virtual void *sub_1002C740() = 0;
	virtual void *sub_1002D8D0() = 0;
	virtual void *sub_1002E400() = 0;
	virtual void *sub_1004ED10() = 0;
	virtual void *sub_1003B210() = 0;
	virtual void *nullsub_9() = 0;
	virtual void *nullsub_10() = 0;
	virtual void *sub_1003B730() = 0;
	virtual void *sub_100344F0() = 0;
	virtual void *sub_10042880() = 0;
	virtual void *sub_10044700() = 0;
	virtual void *sub_1002C220() = 0;
	virtual void *sub_10042620() = 0;
	virtual void *sub_10042910() = 0;
	virtual void *nullsub_176() = 0;
	virtual void *sub_10044A50() = 0;
	virtual void *nullsub_177() = 0;
	virtual void *sub_10019650() = 0;
	virtual void *sub_10019660() = 0;
	virtual void *sub_10019670() = 0;
	virtual void *sub_100467C0() = 0;
	virtual void *sub_10043070() = 0;
	virtual void *sub_10040C30() = 0;
	virtual void *sub_100471C0() = 0;
	virtual void *sub_100461B0() = 0;
	virtual void *sub_1004E2D0() = 0;
	virtual void *sub_100331A0() = 0;
	virtual void *sub_1001A0D0() = 0;
	virtual void *sub_1001A8D0() = 0;
	virtual void *sub_1001A950() = 0;
	virtual void *sub_10019680() = 0;
	virtual void *sub_10019690() = 0;
	virtual void *sub_10043420() = 0;
	virtual void *sub_100196B0() = 0;
	virtual void *sub_1004C230() = 0;
	virtual void *sub_100196C0() = 0;
	virtual void *sub_100196D0() = 0;
	virtual void *sub_101653F0() = 0;
	virtual void *nullsub_22() = 0;
	virtual void *sub_10164EC0() = 0;
	virtual void *nullsub_178() = 0;
	virtual void *sub_10165410() = 0;
	virtual void *nullsub_179() = 0;
	virtual void *sub_101686F0() = 0;
	virtual void *sub_100196F0() = 0;
	virtual void *sub_10019700() = 0;
	virtual void *sub_10019710() = 0;
	virtual void *sub_10019720() = 0;
	virtual void *sub_10019730() = 0;
	virtual void *sub_10019740() = 0;
	virtual void *sub_10019750() = 0;
	virtual void *sub_10019760() = 0;
	virtual void *sub_10019960() = 0;
	virtual void *sub_10019970() = 0;
	virtual void *sub_10019770() = 0;
	virtual void *sub_10019780() = 0;
	virtual void *C_BaseEntity__EyePosition() = 0;
	virtual void *sub_10019DF0() = 0;
	virtual void *sub_10019E00() = 0;
	virtual void *sub_10019E10() = 0;
	virtual void *sub_1001A090() = 0;
	virtual void *sub_10019870() = 0;
	virtual const Vector &GetViewOffset() = 0;
};


class C_BaseViewModel
{
public:
	virtual void *sub_10023470() = 0;
	virtual void *sub_100420F0() = 0;
	virtual void *sub_10042110() = 0;
	virtual void *sub_100194D0() = 0;
	virtual void *sub_100194E0() = 0;
	virtual void *sub_100194F0() = 0;
	virtual void *sub_10019500() = 0;
	virtual void *sub_10019510() = 0;
	virtual void *sub_10019520() = 0;
	virtual void *sub_10034210() = 0;
	virtual void *sub_10042070() = 0;
	virtual void *C_BaseEntity__GetAbsOrigin() = 0;
	virtual void *C_BaseEntity__GetAbsAngles() = 0;
	virtual void *sub_1002BAD0() = 0;
	virtual void *sub_10039EE0() = 0;
	virtual void *sub_10043480() = 0;
	virtual void *sub_10043270() = 0;
	virtual void *sub_10022750() = 0;
	virtual void *sub_10022770() = 0;
	virtual void *sub_1001CC10() = 0;
	virtual void *sub_1001A1E0() = 0;
	virtual void *sub_100194A0() = 0;
	virtual void *sub_1001B830() = 0;
	virtual void *sub_1001A1F0() = 0;
	virtual void *sub_1001A200() = 0;
	virtual void *C_BaseEntity__TraceAttack() = 0;
	virtual void *sub_1001A210() = 0;
	virtual void *sub_1001A230() = 0;
	virtual void *sub_1001A2D0() = 0;
	virtual void *sub_1001A2F0() = 0;
	virtual void *sub_1001A300() = 0;
	virtual void *sub_10022790() = 0;
	virtual void *nullsub_170() = 0;
	virtual void *nullsub_220() = 0;
	virtual void *nullsub_171() = 0;
	virtual void *nullsub_172() = 0;
	virtual void *sub_1001A010() = 0;
	virtual void *sub_10019FA0() = 0;
	virtual void *C_BaseEntity__KeyValue() = 0;
	virtual void *C_BaseEntity__GetKeyValue() = 0;
	virtual void *nullsub_173() = 0;
	virtual void *sub_100487C0() = 0;
	virtual void *sub_10019890() = 0;
	virtual void *unknown_0() = 0;
	virtual void *sub_10031930() = 0;
	virtual void *sub_10019530() = 0;
	virtual void *sub_10030B50() = 0;
	virtual void *sub_10030BE0() = 0;
	virtual void *sub_10043470() = 0;
	virtual void *sub_100430D0() = 0;
	virtual void *sub_1004CD20() = 0;
	virtual void *sub_10049060() = 0;
	virtual void *nullsub_174() = 0;
	virtual void *unknown_1() = 0;
	virtual void *sub_100427B0() = 0;
	virtual void *sub_10043280() = 0;
	virtual void *sub_100474F0() = 0;
	virtual void *C_BaseEntity__OnRestore() = 0;
	virtual void *sub_100232E0() = 0;
	virtual void *sub_10019950() = 0;
	virtual void *sub_10044FE0() = 0;
	virtual void *sub_10047500() = 0;
	virtual void *sub_1001A150() = 0;
	virtual void *sub_1001A120() = 0;
	virtual void *sub_1002E490() = 0;
	virtual void *sub_1002D580() = 0;
	virtual void *sub_100421C0() = 0;
	virtual void *sub_100495A0() = 0;
	virtual void *sub_100495E0() = 0;
	virtual void *sub_10019830() = 0;
	virtual void *sub_10019850() = 0;
	virtual void *sub_1001BEE0() = 0;
	virtual void *nullsub_175() = 0;
	virtual void *sub_10019810() = 0;
	virtual void *sub_100197F0() = 0;
	virtual void *sub_10022D70() = 0;
	virtual void *sub_10022DB0() = 0;
	virtual void *sub_1002F7F0() = 0;
	virtual void *sub_10042B10() = 0;
	virtual void *unknown_2() = 0;
	virtual void *unknown_3() = 0;
	virtual void *sub_10042B30() = 0;
	virtual void *sub_10042B40() = 0;
	virtual void *sub_10042B80() = 0;
	virtual void *sub_100195B0() = 0;
	virtual void *sub_100195C0() = 0;
	virtual void *sub_100195D0() = 0;
	virtual void *sub_100195E0() = 0;
	virtual void *sub_100195F0() = 0;
	virtual void *sub_10019600() = 0;
	virtual void *sub_100420C0() = 0;
	virtual void *sub_100445A0() = 0;
	virtual void *sub_1004D170() = 0;
	virtual void *C_BaseAnimating__Simulate() = 0;
	virtual void *sub_10019610() = 0;
	virtual void *sub_100319C0() = 0;
	virtual void *sub_10042780() = 0;
	virtual void *sub_1006AC30() = 0;
	virtual void *sub_100322D0() = 0;
	virtual void *sub_1002C740() = 0;
	virtual void *sub_1002D8D0() = 0;
	virtual void *sub_100232F0() = 0;
	virtual void *sub_1004ED10() = 0;
	virtual void *sub_1006AC70() = 0;
	virtual void *nullsub_9() = 0;
	virtual void *nullsub_10() = 0;
	virtual void *sub_1003B730() = 0;
	virtual void *sub_1006B1F0() = 0;
	virtual void *sub_10042880() = 0;
	virtual void *sub_10044700() = 0;
	virtual void *sub_1002C220() = 0;
	virtual void *sub_10042620() = 0;
	virtual void *sub_10042910() = 0;
	virtual void *nullsub_176() = 0;
	virtual void *sub_10044A50() = 0;
	virtual void *nullsub_177() = 0;
	virtual void *sub_10019650() = 0;
	virtual void *sub_10019660() = 0;
	virtual void *sub_10019670() = 0;
	virtual void *sub_100467C0() = 0;
	virtual void *sub_10043070() = 0;
	virtual void *sub_10022780() = 0;
	virtual void *sub_100471C0() = 0;
	virtual void *sub_1006AF10() = 0;
	virtual void *sub_1004E2D0() = 0;
	virtual void *sub_100331A0() = 0;
	virtual void *sub_1001A0D0() = 0;
	virtual void *sub_1001A8D0() = 0;
	virtual void *sub_1001A950() = 0;
	virtual void *sub_10019680() = 0;
	virtual void *sub_10019690() = 0;
	virtual void *sub_10043420() = 0;
	virtual void *sub_100196B0() = 0;
	virtual void *sub_1004C230() = 0;
	virtual void *sub_100196C0() = 0;
	virtual void *sub_100196D0() = 0;
	virtual void *sub_101653F0() = 0;
	virtual void *nullsub_22() = 0;
	virtual void *sub_10164EC0() = 0;
	virtual void *nullsub_178() = 0;
	virtual void *sub_10165410() = 0;
	virtual void *nullsub_179() = 0;
	virtual void *sub_101686F0() = 0;
	virtual void *sub_100196F0() = 0;
	virtual void *sub_10019700() = 0;
	virtual void *sub_10019710() = 0;
	virtual void *sub_10019720() = 0;
	virtual void *sub_10019730() = 0;
	virtual void *sub_10019740() = 0;
	virtual void *sub_10019750() = 0;
	virtual void *sub_10019760() = 0;
	virtual void *sub_1001E000() = 0;
	virtual void *sub_1001E010() = 0;
	virtual void *sub_10019770() = 0;
	virtual void *sub_10019780() = 0;
	virtual void *C_BaseEntity__EyePosition() = 0;
	virtual void *sub_10019DF0() = 0;
	virtual void *sub_10019E00() = 0;
	virtual void *sub_10019E10() = 0;
	virtual void *sub_1001A090() = 0;
	virtual void *sub_10019870() = 0;
	virtual void *C_BaseEntity__GetViewOffset() = 0;
	virtual void *C_BaseEntity__SetViewOffset() = 0;
	virtual void *C_BaseEntity__GetGroundVelocityToApply() = 0;
	virtual void *C_BaseEntity__ShouldInterpolate() = 0;
	virtual void *C_BaseEntity__BoneMergeFastCullBloat() = 0;
	virtual void *sub_10043080() = 0;
	virtual void *C_BaseEntity__GetShadowUseOtherEntity() = 0;
	virtual void *C_BaseEntity__SetShadowUseOtherEntity() = 0;
	virtual void *sub_100197E0() = 0;
	virtual void *sub_10019930() = 0;
	virtual void *nullsub_180() = 0;
	virtual void *sub_1006AF40() = 0;
	virtual void *C_BaseAnimating__SetBoneController() = 0;
	virtual void *C_BaseAnimating__GetPoseParameters() = 0;
	virtual void *sub_10039850() = 0;
	virtual void *sub_1006ADC0() = 0;
	virtual void *sub_100334A0() = 0;
	virtual void *sub_10033550() = 0;
	virtual void *sub_10030150() = 0;
	virtual void *sub_1002FD00() = 0;
	virtual void *sub_1002BEA0() = 0;
	virtual void *sub_10033F70() = 0;
	virtual void *sub_1003E610() = 0;
	virtual void *sub_1006AFA0() = 0;
	virtual void *sub_1002DD30() = 0;
	virtual void *C_BaseAnimating__DispatchMuzzleEffect() = 0;
	virtual void *sub_1003A3C0() = 0;
	virtual void *sub_1003C050() = 0;
	virtual void *sub_1003BDF0() = 0;
	virtual void *sub_1003E390() = 0;
	virtual void *nullsub_79() = 0;
	virtual void *sub_1003AD80() = 0;
	virtual void *sub_1003CF70() = 0;
	virtual void *sub_1002BFE0() = 0;
	virtual void *sub_1002F590() = 0;
	virtual void *sub_1002B8D0() = 0;
	virtual void *C_BaseAnimating__GetRagdollInitBoneArrays() = 0;
	virtual void *sub_100345B0() = 0;
	virtual void *sub_100346C0() = 0;
	virtual void *sub_100314E0() = 0;
	virtual void *sub_1002C000() = 0;
	virtual void *sub_1002E740() = 0;
	virtual void *sub_1002BFF0() = 0;
	virtual void *nullsub_181() = 0;
	virtual void *sub_100198D0() = 0;
	virtual void *C_BaseViewModel__UncorrectViewModelAttachment() = 0;
	virtual void *sub_1002C400() = 0;
	virtual void *nullsub_221() = 0;
	virtual void *nullsub_183() = 0;
	virtual void *sub_10019900() = 0;
	virtual void *sub_10023330() = 0;
	virtual void *sub_10023300() = 0;
	virtual void *sub_10019910() = 0;
	virtual void *sub_1006ACC0() = 0;
	virtual void *sub_1002BAC0() = 0;
	virtual void *sub_1002B9B0() = 0;
	virtual void *C_BaseAnimating__ComputeStencilState() = 0;
	virtual void *C_BaseAnimating__LastBoneChangedTime() = 0;
	virtual void *sub_10022F50() = 0;
	virtual void *sub_10022E30() = 0;
	virtual void *sub_100223E0() = 0;
	virtual void *C_BaseViewModel__CalcViewModelView() = 0;
	virtual void *nullsub_222() = 0;
	virtual void *sub_10023350() = 0;
	virtual void *sub_100223D0() = 0;
	virtual void *sub_10022820() = 0;
	virtual C_BaseCombatWeapon *GetOwningWeapon() = 0;
};

class CBasePlayer
{
public:
	virtual void *fn0() = 0;
	virtual void *fn1() = 0;
	virtual void *fn2() = 0;
	virtual void *fn3() = 0;
	virtual void *fn4() = 0;
	virtual void *fn5() = 0;
	virtual void *fn6() = 0;
	virtual void *fn7() = 0;
	virtual void *fn8() = 0;
	virtual void *fn9() = 0;
	virtual void *fn10() = 0;
	virtual const Vector & GetAbsOrigin() = 0;
	virtual const QAngle & GetAbsAngles() = 0;
	virtual void *sub_1002BAD0() = 0;
	virtual void *sub_1004F000() = 0;
	virtual void *sub_10043480() = 0;
	virtual void *sub_10043270() = 0;
	virtual void *sub_10063390() = 0;
	virtual void *sub_100633D0() = 0;
	virtual void *sub_1001CC10() = 0;
	virtual void *sub_1001A1E0() = 0;
	virtual void *sub_100194A0() = 0;
	virtual void *sub_1001B830() = 0;
	virtual void *sub_1001A1F0() = 0;
	virtual void *sub_1001A200() = 0;
	virtual void *C_BaseEntity__TraceAttack() = 0;
	virtual void *sub_1001A210() = 0;
	virtual void *sub_10020C00() = 0;
	virtual void *sub_1001A2D0() = 0;
	virtual void *sub_10011D30() = 0;
	virtual void *sub_1001F240() = 0;
	virtual void *sub_10063B90() = 0;
	virtual void *nullsub_170() = 0;
	virtual void *nullsub_70() = 0;
	virtual void *nullsub_171() = 0;
	virtual void *nullsub_172() = 0;
	virtual void *sub_1001A010() = 0;
	virtual void *sub_10019FA0() = 0;
	virtual void *C_BaseEntity__KeyValue() = 0;
	virtual void *C_BaseEntity__GetKeyValue() = 0;
	virtual void *nullsub_173() = 0;
	virtual void *sub_100487C0() = 0;
	virtual void *sub_10019890() = 0;
	virtual void *unknown_0() = 0;
	virtual void *sub_10031930() = 0;
	virtual void *sub_10019530() = 0;
	virtual void *sub_10030B50() = 0;
	virtual void *sub_10030BE0() = 0;
	virtual void *sub_10043470() = 0;
	virtual void *sub_100430D0() = 0;
	virtual void *sub_1004CD20() = 0;
	virtual void *sub_10049060() = 0;
	virtual void *nullsub_174() = 0;
	virtual void *unknown_1() = 0;
	virtual void *sub_100427B0() = 0;
	virtual void *sub_10043280() = 0;
	virtual void *sub_100474F0() = 0;
	virtual void *C_BasePlayer__OnRestore() = 0;
	virtual void *sub_100432A0() = 0;
	virtual void *sub_10019950() = 0;
	virtual void *sub_10044FE0() = 0;
	virtual void *sub_10047500() = 0;
	virtual void *sub_1001A150() = 0;
	virtual void *sub_1001A120() = 0;
	virtual void *sub_1002E490() = 0;
	virtual void *sub_1002D580() = 0;
	virtual void *sub_100421C0() = 0;
	virtual void *sub_100495A0() = 0;
	virtual void *sub_100495E0() = 0;
	virtual void *sub_10019830() = 0;
	virtual void *sub_10019850() = 0;
	virtual void *sub_1001BEE0() = 0;
	virtual void *nullsub_175() = 0;
	virtual void *sub_10019810() = 0;
	virtual void *sub_100197F0() = 0;
	virtual void *sub_1002BA20() = 0;
	virtual void *sub_1002FA60() = 0;
	virtual void *sub_1002F7F0() = 0;
	virtual void *sub_10042B10() = 0;
	virtual void *unknown_2() = 0;
	virtual void *unknown_3() = 0;
	virtual void *sub_10042B30() = 0;
	virtual void *sub_10042B40() = 0;
	virtual void *sub_10042B80() = 0;
	virtual void *sub_100195B0() = 0;
	virtual void *sub_100195C0() = 0;
	virtual void *sub_100195D0() = 0;
	virtual void *sub_100195E0() = 0;
	virtual void *sub_100195F0() = 0;
	virtual void *sub_10019600() = 0;
	virtual void *sub_100420C0() = 0;
	virtual void *sub_100445A0() = 0;
	virtual void *sub_1004D170() = 0;
	virtual void *sub_100691D0() = 0;
	virtual void *sub_10019610() = 0;
	virtual void *sub_100319C0() = 0;
	virtual void *sub_10042780() = 0;
	virtual void *sub_10042520() = 0;
	virtual void *sub_100645B0() = 0;
	virtual void *sub_1002C740() = 0;
	virtual void *sub_1002D8D0() = 0;
	virtual void *sub_1002E400() = 0;
	virtual void *sub_1004ED10() = 0;
	virtual void *sub_1003FF10() = 0;
	virtual void *sub_10040530() = 0;
	virtual void *sub_100406A0() = 0;
	virtual void *sub_1003B730() = 0;
	virtual void *sub_100344F0() = 0;
	virtual void *sub_10042880() = 0;
	virtual void *sub_10044700() = 0;
	virtual void *sub_1002C220() = 0;
	virtual void *sub_10042620() = 0;
	virtual void *sub_10042910() = 0;
	virtual void *nullsub_176() = 0;
	virtual void *sub_10044A50() = 0;
	virtual void *nullsub_177() = 0;
	virtual void *unknown_4() = 0;
	virtual void *sub_10019660() = 0;
	virtual void *sub_10019670() = 0;
	virtual void *sub_100467C0() = 0;
	virtual void *sub_10043070() = 0;
	virtual void *sub_1004C1C0() = 0;
	virtual void *sub_100471C0() = 0;
	virtual void *sub_10063640() = 0;
	virtual void *sub_1004E2D0() = 0;
	virtual void *sub_100331A0() = 0;
	virtual void *sub_1001A0D0() = 0;
	virtual void *sub_1001A8D0() = 0;
	virtual void *sub_1001A950() = 0;
	virtual void *sub_10063610() = 0;
	virtual void *sub_10019690() = 0;
	virtual void *sub_10068110() = 0;
	virtual void *sub_100196B0() = 0;
	virtual void *sub_1004C230() = 0;
	virtual void *sub_100196C0() = 0;
	virtual void *sub_100196D0() = 0;
	virtual void *sub_101653F0() = 0;
	virtual void *nullsub_22() = 0;
	virtual void *sub_10164EC0() = 0;
	virtual void *nullsub_178() = 0;
	virtual void *sub_10069930() = 0;
	virtual void *nullsub_179() = 0;
	virtual void *sub_100673C0() = 0;
	virtual void *sub_10069850() = 0;
	virtual void *sub_1001E040() = 0;
	virtual void *sub_1001E050() = 0;
	virtual void *sub_10019720() = 0;
	virtual void *sub_10019730() = 0;
	virtual void *sub_10019740() = 0;
	virtual void *sub_10019750() = 0;
	virtual void *sub_10019760() = 0;
	virtual void *sub_1001E000() = 0;
	virtual void *sub_1001E010() = 0;
	virtual void *sub_10019770() = 0;
	virtual void *sub_10019780() = 0;
	virtual Vector EyePosition() = 0;
	virtual const QAngle &EyeAngles() = 0;
	virtual void *sub_1001ECA0() = 0;
	virtual void *sub_10019E10() = 0;
	virtual void *sub_1001A090() = 0;
	virtual void *sub_10019870() = 0;
	virtual void *sub_10019E30() = 0;
	virtual void *C_BaseEntity__SetViewOffset() = 0;
	virtual void *C_BaseEntity__GetGroundVelocityToApply() = 0;
	virtual void *C_BasePlayer__ShouldInterpolate() = 0;
	virtual void *C_BaseEntity__BoneMergeFastCullBloat() = 0;
	virtual void *sub_10043080() = 0;
	virtual void *C_BaseEntity__GetShadowUseOtherEntity() = 0;
	virtual void *C_BaseEntity__SetShadowUseOtherEntity() = 0;
	virtual void *sub_100197E0() = 0;
	virtual void *sub_10019930() = 0;
	virtual void *nullsub_180() = 0;
	virtual void *C_BaseAnimating__GetBoneControllers() = 0;
	virtual void *C_BaseAnimating__SetBoneController() = 0;
	virtual void *C_BaseAnimating__GetPoseParameters() = 0;
	virtual void *sub_10039850() = 0;
	virtual void *sub_1002F430() = 0;
	virtual void *sub_100334A0() = 0;
	virtual void *sub_10033550() = 0;
	virtual void *sub_10030150() = 0;
	virtual void *sub_1002FD00() = 0;
	virtual void *sub_1002BEA0() = 0;
	virtual void *sub_10033F70() = 0;
	virtual void *sub_1003E610() = 0;
	virtual void *sub_10038770() = 0;
	virtual void *sub_1002DD30() = 0;
	virtual void *C_BaseAnimating__DispatchMuzzleEffect() = 0;
	virtual void *sub_1003A3C0() = 0;
	virtual void *sub_1004EFD0() = 0;
	virtual void *sub_1003BDF0() = 0;
	virtual void *sub_1003E390() = 0;
	virtual void *nullsub_79() = 0;
	virtual void *sub_1003AD80() = 0;
	virtual void *sub_1003CF70() = 0;
	virtual void *sub_1002BFE0() = 0;
	virtual void *sub_1002F590() = 0;
	virtual void *sub_1002B8D0() = 0;
	virtual void *C_BasePlayer__GetRagdollInitBoneArrays() = 0;
	virtual void *sub_100345B0() = 0;
	virtual void *sub_100346C0() = 0;
	virtual void *sub_100314E0() = 0;
	virtual void *sub_1002C000() = 0;
	virtual void *sub_1002E740() = 0;
	virtual void *sub_1002BFF0() = 0;
	virtual void *nullsub_181() = 0;
	virtual void *sub_100198D0() = 0;
	virtual void *nullsub_182() = 0;
	virtual void *C_BasePlayer__DoMuzzleFlash() = 0;
	virtual void *C_BaseAnimating__ProcessMuzzleFlashEvent() = 0;
	virtual void *nullsub_183() = 0;
	virtual void *sub_10019900() = 0;
	virtual void *sub_1002C7B0() = 0;
	virtual void *sub_1002BAB0() = 0;
	virtual void *sub_10019910() = 0;
	virtual void *nullsub_184() = 0;
	virtual void *sub_1002BAC0() = 0;
	virtual void *sub_1002B9B0() = 0;
	virtual void *C_BaseAnimating__ComputeStencilState() = 0;
	virtual void *C_BaseAnimating__LastBoneChangedTime() = 0;
	virtual void *sub_1004F2E0() = 0;
	virtual void *sub_1001E020() = 0;
	virtual void *sub_1001E030() = 0;
	virtual void *sub_1004F430() = 0;
	virtual void *sub_1004F1F0() = 0;
	virtual void *sub_1004F170() = 0;
	virtual void *sub_1004F1C0() = 0;
	virtual void *sub_10012380() = 0;
	virtual void *sub_100122E0() = 0;
	virtual void *sub_10012620() = 0;
	virtual void *sub_10012430() = 0;
	virtual void *sub_10012DE0() = 0;
	virtual void *sub_10011DC0() = 0;
	virtual void *nullsub_205() = 0;
	virtual void *sub_10012EB0() = 0;
	virtual void *sub_1001E070() = 0;
	virtual void *sub_1001E080() = 0;
	virtual void *sub_1001E090() = 0;
	virtual void *C_BaseCombatCharacter__GetFootstepRunThreshold() = 0;
	virtual void *C_BaseCombatCharacter__GetClass() = 0;
	virtual void *sub_1001E0C0() = 0;
	virtual void *sub_10069870() = 0;
	virtual void *sub_10040B90() = 0;
	virtual void *sub_1001ED80() = 0;
	virtual void *sub_10013330() = 0;
	virtual void *sub_10012A90() = 0;
	virtual C_BaseCombatWeapon *GetActiveWeapon() = 0;
	virtual void *sub_10021E00() = 0;
	virtual void *nullsub_240() = 0;
	virtual void *sub_10021980() = 0;
	virtual void *CalcViewModelView(const Vector &eyeOrigin, const QAngle &eyeAngles) = 0;
	
	char pad_0000[252]; //0x0004
	Vector m_vecVelocity; //0x0100
	char pad_010C[56]; //0x010C
	int m_hGroundEntity; //0x0144

};//Size: 0x0148
static_assert(sizeof(CBasePlayer) == 0x148);

class C_TerrorViewModel
{
public:
	virtual void *sub_10287530() = 0;
	virtual void *sub_100420F0() = 0;
	virtual void *sub_10042110() = 0;
	virtual void *sub_100194D0() = 0;
	virtual void *sub_100194E0() = 0;
	virtual void *sub_100194F0() = 0;
	virtual void *sub_10019500() = 0;
	virtual void *sub_10019510() = 0;
	virtual void *sub_10019520() = 0;
	virtual void *sub_10034210() = 0;
	virtual void *sub_10042070() = 0;
	virtual void *C_BaseEntity__GetAbsOrigin() = 0;
	virtual void *C_BaseEntity__GetAbsAngles() = 0;
	virtual void *sub_1002BAD0() = 0;
	virtual void *sub_10039EE0() = 0;
	virtual void *sub_10043480() = 0;
	virtual void *sub_10043270() = 0;
	virtual void *sub_10286010() = 0;
	virtual void *sub_10022770() = 0;
	virtual void *sub_1001CC10() = 0;
	virtual void *sub_1001A1E0() = 0;
	virtual void *sub_100194A0() = 0;
	virtual void *sub_1001B830() = 0;
	virtual void *sub_1001A1F0() = 0;
	virtual void *sub_1001A200() = 0;
	virtual void *C_BaseEntity__TraceAttack() = 0;
	virtual void *sub_1001A210() = 0;
	virtual void *sub_1001A230() = 0;
	virtual void *sub_1001A2D0() = 0;
	virtual void *sub_1001A2F0() = 0;
	virtual void *sub_1001A300() = 0;
	virtual void *sub_10022790() = 0;
	virtual void *nullsub_170() = 0;
	virtual void *sub_102860A0() = 0;
	virtual void *nullsub_171() = 0;
	virtual void *nullsub_172() = 0;
	virtual void *sub_1001A010() = 0;
	virtual void *sub_10019FA0() = 0;
	virtual void *C_BaseEntity__KeyValue() = 0;
	virtual void *C_BaseEntity__GetKeyValue() = 0;
	virtual void *nullsub_173() = 0;
	virtual void *sub_100487C0() = 0;
	virtual void *sub_10019890() = 0;
	virtual void *unknown_0() = 0;
	virtual void *sub_10031930() = 0;
	virtual void *sub_10019530() = 0;
	virtual void *sub_10030B50() = 0;
	virtual void *sub_10030BE0() = 0;
	virtual void *sub_10043470() = 0;
	virtual void *sub_100430D0() = 0;
	virtual void *sub_1004CD20() = 0;
	virtual void *sub_10049060() = 0;
	virtual void *sub_10286A30() = 0;
	virtual void *unknown_1() = 0;
	virtual void *sub_100427B0() = 0;
	virtual void *sub_10043280() = 0;
	virtual void *sub_100474F0() = 0;
	virtual void *C_BaseEntity__OnRestore() = 0;
	virtual void *sub_100232E0() = 0;
	virtual void *sub_10019950() = 0;
	virtual void *sub_10044FE0() = 0;
	virtual void *sub_10047500() = 0;
	virtual void *sub_1001A150() = 0;
	virtual void *sub_1001A120() = 0;
	virtual void *sub_1002E490() = 0;
	virtual void *sub_1002D580() = 0;
	virtual void *sub_100421C0() = 0;
	virtual void *sub_100495A0() = 0;
	virtual void *sub_100495E0() = 0;
	virtual void *sub_10019830() = 0;
	virtual void *sub_10019850() = 0;
	virtual void *sub_1001BEE0() = 0;
	virtual void *nullsub_175() = 0;
	virtual void *sub_10019810() = 0;
	virtual void *sub_100197F0() = 0;
	virtual void *sub_10022D70() = 0;
	virtual void *sub_10022DB0() = 0;
	virtual void *sub_1002F7F0() = 0;
	virtual void *sub_10042B10() = 0;
	virtual void *unknown_2() = 0;
	virtual void *unknown_3() = 0;
	virtual void *sub_10042B30() = 0;
	virtual void *sub_10042B40() = 0;
	virtual void *sub_10042B80() = 0;
	virtual void *sub_100195B0() = 0;
	virtual void *sub_100195C0() = 0;
	virtual void *sub_100195D0() = 0;
	virtual void *sub_100195E0() = 0;
	virtual void *sub_100195F0() = 0;
	virtual void *sub_10019600() = 0;
	virtual void *sub_100420C0() = 0;
	virtual void *sub_100445A0() = 0;
	virtual void *sub_1004D170() = 0;
	virtual void *C_TerrorViewModel__Simulate() = 0;
	virtual void *sub_10019610() = 0;
	virtual void *sub_100319C0() = 0;
	virtual void *sub_10042780() = 0;
	virtual void *sub_1006AC30() = 0;
	virtual void *sub_100322D0() = 0;
	virtual void *sub_1002C740() = 0;
	virtual void *sub_1002D8D0() = 0;
	virtual void *sub_100232F0() = 0;
	virtual void *sub_1004ED10() = 0;
	virtual void *sub_10286E70() = 0;
	virtual void *sub_10287560() = 0;
	virtual void *sub_102876A0() = 0;
	virtual void *sub_1003B730() = 0;
	virtual void *sub_1006B1F0() = 0;
	virtual void *sub_10042880() = 0;
	virtual void *sub_10044700() = 0;
	virtual void *sub_1002C220() = 0;
	virtual void *sub_10042620() = 0;
	virtual void *sub_10042910() = 0;
	virtual void *nullsub_176() = 0;
	virtual void *sub_10044A50() = 0;
	virtual void *nullsub_177() = 0;
	virtual void *sub_10019650() = 0;
	virtual void *sub_10019660() = 0;
	virtual void *sub_10019670() = 0;
	virtual void *sub_100467C0() = 0;
	virtual void *sub_10043070() = 0;
	virtual void *sub_10022780() = 0;
	virtual void *sub_100471C0() = 0;
	virtual void *sub_1006AF10() = 0;
	virtual void *sub_1004E2D0() = 0;
	virtual void *sub_100331A0() = 0;
	virtual void *sub_1001A0D0() = 0;
	virtual void *sub_1001A8D0() = 0;
	virtual void *sub_1001A950() = 0;
	virtual void *sub_1017A4E0() = 0;
	virtual void *sub_10019690() = 0;
	virtual void *sub_10043420() = 0;
	virtual void *sub_100196B0() = 0;
	virtual void *sub_1004C230() = 0;
	virtual void *sub_100196C0() = 0;
	virtual void *sub_100196D0() = 0;
	virtual void *sub_101653F0() = 0;
	virtual void *nullsub_22() = 0;
	virtual void *sub_10164EC0() = 0;
	virtual void *nullsub_178() = 0;
	virtual void *sub_10165410() = 0;
	virtual void *nullsub_179() = 0;
	virtual void *sub_101686F0() = 0;
	virtual void *sub_100196F0() = 0;
	virtual void *sub_10019700() = 0;
	virtual void *sub_10019710() = 0;
	virtual void *sub_10019720() = 0;
	virtual void *sub_10019730() = 0;
	virtual void *sub_10019740() = 0;
	virtual void *sub_10019750() = 0;
	virtual void *sub_10019760() = 0;
	virtual void *sub_1001E000() = 0;
	virtual void *sub_1001E010() = 0;
	virtual void *sub_10019770() = 0;
	virtual void *sub_10019780() = 0;
	virtual void *C_BaseEntity__EyePosition() = 0;
	virtual void *sub_10019DF0() = 0;
	virtual void *sub_10019E00() = 0;
	virtual void *sub_10019E10() = 0;
	virtual void *sub_1001A090() = 0;
	virtual void *sub_10019870() = 0;
	virtual void *C_BaseEntity__GetViewOffset() = 0;
	virtual void *C_BaseEntity__SetViewOffset() = 0;
	virtual void *C_BaseEntity__GetGroundVelocityToApply() = 0;
	virtual void *C_BaseEntity__ShouldInterpolate() = 0;
	virtual void *C_BaseEntity__BoneMergeFastCullBloat() = 0;
	virtual void *sub_10043080() = 0;
	virtual void *C_BaseEntity__GetShadowUseOtherEntity() = 0;
	virtual void *C_BaseEntity__SetShadowUseOtherEntity() = 0;
	virtual void *sub_100197E0() = 0;
	virtual void *sub_10019930() = 0;
	virtual void *nullsub_180() = 0;
	virtual void *sub_1006AF40() = 0;
	virtual void *C_BaseAnimating__SetBoneController() = 0;
	virtual void *C_BaseAnimating__GetPoseParameters() = 0;
	virtual void *sub_10039850() = 0;
	virtual void *sub_1006ADC0() = 0;
	virtual void *sub_100334A0() = 0;
	virtual void *sub_10033550() = 0;
	virtual void *sub_10030150() = 0;
	virtual void *sub_1002FD00() = 0;
	virtual void *sub_1002BEA0() = 0;
	virtual void *sub_10033F70() = 0;
	virtual void *sub_1003E610() = 0;
	virtual void *sub_1006AFA0() = 0;
	virtual void *sub_1002DD30() = 0;
	virtual void *C_BaseAnimating__DispatchMuzzleEffect() = 0;
	virtual void *sub_1003A3C0() = 0;
	virtual void *sub_1003C050() = 0;
	virtual void *sub_1003BDF0() = 0;
	virtual void *sub_1003E390() = 0;
	virtual void *nullsub_79() = 0;
	virtual void *sub_1003AD80() = 0;
	virtual void *sub_1003CF70() = 0;
	virtual void *sub_1002BFE0() = 0;
	virtual void *sub_1002F590() = 0;
	virtual void *sub_1002B8D0() = 0;
	virtual void *C_BaseAnimating__GetRagdollInitBoneArrays() = 0;
	virtual void *sub_100345B0() = 0;
	virtual void *sub_100346C0() = 0;
	virtual void *sub_100314E0() = 0;
	virtual void *sub_1002C000() = 0;
	virtual void *sub_1002E740() = 0;
	virtual void *sub_1002BFF0() = 0;
	virtual void *nullsub_181() = 0;
	virtual void *sub_100198D0() = 0;
	virtual void *C_BaseViewModel__UncorrectViewModelAttachment() = 0;
	virtual void *sub_1002C400() = 0;
	virtual void *nullsub_221() = 0;
	virtual void *nullsub_183() = 0;
	virtual void *sub_10019900() = 0;
	virtual void *sub_10023330() = 0;
	virtual void *sub_10023300() = 0;
	virtual void *sub_10019910() = 0;
	virtual void *sub_1006ACC0() = 0;
	virtual void *sub_1002BAC0() = 0;
	virtual void *sub_1002B9B0() = 0;
	virtual void *C_BaseAnimating__ComputeStencilState() = 0;
	virtual void *C_BaseAnimating__LastBoneChangedTime() = 0;
	virtual void *sub_10022F50() = 0;
	virtual void *sub_10022E30() = 0;
	virtual void *sub_102860E0() = 0;
	virtual void *CalcViewModelView(CBasePlayer *owner, const Vector &eyePosition, const QAngle &eyeAngles) = 0;
	virtual void *nullsub_222() = 0;
	virtual void *sub_10023350() = 0;
	virtual void *sub_100223D0() = 0;
	virtual void *sub_10022820() = 0;
	virtual void *C_BaseViewModel__GetOwningWeapon() = 0;
	virtual void *sub_1006B8B0() = 0;
	virtual void *sub_102860F0() = 0;
};

class IHandleEntity
{
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle(int handle) = 0;
	virtual void GetRefEHandle() const = 0;
};

class IServerUnknown : public IHandleEntity
{
public:
	// Gets the interface to the collideable + networkable representation of the entity
	virtual void *GetCollideable() = 0;
	virtual void *GetNetworkable() = 0;
	virtual CBaseEntity *GetBaseEntity() = 0;
};

class CBaseEdict
{
public:

	// Returns an IServerEntity if FL_FULLEDICT is set or NULL if this 
	// is a lightweight networking entity.
	void *GetIServerEntity();
	const void *GetIServerEntity() const;

	void *GetNetworkable();
	IServerUnknown *GetUnknown();

	// Set when initting an entity. If it's only a networkable, this is false.
	void				SetEdict(IServerUnknown *pUnk, bool bFullEdict);

	int					AreaNum() const;
	const char *GetClassName() const;

	bool				IsFree() const;
	void				SetFree();
	void				ClearFree();

	bool				HasStateChanged() const;
	void				ClearStateChanged();
	void				StateChanged();
	void				StateChanged(unsigned short offset);

	void				ClearTransmitState();

	void SetChangeInfo(unsigned short info);
	void SetChangeInfoSerialNumber(unsigned short sn);
	unsigned short	 GetChangeInfo() const;
	unsigned short	 GetChangeInfoSerialNumber() const;

public:

	// NOTE: this is in the edict instead of being accessed by a virtual because the engine needs fast access to it.
	// NOTE: YOU CAN'T CHANGE THE LAYOUT OR SIZE OF CBASEEDICT AND REMAIN COMPATIBLE WITH HL2_VC6!!!!!
#ifdef _XBOX
	unsigned short m_fStateFlags;
#else
	int	m_fStateFlags;
#endif	

	// NOTE: this is in the edict instead of being accessed by a virtual because the engine needs fast access to it.
	// int m_NetworkSerialNumber;

	// NOTE: m_EdictIndex is an optimization since computing the edict index
	// from a CBaseEdict* pointer otherwise requires divide-by-20. values for
	// m_NetworkSerialNumber all fit within a 16-bit integer range, so we're
	// repurposing the other 16 bits to cache off the index without changing
	// the overall layout or size of this struct. existing mods compiled with
	// a full 32-bit serial number field should still work. henryg 8/17/2011
#if VALVE_LITTLE_ENDIAN
	short m_NetworkSerialNumber;
	short m_EdictIndex;
#else
	short m_EdictIndex;
	short m_NetworkSerialNumber;
#endif

	// NOTE: this is in the edict instead of being accessed by a virtual because the engine needs fast access to it.
	void *m_pNetworkable;

	IServerUnknown *m_pUnk;


public:

	void *GetChangeAccessor(); // The engine implements this and the game .dll implements as
	const void *GetChangeAccessor() const; // The engine implements this and the game .dll implements as
	// as callback through to the engine!!!

	// NOTE: YOU CAN'T CHANGE THE LAYOUT OR SIZE OF CBASEEDICT AND REMAIN COMPATIBLE WITH HL2_VC6!!!!!
	// This breaks HL2_VC6!!!!!
	// References a CEdictChangeInfo with a list of modified network props.
	//unsigned short m_iChangeInfo;
	//unsigned short m_iChangeInfoSerialNumber;

	friend void InitializeEntityDLLFields(void *pEdict);
};

struct edict_t : public CBaseEdict
{
public:
	void *GetCollideable();

	// The server timestampe at which the edict was freed (so we can try to use other edicts before reallocating this one)
	float		freetime;
};