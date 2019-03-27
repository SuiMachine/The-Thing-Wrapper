Important note, as I have not made it clear before. Since I developed the "wrapper" first as dinput8 and then reworked it to d3d8, in order to be able to hook d3d8 calls.

Use one **OR** the other. Do **NOT** use both at the same time. The scope of changes they apply is pretty much the same, aside from d3d8 wrapper being able to use some additional features listed under ``[D3D8]`` section of ini file. However, since there exists D3D8 wrappers that actually do apply quite extensive improvements (like dgVoodoo 2) you may sometimes want to use dinput8.dll instead and use DirectX wrapper (like dgVoodoo 2) for API wrapping D3D8 calls instead. Your choice.

As for installation it boils down to:

1. Download *either*:
  *``d3d8.dll`` and ``d3d8.ini``
or
  *``dinput8.dll`` and ``dinput8.ini``
2. Move both dll and ini to a folder where ``TheThing.exe`` is located.