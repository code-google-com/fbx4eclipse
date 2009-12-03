/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material 
 (collectively the "Data") in these files contain unpublished information 
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
 Canada and United States of America federal copyright law and by international 
 treaties. 
 
 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 
 
 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE. 

**************************************************************************************/

/////////////////////////////////////////////////////////////////////////
//
// This example illustrates how to display the content of a FBX or a OBJ file
// in a graphical window. This program is based on the OpenGL Utility Toolkit 
// (GLUT). Start the program on the command line by providing a FBX or a 
// OBJ file name. A menu is provided to select the current camera and the current 
// take.
//
// Among other things, the example illustrates how to:
// 1)  import a scene from a .FBX or .OBJ file;
// 2)  convert the nurbs and patch attribute types of a scene into mesh 
//     node attributes;
// 3)  get the list of all the cameras in the scene;
// 4)  find the current camera;
// 5)  get the relevant settings of a camera depending on it's projection
//     type and aperture mode;
// 6)  compute the local and global positions of a node;
// 7)  compute the orientation of a camera;
// 8)  compute the orientation of a light;
// 9)  compute the shape deformation of mesh vertices;
// 10) compute the link deformation of mesh vertices.
// 11) display the point cache simulation of a mesh.
// 12) get the list of all pose in the scene;
// 13) show the scene using at a specific pose.
//
/////////////////////////////////////////////////////////////////////////

#include <fbxsdk.h>
#include <math.h>

#ifdef WIN32
#include <windows.h>
#endif

//#define GLUT_DISABLE_ATEXIT_HACK
#if defined(__MACH__)
#include <GLUT/glut.h>
#else
#include "GL/glut.h"
#endif

#include <fbxfilesdk/fbxfilesdk_nsuse.h>

#include "InitScene.h"
#include "SetCamera.h"
#include "DrawScene.h"
#include "Texture.h"
#include "Common.h"
#include "AppSettings.h"
#include "ResourceManager.h"

void ExitFunction();
void CreateMenus();
void DrawText(KString& pText);
void CameraSelectionCallback(int pItem);
void TakeSelectionCallback(int pItem);
void MenuSelectionCallback(int pItem);
void PoseSelectionCallback(int pItem);
void TimerCallback(int);
void DisplayCallback();
void ReshapeCallback(int pWidth, int pHeight);
void KeyboardCallback(unsigned char pKey, int, int);
void MouseCallback(int button, int state, int x, int y);
void MotionCallback(int x, int y);

KString ExtractDirectory(KString pFilePath);

// Global variables.
KFbxSdkManager* gSdkManager;
KFbxImporter* gImporter;
KFbxScene* gScene;
int gSceneStatus;
bool bFlipUV;

template<class KString> 
class DeletionPolicy
{
public:
    static inline void DeleteIt(KString** ptr) 
    {
        if ( *ptr != NULL )
        {
            delete *ptr;
            *ptr = NULL;
        }
        KString::AllocatorRelease();
    }
};

KScopedPtr<KString, DeletionPolicy<KString> > gWindowMessage;
KScopedPtr<KString, DeletionPolicy<KString> > gFileName;

KArrayTemplate<KFbxNode*>  gCameraArray;
KArrayTemplate<KString*>   gTakeNameArray;
KArrayTemplate<KFbxPose*>  gPoseArray;
KArrayTemplate<VSTexture*> gTextureArray;
KTime gPeriod, gStart, gStop, gCurrentTime;
int gPoseIndex;
int gX, gY;
KFbxVector4 gCamPosition, gCamCenter;
double gRoll;
int gCameraStatus;
int mainWin = 1;

// Scene status.
enum { UNLOADED, MUST_BE_LOADED, MUST_BE_REFRESHED, REFRESHED };
// Camera status.
enum { CAMERA_NOTHING, CAMERA_ORBIT, CAMERA_ZOOM, CAMERA_PAN};


