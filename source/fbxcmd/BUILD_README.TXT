                     fbxcmd 1.0
                  =================

Build requirements:
  1. Visual Studio 2008 (Express, Standard, Professional, Team)

  2. FBX SDK version 2010.2.  
     - Available for free from Autodesk website (with registration)

  3. 


Required Configuration Changes:
  1. Edit fbxcmd\Sheets\fbx_Settings.vsprops 
     - Change FBXSDKDir Usermacro to reference the installed location of the FBX SDK

  2. Load dao.sln into Visual Studio 2008.  
     - Ignore warning if building using Express Edition.
    
    Enjoy!
