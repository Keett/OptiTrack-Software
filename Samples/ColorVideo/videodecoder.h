//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================


//============================================================================================-----
//============================================================================================-----
//== H.264 Video Encoder Module 
//==
//== This pluggable module is designed to be attached to one Camera SDK camera.  You can
//== Instantiate as many as you need for as many cameras.  This is the same design pattern
//== as our pluggable JPEG decoders used in the Camera SDK and Motive.
//==
//============================================================================================-----
//============================================================================================-----

#ifndef _H264DECOMPRESSOR_H_
#define _H264DECOMPRESSOR_H_

#if defined(LIBAVIMPORTED)

#include "Core/Platform.h"

#include "cameramodulebase.h"

#include <stdio.h>

//== Libav Includes ==========================================================================-----

extern "C" {
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

//== Video Decoder Camera Module Utilizing Libav for Decode ==================================-----

namespace CameraLibrary
{
    const int kH264DefaultBufferSize = 32768;

    class cModuleVideoDecompressorLibav : public CameraLibrary::cCameraModule
    {
    public:
        cModuleVideoDecompressorLibav();
        ~cModuleVideoDecompressorLibav();

        void OutgoingComm( Camera *Camera, unsigned char *Buffer, long BufferSize ) {}
        bool PostVideoData( Camera *Camera, unsigned char *Buffer, long BufferSize, Frame *Frame, int FrameWidth, int FrameHeight, unsigned char *AlignedFrameBuffer, long AlignedFrameBufferSize ) override;

        int  Read( unsigned char * Buffer, int BufferSize );

        unsigned char * AlignedMemoryAllocation( long size );

    private:
        bool             Initialize( unsigned char * Buffer, int BufferSize );
        void             Shutdown();

        bool             mLibavInitialized;
        
        SwsContext *     mColorConversionContext;
        AVFrame *        mLibavDecodeFrame;
        AVCodecContext * mDecoderContext;
        AVFrame *        mLibavFrame;
        AVCodec *        mDecoder;
        
    };
}

#else

#include "Core/Platform.h"
#include "cameramodulebase.h"

namespace CameraLibrary
{
    class cModuleVideoDecompressorLibav : public CameraLibrary::cCameraModule
    {
    };
}

#endif

#endif