
//=================================================================================-----
//== NaturalPoint 2017
//== Camera Library SDK Sample
//==
//== This sample connects to a single OptiTrack color camera, decodes, and displays
//== the camera's H.264 video.
//==
//== you will need to download the latest libav prebuilt binaries from the libav
//== web site for this sample application to compile and run properly.
//==
//== 1. download the binaries from: http://builds.libav.org/windows/release-lgpl/
//==    note: make sure you download binaries that match your 32bit or 64bit target.
//==          if there is a mismatch, you'll get link errors for unknown libav symbols.
//== 
//==    The filenames of prebuilt libav libraries, can be misleading:
//==
//==        livab-i686 is the library you want for 32-bit build targets.
//==        libav-x86_64 is the library you want for 64-bit build targets.
//==
//== 2. unzip the download file into this sample application's folder ./ColorVideo/
//== 3. rename the resulting folder to 'libav' removing platform & build details
//==    from the folder name.
//== 4. The resulting file folder tree should look as follows:
//==
//==    ./ColorVideo/
//==                 libav/
//==                       usr/
//==                           bin/
//==                           include/
//==                           lib/
//==                           share/
//== 
//== 5. Compile the sample application with at least one OptiTrack color
//==    camera connected.
//== 6. Copy DLL files from your libav/user/bin folder into the solution's working
//==    directory which is ./ColorVideo/
//== 7. Copy the appropriate Camera SDK's DLL from the ../Camera SDK/lib
//==    folder.  The sample uses CameraLibrary2008x64S.DLL for 64-bit targets
//==    and CameraLibrary2008S.DLL for 32-bit targets.
//== 8. Run the sample application with at least one OptiTrack color camera connected.
//==
//=================================================================================-----

#include "cameralibrary.h"     //== Camera Library header file ======================---
#include "supportcode.h"       //== Boiler-plate code for application window init ===---
#include "videodecoder.h"      //== libav h.264 decoder implementation ==============---

using namespace CameraLibrary; 

int main(int argc, char* argv[])
{
	//== For OptiTrack Ethernet cameras, it's important to enable development mode if you
	//== want to stop execution for an extended time while debugging without disconnecting
	//== the Ethernet devices.  Lets do that now:

    CameraLibrary_EnableDevelopment();

	//== Initialize Camera SDK ==--

	CameraLibrary::CameraManager::X();

	//== At this point the Camera SDK is actively looking for all connected cameras and will initialize
	//== them on it's own.

	//== Now, lets pop a dialog that will persist until there is at least one camera that is initialized
	//== or until canceled.

	PopWaitingDialog();

    //== Get a connected camera ================----

    Camera *camera = CameraManager::X().GetCamera();

    //== If no device connected, pop a message box and exit ==--

    if(camera==0)
    {
        MessageBox(0,"Please connect a camera","No Device Connected", MB_OK);
        return 1;
    }

	//== Determine camera resolution to size application window ==----
	
    int cameraWidth  = camera->Width();
    int cameraHeight = camera->Height();

 	//== Open the application window =============================----
		
    if (!CreateAppWindow("Camera Library SDK - Sample",cameraWidth,cameraHeight,32,gFullscreen))
	    return 0;

    //== Create a texture to push the rasterized camera image ====----

    //== We're using textures because it's an easy & cpu light 
    //== way to utilize the 3D hardware to display camera
    //== imagery at high frame rates

    Surface  Texture(cameraWidth, cameraHeight);
    Bitmap * framebuffer = new Bitmap(cameraWidth, cameraHeight, Texture.PixelSpan()*4,
                               Bitmap::ThirtyTwoBit, Texture.GetBuffer());

    //== Color Video Mode ==--

    camera->SetVideoType( Core::VideoMode );                     //== Select Color Video ==============---
    camera->AttachModule( new cModuleVideoDecompressorLibav() ); //== create & attach video decoder ==--
    camera->SetLateDecompression( true );

    //== Set camera settings ==--
    
    camera->SetExposure( 5000 );

    //== Start camera output ==--

    camera->Start();

    //== Turn on some overlay text so it's clear things are     ===---
    //== working even if there is nothing in the camera's view. ===---

    camera->SetTextOverlay(true);

    //== Ok, start main loop.  This loop fetches and displays   ===---
    //== camera frames.                                         ===---

    while(1)
    {
        //== Fetch a new frame from the camera ===---

        Frame *frame = camera->GetLatestFrame();

        if(frame)
        {
            //== Ok, we've received a new frame, lets do something
            //== with it.

            //== Lets have the Camera Library raster the camera's
            //== image into our texture.

            frame->Rasterize(framebuffer);

            //== at this point the framebuffer object contains the decompressed
            //== color image of the frame.

            //== you can fetch individual pixels from this with the following:
            //
            //== PIXEL pixel = framebuffer->GetPixel( x, y );
            //==
            //== pixels are 32-bit integers that are in the format RGBA
            //==
            //== int red   = ( pixel>>24) & 255;
            //== int green = ( pixel>>16) & 255;
            //== int blue  = ( pixel>>8 ) & 255;
            //== int alpha = pixel&255;
            //== 

            //== Display Camera Image ============--

            if(!DrawGLScene(&Texture, frame->Width(), frame->Height()))  
                break;

            //== Escape key to exit application ==--

            if (keys[VK_ESCAPE])
                break;

            //== Release frame =========--

            frame->Release();
        }

	    Sleep(2);

        //== Service Windows Message System ==--

        if(!PumpMessages())
            break;
    }

    //== Close window ==--

    CloseWindow();

    //== Stop video stream ==--

    camera->Stop();
    Sleep( 250 );

    //== Release camera ==--

    camera->Release();

    //== Shutdown Camera Library ==--

    CameraManager::X().Shutdown();

    //== Exit the application.  Simple! ==--

	return 1;
}

