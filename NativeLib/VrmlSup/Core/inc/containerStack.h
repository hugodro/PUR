#ifndef _CONTAINERSTACK_H_
#define _CONTAINERSTACK_H_
/**************************************************
* File: containerStack.h.
* Desc: Definition of the ContainerStack and associated classes.
* Module: AkraLog : VrmlSupport.
* Rev: 10 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>


class ContainerVrml;


class StackBlock : public AkObject {
  friend class ContainerStack;
  protected:
    StackBlock *next;
    unsigned int nbrElements;
    ContainerVrml *elements[126];

  public:
    StackBlock(void);
    StackBlock(StackBlock *pastBlock);
    bool isUsed(void);
};


class ContainerStack : public AkObject {
  protected:
    StackBlock *topElements;

  public:	// Instance methods.
    ContainerStack(void);
    bool isUsed(void);
    ContainerVrml *topNode(void);
    void push(ContainerVrml *aContainer);
    ContainerVrml *pop(void);
};


#endif		/* _CONTAINERSTACK_H_ */
