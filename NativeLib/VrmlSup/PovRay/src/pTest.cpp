/**************************************************
* File: pTest.cc.
* Desc: Test of the povray as a dynamic library..
* Module: AkraLog : VrmlSup.
* Rev: 28 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#define DO_TEST2

#if defined(DO_TEST1)
void Test1(void);
#endif
#if defined(DO_TEST2)
void Test2(void);
#endif


int main(int argc, char **argv)
{
    Test2();
}


#if defined(DO_TEST1)

#include "pLink.h"

void Test1(void)
{
    LinkPovRay *povLink;

    povLink= LinkPovRay::createContext();
}
#endif		/* DO_TEST1 */

#if defined(DO_TEST2)

extern "C" {
#include <mach-o/rld.h>
#include <streams/streams.h>
#include <stdio.h>
}

void Test2(void)
{
    NXStream *outputStream;
    mach_header *header;
    int loaded, lookupResult;
    char *inputName[2];
    void (*symbolAddress)(void);

    outputStream= NXOpenFile(fileno(stdout), NX_WRITEONLY);
    inputName[0]= "testLib.lib";
    inputName[1]= NULL;
    loaded= rld_load(outputStream, &header, inputName, NULL) != 0;
    NXFlush(outputStream);

    lookupResult= rld_lookup(NULL, "_createContext__10LinkPovRay", (long unsigned int *)&symbolAddress);
    if (lookupResult == 1) {
	symbolAddress();
    }
}


#include <stdlib.h>

FakeFonction(void) {
    atoi(NULL);
    fgetc(NULL);
    fgets(NULL, NULL, NULL);
    fopen(NULL, NULL);
    fread(NULL, NULL, NULL, NULL);
    freopen(NULL, NULL, NULL);
    fscanf(NULL, NULL, NULL);
    fseek(NULL, NULL, NULL);
    ftell(NULL);
    gettimeofday(NULL, NULL, NULL);
    memcmp(NULL, NULL, NULL);
    rename(NULL, NULL);
    setvbuf(NULL, NULL, NULL, NULL);
    sscanf(NULL, NULL, NULL);
    strlen(NULL);
    strncat();
    system(NULL);
    tolower();
    toupper();
    ungetc(NULL, NULL);
    acos(0.0);
    asin(0.0);
    atan(0.0);
    atan2(0.0);
    ceil(0.0);
    cos(0.0);
    cosh(0.0);
    exp(0.0);
    floor(0.0);
    log(0.0);
    pow(0.0);
    sin(0.0);
    sinh(0.0);
    sqrt(0.0);
    tan(0.0);
    fmod(0.0, 1.0);
}

#endif		/* DO_TEST2 */
