/****************************************************************************
*                bmp.h
*
*  This module contains all defines, typedefs, and prototypes for BMP.C.
*
*  This module was written by Tim Rowley (tor@cs.brown.edu)
*
*  from Persistence of Vision Raytracer
*  Copyright 1996-1997 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file. If
*  POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's POVRAY forum. The
*  The latest version of POV-Ray may be found there as well. POVRAY files can
*  also be found on the world wide web at http://www.povray.org/.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

#ifndef BMP_H
#define BMP_H

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

FILE_HANDLE *Get_BMP_File_Handle PARAMS((void));
void Read_BMP_Image PARAMS((IMAGE *Image, char *filename));

#endif
