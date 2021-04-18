//======================================================================================================-----
//== NaturalPoint 2009
//======================================================================================================-----

// NOTE: The comments immediately following seem misleading. Here's what I've found looking at this code:
//
//      1. Matrix::get_YRP() is consistent with Eberly's ZXY factorization (WITHOUT handling degenerate cases). [1]
//      2. Returned values: "Pitch" is rotation about X, "yaw" is rotation about Y, and "roll" is rotation about Z.
//      3. That IS consistent with Computer Graphics: Principles and Practice (Foley, van Dam, et al), which describes
//         a right-handed, Y-up, Z-forward coordinate system, but NOT with Essential Mathematics (Van Verth, et al).
//      4. That means that get_YRP() actually returns Tait-Bryan angles in the sequence z-x'-y'' (roll, pitch, yaw)!
//
// [1]: Eberly, David (1999). Euler Angle Formulas. Section 2.5, formula 13.
//      Retrieved from http://www.geometrictools.com/Documentation/EulerAngles.pdf
//
// - Zach Brockway, 2016-04-11
//
// Original comments follow.


// Core::Matrix uses CLASSIC coordinates. It matches the math shown in the following
// textbooks:
// Computer Graphics: Foley et al (seminal)
// Essential Mathematics for Games and Interactive Applications: Van Verth et al
//
// That means the distance from the viewer to the view is the X coordinate
// up and down is Z
// left and right is Y
// This module is backwards X & Y relative to these coordinates.
// Thus when we create matricies, we need to swap X and Y
// It is better to do the swap than to rewrite the math for the weird
// coordinate system.
// we also need to invert roll after using this.
// R_z = yaw
// R_y = pitch
// R_x = roll

#ifndef __CAMERALIBRARY__MATH__
#define __CAMERALIBRARY__MATH__

#ifdef __PLATFORM__LINUX__
#   include <stdlib.h>           //== for abs
#endif

#include "cameracommonglobals.h"


typedef double Real;


namespace Core
{
class cSerializer;
class Matrix;


double CLAPI DistanceBetweenAngles ( double Angle1, double Angle2 );
double CLAPI AngleRollover ( double LastAngle, double NewAngle );


class CLAPI Angle
{
public:
    Angle() {
        mAngle = 0;
    }
    ~Angle() { }

    void operator/= ( double Value );
    void operator*= ( double Value );
    void operator-= ( double Value );
    void operator+= ( double Value );
    Angle &operator= ( double a );
    operator double();

    void FromXY ( double X, double Y );
private:
    void   BoundsCheck();
    double mAngle;
};


class CLAPI Vector
{
public:
    Vector()
        : x( 0.0 )
        , y( 0.0 )
        , z( 0.0 )
    {
    }

    Vector( double X, double Y, double Z )
        : x( X )
        , y( Y )
        , z( Z )
    {
    }

    double X();
    double Y();
    double Z();

    void Set( double X, double Y, double Z )
    {
        x = X;
        y = Y;
        z = Z;
    }

    void operator/= ( double d );
    void operator*= ( double d );
    void operator-= ( const Vector vec );
    void operator+= ( const Vector vec );

    void operator*= ( const Matrix *mat );

    double Dot      ( const Vector vec );
    void   Cross    ( const Vector a, const Vector b );
    double Length   ();
    void   Normalize();
    double Distance ( Vector &vec );
    double SquareDistance ( Vector &vec );
    double x, y, z;
};

CLAPI bool  LineLineIntersect ( Vector &p1,Vector &p2,Vector &p3,Vector &p4,Vector &pa,Vector &pb );

CLAPI bool   IntersectionPointLine ( Vector &Point, Vector &LineStart, Vector &LineEnd, Vector &Intersection );
CLAPI bool   DistancePointLine ( Vector &Point, Vector &LineStart, Vector &LineEnd, float &Distance );
CLAPI float  DistancePointLine ( Vector *Point, Vector *LineStart, Vector *LineEnd );
CLAPI double ClosestDistancePointLine ( Vector &Point, Vector &LineStart, Vector &LineEnd );

CLAPI Vector IntersectPointLine ( Vector &Point, Vector &LineStart, Vector &LineEnd );

inline float fastsqrt ( float val )  {
    union
    {
        int tmp;
        float val;
    } u;
    u.val = val;
    u.tmp -= 1<<23; /* Remove last bit so 1.0 gives 1.0 */
    /* tmp is now an approximation to logbase2(val) */
    u.tmp >>= 1; /* divide by 2 */
    u.tmp += 1<<29; /* add 64 to exponent: (e+127)/2 =(e/2)+63, */
    /* that represents (e/2)-64 but we want e/2 */
    return u.val;
}

struct VectorPtr
{
    int Count;
    double *p;
    int increment;
    void operator++();
    operator double();
};

class CLAPI Matrix
{
public:
    Matrix ( int _R, int _C,  double *p );
    Matrix ( Vector vec );
    Matrix();

