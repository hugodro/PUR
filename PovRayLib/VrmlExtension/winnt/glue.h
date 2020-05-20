#ifndef _POVRAYGLUE_H_
#define _POVRAYGLUE_H_
/**************************************************
* File: glue.h.
* Desc: Definition of the PovRayGlue class.
* Module: AkraLog : PovRayExt.
* Rev: 14 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>
#include <VrmlParser/v2OptimumDefs.h>

class PovObject;

class PovRayGlue : public AkObject {
  public:
    static void defineCamera(void *framePtr, void **params);
    static void frameObject(PovObject *anObject);
};

#endif		/* _POVRAYGLUE_H_ */
