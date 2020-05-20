/****************************************************************************
*                   txttest.h
*
*  This module contains all defines, typedefs, and prototypes for TXTTEST.C.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file. If
*  POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's Graphics Developer's
*  Forum.  The latest version of POV-Ray may be found there as well.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/


#ifndef TXTTEST_H
#define TXTTEST_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/




/*****************************************************************************
* Global typedefs
******************************************************************************/




/*****************************************************************************
* Global variables
******************************************************************************/




/*****************************************************************************
* Global functions
******************************************************************************/

DBL pattern1 PARAMS((VECTOR EPoint, TPATTERN *TPat));
DBL pattern2 PARAMS((VECTOR EPoint, TPATTERN *TPat));
DBL pattern3 PARAMS((VECTOR EPoint, TPATTERN *TPat)); 
void bumpy1 PARAMS((VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal));
void bumpy2 PARAMS((VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal));
void bumpy3 PARAMS((VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal));



#endif
