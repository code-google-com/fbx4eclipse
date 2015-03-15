Here is a quick summary of common commands used in conjunction with the tools in this project to unpack and convert Dragon Age: Origins.

Quick Start:
  * Use daocmd.exe or ErfEditor.exe to unpack ERF and RIM files in Dragon Age\packages\core\data
    * daocmd erf "%ProgramFiles%\Electronic Arts\Dragon Age Origins\packages\core\data\modelhierarchies.erf"
    * daocmd erf "%ProgramFiles%\Electronic Arts\Dragon Age Origins\packages\core\data\textures.erf"
    * daocmd erf "%ProgramFiles%\Electronic Arts\Dragon Age Origins\packages\core\textures\high\texturepack.erf"

  * Convert MMH to FBX
    * fbxcmd Convert c\_berkbeara\_0.mmh "%TEMP%\c\_berkbeara\_0.fbx"

  * Convert MMH to 3ds
    * fbxcmd Convert c\_berkbeara\_0.mmh "%TEMP%\c\_berkbeara\_0.3ds"

  * Convert MMH to COLLADA DAE
    * fbxcmd Convert c\_berkbeara\_0.mmh "%TEMP%\c\_berkbeara\_0.dae"

  * Convert MMH to Wavefront OBJ
    * fbxcmd Convert c\_berkbeara\_0.mmh "%TEMP%\c\_berkbeara\_0.obj"

  * View Scene
    * viewscene c\_berkbeara\_0.mmh
    * viewscene "%TEMP%\c\_berkbeara\_0.fbx"
    * viewscene "%TEMP%\c\_berkbeara\_0.dae"

  * Convert FBX to MMH.XML
    * fbxcmd convert c\_berkbeara\_0.fbx c\_berkbeara\_0.mmh.xml

  * Compile MMH.XML to MMH and MSH.XML to MSH
    * "%ProgramFiles%\Electronic Arts\Dragon Age Origins\tools\ResourceBuild\Processors\MMH\GraphicsProcessorMMH.exe" c\_berkbeara\_0.mmh.xml
    * "%ProgramFiles%\Electronic Arts\Dragon Age Origins\tools\ResourceBuild\Processors\MSH\GraphicsProcessorMSH.exe" -platform pc mmdtogff c\_berkbeara\_0.msh.xml
