/****************************************************************************
*                   render.h
*
*  This module contains all defines, typedefs, and prototypes for RENDER.C.
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


#ifndef RENDER_H
#define RENDER_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/




/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef enum
{
  CSV,
  SYS,
  PPM,
  TARGA,
  PNG,
  NONE
} Histogram_Types ;

/*****************************************************************************
* Global variables
******************************************************************************/

extern int Trace_Level, Max_Trace_Level, Highest_Trace_Level;

extern DBL ADC_Bailout;

extern unsigned long              *histogram_grid ;
extern unsigned long              max_histogram_value ;
extern FILE_HANDLE                *Histogram_File_Handle ;

/*****************************************************************************
* Global functions
******************************************************************************/

void Read_Rendered_Part PARAMS((char *New_Fname));
void Start_Tracing_Mosaic_Smooth PARAMS((int StartPixelSize, int EndPixelSize));
void Start_Tracing_Mosaic_Preview PARAMS((int StartPixelSize, int EndPixelSize));
void Start_Adaptive_Tracing PARAMS((void));
void Start_Non_Adaptive_Tracing PARAMS((void));
void Initialize_Renderer PARAMS((void));
void Terminate_Renderer PARAMS((void));
DBL  Trace PARAMS((RAY *Ray, COLOUR Colour, DBL Weight));
void Check_User_Abort PARAMS((int Do_Stats));
void write_histogram PARAMS ((char *filename)) ;
void destroy_histogram PARAMS ((void)) ;

#endif
