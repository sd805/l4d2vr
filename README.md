# L4D2 VR Prototype
### Use this mod at your own risk of getting VAC banned. Use the -insecure launch option to help protect yourself. (Also contains lots of flashing lights)

#### [Video demo](https://www.youtube.com/watch?v=zU-8-9qe6wQ)

## Things that work
* Singleplayer and multiplayer (see below)
* 6DoF VR view
* Motion controls for guns and melee weapons
* Workshop content

## Things that need fixing

* Interactions and throwables require you to aim with your face
* Roomscale needs work
* CPU is underutilized

## How to use
1. Download [L4D2VR.zip](https://github.com/sd805/l4d2vr/releases) and extract the files to your Left 4 Dead 2 directory (steamapps\common\Left 4 Dead 2)
2. Launch SteamVR, then launch Left 4 Dead 2 with these launch options:
   
   ``` -insecure -window -novid +mat_motion_blur_percent_of_screen_max 0 +crosshair 0 -w 1280 -h 720 +mat_queue_mode 0 +mat_vsync 0 +mat_antialias 0 +mat_grain_scale_override 0 ```

3. At the menu, feel free to change [these video settings](https://i.imgur.com/yYQMXs6.jpg).
4. Load into a campaign. 
5. To recenter the camera height, press down on the left stick. To see the HUD, aim the controller up or down.

## How to play multiplayer
* The host must have the mod installed and the server must be set to local. Other players can play in VR too (if they also installed the mod).
* To host a local server, click Play Campaign -> Play With Friends -> Create new campaign lobby -> Server Type: Local Server
* You can make your game public if you want.
* Versus works but it's barely been tested.

## Troubleshooting
If the game isn't loading in VR:
* Disable SteamVR theater in [Steam settings](https://external-preview.redd.it/1WdLExouo_YKhTGT6C5GGrOjeWO7qNdIdDRvIRBhw-0.png?auto=webp&s=0d4447a9d954e1ec15b2c010cf50eeabd51f4197)

If the game is stuttering, try: 
* Steam Settings -> Shader Pre-Caching -> Allow background processing of Vulkan shaders

If the game is crashing, try:
* Lowering video settings
* Disabling all add-ons then verifying integrity of game files
* Re-installing the game

## Build instructions
1. ``` git clone --recurse-submodules https://github.com/sd805/l4d2vr.git ```
2. Open l4d2vr.sln
3. Set to x86 Debug or Release
4. Build -> Build Solution

Note: After building, it will attempt to copy the new d3d9.dll to your L4D2 directory.

## Utilizes code from
* [VirtualFortress2](https://github.com/PinkMilkProductions/VirtualFortress2)
* [gmcl_openvr](https://github.com/Planimeter/gmcl_openvr/)
* [dxvk](https://github.com/TheIronWolfModding/dxvk/tree/vr-dx9-rel)
* [source-sdk-2013](https://github.com/ValveSoftware/source-sdk-2013/)
