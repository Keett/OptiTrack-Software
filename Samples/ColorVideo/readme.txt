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
