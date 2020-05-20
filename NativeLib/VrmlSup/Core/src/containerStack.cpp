/**************************************************
* File: containerStack.cc.
* Desc: Implementation of the ContainerStack and associated classes.
* Module: AkraLog : VrmlSupport.
* Rev: 10 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "containerStack.h"
#ifndef NULL
#define NULL	(0L)
#endif


/**************************************************
* Implementation: StackBlock.
**************************************************/

StackBlock::StackBlock(void)
{
    next= NULL;
    nbrElements= 0;
}


StackBlock::StackBlock(StackBlock *pastBlock)
{
    next= pastBlock;
    nbrElements= 0;
}


bool StackBlock::isUsed(void)
{
    if (nbrElements == 0) {
	    // ATTN: Is this test really necessary, or it is ok just to send true ?
	if (next != NULL) return (next->nbrElements != 0);
	else return false;
    }
    else return true;
}


/**************************************************
* Implementation: ContainerStack.
**************************************************/

ContainerStack::ContainerStack(void)
{
    topElements= new StackBlock();
}


bool ContainerStack::isUsed(void)
{
    return topElements->isUsed();
}


ContainerVrml *ContainerStack::topNode(void)
{
    if (topElements->nbrElements != 0) {
	return topElements->elements[topElements->nbrElements-1];
    }
    else {
	if (topElements->next != NULL) {
	    return topElements->next->elements[topElements->next->nbrElements-1];
	}
    }

    return NULL;
}


void ContainerStack::push(ContainerVrml *aContainer)
{
    if (topElements->nbrElements == 126) {
	topElements= new StackBlock(topElements);
    }
    topElements->elements[topElements->nbrElements++]= aContainer;
}


ContainerVrml *ContainerStack::pop(void)
{
    if (topElements->nbrElements == 0) {
	StackBlock *tmpBlock= topElements->next;
	if (tmpBlock != NULL) {
	    delete topElements;
	    topElements= tmpBlock;
	}
	else return NULL;
    }

    topElements->nbrElements--;
    return topElements->elements[topElements->nbrElements];
}
