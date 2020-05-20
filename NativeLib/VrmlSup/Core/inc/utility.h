#ifndef _UTILITYWINNT_H_
#define _UTILITYWINNT_H_
/**************************************************
* File: utility.h.
* Desc: Native lib wrapper for Window NT.
* Module: AkraLog : JavaKit.
* Rev: 28 juin 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>

class AkPoint;
class AkSize;

class AkRectangle : public AkObject {
    AkPoint *origin;
    AkSize *size;
  public:
    AkRectangle();
    AkRectangle(AkRectangle *aRect);
    AkRectangle(AkPoint *aPoint, AkSize *aSize);
    AkRectangle(float x, float y, float width, float height);
    float getXPos(void);
    float getYPos(void);
    float getWidth(void);
    float getHeight(void);
    void setXPos(float aValue);
    void setYPos(float aValue);
    void setWidth(float aValue);
    void setHeight(float aValue);
};

class AkPoint : public AkObject {
  friend class AkRectangle;
  protected:
    float x;
    float y;
  
  public:
    AkPoint(float aX, float aY);
};

class AkSize : public AkObject {
  friend class AkRectangle;
  protected:
    float width;
    float height;

  public:
    AkSize(float aWidth, float aHeight);
};

#endif		/* _UTILITYWINNT_H_ */
