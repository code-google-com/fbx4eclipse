                   tazpn's DA:O 3ds Max Tools 1.0
                   ==============================

    This plugin currently consists of a simple importer and exporter 
      for mesh hierarchies from Dragon Age: Origins.
    
    Requirements
    ------------

      3D Studio Max 4, 5, 6, 7, 8, 9, 2008, 2009, 2010
      gmax 1.2

    Installation
    ------------

    If installing with installer:
    
      The installer will use the registry install locations. Testing is with
        English Windows and English 3ds Max other locales may not work properly.
      
      If Source is checked it will install the source to 
         Neverwinter Nights 2\Source\nwn2utils
      
    
    If installing from the RAR or 7z archive files:
      Copy appropriate version of daomax.dlu to your 3dsmax\plugins directory.
      Copy daomax.ini to your 3dsmax\plugcfg directory.
        
      For 3ds Max 9 or higher, copy daomax.ini to your : 
        {sd}\Documents and Settings\{user name}\Local Settings\Application Data\
          Autodesk\3dsmax\9 - {xx}bit\enu\plugcfg
		  
        , where {sd} is your system drive, {username} is your login, and {xx} is 
          32 or 64 depending on your operating system.
          
      For gmax 1.2, copy nw2gmax.exe to your "\gmax12" directory.
	  
	  If you are running gmax 1.2 on Vista or Windows 7 and get the following error on import:
			CoCreateInstance() failed
			Please check your registry entries
			CLSID {F088EA74-2E87-11D3-B1F3-00C0F03C37D3}

		Open a Command Prompt as Administrator and run the following in the gmax directory.
			"regsvr32.exe MAXComponents.dll"
          
     Its very useful to "Configure User Paths" for "External Files" and add the 
       locations where you unzipped "nwn2_Materials.zip".  You probably also want
       to add the NWN2 Override and your My Documents/NWN2/Override directories
       as well before the other location.

    Documentation
    -------------
     MDB File Format: http://www.nwn2wiki.org/MDB_Format

    Usage
    -----
    
    Importer:
      Import files using "File->Import->DA:O Importer(*.MDB)"
      
    Exporter:
      Export files using "File->Export->DA:O Exporter(*.MMH,*.MSH)"
    
    


    Known Issues
    ------------
      None
    
      
    Contact
    ---------
    The author can be reached at: tazpn314 [at] gmail [dot] com
        
    Copyright
    ---------
      
    Copyright (c) 2009. All rights reserved.
    
    Legal
    -----
      
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
