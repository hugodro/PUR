/****************************************************************************
*                   boxes.h
*
*  This module contains all defines, typedefs, and prototypes for BOXES.C.
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


#ifndef BOXES_H
#define BOXES_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define BOX_OBJECT (BASIC_OBJECT)



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Box_Struct BOX;

struct Box_Struct
{
  OBJECT_FIELDS
  TRANSFORM *Trans;
  VECTOR bounds[2];
};



/*****************************************************************************
* Global variables
******************************************************************************/

extern METHODS Box_Methods;



/*****************************************************************************
* Global functions
******************************************************************************/

int  Intersect_Box PARAMS((RAY *Ray, BOX *box, DBL *Depth1, DBL *Depth2, int *Side1, int *Side2));
BOX  *Create_Box PARAMS((void));
void Destroy_Box PARAMS((OBJECT *Object));
void *Copy_Box PARAMS((OBJECT *Object));
void Compute_Box_BBox PARAMS((BOX *Box));

#endif
