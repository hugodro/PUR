/**************************************************
* File: maths.cc.
* Desc: Implementation of the matricial mathematic functions.
* Module: AkraLog : PovRayExt.
* Rev: 18 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#if defined(__linux__)
#include <math.h>
#endif

#if defined(_WIN32)
#include <math.h>
#endif
#include "maths.h"

/**************************************************
* Implementation: Vector3.
**************************************************/

Vector3::Vector3(void)
{
    Vc[0]= Vc[1]= Vc[2]= 0.0;
}


Vector3::Vector3(double a, double b, double c)
{
    Vc[0]= a;
    Vc[1]= b;
    Vc[2]= c;
}


/**************************************************
* Implementation: Vector4.
**************************************************/

Vector4::Vector4(void)
{
    Vc[0]= Vc[1]= Vc[2]= Vc[3]= 0.0;
}


Vector4::Vector4(double a, double b, double c, double d)
{
    Vc[0]= a;
    Vc[1]= b;
    Vc[2]= c;
    Vc[3]= d;
}


/**************************************************
* Implementation: TrMatrix.
**************************************************/

TrMatrix::TrMatrix(void)
{
    M[0][0]= M[1][1]= M[2][2]= M[3][3]= 1.0;
    M[0][1]= M[0][2]= M[0][3]= 0.0;
    M[1][0]= M[1][2]= M[1][3]= 0.0;
    M[2][0]= M[2][1]= M[2][3]= 0.0;
    M[3][0]= M[3][1]= M[3][2]= 0.0;
}


void TrMatrix::multiply(TrMatrix *aMatrix)
{
    double tmpResult[4][4];

#define B			aMatrix->M

    tmpResult[0][0]= M[0][0] * B[0][0] + M[0][1] * B[1][0] + M[0][2] * B[2][0] + M[0][3] * B[3][0];
    tmpResult[0][1]= M[0][0] * B[0][1] + M[0][1] * B[1][1] + M[0][2] * B[2][1] + M[0][3] * B[3][1];
    tmpResult[0][2]= M[0][0] * B[0][2] + M[0][1] * B[1][2] + M[0][2] * B[2][2] + M[0][3] * B[3][2];
    tmpResult[0][3]= M[0][0] * B[0][3] + M[0][1] * B[1][3] + M[0][2] * B[2][3] + M[0][3] * B[3][3];

    tmpResult[1][0]= M[1][0] * B[0][0] + M[1][1] * B[1][0] + M[1][2] * B[2][0] + M[1][3] * B[3][0];
    tmpResult[1][1]= M[1][0] * B[0][1] + M[1][1] * B[1][1] + M[1][2] * B[2][1] + M[1][3] * B[3][1];
    tmpResult[1][2]= M[1][0] * B[0][2] + M[1][1] * B[1][2] + M[1][2] * B[2][2] + M[1][3] * B[3][2];
    tmpResult[1][3]= M[1][0] * B[0][3] + M[1][1] * B[1][3] + M[1][2] * B[2][3] + M[1][3] * B[3][3];

    tmpResult[2][0]= M[2][0] * B[0][0] + M[2][1] * B[1][0] + M[2][2] * B[2][0] + M[2][3] * B[3][0];
    tmpResult[2][1]= M[2][0] * B[0][1] + M[2][1] * B[1][1] + M[2][2] * B[2][1] + M[2][3] * B[3][1];
    tmpResult[2][2]= M[2][0] * B[0][2] + M[2][1] * B[1][2] + M[2][2] * B[2][2] + M[2][3] * B[3][2];
    tmpResult[2][3]= M[2][0] * B[0][3] + M[2][1] * B[1][3] + M[2][2] * B[2][3] + M[2][3] * B[3][3];

    tmpResult[3][0]= M[3][0] * B[0][0] + M[3][1] * B[1][0] + M[3][2] * B[2][0] + M[3][3] * B[3][0];
    tmpResult[3][1]= M[3][0] * B[0][1] + M[3][1] * B[1][1] + M[3][2] * B[2][1] + M[3][3] * B[3][1];
    tmpResult[3][2]= M[3][0] * B[0][2] + M[3][1] * B[1][2] + M[3][2] * B[2][2] + M[3][3] * B[3][2];
    tmpResult[3][3]= M[3][0] * B[0][3] + M[3][1] * B[1][3] + M[3][2] * B[2][3] + M[3][3] * B[3][3];

    M[0][0]= tmpResult[0][0]; M[0][1]= tmpResult[0][1]; M[0][2]= tmpResult[0][2]; M[0][3]= tmpResult[0][3];
    M[1][0]= tmpResult[1][0]; M[1][1]= tmpResult[1][1]; M[1][2]= tmpResult[1][2]; M[1][3]= tmpResult[1][3];
    M[2][0]= tmpResult[2][0]; M[2][1]= tmpResult[2][1]; M[2][2]= tmpResult[2][2]; M[2][3]= tmpResult[2][3];
    M[3][0]= tmpResult[3][0]; M[3][1]= tmpResult[3][1]; M[3][2]= tmpResult[3][2]; M[3][3]= tmpResult[3][3];
}


