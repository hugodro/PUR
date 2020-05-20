/**************************************************
* File: lowSFNode.cc.
* Desc: Implementation of the vrml.basic.SFNode support class, in C++.
* Module: AkraLog : Animation.
* Rev: 10 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vm/jmachine.h>
#include <vm/classFile.h>
#include <vm/jvInstance.h>
#include <native/nError.h>
#include "lowSFNode.h"


/**************************************************
* Implementation: vrml_basic_SFNode.
**************************************************/

JVVariable *vrml_basic_SFNode::llowField;
bool vrml_basic_SFNode::sfNodeMustFetchDict= true;

bool vrml_basic_SFNode::sfNodeFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (sfNodeMustFetchDict) {
	llowField= aMachine->getFieldOfClass(aClass, "linkLow");
	sfNodeMustFetchDict= false;
    }

    return (llowField != NULL);
}


vrml_basic_SFNode::vrml_basic_SFNode(JVInstance *anInstance, JVMachine *aMachine)
{
    javaEquiv= anInstance;
    if (sfNodeMustFetchDict) {
	if (!sfNodeFetchDictionary(aMachine, javaEquiv->category())) {
#if defined(USE_THROW)
	    throw new NativeError(1);
#endif
	}
    }
}


vrml_basic_SFNode::~vrml_basic_SFNode(void)
{
    // Anything to do ?
}


JVInstance *vrml_basic_SFNode::get_this(void)
{
    return javaEquiv;
}


int vrml_basic_SFNode::get_linkLow(void)
{
    return (int)javaEquiv->getField(llowField);
}


/**************************************************
* Implementation: vrml_nodes_Coordinate.
**************************************************/

JVVariable *vrml_nodes_Coordinate::pointsField;
bool vrml_nodes_Coordinate::cooNodeMustFetchDict= true;

bool vrml_nodes_Coordinate::cooNodeFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (cooNodeMustFetchDict) {
	pointsField= aMachine->getFieldOfClass(aClass, "point");
	cooNodeMustFetchDict= false;
    }

    return (pointsField != NULL);
}


vrml_nodes_Coordinate::vrml_nodes_Coordinate(JVInstance *anInstance, JVMachine *aMachine)
    : vrml_basic_SFNode(anInstance, aMachine)
{
    if (cooNodeMustFetchDict) {
	if (!cooNodeFetchDictionary(aMachine, javaEquiv->category())) {
#if defined(USE_THROW)
	    throw new NativeError(1);
#endif
	}
    }
}


vrml_nodes_Coordinate::~vrml_nodes_Coordinate(void)
{
    // Anything to do ?
}


JVArrayArray *vrml_nodes_Coordinate::get_points(void)
{
    return (JVArrayArray *)javaEquiv->getField(pointsField);
}


