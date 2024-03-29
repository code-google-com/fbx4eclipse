             tazpn's DAO Tools 1.5
             =====================

    This packages includes an command line application for
      converting Dragon Age models from the native format
      to something that can be manipulated or used a reference for 
      non-derivative works.
      
    Currently the package only converts from native MMH files to 
      FBX, 3DS, DAE, OBJ formats which are probably the most widely 
      interopable formats used in the industry.
      
    Included is a viewer largely derived from the Autodesk FBX SDK
      modified slightly to support DDS files and loading MMH files 
      directly.
      
    Enjoy!

    
    
Quick Start:
  1. Use daocmd.exe or ErfEditor.exe to unpack ERF and RIM files in 
       Dragon Age\packages\core\data
    > daocmd erf "%ProgramFiles%\Electronic Arts\Dragon Age Origins\packages\core\data\modelhierarchies.erf"
    > daocmd erf "%ProgramFiles%\Electronic Arts\Dragon Age Origins\packages\core\data\textures.erf"
    > daocmd erf "%ProgramFiles%\Electronic Arts\Dragon Age Origins\packages\core\textures\high\texturepack.erf"
    
  2. Convert MMH to FBX 
    > fbxcmd Convert c_berkbeara_0.mmh "%TEMP%\c_berkbeara_0.fbx"
    
  3. Convert MMH to 3ds
    > fbxcmd Convert c_berkbeara_0.mmh "%TEMP%\c_berkbeara_0.3ds"
    
  4. Convert MMH to COLLADA DAE
    > fbxcmd Convert c_berkbeara_0.mmh "%TEMP%\c_berkbeara_0.dae"
    
  5. Convert MMH to Wavefront OBJ
    > fbxcmd Convert c_berkbeara_0.mmh "%TEMP%\c_berkbeara_0.obj"

  6. View Scene
    > viewscene c_berkbeara_0.mmh 
    > viewscene "%TEMP%\c_berkbeara_0.fbx"
    > viewscene "%TEMP%\c_berkbeara_0.dae"

  7. Convert FBX to MMH.XML
    > fbxcmd convert c_berkbeara_0.fbx c_berkbeara_0.mmh.xml
    
  8. Compile MMH.XML to MMH and MSH.XML to MSH
    > "%ProgramFiles%\Electronic Arts\Dragon Age Origins\tools\ResourceBuild\Processors\MMH\GraphicsProcessorMMH.exe" c_berkbeara_0.mmh.xml
    > "%ProgramFiles%\Electronic Arts\Dragon Age Origins\tools\ResourceBuild\Processors\MSH\GraphicsProcessorMSH.exe" -platform pc mmdtogff c_berkbeara_0.msh.xml
    
TODO:
  - Support physics models (collision import/export)
  - Clean up existing conversion
  - Way too much to list here really

  
Anticipated Issues:
  - When converting from FBX files I expect a lot of 'required' information 
    will not be available so hand editing files (or additional tools) 
    may be required to get content to work in the game.
    

Known Issues:

  - While the tools aim to support widely supported formats 3DS, FBX, DAE 
     these formats are not really transportable across in the various 3d 
     modeling packages in practice.  Useful information is frequently lost.
     
  - My experience is that XSI, blender, 3ds max, MotionBuilder cannot really 
     import or export the files properly.  This seems likely to mostly due 
     to limitions of the importer/exporter adapters in those packages.  I aim 
     to get as compatible as possible but that is a very tall order.