void TrMatrix::translateBy(Vector3 *aXlate)
{
    double tmpResult[4];

    tmpResult[0]= M[0][0] * aXlate->Vc[0] + M[0][1] * aXlate->Vc[1] + M[0][2] * aXlate->Vc[2] + M[0][3];
    tmpResult[1]= M[1][0] * aXlate->Vc[0] + M[1][1] * aXlate->Vc[1] + M[1][2] * aXlate->Vc[2] + M[1][3];
    tmpResult[2]= M[2][0] * aXlate->Vc[0] + M[2][1] * aXlate->Vc[1] + M[2][2] * aXlate->Vc[2] + M[2][3];
    tmpResult[3]= M[3][0] * aXlate->Vc[0] + M[3][1] * aXlate->Vc[1] + M[3][2] * aXlate->Vc[2] + M[3][3];

    M[0][3]= tmpResult[0];
    M[1][3]= tmpResult[1];
    M[2][3]= tmpResult[2];
    M[3][3]= tmpResult[3];
}


void TrMatrix::rotateBy(Vector4 *aRotate)
{
    double S[3][3], R[3][3], sinTheta, cosTheta, tmpResult[4][3];

    S[0][0]= aRotate->Vc[0] * aRotate->Vc[0];
    S[0][1]= S[1][0]= aRotate->Vc[0] * aRotate->Vc[1];
    S[1][1]= aRotate->Vc[1] * aRotate->Vc[1];
    S[0][2]= S[2][0]= aRotate->Vc[0] * aRotate->Vc[2];
    S[1][2]= S[2][1]= aRotate->Vc[1] * aRotate->Vc[2];
    S[2][2]= aRotate->Vc[2] * aRotate->Vc[2];
    cosTheta= cos(aRotate->Vc[3]);
    sinTheta= sin(aRotate->Vc[3]);
    R[0][0]= S[0][0] + cosTheta * (1 - S[0][0]);
    R[0][1]= S[0][1] - cosTheta * S[0][1] - sinTheta * aRotate->Vc[2];
    R[0][2]= S[0][2] - cosTheta * S[0][2] + sinTheta * aRotate->Vc[1];
    R[1][0]= S[1][0] - cosTheta * S[1][0] + sinTheta * aRotate->Vc[2];
    R[1][1]= S[1][1] + cosTheta * (1 - S[1][1]);
    R[1][2]= S[1][2] - cosTheta * S[1][2] - sinTheta * aRotate->Vc[0];
    R[2][0]= S[2][0] - cosTheta * S[2][0] - sinTheta * aRotate->Vc[1];
    R[2][1]= S[2][1] - cosTheta * S[2][1] + sinTheta * aRotate->Vc[0];
    R[2][2]= S[2][2] + cosTheta * (1 - S[2][2]);

    tmpResult[0][0]= M[0][0] * R[0][0] + M[0][1] * R[1][0] + M[0][2] * R[2][0];
    tmpResult[0][1]= M[0][0] * R[0][1] + M[0][1] * R[1][1] + M[0][2] * R[2][1];
    tmpResult[0][2]= M[0][0] * R[0][2] + M[0][1] * R[1][2] + M[0][2] * R[2][2];

    tmpResult[1][0]= M[1][0] * R[0][0] + M[1][1] * R[1][0] + M[1][2] * R[2][0];
    tmpResult[1][1]= M[1][0] * R[0][1] + M[1][1] * R[1][1] + M[1][2] * R[2][1];
    tmpResult[1][2]= M[1][0] * R[0][2] + M[1][1] * R[1][2] + M[1][2] * R[2][2];

    tmpResult[2][0]= M[2][0] * R[0][0] + M[2][1] * R[1][0] + M[2][2] * R[2][0];
    tmpResult[2][1]= M[2][0] * R[0][1] + M[2][1] * R[1][1] + M[2][2] * R[2][1];
    tmpResult[2][2]= M[2][0] * R[0][2] + M[2][1] * R[1][2] + M[2][2] * R[2][2];

    tmpResult[3][0]= M[3][0] * R[0][0] + M[3][1] * R[1][0] + M[3][2] * R[2][0];
    tmpResult[3][1]= M[3][0] * R[0][1]  + M[3][1] * R[1][1] + M[3][2] * R[2][1];
    tmpResult[3][2]= M[3][0] * R[0][2] + M[3][1] * R[1][2] + M[3][2] * R[2][2];

    M[0][0]= tmpResult[0][0]; M[0][1]= tmpResult[0][1]; M[0][2]= tmpResult[0][2];
    M[1][0]= tmpResult[1][0]; M[1][1]= tmpResult[1][1]; M[1][2]= tmpResult[1][2];
    M[2][0]= tmpResult[2][0]; M[2][1]= tmpResult[2][1]; M[2][2]= tmpResult[2][2];
    M[3][0]= tmpResult[3][0]; M[3][1]= tmpResult[3][1]; M[3][2]= tmpResult[3][2];
}


