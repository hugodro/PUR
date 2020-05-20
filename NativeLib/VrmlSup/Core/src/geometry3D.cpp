/**************************************************
* File: geometry3D.cc.
* Desc: Implementation of the classes for 3D geometric representations and computations.
* Module: AkraLog : VrmlSupport.
* Rev: 11 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <math.h>
#include "geometry3D.h"


/**************************************************
* Implementation: AkCoord3D.
**************************************************/

AkCoord3D::AkCoord3D(void)
{
    points[0]= points[1]= points[2]= 0.0f;
}


AkCoord3D::AkCoord3D(float x, float y, float z)
{
    points[0]= x; points[1]= y; points[2]= z;
}


void AkCoord3D::setXYZ(float x, float y, float z)
{
    points[0]= x; points[1]= y; points[2]= z;
}


/**************************************************
* Implementation: AkCoord4D.
**************************************************/

AkCoord4D::AkCoord4D(void)
{
    points[0]= points[1]= points[2]= points[3]= 0.0f;
}


AkCoord4D::AkCoord4D(float x, float y, float z, float r)
{
    points[0]= x; points[1]= y; points[2]= z; points[3]= r;
}


void AkCoord4D::setXYZR(float x, float y, float z, float r)
{
    points[0]= x; points[1]= y; points[2]= z; points[3]= r;
}


/**************************************************
* Implementation: AkColor.
**************************************************/

AkColor::AkColor(void)
{
    indexes[0]= indexes[1]= indexes[2]= 0.0f;
}


AkColor::AkColor(float r, float g, float b)
{
    indexes[0]= r; indexes[1]= g; indexes[2]= b;
}


void AkColor::setRGB(float r, float g, float b)
{
    indexes[0]= r; indexes[1]= g; indexes[2]= b;
}


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


void Vector3::setTo(double a, double b, double c)
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


void Vector4::setTo(double a, double b, double c, double d)
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


// Low-res vectors.
void TrMatrix::translateBy(AkCoord3D *aXlate)
{
    double tmpResult[4];

    tmpResult[0]= M[0][0] * aXlate->points[0] + M[0][1] * aXlate->points[1] + M[0][2] * aXlate->points[2] + M[0][3];
    tmpResult[1]= M[1][0] * aXlate->points[0] + M[1][1] * aXlate->points[1] + M[1][2] * aXlate->points[2] + M[1][3];
    tmpResult[2]= M[2][0] * aXlate->points[0] + M[2][1] * aXlate->points[1] + M[2][2] * aXlate->points[2] + M[2][3];
    tmpResult[3]= M[3][0] * aXlate->points[0] + M[3][1] * aXlate->points[1] + M[3][2] * aXlate->points[2] + M[3][3];

    M[0][3]= tmpResult[0];
    M[1][3]= tmpResult[1];
    M[2][3]= tmpResult[2];
    M[3][3]= tmpResult[3];
}


void TrMatrix::rotateBy(AkCoord4D *aRotate)
{
    double S[3][3], R[3][3], sinTheta, cosTheta, tmpResult[4][3];

    S[0][0]= aRotate->points[0] * aRotate->points[0];
    S[0][1]= S[1][0]= aRotate->points[0] * aRotate->points[1];
    S[1][1]= aRotate->points[1] * aRotate->points[1];
    S[0][2]= S[2][0]= aRotate->points[0] * aRotate->points[2];
    S[1][2]= S[2][1]= aRotate->points[1] * aRotate->points[2];
    S[2][2]= aRotate->points[2] * aRotate->points[2];
    cosTheta= cos(aRotate->points[3]);
    sinTheta= sin(aRotate->points[3]);
    R[0][0]= S[0][0] + cosTheta * (1 - S[0][0]);
    R[0][1]= S[0][1] - cosTheta * S[0][1] - sinTheta * aRotate->points[2];
    R[0][2]= S[0][2] - cosTheta * S[0][2] + sinTheta * aRotate->points[1];
    R[1][0]= S[1][0] - cosTheta * S[1][0] + sinTheta * aRotate->points[2];
    R[1][1]= S[1][1] + cosTheta * (1 - S[1][1]);
    R[1][2]= S[1][2] - cosTheta * S[1][2] - sinTheta * aRotate->points[0];
    R[2][0]= S[2][0] - cosTheta * S[2][0] - sinTheta * aRotate->points[1];
    R[2][1]= S[2][1] - cosTheta * S[2][1] + sinTheta * aRotate->points[0];
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


void TrMatrix::scaleBy(AkCoord3D *aScaling)
{
    double tmpResult[4][3];

    M[0][0]*= aScaling->points[0];
    M[0][1]*= aScaling->points[1];
    M[0][2]*= aScaling->points[2];

    M[1][0]*= aScaling->points[0];
    M[1][1]*= aScaling->points[1];
    M[1][2]*= aScaling->points[2];

    M[2][0]*= aScaling->points[0];
    M[2][1]*= aScaling->points[1];
    M[2][2]*= aScaling->points[2];

    M[3][0]*= aScaling->points[0];
    M[3][1]*= aScaling->points[1];
    M[3][2]*= aScaling->points[2];
}


void TrMatrix::translationOf(TrMatrix *aMatrix, AkCoord3D *aXlate)
{
    M[0][3]= aMatrix->M[0][0] * aXlate->points[0] + aMatrix->M[0][1] * aXlate->points[1] + aMatrix->M[0][2] * aXlate->points[2] + aMatrix->M[0][3];
    M[1][3]= aMatrix->M[1][0] * aXlate->points[0] + aMatrix->M[1][1] * aXlate->points[1] + aMatrix->M[1][2] * aXlate->points[2] + aMatrix->M[1][3];
    M[2][3]= aMatrix->M[2][0] * aXlate->points[0] + aMatrix->M[2][1] * aXlate->points[1] + aMatrix->M[2][2] * aXlate->points[2] + aMatrix->M[2][3];
    M[3][3]= aMatrix->M[3][0] * aXlate->points[0] + aMatrix->M[3][1] * aXlate->points[1] + aMatrix->M[3][2] * aXlate->points[2] + aMatrix->M[3][3];
    M[0][0]= aMatrix->M[0][0]; M[0][1]= aMatrix->M[0][1]; M[0][2]= aMatrix->M[0][2];
    M[1][0]= aMatrix->M[1][0]; M[1][1]= aMatrix->M[1][1]; M[1][2]= aMatrix->M[1][2];
    M[2][0]= aMatrix->M[2][0]; M[2][1]= aMatrix->M[2][1]; M[2][2]= aMatrix->M[2][2];
    M[3][0]= aMatrix->M[3][0]; M[3][1]= aMatrix->M[3][1]; M[3][2]= aMatrix->M[3][2];
}


void TrMatrix::transformPoint(AkCoord3D *aPoint)
{
    double tmpResult[3];

    tmpResult[0]= M[0][0] * aPoint->points[0] + M[0][1] * aPoint->points[1] + M[0][2] * aPoint->points[2] + M[0][3];
    tmpResult[1]= M[1][0] * aPoint->points[0] + M[1][1] * aPoint->points[1] + M[1][2] * aPoint->points[2] + M[1][3];
    tmpResult[2]= M[2][0] * aPoint->points[0] + M[2][1] * aPoint->points[1] + M[2][2] * aPoint->points[2] + M[2][3];

    aPoint->points[0]= (float)tmpResult[0]; aPoint->points[1]= (float)tmpResult[1]; aPoint->points[2]= (float)tmpResult[2];
}