// Menu item ids.
#define PRODUCER_PERSPECTIVE_ITEM   100
#define PRODUCER_TOP_ITEM           101
#define PRODUCER_BOTTOM_ITEM        102
#define PRODUCER_FRONT_ITEM         103
#define PRODUCER_BACK_ITEM          104
#define PRODUCER_RIGHT_ITEM         105
#define PRODUCER_LEFT_ITEM          106
#define CAMERA_SWITCHER_ITEM        107
#define PLAY_ANIMATION              200
#define EXIT_ITEM                   300

#define SAMPLE_FILENAME "humanoid.fbx"

int main(int argc, char** argv)
{
    // Set exit function to destroy objects created by the FBX SDK.
    atexit(ExitFunction);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(720, 486); 
    glutInitWindowPosition(100, 100);
    int mainWin = glutCreateWindow("ViewScene");

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    glutDisplayFunc(DisplayCallback); 
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);

    // Initialize the scene status flag.
    gSceneStatus = UNLOADED;

    // Create the FBX SDK manager which is the object allocator for almost 
    // all the classes in the SDK and create the scene.
    InitializeSdkObjects(gSdkManager, gScene);

	gWindowMessage.Reset(new KString());
	gFileName.Reset(new KString());

	// Get ini settings
	AppSettings renderSettings("Render");
	ResourceManager::LoadSettings(renderSettings);
	bFlipUV = renderSettings.GetSetting("FlipUV", true);

	if(argc <= 1)
    {
        *gFileName = SAMPLE_FILENAME;
    }
    else
    {
        *gFileName = argv[1];
    }

    if (gSdkManager)
    {
        // Create the importer.
		int lFileFormat = -1;
        gImporter = KFbxImporter::Create(gSdkManager,"");
        if (!gSdkManager->GetIOPluginRegistry()->DetectFileFormat(*gFileName, lFileFormat) )
        {
            // Unrecognizable file format. Try to fall back to KFbxImporter::eFBX_BINARY
            lFileFormat = gSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );;
        }

		// Initialize the importer by providing a filename.
        if(gImporter->Initialize(gFileName->Buffer(), lFileFormat) == true)
        {
            // The file is going to be imported at 
            // the end of the first display callback.
            *gWindowMessage = "Importing file ";
            *gWindowMessage += *gFileName;
            *gWindowMessage += "\nPlease wait!";

            // Set scene status flag to ready to load.
            gSceneStatus = MUST_BE_LOADED;
        }
        else
        {
            *gWindowMessage = "Unable to open file ";
            *gWindowMessage += *gFileName;
            *gWindowMessage += "\nError reported: ";
            *gWindowMessage += gImporter->GetLastErrorString();
            *gWindowMessage += "\nEsc to exit";
        }
    }
    else
    {
        *gWindowMessage = "Unable to create the FBX SDK manager";
        *gWindowMessage += "\nEsc to exit";
    }

    // When gPoseIndex is not -1, draw the scene at that pose
    gPoseIndex = -1;

    glutMainLoop();

    return 0;
}


// Function to destroy objects created by the FBX SDK.
void ExitFunction()
{

    // Delete the FBX SDK manager. All the objects that have been allocated 
    // using the FBX SDK manager and that haven't been explicitely destroyed 
    // are automatically destroyed at the same time.
    if (gSdkManager) gSdkManager->Destroy();
    gSdkManager = NULL;

	//// Delete the array used to associate menu ids with take names.
	//DeleteAndClear(gTakeNameArray);

	//// Delete any texture allocated
	//DeleteAndClear(gTextureArray);
}