void TrMatrix::scaleBy(Vector3 *aScaling)
{
    double tmpResult[4][3];

    M[0][0]*= aScaling->Vc[0];
    M[0][1]*= aScaling->Vc[1];
    M[0][2]*= aScaling->Vc[2];

    M[1][0]*= aScaling->Vc[0];
    M[1][1]*= aScaling->Vc[1];
    M[1][2]*= aScaling->Vc[2];

    M[2][0]*= aScaling->Vc[0];
    M[2][1]*= aScaling->Vc[1];
    M[2][2]*= aScaling->Vc[2];

    M[3][0]*= aScaling->Vc[0];
    M[3][1]*= aScaling->Vc[1];
    M[3][2]*= aScaling->Vc[2];
}


void TrMatrix::translationOf(TrMatrix *aMatrix, Vector3 *aXlate)
{
    M[0][3]= aMatrix->M[0][0] * aXlate->Vc[0] + aMatrix->M[0][1] * aXlate->Vc[1] + aMatrix->M[0][2] * aXlate->Vc[2] + aMatrix->M[0][3];
    M[1][3]= aMatrix->M[1][0] * aXlate->Vc[0] + aMatrix->M[1][1] * aXlate->Vc[1] + aMatrix->M[1][2] * aXlate->Vc[2] + aMatrix->M[1][3];
    M[2][3]= aMatrix->M[2][0] * aXlate->Vc[0] + aMatrix->M[2][1] * aXlate->Vc[1] + aMatrix->M[2][2] * aXlate->Vc[2] + aMatrix->M[2][3];
    M[3][3]= aMatrix->M[3][0] * aXlate->Vc[0] + aMatrix->M[3][1] * aXlate->Vc[1] + aMatrix->M[3][2] * aXlate->Vc[2] + aMatrix->M[3][3];
    M[0][0]= aMatrix->M[0][0]; M[0][1]= aMatrix->M[0][1]; M[0][2]= aMatrix->M[0][2];
    M[1][0]= aMatrix->M[1][0]; M[1][1]= aMatrix->M[1][1]; M[1][2]= aMatrix->M[1][2];
    M[2][0]= aMatrix->M[2][0]; M[2][1]= aMatrix->M[2][1]; M[2][2]= aMatrix->M[2][2];
    M[3][0]= aMatrix->M[3][0]; M[3][1]= aMatrix->M[3][1]; M[3][2]= aMatrix->M[3][2];
}