    int Rows;
    int Columns;
    int Count;
    double Data[4*4];

    void Row ( VectorPtr &v, int r );
    void Column ( VectorPtr &v, int c );
    double Norm();
    void Normalize();
    Matrix &operator/= ( double d );
    Matrix &operator*= ( double d );
    Matrix &operator-= ( const Matrix &rhs );
    Matrix &operator+= ( const Matrix &rhs );
    static double Dot ( VectorPtr &lhs, VectorPtr &rhs );
    void Transpose();
    const Matrix &Multiply ( Matrix &lhs, Matrix &rhs );
    void Cross ( Matrix &lhs, Matrix &rhs );
    void set_Column ( Matrix &rhs, int c );
    int offset ( int r, int c );
    void get_YPR ( double &yaw,double &pitch,double &roll );
    void get_YRP ( double &yaw,double &pitch,double &roll );
    void get_RPY ( double &yaw,double &pitch,double &roll );
    void CalculateOrthogonalMatrix ( const Vector v1, const Vector v2, const Vector v3 );
    void CalculateOrthogonalMatrix ( Matrix &p0, Matrix &p1, Matrix &p2 );
    void CombineOrthogonalMatricies ( Matrix &p0, Matrix &p1, Matrix &p2, Matrix &q0, Matrix &q1, Matrix &q2 );
    void CreateFromYPR ( double yaw, double pitch, double roll );
    void CreateFromYRP ( double yaw, double pitch, double roll );
    void Identity();
    void GetQuat ( float &qx, float &qy, float &qz, float &qw );
};

class CLAPI Quaternion
{
public:
    Quaternion( double X, double Y, double Z, double W );
    Quaternion();

    double x,y,z,w;

    static Quaternion FromOrienationMatrix ( float *m );
    void ToOrientationMatrix ( float *m );
    static Quaternion FromOrienationMatrix ( double *m );
    void ToOrientationMatrix ( double *m );

    Quaternion operator * ( const Quaternion q2 ) const;

    void Normalize();
    void Inverse();

    static Core::Quaternion Slerp ( Core::Quaternion qa, Core::Quaternion qb, double t );
};

struct CLAPI DistortionModel
{
    DistortionModel();

    bool   Distort;                     //== Enable/Disable Distortion Model
    double LensCenterX;                 //== Primary Point X (in pixels)
    double LensCenterY;                 //== Primary Point Y (in pixels)
    double HorizontalFocalLength;       //== Horizontal Focal Length (in pixels)
    double VerticalFocalLength;         //== Vertical Focal Length {in pixels)
    double KC1;                         //== Distortion Parameter 1
    double KC2;                         //== Distortion Parameter 2
    double KC3;                         //== Distortion Parameter 3
    double Tangential0;                 //== Tangential 0
    double Tangential1;                 //== Tangential 1
};

CLAPI void Distort2DPoint   ( DistortionModel &Model, float &X, float &Y );
CLAPI void Undistort2DPoint ( DistortionModel &Model, float &X, float &Y );

template <typename T>
void Swap ( T &A, T &B )
{
    T temp = A;
    A = B;
    B = temp;
}

template <typename T>
void LowToHigh ( T &A, T &B )
{
    if ( A>B )
    {
        T temp = A;
        A = B;
        B = temp;
    }
}



} // namespace Core

#endif // #ifndef __CAMERALIBRARY__MATH__

