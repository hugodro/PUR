#ifndef _POVGLUEMATHS_H_
#define _POVGLUEMATHS_H_
/**************************************************
* File: maths.h.
* Desc: Definition of the matricial mathematic functions.
* Module: AkraLog : PovRayExt.
* Rev: 18 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>


class Vector3 : public AkObject {
  public:
    double Vc[3];

  public:
    Vector3(void);
    Vector3(double a, double b, double c);
};


class Vector4 : public AkObject {
  public:
    double Vc[4];

  public:
    Vector4(void);
    Vector4(double a, double b, double c, double d);
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
};


class Math : public AkObject {
  public:
    static void applyVectorRotation(double *result, double *rotation, double *source);
    static void applyMatrixRotation(double (*result)[4], double (*source)[4], double *rotation);
    static void applyMatrixTranslation(double result[4][4], double a[4][4], double *t);
    static void applyMatrixScaling(double (*result)[4], double (*a)[4], double *s);
    static void multiplyMatrix(double (*result)[4], double (*a)[4], double (*b)[4]);
};


#endif		/* _POVGLUEMATHS_H_ */
