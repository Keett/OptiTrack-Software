//======================================================================================================
// Copyright 2014, NaturalPoint Inc.
//======================================================================================================
#pragma once

// System includes
#include <string>

// Local includes
#include "Core/IBasicStream.h"
#include "Core/BuildConfig.h"

namespace Core
{
    class cUID;
    class cFilename;

    class CORE_API cIReader : public cIBasicStream
    {
    public:
        virtual ~cIReader() { }

        virtual unsigned long long ReadData( unsigned char *buffer, unsigned long long bufferSize ) = 0;

        virtual int     ReadInt() = 0;
        virtual long long ReadLongLong() = 0;
        virtual long    ReadLong () = 0;
        virtual short   ReadShort() = 0;

        virtual double  ReadDouble() = 0;
        virtual float   ReadFloat() = 0;
        virtual bool    ReadBool() = 0;
        virtual bool    IsEOF() const = 0;

        virtual std::string ReadString() = 0;
        virtual std::wstring ReadWString() = 0;

        virtual cUID    ReadUID() = 0;

        virtual unsigned char ReadByte() = 0;

        //==============================================================================================
        // Included interfaces
        //==============================================================================================

        // cIStream
        virtual unsigned long long Tell() const = 0;
        virtual bool    Seek( unsigned long long pos ) = 0;
        virtual unsigned long long Size() const = 0;
    };
}

