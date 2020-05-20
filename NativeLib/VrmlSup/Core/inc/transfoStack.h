#ifndef _STACKTRANSFO_H_
#define _STACKTRANSFO_H_
/**************************************************
* File: stackTransfo.h.
* Desc: Definition of the TransfoStack class.
* Module: AkraLog : VrmlSupport.
* Rev: 23 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>
#include "geometry3D.h"


class TransfoStack : public AkObject {
  protected:
    TransfoStack *previous;
    TrMatrix matrix;

  public:
    TransfoStack(void);
    TransfoStack(AkCoord3D *t, AkCoord4D *r, AkCoord3D *s, AkCoord3D *c, AkCoord4D *sr, TransfoStack *link);
    TrMatrix *getMatrix(void);
    TransfoStack *getPrevious(void);
};


#endif		/* _STACKTRANSFO_H_ */
