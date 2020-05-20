/**************************************************
* File: povExtensions.cc.
* Desc: Implementation of the extended VRML geometric classes, for PovObjects follow-up.
* Module: Plzen : VrmlSup.
* Rev: 28 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "pObject.h"
#include "povExtensions.h"


/**************************************************
* Implementation: ExtBoxVrml.
**************************************************/

ExtBoxVrml::ExtBoxVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : BoxVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtBoxVrml::getPovObject(void)
{
    return dual;
}


void ExtBoxVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtConeVrml.
**************************************************/

ExtConeVrml::ExtConeVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : ConeVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtConeVrml::getPovObject(void)
{
    return dual;
}


void ExtConeVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtCylinderVrml.
**************************************************/

ExtCylinderVrml::ExtCylinderVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : CylinderVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtCylinderVrml::getPovObject(void)
{
    return dual;
}


void ExtCylinderVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtElevationGridVrml.
**************************************************/

ExtElevationGridVrml::ExtElevationGridVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : ElevationGridVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtElevationGridVrml::getPovObject(void)
{
    return dual;
}


void ExtElevationGridVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtExtrusionVrml.
**************************************************/

ExtExtrusionVrml::ExtExtrusionVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : ExtrusionVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtExtrusionVrml::getPovObject(void)
{
    return dual;
}


void ExtExtrusionVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtIndexedFaceSetVrml.
**************************************************/

ExtIndexedFaceSetVrml::ExtIndexedFaceSetVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : IndexedFaceSetVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtIndexedFaceSetVrml::getPovObject(void)
{
    return dual;
}


void ExtIndexedFaceSetVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtIndexedLineSetVrml.
**************************************************/

ExtIndexedLineSetVrml::ExtIndexedLineSetVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : IndexedLineSetVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtIndexedLineSetVrml::getPovObject(void)
{
    return dual;
}


void ExtIndexedLineSetVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtPointSetVrml.
**************************************************/

ExtPointSetVrml::ExtPointSetVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : PointSetVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtPointSetVrml::getPovObject(void)
{
    return dual;
}


void ExtPointSetVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtSphereVrml.
**************************************************/

ExtSphereVrml::ExtSphereVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : SphereVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtSphereVrml::getPovObject(void)
{
    return dual;
}


void ExtSphereVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtTextVrml.
**************************************************/

ExtTextVrml::ExtTextVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : TextVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtTextVrml::getPovObject(void)
{
    return dual;
}


void ExtTextVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtDirectionalLightVrml.
**************************************************/

ExtDirectionalLightVrml::ExtDirectionalLightVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : DirectionalLightVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtDirectionalLightVrml::getPovObject(void)
{
    return dual;
}


void ExtDirectionalLightVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtPointLightVrml.
**************************************************/

ExtPointLightVrml::ExtPointLightVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : PointLightVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtPointLightVrml::getPovObject(void)
{
    return dual;
}


void ExtPointLightVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtSpotLightVrml.
**************************************************/

ExtSpotLightVrml::ExtSpotLightVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : SpotLightVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovObject *ExtSpotLightVrml::getPovObject(void)
{
    return dual;
}


void ExtSpotLightVrml::setPovEquiv(PovObject *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


/**************************************************
* Implementation: ExtAppearanceVrml.
**************************************************/

ExtAppearanceVrml::ExtAppearanceVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : AppearanceVrml(anEquiv, aMachine)
{
    dual= NULL;
}


PovTexture *ExtAppearanceVrml::getPovTexture(void)
{
    return dual;
}


void ExtAppearanceVrml::setPovTexture(PovTexture *pObject)
{
    if (dual != NULL) delete dual;
    dual= pObject;
}


