#ifndef _GEOMETRY3D_H_
#define _GEOMETRY3D_H_
/**************************************************
* File: geometry3D.h.
* Desc: Definition of the classes for 3D geometric representations and computations.
* Module: AkraLog : VrmlSupport.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <math.h>			// For sqrt.

class AkCoord3D {
  public:
    float points[3];

  public:	// Instance methods.
    AkCoord3D(void);
    AkCoord3D(float x, float y, float z);
    void setXYZ(float x, float y, float z);
    inline void add(AkCoord3D &a, AkCoord3D &b) {
	    points[0]= a.points[0] + b.points[0]; points[1]= a.points[1] + b.points[1]; points[2]= a.points[2] + b.points[2];
	};
    inline void sub(AkCoord3D &a, AkCoord3D &b) {
	    points[0]= a.points[0] - b.points[0]; points[1]= a.points[1] - b.points[1]; points[2]= a.points[2] - b.points[2];
	};
    inline float dot(AkCoord3D &a) {
	    return (points[0] * a.points[0])+ (points[1] *a.points[1]) + (points[2] * a.points[2]);
	};
    inline void cross(AkCoord3D &a, AkCoord3D &b) {
	    points[0]= a.points[1] * b.points[2] - a.points[2] * b.points[1];
	    points[1]= a.points[2] * b.points[0] - a.points[0] * b.points[2];
	    points[2]= a.points[0] * b.points[1] - a.points[1] * b.points[0];
	};
    inline float sumSquares(void) {
	    return points[0] * points[0] + points[1] * points[1] + points[2] * points[2];
	};
    inline void normalize(void) {
	    float length= (float)sqrt((double)(points[0] * points[0] + points[1] * points[1] + points[2] * points[2]));
	    if (length != 0.0) {
		length= 1.0 / length;
		points[0]*= length;
		points[1]*= length;
		points[2]*= length;
	    }
	};
};


class AkCoord4D {
  public:	// Instance variables.
    float points[4];

  public:	// Instance methods.
    AkCoord4D(void);
    AkCoord4D(float x, float y, float z, float ang);
    void setXYZR(float x, float y, float z, float r);
};


class AkColor {
  public:
    float indexes[3];

  public:
    AkColor(void);
    AkColor(float r, float g, float b);
    void setRGB(float r, float g, float b);
};


// ATTN: TMPTMP: Copy from the AkMath package.

class Vector3 : public AkObject {
  public:
    double Vc[3];

  public:
    Vector3(void);
    Vector3(double a, double b, double c);
    void setTo(double a, double b, double c);
};


class Vector4 : public AkObject {
  public:
    double Vc[4];

  public:
    Vector4(void);
    Vector4(double a, double b, double c, double d);
    void setTo(double a, double b, double c, double d);
};


class TrMatrix : public AkObject {
  public:
    double M[4][4];

  public:
    TrMatrix(void);
    void multiply(TrMatrix *aMatrix);
    void translateBy(Vector3 *aXlate);
    void rotateBy(Vector4 *aRotate);
    void scaleBy(Vector3 *aScaling);
    void translationOf(TrMatrix *aMatrix, Vector3 *aXlate);
    void transformPoint(Vector3 *aPoint);

	// Low-res values.
    void translateBy(AkCoord3D *aXlate);
    void rotateBy(AkCoord4D *aRotate);
    void scaleBy(AkCoord3D *aScaling);
    void translationOf(TrMatrix *aMatrix, AkCoord3D *aXlate);
    void transformPoint(AkCoord3D *aPoint);
};


#endif		/* _GEOMETRY3D_H_ */
