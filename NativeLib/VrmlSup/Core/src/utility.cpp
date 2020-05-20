/**************************************************
* File: utility.cc.
* Desc: Native lib wrapper for Window NT.
* Module: AkraLog : VrmlSupport.
* Rev: 28 juin 1997 : REV 0 : Hugo DesRosiers : Creation.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers.
*	Note: Adaptation for the animation demo in VRML.
**************************************************/

#include "utility.h"


/**************************************************
* Implementation: AkRectangle.
**************************************************/

AkRectangle::AkRectangle()
{
    origin= new AkPoint(0.0f, 0.0f);
    size= new AkSize(0.0f, 0.0f);
}


AkRectangle::AkRectangle(AkRectangle *aRect)
{
    origin= new AkPoint(aRect->origin->x, aRect->origin->y);
    size= new AkSize(aRect->size->width, aRect->size->height);
}


AkRectangle::AkRectangle(AkPoint *aPoint, AkSize *aSize)
{
    origin= aPoint;
    size= aSize;
}

AkRectangle::AkRectangle(float x, float y, float width, float height)
{
    origin= new AkPoint(x, y);
    size= new AkSize(width, height);
}


float AkRectangle::getXPos(void)
{
    return origin->x;
}


float AkRectangle::getYPos(void)
{
    return origin->y;
}


float AkRectangle::getWidth(void)
{
    return size->width;
}

float AkRectangle::getHeight(void)
{
    return size->height;
}


void AkRectangle::setXPos(float aValue)
{
    origin->x= aValue;
}


void AkRectangle::setYPos(float aValue)
{
    origin->y= aValue;
}


void AkRectangle::setWidth(float aValue)
{
    size->width= aValue;
}


void AkRectangle::setHeight(float aValue)
{
    size->height= aValue;
}


/**************************************************
* Implementation: AkPoint.
**************************************************/

AkPoint::AkPoint(float aX, float aY)
{
    x= aX;
    y= aY;
}


/**************************************************
* Implementation: AkSize.
**************************************************/

AkSize::AkSize(float aWidth, float aHeight)
{
    width= aWidth;
    height= aHeight;
}
