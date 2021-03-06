//======================================================================================================
// Copyright 2013, NaturalPoint Inc.
//======================================================================================================
#pragma once

// Local includes
#include "Core/BuildConfig.h"
#include "Core/Vector3.h"
#include "Core/Quaternion.h"

namespace Core
{
    /// <summary>Represents a "simple" transform (i.e. scale is assumed to be 1).</summary>
    template <typename T>
    class cSimpleTransform
    {
    public:
        cSimpleTransform() : mTranslation( cVector3<T>::kZero ), mRotation( cQuaternion<T,true>::kIdentity )
        {
        }
        cSimpleTransform( const cVector3<T> &translation, const cQuaternion<T,true> &rotation ) :
            mTranslation( translation ), mRotation( rotation )
        {
        }

        /// <summary>Set the translation component.</summary>
        inline void     SetTranslation( const cVector3<T> &translate )
        {
            mTranslation = translate;
        }

        /// <summary>Set the rotation component.</summary>
        inline void     SetRotation( const cQuaternion<T,true> &rotate )
        {
            mRotation = rotate;
        }

        /// <summary>Accumulates the given rotation into the transform.</summary>
        inline void     Rotate( const cQuaternion<T,true> &rotation )
        {
            mRotation *= rotation;
        }

        /// <summary>
        /// Accumulates the given translation into the transform. Given translation is understood to
        /// to be in the local coordinate system.
        /// </summary>
        inline void     Translate( const cVector3<T> &translation )
        {
            mTranslation += mRotation.Rotate( translation );
        }

        /// <summary>Retrieve the translation component.</summary>
        inline cVector3<T> Translation() const { return mTranslation; }

        /// <summary>Retrieve the rotation component.</summary>
        inline cQuaternion<T,true> Rotation() const { return mRotation; }

        /// <summary>Invert this transform in place.</summary>
        inline void     Invert()
        {
            mRotation.Invert();
            mTranslation = mRotation.Rotate( -mTranslation );
        }

        /// <summary>Transforms our coordinate system by the given transform.</summary>
        inline void     Transform( const cSimpleTransform &transform )
        {
            mTranslation += mRotation.Rotate( transform.mTranslation );
            mRotation = transform.mRotation * mRotation;
        }

        /// <summary>Transforms our coordinate system by the inverse of the given transform.</summary>
        inline void     InverseTransform( const cSimpleTransform &transform )
        {
            mTranslation = transform.InverseTransform( mTranslation );
            mRotation  = mRotation * transform.mRotation.Inverse();
        }

        /// <summary>
        /// Transform the given point through this transform. Transform order is Rotate-Translate by convention.
        /// </summary>
        inline cVector3<T> Transform( const cVector3<T> &pnt ) const
        {
            cVector3<T> result( pnt );

            result = mRotation.Rotate( result );
            result += mTranslation;

            return result;
        }

        /// <summary>
        /// Transform the given point through the inverse of this transform. Transform order is Rotate-Translate by convention.
        /// </summary>
        inline cVector3<T> InverseTransform( const cVector3<T> &pnt ) const
        {
            cVector3<T> result( pnt );

            result -= mTranslation;
            result = mRotation.Inverse().Rotate( result );

            return result;
        }

        /// <summary>Returns true if the transform is the identity (i.e. no transform).</summary>
        inline bool     IsIdentity() const
        {
            return ( mTranslation == cVector3<T>::kZero && mRotation == cQuaternion<T,true>::kIdentity );
        }

        /// <summary>Does a comparison with another transform. This is an approximate comparison.</summary>
        inline bool     Equals( const cSimpleTransform &other, T tolerance ) const
        {
            return ( mTranslation.Equals( other.mTranslation, tolerance ) && mRotation.Equals( other.mRotation, tolerance ) );
        }

        /// <summary>Returns a zero transform.  This means 'no translation or rotation'.</summary>
        static const cSimpleTransform kZeroTransform;


        //====================================================================================
        // Type conversion helpers
        //====================================================================================

        inline Core::cSimpleTransform<float> ToFloat() const
        {
            return Core::cSimpleTransform<float>( Translation().ToFloat(), Rotation().ToFloat() );
        }

        inline Core::cSimpleTransform<double> ToDouble() const
        {
            return Core::cSimpleTransform<double>( Translation().ToDouble(), Rotation.ToDouble() );
        }

    private:
        cVector3<T>     mTranslation;
        cQuaternion<T,true> mRotation;
    };

    template <typename T>
    const cSimpleTransform<T> cSimpleTransform<T>::kZeroTransform( cVector3<T>::kZero, cQuaternion<T,true>::kIdentity );

    // Helpful type definitions

    typedef cSimpleTransform<float> CORE_API cSimpleTransformf;
    typedef cSimpleTransform<double> CORE_API cSimpleTransformd;
}

