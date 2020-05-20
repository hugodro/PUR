/**************************************************
* File: baseVrml.cc.
* Desc: Implementation of the base class for VRML objects.
* Module: AkraLog : VrmlSupport.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vm/jmachine.h>
#include <vm/classFile.h>
#include <vm/jvInstance.h>
#include <vm/stacks.h>
#include <native/nError.h>
#include "baseVrml.h"


/**************************************************
* Implementation: NodeVrml.
**************************************************/

JVVariable *NodeVrml::llowField;
bool NodeVrml::nodeMustFetchDict= true;

bool NodeVrml::nodeFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (nodeMustFetchDict) {
	llowField= aMachine->getFieldOfClass(aClass, "linkLow");
	nodeMustFetchDict= false;
    }

    if (llowField == NULL) {
	return false;
    }
    return true;
}


NodeVrml::NodeVrml(void)
{
    javaEquiv= NULL;
}


NodeVrml::NodeVrml(JVInstance *anEquiv, JVMachine *aMachine)
{
    if (nodeMustFetchDict) {
	if (!nodeFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	    throw new NativeError(1);
#endif
	}
    }
    javaEquiv= anEquiv;
    JVRunValue tmpValue(TYPE_BASIC, this);
    javaEquiv->putField(tmpValue, llowField);
}


NodeVrml::~NodeVrml(void)
{
    if (javaEquiv != NULL) {
	// TODO: notice the equiv of destruction (maybe ?).
	JVRunValue tmpValue(TYPE_BASIC, this);
	javaEquiv->putField(tmpValue, llowField);
    }
}


JVInstance *NodeVrml::getJavaEquiv(void)
{
    return javaEquiv;
}


void NodeVrml::setJavaEquiv(JVInstance *anEquiv)
{
    if (nodeMustFetchDict) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }
    javaEquiv= anEquiv;
    JVRunValue tmpValue(TYPE_BASIC, this);
    javaEquiv->putField(tmpValue, llowField);
}


void NodeVrml::renderOn(RendererVrml *aContext)
{
    return;		// Warning: by default, nothing takes place.
}

void NodeVrml::destroy(void)
{
}


int NodeVrml::get_linkLow(void)
{
    return (int)javaEquiv->getField(llowField);
}

