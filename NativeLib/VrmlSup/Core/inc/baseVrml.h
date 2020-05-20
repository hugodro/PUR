#ifndef _BASICVRML_H_
#define _BASICVRML_H_
/**************************************************
* File: baseVrml.h.
* Desc: Definition of the base class for VRML objects.
* Module: AkraLog : VrmlSupport.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>
#include "geometry3D.h"


class RendererVrml;
class JVInstance;
class JVMachine;
class ClassFile;
class JVVariable;


class NodeVrml : public AkObject {
  protected:	// Class variables.
    static JVVariable *llowField;
    static bool nodeMustFetchDict;

  protected:	// Class methods.
    static bool nodeFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  protected:	// Instance variables.
    JVInstance *javaEquiv;

  public:		// Instance methods.
    NodeVrml(void);
    NodeVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual ~NodeVrml(void);
    JVInstance *getJavaEquiv(void);
    void setJavaEquiv(JVInstance *anEquiv);
    virtual void renderOn(RendererVrml *aContext);

    virtual void destroy(void);

	// Field accessors.
    int get_linkLow(void);
};


#endif		/* _BASICVRML_H_ */
