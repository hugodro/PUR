/****************************************************************************
*                   colour.h
*
*  This module contains all defines, typedefs, and prototypes for COLOUR.C.
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

/* NOTE: FRAME.H contains other colour stuff. */

#ifndef COLOUR_H
#define COLOUR_H



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

COLOUR *Create_Colour PARAMS((void));
COLOUR *Copy_Colour PARAMS((COLOUR Old));
BLEND_MAP_ENTRY *Create_BMap_Entries PARAMS((int Map_Size));
BLEND_MAP_ENTRY *Copy_BMap_Entries PARAMS((BLEND_MAP_ENTRY *Old,int Map_Size,int Type));
BLEND_MAP *Create_Blend_Map PARAMS((void));
BLEND_MAP *Copy_Blend_Map PARAMS((BLEND_MAP *Old));
DBL Colour_Distance PARAMS((COLOUR colour1, COLOUR colour2));
void Add_Colour PARAMS((COLOUR result, COLOUR colour1, COLOUR colour2));
void Scale_Colour PARAMS((COLOUR result, COLOUR colour, DBL factor));
void Clip_Colour PARAMS((COLOUR result, COLOUR colour));
void Destroy_Blend_Map PARAMS((BLEND_MAP *BMap));



#endif
