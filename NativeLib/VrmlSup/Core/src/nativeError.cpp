/**************************************************
* File: nativeError.h.
* Desc: Implementation of the native error exception management.
* Module: AkraLog : Animation.
* Rev: 9 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <native/nError.h>


NativeError::NativeError(int aCode)
{
    errorCode= aCode;
}

// TMPTMP: To solve the linking problem with g++ 3.
extern "C"
{

#if !defined(HACK_RTTL)
void __rethrow(void)
{
}

void __pure_virtual(void)
{
}
#endif

};

