# L4D2 VR Prototype
### Use this mod at your own risk of getting VAC banned. Use the -insecure launch option to help protect yourself.

#### [Video demo](https://www.youtube.com/watch?v=zU-8-9qe6wQ)

## Things that work
* Singleplayer w/ bots
* 6DoF VR view
* Motion controls for guns

## Things that need fixing
* Multiplayer
* Resolution is limited by monitor's resolution (use DSR or VSR to increase your desktop resolution)
* Melee weapons and interactions require you to aim with your face
* Menu must be navigated from monitor
* CPU is underutilized
* Some water/fire shaders are broken

## How to use
1. Download [L4D2VR.zip](https://github.com/sd805/l4d2vr/releases) and extract the files to your Left 4 Dead 2 directory (steamapps\common\Left 4 Dead 2)
2. Launch SteamVR, then launch Left 4 Dead 2 with these launch options:
    * -insecure -window -novid +mat_motion_blur_percent_of_screen_max 0 +crosshair 0
3. Set your video settings to [these](https://i.imgur.com/j6YK1k9.jpg). If you want better texture quality, set Shader Detail to medium, and Paged Pool Memory to high.
4. Start a campaign and press A on your controller (or F6) to activate VR.
5. If the camera starts clipping into walls, press down on the left stick to recenter.

## If the game is crashing, try:
* Setting Paged Pool Memory to High, and Shaders/Effects/Textures to low.
* Disabling SteamVR theater in [Steam settings](https://external-preview.redd.it/1WdLExouo_YKhTGT6C5GGrOjeWO7qNdIdDRvIRBhw-0.png?auto=webp&s=0d4447a9d954e1ec15b2c010cf50eeabd51f4197)
* Disabling all add-ons then verifying integrity of game files
* Re-installing the game

The crashing is likely due to the DX9 to DX12 wrapper. I will look into using DXVK instead.

## Utilizes code from
* [VirtualFortress2](https://github.com/PinkMilkProductions/VirtualFortress2)
* [gmcl_openvr](https://github.com/Planimeter/gmcl_openvr/)
* [neun-auf-zwoelf](https://github.com/Joshua-Ashton/neun-auf-zwoelf/)
* [source-sdk-2013](https://github.com/ValveSoftware/source-sdk-2013/)
