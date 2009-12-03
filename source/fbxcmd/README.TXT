             tazpn's DAO Tools 1.0
             =====================

    This packages includes an command line application for
      converting Dragon Age models from the native format
	  to something that can be manipulated or used a reference for 
	  non-derivative works.
	  
	Currently the package only converts from native MMH files to 
	  FBX, 3DS, DAE, OBJ formats which are probably the most widely 
	  interopable formats used in the industry.
	  
	  
    Enjoy!

TODO:
  - Convert from FBX to DAO compatable (MMH or MMH.XML files)
  - Support physics models
  - Clean up existing 

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
