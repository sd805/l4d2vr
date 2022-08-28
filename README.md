# L4D2 VR Prototype
### Use this mod at your own risk of getting VAC banned. Use the -insecure launch option to help protect yourself.

#### [Video demo](https://www.youtube.com/watch?v=zU-8-9qe6wQ)

## Things that work
* Singleplayer w/ bots
* 6DoF VR view
* Motion controls for guns
* Multiplayer (only if the host has the mod and the server is set to local)

## Things that need fixing

* GUI/HUD is broken (use arrow keys to navigate pause menu)
* Melee weapons and interactions require you to aim with your face
* No roomscale movement
* CPU is underutilized

## How to use
1. Download [L4D2VR.zip](https://github.com/sd805/l4d2vr/releases) and extract the files to your Left 4 Dead 2 directory (steamapps\common\Left 4 Dead 2)
2. Launch SteamVR, then launch Left 4 Dead 2 with these launch options:
    * -insecure -window -novid +mat_motion_blur_percent_of_screen_max 0 +crosshair 0
3. In settings, turn off vsync and multicore rendering, and set the display to windowed mode. The rest of the settings are up to you.
4. Load into a campaign and press A on your controller (or F6) to activate VR. 
5. If the camera starts clipping into walls, press down on the left stick to recenter.

## Troubleshooting
If the game is stuttering, try: 
* Steam Settings -> Shader Pre-Caching -> Allow background processing of Vulkan shaders

If the game is crashing, try:
* Lowering video settings
* Disabling SteamVR theater in [Steam settings](https://external-preview.redd.it/1WdLExouo_YKhTGT6C5GGrOjeWO7qNdIdDRvIRBhw-0.png?auto=webp&s=0d4447a9d954e1ec15b2c010cf50eeabd51f4197)
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