void TrMatrix::transformPoint(Vector3 *aPoint)
{
    double tmpResult[3];

    tmpResult[0]= M[0][0] * aPoint->Vc[0] + M[0][1] * aPoint->Vc[1] + M[0][2] * aPoint->Vc[2] + M[0][3];
    tmpResult[1]= M[1][0] * aPoint->Vc[0] + M[1][1] * aPoint->Vc[1] + M[1][2] * aPoint->Vc[2] + M[1][3];
    tmpResult[2]= M[2][0] * aPoint->Vc[0] + M[2][1] * aPoint->Vc[1] + M[2][2] * aPoint->Vc[2] + M[2][3];

    aPoint->Vc[0]= tmpResult[0]; aPoint->Vc[1]= tmpResult[1]; aPoint->Vc[2]= tmpResult[2];
}


/**************************************************
* Implementation: Math.
**************************************************/


// v[1x3]= Rotation(v[1x4] , v[1x3]).
void Math::applyVectorRotation(double *result, double *rotation, double *source)
{
    double A[3][3], R[3][3], sinTheta, cosTheta, tmpResult[3];

    A[0][0]= rotation[0] * rotation[0];
    A[0][1]= A[1][0]= rotation[0] * rotation[1];
    A[1][1]= rotation[1] * rotation[1];
    A[0][2]= A[2][0]= rotation[0] * rotation[2];
    A[1][2]= A[2][1]= rotation[1] * rotation[2];
    A[2][2]= rotation[2] * rotation[2];
    cosTheta= cos(rotation[3]);
    sinTheta= sin(rotation[3]);
    R[0][0]= A[0][0] + cosTheta * (1 - A[0][0]);
    R[0][1]= A[0][1] - cosTheta * A[0][1] - sinTheta * rotation[2];
    R[0][2]= A[0][2] - cosTheta * A[0][2] + sinTheta * rotation[1];
    R[1][0]= A[1][0] - cosTheta * A[1][0] + sinTheta * rotation[2];
    R[1][1]= A[1][1] + cosTheta * (1 - A[1][1]);
    R[1][2]= A[1][2] - cosTheta * A[1][2] - sinTheta * rotation[0];
    R[2][0]= A[2][0] - cosTheta * A[2][0] - sinTheta * rotation[1];
    R[2][1]= A[2][1] - cosTheta * A[2][1] + sinTheta * rotation[0];
    R[2][2]= A[2][2] + cosTheta * (1 - A[2][2]);
    tmpResult[0]= R[0][0] * source[0] + R[0][1] * source[1] + R[0][2] * source[2];
    tmpResult[1]= R[1][0] * source[0] + R[1][1] * source[1] + R[1][2] * source[2];
    tmpResult[2]= R[2][0] * source[0] + R[2][1] * source[1] + R[2][2] * source[2];
    result[0]= tmpResult[0];
    result[1]= tmpResult[1];
    result[2]= tmpResult[2];
}


