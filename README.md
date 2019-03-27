# The Thing Wrapper
Wrapper for The Thing (video game) to make it easy to run the game in Windowed mode, correct HUD stretching, force anisotropic filtering etc. D3D8 wrapper is based on [ThirteenAG's D3D8 wrapper](https://github.com/ThirteenAG/d3d8-wrapper).

# List of features
* Exposed resolution settings, to allow for non 4:3 resolutions.
* Modified code for window initiation to allow both **Windowed** and **Borderless Windowed Mode**, as well as specify position of a window on desktop.
* Option that overrides default **FOV** for a camera, calculated as horizontal+ screen scaling. You only specify 4:3 FOV value, just like you'd in most games and you don't have to care with wider aspect ratio - wrapper will extend horizontal FOV appropriately for you.
* Improved cinematic scaling to make sure that FMVs are not stretched on non 4:3 resolutions.
* Option to skip intro cinematics.
* Option for scaling 2D rendered elements, so that they don't get stretched on non 4:3 displays.
* Option to override registry values.
* Option to enable MSAA.
* Option to force Anisotropic-flitering.
* Option to enable Trilinear Filtering.
* Option to enable Bump mapping (requires map mapping textures).
* Option to force V-sync (with multiple options for Vsync interval).
* Option to toggle Mouse smoothing.
* Option to force FPS limiter.
* Options to enable various available cheats and developer features left in the game.

# Installation
1. Download *either*:
  *``d3d8.dll`` and ``d3d8.ini``
or
  *``dinput8.dll`` and ``dinput8.ini``
2. Move both dll and ini to a folder where ``TheThing.exe`` is located.