#ifndef _CAMERANT_LOW_H_
#define _CAMERANT_LOW_H_
/**************************************************
* File: cameraNT.h.
* Desc: Definition of the akralog.animation.CameraNT support class, in C++.
* Module: AkraLog : Animation.
* Rev: 9 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>


class JVMachine;
class ClassFile;
class JVInstance;
class JVVariable;

class akralog_animation_CameraNT : public AkObject {
  protected:	// Class variables.
    static JVVariable *llowField, *widthField, *heightField, *vpField;
    static boolean mustFetchDict;

  public:	// Class methods.
    static boolean fetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  protected:	// Instance variables.
    JVInstance *javaEquiv;

  public:	// Instance methods.
    akralog_animation_CameraNT(JVInstance *anInstance, JVMachine *aMachine);
    ~akralog_animation_CameraNT(void);

	// Field accessors.
    JVInstance *get_this(void);
    int get_linkLow(void);
    int get_width(void);
    int get_height(void);
    JVInstance *get_currentVP(void);

};


#endif	/* _CAMERANT_LOW_H_ */