// This function is only called in the first display callback
// to make sure that the application window is opened and a 
// status message is displayed before.
void ImportFunction() 
{
    // Make sure that the scene is ready to load.
    if (gSceneStatus == MUST_BE_LOADED)
    {
        if(gImporter->Import(gScene) == true)
        {
            // Set the scene status flag to refresh 
            // the scene in the first timer callback.
            gSceneStatus = MUST_BE_REFRESHED;

			KFbxGlobalSettings& globals = gScene->GetGlobalSettings();

			// Convert Axis System to what is used in this example, if needed
            KFbxAxisSystem SceneAxisSystem = globals.GetAxisSystem();
			//KFbxAxisSystem OurAxisSystem(KFbxAxisSystem::ZAxis, KFbxAxisSystem::ParityEven, KFbxAxisSystem::RightHanded);
			KFbxAxisSystem OurAxisSystem(KFbxAxisSystem::YAxis, KFbxAxisSystem::ParityOdd, KFbxAxisSystem::RightHanded);
            if( SceneAxisSystem != OurAxisSystem )
            {
                OurAxisSystem.ConvertScene(gScene);
            }

            // Convert Unit System to what is used in this example, if needed
            KFbxSystemUnit SceneSystemUnit = gScene->GetGlobalSettings().GetSystemUnit();
            if( SceneSystemUnit.GetScaleFactor() != 1.0 )
            {
                KFbxSystemUnit OurSystemUnit(1.0);
                OurSystemUnit.ConvertScene(gScene);
            }
			else
			{
				KFbxSystemUnit OurSystemUnit(0.02);
				OurSystemUnit.ConvertScene(gScene);
			}

            // Nurbs and patch attribute types are not supported yet.
            // Convert them into mesh node attributes to have them drawn.
            ConvertNurbsAndPatch(gSdkManager, gScene);

            // Convert any .PC2 point cache data into the .MC format for 
            // vertex cache deformer playback.
            PreparePointCacheData(gScene);

            // Get the list of all the cameras in the scene.
            FillCameraArray(gScene, gCameraArray);

            // Get the list of all the takes.
            gScene->FillTakeNameArray(gTakeNameArray);

            // Get the list of pose in the scene
            FillPoseArray(gScene, gPoseArray);

            // Load the texture data in memory (for supported formats)
            LoadSupportedTextures(gScene, gTextureArray);

            // Create the menus to select the current camera and the current take.
            CreateMenus();

            // Initialize the window message.
            *gWindowMessage = "File ";
            *gWindowMessage += *gFileName;
            *gWindowMessage += "\nClick on the right mouse button to enter menu.";
            *gWindowMessage += "\nEsc to exit.";

            // Initialize the frame period.
            gPeriod.SetTime(0, 0, 0, 1, 0, gScene->GetGlobalTimeSettings().GetTimeMode());

            // Call the timer to display the first frame.
            glutTimerFunc((unsigned int) gPeriod.GetMilliSeconds(), TimerCallback, 0);  
        }
        else
        {
            // Import failed, set the scene status flag accordingly.
            gSceneStatus = UNLOADED;

            *gWindowMessage = "Unable to import file ";
            *gWindowMessage += *gFileName;
            *gWindowMessage += "\nError reported: ";
            *gWindowMessage += gImporter->GetLastErrorString();
        }

        // Destroy the importer to release the file.
        gImporter->Destroy();
    }
}

KString ExtractDirectory(KString pFilePath)
{
    KString lFilePath = pFilePath;
    while (lFilePath.FindAndReplace("\\","/")) {}
    int lIndex = lFilePath.ReverseFind('/');
    if (lIndex >= 0)
    {
        lFilePath = lFilePath.Left(lIndex+1);
    }
    return lFilePath;
}

