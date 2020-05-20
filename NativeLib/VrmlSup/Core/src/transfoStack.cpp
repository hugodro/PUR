/**************************************************
* File: stackTransfo.cc.
* Desc: Implementation of the TransfoStack class.
* Module: AkraLog : VrmlSupport.
* Rev: 23 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "transfoStack.h"


/**************************************************
* Implementation: TransfoStack.
**************************************************/

TransfoStack::TransfoStack(void)
{
    previous= NULL;
}


TransfoStack::TransfoStack(AkCoord3D *t, AkCoord4D *r, AkCoord3D *s, AkCoord3D *c, AkCoord4D *sr, TransfoStack *link)
{
    if (link != NULL) {
	matrix.translationOf(&link->matrix, t);
    }
    else {
	matrix.translateBy(t);
    }

    matrix.translateBy(c);
    matrix.rotateBy(r);
    matrix.rotateBy(sr);
    matrix.scaleBy(s);
    sr->points[3]= -sr->points[3];
    matrix.rotateBy(sr);
    c->points[0]= -c->points[0]; c->points[1]= -c->points[1]; c->points[2]= -c->points[2];
    matrix.translateBy(c);
    previous= link;
}


TrMatrix *TransfoStack::getMatrix(void)
{
    return &matrix;
}


TransfoStack *TransfoStack::getPrevious(void)
{
    return previous;
}
