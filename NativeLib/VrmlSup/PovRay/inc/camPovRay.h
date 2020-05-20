#ifndef _CAMPOVRAY_LOW_H_
#define _CAMPOVRAY_LOW_H_
/**************************************************
* File: camPovRay.h.
* Desc: Definition of the akralog.animation.PovCam support class, in C++.
* Module: AkraLog : Animation.
* Rev: 22 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>

class JVMachine;
class ClassFile;
class JVInstance;
class JVVariable;

class akralog_animation_PovCam : public AkObject {
  protected:	// Class variables.
    static JVVariable *llowField, *formatField, *prefixField, *resoField, *vpField;
    static bool mustFetchDict;

  public:	// Class methods.
    static bool fetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  protected:	// Instance variables.
    JVInstance *javaEquiv;

  public:	// Instance methods.
    akralog_animation_PovCam(JVInstance *anInstance, JVMachine *aMachine);
    ~akralog_animation_PovCam(void);

	// Field accessors.
    JVInstance *get_this(void);
    int get_linkLow(void);
    char *get_formatOutput(void);
    char *get_prefixOutput(void);
    int get_resolution(unsigned int index);
    int get_resolutionLength(void);
    JVInstance *get_currentVP(void);
};
#endif		/* _CAMPOVRAY_LOW_H_ */