// Create the menus to select the current camera and the current take.
void CreateMenus()
{
    int i;

    // Create the submenu to select the current camera.
    int lCameraMenu = glutCreateMenu(CameraSelectionCallback);

    // Add the producer cameras.
    glutAddMenuEntry(PRODUCER_PERSPECTIVE, PRODUCER_PERSPECTIVE_ITEM);
    glutAddMenuEntry(PRODUCER_TOP, PRODUCER_TOP_ITEM);
    glutAddMenuEntry(PRODUCER_BOTTOM, PRODUCER_BOTTOM_ITEM);
    glutAddMenuEntry(PRODUCER_FRONT, PRODUCER_FRONT_ITEM);
    glutAddMenuEntry(PRODUCER_BACK, PRODUCER_BACK_ITEM);
    glutAddMenuEntry(PRODUCER_RIGHT, PRODUCER_RIGHT_ITEM);
    glutAddMenuEntry(PRODUCER_LEFT, PRODUCER_LEFT_ITEM);

    // Add the camera switcher if there is at least one camera in the scene.
    if (gCameraArray.GetCount() > 0)
    {
        glutAddMenuEntry(CAMERA_SWITCHER, CAMERA_SWITCHER_ITEM);
    }

    // Add the cameras contained in the scene.
    for (i = 0; i < gCameraArray.GetCount(); i++)
    {
        glutAddMenuEntry(gCameraArray[i]->GetName(), i);
    }   

    // Create the submenu to select the current take.
    int lTakeMenu = glutCreateMenu(TakeSelectionCallback);
    int lCurrentTakeIndex = 0;

    // Add the take names.
    for (i = 0; i < gTakeNameArray.GetCount(); i++)
    {
        glutAddMenuEntry(gTakeNameArray[i]->Buffer(), i);

        // Track the current take index.
        if (gTakeNameArray[i]->Compare(gScene->GetCurrentTakeName()) == 0)
        {
            lCurrentTakeIndex = i;
        }
    }

    // Call the take selection callback immediately to 
    // initialize the start, stop and current time.
    TakeSelectionCallback(lCurrentTakeIndex);

    int lBindPoseCount = 0;
    int lRestPoseCount = 0;
    // Create a submenu for bind poses
    int lBindPoseMenu = glutCreateMenu(PoseSelectionCallback);

    // Add the list of bind poses
    for (i = 0; i < gPoseArray.GetCount(); i++)
    {
        if (gPoseArray[i]->IsBindPose())
        {
            glutAddMenuEntry(gPoseArray[i]->GetName(), i);
            lBindPoseCount++;
        }
    }

    // Create a submenu for rest poses
    int lRestPoseMenu = glutCreateMenu(PoseSelectionCallback);

    // Add the list of bind poses
    for (i = 0; i < gPoseArray.GetCount(); i++)
    {
        if (gPoseArray[i]->IsRestPose())
        {
            glutAddMenuEntry(gPoseArray[i]->GetName(), i);
            lRestPoseCount++;
        }
    }

    // Create the submenu to go to a specific pose
    int lPoseMenu = 0;
    if (lBindPoseCount>0 || lRestPoseCount>0)
    {
        lPoseMenu = glutCreateMenu(PoseSelectionCallback);
        if (lBindPoseCount>0)
            glutAddSubMenu("Bind Pose", lBindPoseMenu);
        if (lRestPoseCount>0)
            glutAddSubMenu("Rest Pose", lRestPoseMenu);
    }

    // Build the main menu.
    glutCreateMenu(MenuSelectionCallback);
    glutAddSubMenu("Select Camera", lCameraMenu); 
    glutAddSubMenu("Select Take", lTakeMenu); 
    if (lBindPoseCount>0 || lRestPoseCount>0)
        glutAddSubMenu("Go to Pose", lPoseMenu);
    glutAddMenuEntry("Play", PLAY_ANIMATION);
    glutAddMenuEntry("Exit", EXIT_ITEM); 
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


// Draw text in the upper left area of the window.
void DrawText(KString& pText)
{
    glColor3f(1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float lX = 5;
    float lY = glutGet(GLUT_WINDOW_HEIGHT) - 20;

    glRasterPos2f(lX, lY);

    int i, lCount =(int) pText.GetLen();

    for (i = 0; i < lCount; i++) 
    {
        char lC = pText.Buffer()[i];

        if (lC == '\n')
        {
            lY -= 20;
            glRasterPos2f(lX, lY);
        }
        else
        {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, lC);
        }
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}


// Select the current camera.
void CameraSelectionCallback(int pItem)
{
    KFbxGlobalCameraSettings& lGlobalCameraSettings = gScene->GetGlobalCameraSettings();

    if (pItem == PRODUCER_PERSPECTIVE_ITEM)
    {
        lGlobalCameraSettings.SetDefaultCamera(PRODUCER_PERSPECTIVE);
    }
    else if (pItem == PRODUCER_TOP_ITEM)
    {
        lGlobalCameraSettings.SetDefaultCamera(PRODUCER_TOP);
    }
    else if (pItem == PRODUCER_BOTTOM_ITEM)
    {
        lGlobalCameraSettings.SetDefaultCamera(PRODUCER_BOTTOM);
    }
    else if (pItem == PRODUCER_FRONT_ITEM)
    {
        lGlobalCameraSettings.SetDefaultCamera(PRODUCER_FRONT);
    }
    else if (pItem == PRODUCER_BACK_ITEM)
    {
        lGlobalCameraSettings.SetDefaultCamera(PRODUCER_BACK);
    }
    else if (pItem == PRODUCER_RIGHT_ITEM)
    {
        lGlobalCameraSettings.SetDefaultCamera(PRODUCER_RIGHT);
    }
    else if (pItem == PRODUCER_LEFT_ITEM)
    {
        lGlobalCameraSettings.SetDefaultCamera(PRODUCER_LEFT);
    }
    else if (pItem == CAMERA_SWITCHER_ITEM)
    {
        lGlobalCameraSettings.SetDefaultCamera(CAMERA_SWITCHER);
    }
    else if (pItem >= 0 && pItem < gCameraArray.GetCount())
    {
        lGlobalCameraSettings.SetDefaultCamera((char *)gCameraArray[pItem]->GetName());
    }

    // Set the scene status flag to refresh 
    // the scene in the next timer callback.
    gSceneStatus = MUST_BE_REFRESHED;
}


// Select the current take and set the start, stop and current time.
void TakeSelectionCallback(int pItem)
{
    gScene->SetCurrentTake(gTakeNameArray[pItem]->Buffer());

    KFbxTakeInfo* lCurrentTakeInfo = gScene->GetTakeInfo(*(gTakeNameArray[pItem]));

    if (lCurrentTakeInfo)
    {
        gStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
        gStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
    }
    else
    {
        // Take the time line value
        KTimeSpan lTimeLineTimeSpan;
        gScene->GetGlobalTimeSettings().GetTimelineDefautTimeSpan(lTimeLineTimeSpan);

        gStart = lTimeLineTimeSpan.GetStart();
        gStop  = lTimeLineTimeSpan.GetStop();
    }

    gCurrentTime = gStart;

    // Set the scene status flag to refresh 
    // the scene in the next timer callback.
    gSceneStatus = MUST_BE_REFRESHED;
}

void PoseSelectionCallback(int pItem)
{
    gPoseIndex = pItem;
    gSceneStatus = MUST_BE_REFRESHED;
}

// Exit the application from the main menu.
void MenuSelectionCallback(int pItem)
{
    if (pItem == PLAY_ANIMATION)
    {
        gPoseIndex = -1;
    }
    else if (pItem == EXIT_ITEM)
    {
        exit(0);
    }

    gSceneStatus = MUST_BE_REFRESHED;
}


// Trigger the display of the current frame.
void TimerCallback(int)
{
    // Ask to display the current frame only if necessary.
    if (gSceneStatus == MUST_BE_REFRESHED)
    {
        glutPostRedisplay();
    }

    // Loop in the take.
    if (gStop > gStart)
    {
        // Set the scene status flag to refresh 
        // the scene in the next timer callback.
        gSceneStatus = MUST_BE_REFRESHED;

        gCurrentTime += gPeriod;

        if (gCurrentTime > gStop) 
        {
            gCurrentTime = gStart;
        }
    }
    // Avoid displaying the same frame on 
    // and on if the take has no length.
    else
    {
        // Set the scene status flag to avoid refreshing 
        // the scene in the next timer callback.
        gSceneStatus = REFRESHED;
    }

    // Call the timer to display the next frame.
    glutTimerFunc((unsigned int) gPeriod.GetMilliSeconds(), TimerCallback, 0);
}


// Refresh the appplication window.
void DisplayCallback()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Test if the scene has been loaded yet.
    if (gSceneStatus != UNLOADED && gSceneStatus != MUST_BE_LOADED)
    {
        glEnable(GL_DEPTH_TEST);

        // Set the view to the current camera settings.
        SetCamera(gScene, gCurrentTime, gCameraArray);

        if (gPoseIndex == -1)
        {
            // Draw the scene at the current time.
            DrawScene(gScene, gCurrentTime);
        }
        else
        {
            // Draw the scene in the specified pose
            DrawSceneAtPose(gScene, gCurrentTime, gPoseIndex);
        }
    }

    glDisable(GL_DEPTH_TEST);

    // Draw the window message.
    DrawText(*gWindowMessage);

    glFlush();
    glutSwapBuffers();

    // Import the scene if it's ready to load.
    if (gSceneStatus == MUST_BE_LOADED)
    {
        // This function is only called in the first display callback
        // to make sure that the application window is opened and a 
        // status message is displayed before.
        ImportFunction();
    }
}


// Resize the application window.
void ReshapeCallback(int pWidth, int pHeight)
{
    glViewport(0, 0, (GLsizei) pWidth, (GLsizei) pHeight); 
}


// Exit the application from the keyboard.
void KeyboardCallback(unsigned char pKey, int, int)
{
    // Exit on ESC key.
    if (pKey == 27)
    {
        exit(0);
    }

    // Zoom In on '+' or '=' keypad keys
    if (pKey == 43 || pKey == 61)
    {
        KFbxCamera* lCamera = GetCurrentCamera(gScene);
        if(lCamera)
        {
            //double lOriginalAperture = sqrt(lCamera->GetApertureWidth());
            CameraZoomIn(gScene, 10);
            gSceneStatus = MUST_BE_REFRESHED;
        }
    }

    // Zoom Out on '-' or '_' keypad keys
    if (pKey == 45 || pKey == 95)
    {
        KFbxCamera* lCamera = GetCurrentCamera(gScene);
        if(lCamera)
        {
            //double lOriginalAperture = sqrt(lCamera->GetApertureWidth()); 
            CameraZoomOut(gScene, 10);
            gSceneStatus = MUST_BE_REFRESHED;
        }
    }
}

void MouseCallback(int button, int state, int x, int y) {
    // Move the camera (orbit, zoom or pan) with the mouse.

    KFbxCamera* lCamera = GetCurrentCamera(gScene);


    if (lCamera)
    {
        gCamPosition = lCamera->Position.Get();
        gCamCenter = lCamera->InterestPosition.Get();
        gRoll = lCamera->Roll.Get();
    }
    gX = x;
    gY = y;

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        // ORBIT (or PAN)
        switch (state) {
    case GLUT_DOWN:
        if (gCameraStatus == CAMERA_ZOOM) {
            gCameraStatus = CAMERA_PAN;
        } else {
            gCameraStatus = CAMERA_ORBIT;
        }
        break;
    default:
        if (gCameraStatus == CAMERA_PAN) {
            gCameraStatus = CAMERA_ZOOM;
        } else {
            gCameraStatus = CAMERA_NOTHING;
        }
        break;
        }
        break;
    case GLUT_MIDDLE_BUTTON:
        // ZOOM (or PAN)
        switch (state) {
    case GLUT_DOWN:
        if (gCameraStatus == CAMERA_ORBIT) {
            gCameraStatus = CAMERA_PAN;
        } else {
            gCameraStatus = CAMERA_ZOOM;
        }
        break;
    default:
        if (gCameraStatus == CAMERA_PAN) {
            gCameraStatus = CAMERA_ORBIT;
        } else {
            gCameraStatus = CAMERA_NOTHING;
        }
        break;
        }
        break;
    }

}


void MotionCallback(int x, int y) {

    int motion;

    switch (gCameraStatus) {
    case CAMERA_ORBIT:
        CameraOrbit(gScene, gCamPosition, gRoll, x-gX, gY-y);
        gSceneStatus = MUST_BE_REFRESHED;
        break;
    case CAMERA_ZOOM:
        motion = gY-y;
        if (motion > 0) {
            CameraZoomIn(gScene, motion);
        } else {
            CameraZoomOut(gScene, -motion);
        }
        gY = y;
        gSceneStatus = MUST_BE_REFRESHED;
        break;
    case CAMERA_PAN:
        CameraPan(gScene, gCamPosition, gCamCenter, gRoll, x-gX, gY-y);
        gSceneStatus = MUST_BE_REFRESHED;
        break;
    }

}

#ifdef WIN32

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{
    return main(__argc,__argv);
}

#endif