// m'[4x4]= Rotation(m[4x4], v[1x4]).
void Math::applyMatrixRotation(double (*result)[4], double (*source)[4], double *rotation)
{
    double A[3][3], R[3][3], sinTheta, cosTheta, tmpResult[4][4];

    A[0][0]= rotation[0] * rotation[0];
    A[0][1]= A[1][0]= rotation[0] * rotation[1];
    A[1][1]= rotation[1] * rotation[1];
    A[0][2]= A[2][0]= rotation[0] * rotation[2];
    A[1][2]= A[2][1]= rotation[1] * rotation[2];
    A[2][2]= rotation[2] * rotation[2];
    cosTheta= cos(rotation[3]);
    sinTheta= sin(rotation[3]);
    R[0][0]= A[0][0] + cosTheta * (1 - A[0][0]);
    R[0][1]= A[0][1] - cosTheta * A[0][1] - sinTheta * rotation[2];
    R[0][2]= A[0][2] - cosTheta * A[0][2] + sinTheta * rotation[1];
    R[1][0]= A[1][0] - cosTheta * A[1][0] + sinTheta * rotation[2];
    R[1][1]= A[1][1] + cosTheta * (1 - A[1][1]);
    R[1][2]= A[1][2] - cosTheta * A[1][2] - sinTheta * rotation[0];
    R[2][0]= A[2][0] - cosTheta * A[2][0] - sinTheta * rotation[1];
    R[2][1]= A[2][1] - cosTheta * A[2][1] + sinTheta * rotation[0];
    R[2][2]= A[2][2] + cosTheta * (1 - A[2][2]);

    tmpResult[0][0]= source[0][0] * R[0][0] + source[0][1] * R[1][0] + source[0][2] * R[2][0];
    tmpResult[0][1]= source[0][0] * R[0][1] + source[0][1] * R[1][1] + source[0][2] * R[2][1];
    tmpResult[0][2]= source[0][0] * R[0][2] + source[0][1] * R[1][2] + source[0][2] * R[2][2];
    tmpResult[0][3]= source[0][3];

    tmpResult[1][0]= source[1][0] * R[0][0] + source[1][1] * R[1][0] + source[1][2] * R[2][0];
    tmpResult[1][1]= source[1][0] * R[0][1] + source[1][1] * R[1][1] + source[1][2] * R[2][1];
    tmpResult[1][2]= source[1][0] * R[0][2] + source[1][1] * R[1][2] + source[1][2] * R[2][2];
    tmpResult[1][3]= source[1][3];

    tmpResult[2][0]= source[2][0] * R[0][0] + source[2][1] * R[1][0] + source[2][2] * R[2][0];
    tmpResult[2][1]= source[2][0] * R[0][1] + source[2][1] * R[1][1] + source[2][2] * R[2][1];
    tmpResult[2][2]= source[2][0] * R[0][2] + source[2][1] * R[1][2] + source[2][2] * R[2][2];
    tmpResult[2][3]= source[2][3];

    tmpResult[3][0]= source[3][0] * R[0][0] + source[3][1] * R[1][0] + source[3][2] * R[2][0];
    tmpResult[3][1]= source[3][0] * R[0][1]  + source[3][1] * R[1][1] + source[3][2] * R[2][1];
    tmpResult[3][2]= source[3][0] * R[0][2] + source[3][1] * R[1][2] + source[3][2] * R[2][2];
    tmpResult[3][3]= source[3][3];

    result[0][0]= tmpResult[0][0]; result[0][1]= tmpResult[0][1]; result[0][2]= tmpResult[0][2]; result[0][3]= tmpResult[0][3];
    result[1][0]= tmpResult[1][0]; result[1][1]= tmpResult[1][1]; result[1][2]= tmpResult[1][2]; result[1][3]= tmpResult[1][3];
    result[2][0]= tmpResult[2][0]; result[2][1]= tmpResult[2][1]; result[2][2]= tmpResult[2][2]; result[2][3]= tmpResult[2][3];
    result[3][0]= tmpResult[3][0]; result[3][1]= tmpResult[3][1]; result[3][2]= tmpResult[3][2]; result[3][3]= tmpResult[3][3];
}


