#ifndef _SFNODE_LOW_H_
#define _SFNODE_LOW_H_
/**************************************************
* File: lowSFNode.h.
* Desc: Definition of the vrml.basic.SFNode support class, in C++.
* Module: AkraLog : Animation.
* Rev: 10 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>


class JVVariable;
class JVMachine;
class ClassFile;
class JVInstance;
class JVArrayArray;


class vrml_basic_SFNode : public AkObject {
  protected:	// Class variables.
    static JVVariable *llowField;
    static bool sfNodeMustFetchDict;

  public:	// Class methods.
    static bool sfNodeFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  protected:	// Instance variables.
    JVInstance *javaEquiv;

  public:	// Instance methods.
    vrml_basic_SFNode(JVInstance *anInstance, JVMachine *aMachine);
    virtual ~vrml_basic_SFNode(void);

	// Field accessors.
    JVInstance *get_this(void);
    int get_linkLow(void);
};


class vrml_nodes_Coordinate : public vrml_basic_SFNode {
  protected:	// Class variables.
    static JVVariable *pointsField;
    static bool cooNodeMustFetchDict;

  public:	// Class methods.
    static bool cooNodeFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:	// Instance methods.
    vrml_nodes_Coordinate(JVInstance *anInstance, JVMachine *aMachine);
    virtual ~vrml_nodes_Coordinate(void);

	// Field accessors.
    JVArrayArray *get_points(void);

};

#endif	/* _SFNODE_LOW_H_ */

