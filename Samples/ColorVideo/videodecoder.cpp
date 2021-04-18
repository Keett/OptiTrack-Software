//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

//============================================================================================-----
//============================================================================================-----
//== H.264 Video Decoder Module 
//==
//== This pluggable module is designed to be attached to one Camera SDK camera.  You can
//== Instantiate as many as you need for as many cameras.  This is the same design pattern
//== as our pluggable JPEG decoders used in the Camera SDK and Motive.
//==
//============================================================================================-----
//============================================================================================-----

#if defined(LIBAVIMPORTED)

//== Includes ================================================================================-----

#ifdef WIN32
#include <windows.h>
#endif

//== Libav Includes ==========================================================================-----

extern "C"
{
#include "libavutil/pixfmt.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

//== Includes ================================================================================-----

#include "Core/DebugSystem.h"
#include "Core/Serializer.h"
#include "Core/Timer.h"

#include "frame.h"
#include "camera.h"

#include "videodecoder.h"

//== H264 Video Decoder ======================================================================-----

CameraLibrary::cModuleVideoDecompressorLibav::cModuleVideoDecompressorLibav() 
    : mColorConversionContext( 0 )
    , mLibavInitialized( false )
    , mLibavDecodeFrame( 0 )
    , mLibavFrame( 0 )   
{
}

CameraLibrary::cModuleVideoDecompressorLibav::~cModuleVideoDecompressorLibav()
{
    Shutdown();
}

bool CameraLibrary::cModuleVideoDecompressorLibav::PostVideoData (Camera *Camera, unsigned char *Buffer, long BufferSize, Frame *Frame, int FrameWidth, int FrameHeight, unsigned char *AlignedFrameBuffer, long AlignedFrameBufferSize )
{
    if( !mLibavInitialized )
    {
        //== initialize libav ==--
        mLibavInitialized = Initialize( Buffer, BufferSize );
    }
    
    if( !mLibavInitialized )
    {
        //== if initialization has failed just clear the output buffer to black & exit ==--
        memset( AlignedFrameBuffer, 0, AlignedFrameBufferSize );
        return false;
    }

    AVPacket        packet;
    
    av_init_packet( &packet );

    packet.data = (unsigned char*) Buffer;
    packet.size = (int) BufferSize;

    int framefinished=0;

    int nres=avcodec_decode_video2( mDecoderContext, mLibavDecodeFrame, &framefinished, &packet );

    int type = mDecoderContext->thread_type;
    int activeType = mDecoderContext->active_thread_type;
    int count = mDecoderContext->thread_count;
    

    if( nres<0)
    {
        char st[256];
        av_strerror (nres, st, 256);
        printf( "avcodec_decode_video2 >> returned error: %s", st );
        avcodec_flush_buffers( mDecoderContext );
    }

    if( framefinished )
    {
        // Convert the image from its native format to RGBA
        
        unsigned char * alignedDestinationBuffer = AlignedFrameBuffer;
        int destWidth = (AlignedFrameBufferSize/FrameHeight)/4;
        int destHeight = FrameHeight;

        mColorConversionContext = sws_getCachedContext( mColorConversionContext,
            mDecoderContext->width, mDecoderContext->height, mDecoderContext->pix_fmt, mDecoderContext->width, mDecoderContext->height,
            AV_PIX_FMT_RGBA, SWS_BILINEAR, 0, 0, 0 );

        // Allocate an AVFrame structure

        if( mLibavFrame==0 )
        {
            mLibavFrame=av_frame_alloc();
            if( mLibavFrame==NULL )
            {

                printf( "unable to allocate libav frame" );
            }
        }

        avpicture_fill( (AVPicture *) mLibavFrame, alignedDestinationBuffer, AV_PIX_FMT_RGBA,
            destWidth, destHeight );

        if( !mLibavDecodeFrame->data[ 0 ] )
        {
            printf( "no frame data" );
        }
        else
        {
            sws_scale
                (
                mColorConversionContext,
                (uint8_t const * const *) mLibavDecodeFrame->data,
                mLibavDecodeFrame->linesize,
                0,
                mDecoderContext->height,
                mLibavFrame->data,
                mLibavFrame->linesize
                );


            if( !mLibavFrame->data[ 0 ] )
            {
                printf( "averror unable to get libav frame data = NULL done reading file" );
            }
        }    
    }
    
    return true;  //== Report that we decoded the data into the frame buffer ==--
}

bool CameraLibrary::cModuleVideoDecompressorLibav::Initialize( unsigned char * Buffer, int BufferSize )
{
    int ret = 0;
    
    av_register_all();

    mDecoder = avcodec_find_decoder( AV_CODEC_ID_H264 );
    if( !mDecoder )
    {
         printf( "unable to find h264 decoder" );
         return false;
    }

    mDecoderContext = avcodec_alloc_context3( mDecoder );
    if( !mDecoderContext )
    {
        printf( "unable to allocate decoder context" );
        return false;
    }

    int plz = mDecoder->capabilities&CODEC_CAP_SLICE_THREADS;

    avcodec_get_context_defaults3( mDecoderContext, mDecoder );

    mDecoderContext->ticks_per_frame=1;

    AVDictionary* dictionary = nullptr;

    int res = avcodec_open2( mDecoderContext, mDecoder, &dictionary );

    if( res < 0 )
    {
        char str_err[ 256 ];
        av_strerror(res,str_err,256);
        printf( "unable to open decoder: %s : (%d)", str_err, res );
        return false;
    }

    mLibavDecodeFrame = av_frame_alloc();
    if ( !mLibavDecodeFrame )
    {
        printf( "Could not allocate decode frame" );
        return false;
    }

    return true;
}

void CameraLibrary::cModuleVideoDecompressorLibav::Shutdown()
{
    if( mLibavInitialized )
    {
        mLibavInitialized = false;

        if( mLibavDecodeFrame )
        {
            av_frame_free( &mLibavDecodeFrame );
        }
        
        if( mLibavFrame )
        {
            av_frame_free( &mLibavFrame );
        }

        if( mColorConversionContext )
        {
            sws_freeContext( mColorConversionContext );
        }

        if( mDecoderContext )
        {
            avcodec_free_context( &mDecoderContext );
        }
    }
}

unsigned char * CameraLibrary::cModuleVideoDecompressorLibav::AlignedMemoryAllocation( long size )
{
    return (unsigned char*) av_malloc( size );
}

#else

#endif