void Math::applyMatrixTranslation(double result[4][4], double a[4][4], double *t)
{
    double tmpResult[4][4];

    tmpResult[0][3]= a[0][0] * t[0] + a[0][1] * t[1] + a[0][2] * t[2] + a[0][3];
    tmpResult[1][3]= a[1][0] * t[0] + a[1][1] * t[1] + a[1][2] * t[2] + a[1][3];
    tmpResult[2][3]= a[2][0] * t[0] + a[2][1] * t[1] + a[2][2] * t[2] + a[2][3];
    tmpResult[3][3]= a[3][0] * t[0] + a[3][1] * t[1] + a[3][2] * t[2] + a[3][3];

    result[0][0]= a[0][0]; result[0][1]= a[0][1]; result[0][2]= a[0][2];
    result[1][0]= a[1][0]; result[1][1]= a[1][1]; result[1][2]= a[1][2];
    result[2][0]= a[2][0]; result[2][1]= a[2][1]; result[2][2]= a[2][2];
    result[3][0]= a[3][0]; result[3][1]= a[3][1]; result[3][2]= a[3][2];
    result[0][3]= tmpResult[0][3];
    result[1][3]= tmpResult[1][3];
    result[2][3]= tmpResult[2][3];
    result[3][3]= tmpResult[3][3];
}


void Math::applyMatrixScaling(double (*result)[4], double (*a)[4], double *s)
{
    double tmpResult[4][4];

    tmpResult[0][0]= a[0][0] * s[0]; tmpResult[0][1]= a[0][1] * s[1]; tmpResult[0][2]= a[0][2] * s[2]; tmpResult[0][3]= a[0][3];

    tmpResult[1][0]= a[1][0] * s[0]; tmpResult[1][1]= a[1][1] * s[1]; tmpResult[1][2]= a[1][2] * s[2]; tmpResult[1][3]= a[1][3];

    tmpResult[2][0]= a[2][0] * s[0]; tmpResult[2][1]= a[2][1] * s[1]; tmpResult[2][2]= a[2][2] * s[2]; tmpResult[2][3]= a[2][3];

    tmpResult[3][0]= a[3][0] * s[0]; tmpResult[3][1]= a[3][1] * s[1]; tmpResult[3][2]= a[3][2] * s[2]; tmpResult[3][3]= a[3][3];


    result[0][0]= tmpResult[0][0]; result[0][1]= tmpResult[0][1]; result[0][2]= tmpResult[0][2]; result[0][3]= tmpResult[0][3];
    result[1][0]= tmpResult[1][0]; result[1][1]= tmpResult[1][1]; result[1][2]= tmpResult[1][2]; result[1][3]= tmpResult[1][3];
    result[2][0]= tmpResult[2][0]; result[2][1]= tmpResult[2][1]; result[2][2]= tmpResult[2][2]; result[2][3]= tmpResult[2][3];
    result[3][0]= tmpResult[3][0]; result[3][1]= tmpResult[3][1]; result[3][2]= tmpResult[3][2]; result[3][3]= tmpResult[3][3];
}


void Math::multiplyMatrix(double (*result)[4], double (*a)[4], double (*b)[4])
{
    double tmpResult[4][4];

    tmpResult[0][0]= a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0];
    tmpResult[0][1]= a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1];
    tmpResult[0][2]= a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2];
    tmpResult[0][3]= a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3];

    tmpResult[1][0]= a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0];
    tmpResult[1][1]= a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1];
    tmpResult[1][2]= a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2];
    tmpResult[1][3]= a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3];

    tmpResult[2][0]= a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0];
    tmpResult[2][1]= a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1];
    tmpResult[2][2]= a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2];
    tmpResult[2][3]= a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3];

    tmpResult[3][0]= a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0];
    tmpResult[3][1]= a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1];
    tmpResult[3][2]= a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2];
    tmpResult[3][3]= a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3];

    result[0][0]= tmpResult[0][0]; result[0][1]= tmpResult[0][1]; result[0][2]= tmpResult[0][2]; result[0][3]= tmpResult[0][3];
    result[1][0]= tmpResult[1][0]; result[1][1]= tmpResult[1][1]; result[1][2]= tmpResult[1][2]; result[1][3]= tmpResult[1][3];
    result[2][0]= tmpResult[2][0]; result[2][1]= tmpResult[2][1]; result[2][2]= tmpResult[2][2]; result[2][3]= tmpResult[2][3];
    result[3][0]= tmpResult[3][0]; result[3][1]= tmpResult[3][1]; result[3][2]= tmpResult[3][2]; result[3][3]= tmpResult[3][3];
}



