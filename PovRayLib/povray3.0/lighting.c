/****************************************************************************
*                   lighting.c
*
*  This module calculates lighting properties like ambient, diffuse, specular,
*  reflection, refraction, etc.
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

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "blob.h"
#include "bbox.h"
#include "colour.h"
#include "halos.h"
#include "image.h"
#include "lbuffer.h"
#include "lighting.h"
#include "mesh.h"
#include "normal.h"
#include "objects.h"
#include "octree.h"
#include "pattern.h"  /* [CEY 10/94] */
#include "pigment.h"
#include "povray.h"
#include "radiosit.h"
#include "ray.h"
#include "render.h"
#include "texture.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define BLACK_LEVEL 0.003

/*
 * "Small_Tolerance" is just too tight for higher order polynomial equations.
 * this value should probably be a variable of some sort, but for now just
 * use a reasonably small value.  If people render real small objects real
 * close to each other then there may be some shading problems.  Otherwise
 * having SHADOW_TOLERANCE as large as this won't affect images.
 */

#define SHADOW_TOLERANCE 1.0e-3

/* Number of inital entries in the texture and weight list. */

#define NUMBER_OF_ENTRIES 16



/*****************************************************************************
* Local typedefs
******************************************************************************/

/*
 * List to store light colours during shadow testing
 * to avoid repeated testing with layered textures.
 */

typedef struct Light_Tested_Struct LIGHT_TESTED;

struct Light_Tested_Struct
{
  int    Tested;
  COLOUR Colour;
};



/*****************************************************************************
* Local variables
******************************************************************************/

static LIGHT_TESTED *Light_List;
static TEXTURE **Texture_List;
static DBL *Weight_List;

static int Number_Of_Textures_And_Weights;



/*****************************************************************************
* Static functions
******************************************************************************/

static void block_area_light PARAMS((LIGHT_SOURCE *Light_Source,
  DBL *Light_Source_Depth, RAY *Light_Source_Ray, RAY *Eye_Ray,
  VECTOR IPoint, COLOUR Light_Colour, int u1, int v1, int u2, int v2, int Level));

static void block_point_light PARAMS((LIGHT_SOURCE *Light_Source,
  DBL *Light_Source_Depth, RAY *Light_Source_Ray, COLOUR Light_Colour));

static void block_point_light_LBuffer PARAMS((LIGHT_SOURCE *Light_Source,
  DBL *Light_Source_Depth, RAY *Light_Source_Ray, COLOUR Light_Colour));

static void do_light PARAMS((LIGHT_SOURCE *Light_Source,
  DBL *Light_Source_Depth, RAY *Light_Source_Ray, RAY *Eye_Ray, VECTOR IPoint,
  COLOUR Light_Colour));

static int do_blocking PARAMS((INTERSECTION *Local_Intersection,
  RAY *Light_Source_Ray, COLOUR Light_Colour, ISTACK *Local_Stack));

static void do_phong PARAMS((FINISH *Finish, RAY *Light_Source_Ray,
  VECTOR Eye, VECTOR Layer_Normal, COLOUR Colour, COLOUR Light_Colour,
  COLOUR Layer_Texture_Colour));

static void do_specular PARAMS((FINISH *Finish, RAY *Light_Source_Ray,
  VECTOR REye, VECTOR Layer_Normal, COLOUR Colour, COLOUR Light_Colour,
  COLOUR Layer_Pigment_Colour));

static void do_diffuse PARAMS((FINISH *Finish, RAY *Light_Source_Ray,
  VECTOR Layer_Normal, COLOUR Colour, COLOUR Light_Colour,
  COLOUR Layer_Pigment_Colour, DBL Attenuation));

static void do_irid PARAMS((FINISH *Finish, RAY *Light_Source_Ray,
  VECTOR Layer_Normal, VECTOR IPoint, COLOUR Colour));

static void Diffuse PARAMS((FINISH *Finish, VECTOR IPoint, RAY *Eye, VECTOR Layer_Normal,
  COLOUR Layer_Pigment_Colour, COLOUR Colour,DBL Attenuation, OBJECT *Object));

static void Reflect PARAMS((RGB Reflection, VECTOR IPoint, RAY *Ray, VECTOR Layer_Normal, COLOUR Colour, DBL Weight));

static void Refract PARAMS((OBJECT *Object, TEXTURE *Texture, VECTOR IPoint,
  RAY *Ray, VECTOR Layer_Normal, COLOUR Colour, DBL Weight));

static void filter_shadow_ray PARAMS((INTERSECTION *Ray_Intersection,
  RAY *Light_Source_Ray, COLOUR Colour));

static int create_texture_list PARAMS((INTERSECTION *Ray_Intersection));

static void do_texture_map PARAMS((COLOUR Result_Colour,
  TEXTURE *Texture, VECTOR IPoint, VECTOR Raw_Normal, RAY *Ray, DBL Weight,
  INTERSECTION *Ray_Intersection, int Shadow_Flag));

static void average_textures PARAMS((COLOUR Result_Colour,
  TEXTURE *Texture, VECTOR IPoint, VECTOR Raw_Normal, RAY *Ray, DBL Weight,
  INTERSECTION *Ray_Intersection, int Shadow_Flag));

static void compute_lighted_texture PARAMS((COLOUR Result_Colour,
  TEXTURE *Texture, VECTOR IPoint, VECTOR Raw_Normal, RAY *Ray, DBL Weight,
  INTERSECTION *Ray_Intersection));

static void compute_shadow_texture PARAMS((COLOUR Filter_Colour,
  TEXTURE *Texture, VECTOR IPoint, VECTOR Raw_Normal, RAY *Ray,
  INTERSECTION *Ray_Intersection));

static void block_light_source PARAMS((LIGHT_SOURCE *Light,
  DBL Depth, RAY *Light_Source_Ray, RAY *Eye_Ray, VECTOR P, COLOUR Colour));

static void do_light_ray_atmosphere PARAMS((RAY *Light_Source_Ray,
  INTERSECTION *Ray_Intersection, TEXTURE *Texture, COLOUR Colour, int Valid_Object));



/*****************************************************************************
*
* FUNCTION
*
*   Initialize_Lighting_Code
*
* INPUT
*
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Allocate lists needed during lighting calculations.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
*   Okt 1994 : Added initialization of Light_List and test if there are
*              any light sources in the scene. [DB]
*
******************************************************************************/

void Initialize_Lighting_Code()
{
  int i;

  Light_List = NULL;
  Texture_List = NULL;
  Weight_List  = NULL;

  /* Allocate memory for light list. */

  if (Frame.Number_Of_Light_Sources > 0)
  {
    Light_List = (LIGHT_TESTED *)POV_MALLOC(Frame.Number_Of_Light_Sources*sizeof(LIGHT_TESTED), "temporary light list");

    for (i = 0; i < Frame.Number_Of_Light_Sources; i++)
    {
      Light_List[i].Tested = FALSE;

      Make_ColourA(Light_List[i].Colour, 0.0, 0.0, 0.0, 0.0, 0.0);
    }
  }

  /* Allocate memory for texture and weight list. */

  Number_Of_Textures_And_Weights = NUMBER_OF_ENTRIES;

  Texture_List = (TEXTURE **)POV_MALLOC(Number_Of_Textures_And_Weights*sizeof(TEXTURE *), "texture list");

  Weight_List = (DBL *)POV_MALLOC(Number_Of_Textures_And_Weights*sizeof(DBL), "weight list");
}



/*****************************************************************************
*
* FUNCTION
*
*   Reinitialize_Lighting_Code
*
* INPUT
*
*   Number_Of_Entries - New number of entries in the texture/weight lists
*   
* OUTPUT
*
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Resize variable lists needed during lighting calculation.
*
* CHANGES
*
*   Jul 1995 : Creation.
*
*   Mar 1996 : We have to pass pointers to the lists to resize because during
*              resizing the pointers to the lists change and thus the calling
*              functions does not longer know where the lists are if the
*              pointers to the lists where passed to it using arguments. [DB]
*
******************************************************************************/

void Reinitialize_Lighting_Code(Number_Of_Entries, Textures, Weights)
int Number_Of_Entries;
TEXTURE ***Textures;
DBL **Weights;
{
  if (Number_Of_Entries > Number_Of_Textures_And_Weights)
  {
    if (Number_Of_Entries >= INT_MAX / 2)
    {
      Error("Too many entries in texture and weight lists.\n");
    }

    Number_Of_Textures_And_Weights = Number_Of_Entries;

    Texture_List = (TEXTURE **)POV_REALLOC(Texture_List, Number_Of_Textures_And_Weights*sizeof(TEXTURE *), "texture list");

    Weight_List = (DBL *)POV_REALLOC(Weight_List, Number_Of_Textures_And_Weights*sizeof(DBL), "weight list");

    *Textures = Texture_List;
    *Weights  = Weight_List;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Deinitialize_Lighting_Code
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Destroy all lists needed during lighting calculation.
*
* CHANGES
*
*   Sep 1994 : Creation.
*
*   Jul 1995 : Added code to free local texture and weight lists. [DB]
*
******************************************************************************/

void Deinitialize_Lighting_Code()
{
  if (Light_List != NULL)
  {
    POV_FREE(Light_List);
  }

  if (Texture_List != NULL)
  {
    POV_FREE(Texture_List);
  }

  if (Weight_List != NULL)
  {
    POV_FREE(Weight_List);
  }

  Light_List   = NULL;
  Texture_List = NULL;
  Weight_List  = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   Determine_Apparent_Colour
*
* INPUT
*
*   Ray_Intersection - info on where ray hit & object it hit
*   Ray              - the ray from which object is seen
*   Weight           - Automatic depth control value
*
* OUTPUT
*
*   Colour           - resulting color is added to given color. The RGB
*                      components are significant. The transmittance channel
*                      is used as an alpha channel.
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Given an intersection point, a ray, add that point's visible color
*   to the given colour and return it.  This routine just does preliminary
*   initializations and calls to set up the multi-texture blob list if any.
*   Then it calls do_texture_map which in turn calls compute_lighted_texture
*   to do the actual lighting calculations.  These functions were seperated
*   from this function because do_texture_map may need to call itself
*   recursively.
*
* CHANGES
*
*   Sep 1994 : Code for multi-textured blobs added. [DB]
*
*   Nov 1994 : Moved calls to Fog and Rainbow into tracing functions. [DB]
*
*   Jan 1995 : Moved much of code to do_texture_map and
*              compute_lighted_texture [CEY]
*
*   Jul 1995 : Added code to support alpha channel. [DB]
*
*   Mar 1996 : Fixed severe bug (weight and texture lists were not saved) [DB]
*
******************************************************************************/

void Determine_Apparent_Colour(Ray_Intersection, Colour, Ray, Weight)
INTERSECTION *Ray_Intersection;
COLOUR Colour;
RAY *Ray;
DBL Weight;
{
  int i, Texture_Count;
  size_t savelights_size, save_tw_size;
  DBL *save_Weights = NULL;
  COLOUR C1;
  VECTOR Raw_Normal;
  VECTOR IPoint;
  TEXTURE *Texture, **save_Textures = NULL;
  LIGHT_TESTED *savelights = NULL;

  Assign_Vector(IPoint,Ray_Intersection->IPoint);

  /*
   * Save existing light list if any. If we are not top level in recursion
   * depth, this information may be reused by upper level of trace.
   */

  savelights_size = (size_t)(Frame.Number_Of_Light_Sources*sizeof(LIGHT_TESTED));

  if (savelights_size > 0)
  {
    savelights = (LIGHT_TESTED *)POV_MALLOC(savelights_size, "Light list stack");

    memcpy(savelights, Light_List, savelights_size);
  }

  /* Init light list. */

  for (i = 0; i < Frame.Number_Of_Light_Sources; i++)
  {
    Light_List[i].Tested = FALSE;
  }

  /* Get the normal to the surface */

  Normal(Raw_Normal, Ray_Intersection->Object, Ray_Intersection);

  /*
   * Save texture and weight lists.
   */

  save_tw_size = (size_t)Number_Of_Textures_And_Weights;

  if (save_tw_size > 0)
  {
    save_Weights = (DBL *)POV_MALLOC(save_tw_size * sizeof(DBL), "Weight list stack");

    memcpy(save_Weights, Weight_List, save_tw_size * sizeof(DBL));

    save_Textures = (TEXTURE **)POV_MALLOC(save_tw_size * sizeof(TEXTURE *), "Weight list stack");

    memcpy(save_Textures, Texture_List, save_tw_size * sizeof(TEXTURE *));
  }

  /* Get texture list and weights. */

  Texture_Count = create_texture_list (Ray_Intersection);

  /*
   * Now, we perform the lighting calculations by stepping through
   * the list of textures and summing the weighted color.
   */

  for (i = 0; i < Texture_Count; i++)
  {
    /* If contribution of this texture is neglectable skip ahead. */

    if (Weight_List[i] < BLACK_LEVEL)
    {
      continue;
    }

    Texture = Texture_List[i];

    do_texture_map(C1, Texture, IPoint, Raw_Normal, Ray, Weight, Ray_Intersection, FALSE);

    Colour[RED]   += Weight_List[i] * C1[RED];
    Colour[GREEN] += Weight_List[i] * C1[GREEN];
    Colour[BLUE]  += Weight_List[i] * C1[BLUE];

    /* Use transmittance value for alpha channel support. [DB] */

/*
    Colour[TRANSM]  += Weight_List[i] * C1[TRANSM];
*/
    Colour[TRANSM] *= C1[TRANSM];
  }

  /* Restore the light list to its original form */

  if (savelights_size > 0)
  {
    memcpy(Light_List, savelights, savelights_size);

    POV_FREE(savelights);
  }

  /* Restore the weight and texture list. */

  if (save_tw_size > 0)
  {
    memcpy(Weight_List, save_Weights, save_tw_size * sizeof(DBL));
    memcpy(Texture_List, save_Textures, save_tw_size * sizeof(TEXTURE *));

    POV_FREE(save_Weights);
    POV_FREE(save_Textures);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Test_Shadow
*
* INPUT
*
*   Light            - Light source
*   P                - Point to test
*
* OUTPUT
*
*   Depth            - Distance to light source
*   Light_Source_Ray - Light ray pointing towards the light source
*   Eye_Ray          - Current viewing ray
*   Colour           - Light color reaching point P
*
* RETURNS
*
*   int - TRUE if point lies in shadow
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Test if a given point is in shadow in respect to the given light source.
*
*   The viewing ray is used to initialize the ray containers of the
*   light source ray.
*
* CHANGES
*
*   Nov 1994 : Creation.
*
******************************************************************************/

int Test_Shadow(Light, Depth, Light_Source_Ray, Eye_Ray, P, Colour)
LIGHT_SOURCE *Light;
DBL *Depth;
RAY *Light_Source_Ray, *Eye_Ray;
VECTOR P;
COLOUR Colour;
{
  do_light(Light, Depth, Light_Source_Ray, Eye_Ray, P, Colour);

  /*
   * There's no need to test for shadows if no light
   * is coming from the light source.
   */

  if ((Colour[X] < BLACK_LEVEL) && (Colour[Y] < BLACK_LEVEL) && (Colour[Z] < BLACK_LEVEL))
  {
    return(TRUE);
  }
  else
  {
    /* Test for shadows. */

    if ((opts.Quality_Flags & Q_SHADOW) && (Light->Light_Type != FILL_LIGHT_SOURCE))
    {
      block_light_source(Light, *Depth, Light_Source_Ray, Eye_Ray, P, Colour);

      if ((Colour[X] < BLACK_LEVEL) && (Colour[Y] < BLACK_LEVEL) && (Colour[Z] < BLACK_LEVEL))
      {
        return(TRUE);
      }
    }
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   block_point_light_LBuffer
*
* INPUT
*
*   Light_Source       - Light source to test
*
* OUTPUT
*
*   Light_Source_Depth - (Remaining) distance to the light source
*   Light_Source_Ray   - (Remaining) ray to the light source
*   Colour             - Color reaching initial point from light source
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Determine how much light from the given light source arrives at the
*   given point (starting point of the light source ray). The light
*   is blocked by solid objects and/or attenuated by translucent objects.
*
*   Note that both the distance to the light source and the light source
*   ray are modified. Thus after a call to this function one knows
*   how much distance remains to the light source and where the last
*   intersection point with a translucent object was (starting point
*   of light source ray after the call).
*
*   This function uses the light buffer to speed up shadow calculation.
*
* CHANGES
*
*   Jul 1994 : Creation.
*
******************************************************************************/

static void block_point_light_LBuffer(Light_Source, Light_Source_Depth, Light_Source_Ray, Light_Colour)
LIGHT_SOURCE *Light_Source;
DBL *Light_Source_Depth;
RAY *Light_Source_Ray;
COLOUR Light_Colour;
{
  int Quit_Looking, Not_Found_Shadow, Cache_Me;
  int u, v, axis;
  DBL ax, ay, az;
  VECTOR V1;
  OBJECT *Blocking_Object;
  ISTACK *Local_Stack;
  INTERSECTION *Local_Intersection, Bounded_Intersection;

  Local_Stack = open_istack();

  Quit_Looking = FALSE;

  /* First test the cached object (don't cache semi-transparent objects). */

  if (Light_Source->Shadow_Cached_Object != NULL)
  {
    Increase_Counter(stats[Shadow_Ray_Tests]);

    if (Ray_In_Bound(Light_Source_Ray, Light_Source->Shadow_Cached_Object->Bound))
    {
      if (All_Intersections(Light_Source->Shadow_Cached_Object, Light_Source_Ray, Local_Stack))
      {
        while ((Local_Intersection=pop_entry(Local_Stack)) != NULL)
        {
          if ((!Test_Flag(Local_Intersection->Object, NO_SHADOW_FLAG)) &&
              (Local_Intersection->Depth < *Light_Source_Depth-SHADOW_TOLERANCE) &&
              (Local_Intersection->Depth > SHADOW_TOLERANCE))
          {
            if (do_blocking(Local_Intersection, Light_Source_Ray, Light_Colour, Local_Stack))
            {
              Quit_Looking = TRUE;

              Increase_Counter(stats[Shadow_Cache_Hits]);

              break;
            }
          }
        }
      }
    }

    /* Exit if the cached object was hit. */

    if (Quit_Looking)
    {
      close_istack(Local_Stack);

      return;
    }
  }

  /*
   * Determine the side and the coordinates at which the ray
   * pierces the cube enclosing the light source.
   */

  V1[X] = -Light_Source_Ray->Direction[X];
  V1[Y] = -Light_Source_Ray->Direction[Y];
  V1[Z] = -Light_Source_Ray->Direction[Z];

  ax = fabs(V1[X]);
  ay = fabs(V1[Y]);
  az = fabs(V1[Z]);

  if ((ax>ay) && (ax>az))
  {
    if (V1[X]>0.0)
    {
      axis = XaxisP;
    }
    else
    {
      axis = XaxisM;
    }

    u = (int)(MAX_BUFFER_ENTRY * V1[Y]/ax);
    v = (int)(MAX_BUFFER_ENTRY * V1[Z]/ax);
  }
  else
  {
    if (ay>az)
    {
      if (V1[Y]>0.0)
      {
        axis = YaxisP;
      }
      else
      {
        axis = YaxisM;
      }

      u = (int)(MAX_BUFFER_ENTRY * V1[X]/ay);
      v = (int)(MAX_BUFFER_ENTRY * V1[Z]/ay);
    }
    else
    {
      if (V1[Z]>0.0)
      {
        axis = ZaxisP;
      }
      else
      {
        axis = ZaxisM;
      }

      u = (int)(MAX_BUFFER_ENTRY * V1[X]/az);
      v = (int)(MAX_BUFFER_ENTRY * V1[Y]/az);
    }
  }

  /* If there are no objects in the direction of the ray we can exit. */

  if (Light_Source->Light_Buffer[axis] == NULL)
  {
    close_istack(Local_Stack);

    return;
  }

  /* Look for shadows. */

  Not_Found_Shadow = TRUE;

  Cache_Me = FALSE;

  while (!Quit_Looking)
  {
    Increase_Counter(stats[Shadow_Ray_Tests]);

    Bounded_Intersection.Depth = *Light_Source_Depth;

    if (Intersect_Light_Tree(Light_Source_Ray, Light_Source->Light_Buffer[axis], u, v, &Bounded_Intersection, &Blocking_Object))
    {
      if (Bounded_Intersection.Depth > *Light_Source_Depth)
      {
        /* Intersection was beyond the light. */

        break;
      }

      if (!Test_Flag(Bounded_Intersection.Object, NO_SHADOW_FLAG))
      {
        if (Blocking_Object != Light_Source->Shadow_Cached_Object)
        {
          Increase_Counter(stats[Shadow_Rays_Succeeded]);

          filter_shadow_ray(&Bounded_Intersection, Light_Source_Ray, Light_Colour);

          if ((fabs(Light_Colour[RED])   < BLACK_LEVEL) &&
              (fabs(Light_Colour[GREEN]) < BLACK_LEVEL) &&
              (fabs(Light_Colour[BLUE])  < BLACK_LEVEL) &&
              (Test_Flag(Blocking_Object, OPAQUE_FLAG)))
          {
            Cache_Me = Not_Found_Shadow;

            break; /* from while */
          }
        }
      }

      /* Move the ray to the point of intersection, plus some */

      *Light_Source_Depth -= Bounded_Intersection.Depth;

      Assign_Vector(Light_Source_Ray->Initial, Bounded_Intersection.IPoint);

      Not_Found_Shadow = FALSE;
    }
    else
    {
      /* No intersections in the direction of the ray. */

      break;
    }
  }

  if (Cache_Me)
  {
    Light_Source->Shadow_Cached_Object = Blocking_Object;
  }

  close_istack(Local_Stack);
}



/*****************************************************************************
*
* FUNCTION
*
*   block_point_light
*
* INPUT
*
*   Light_Source       - Light source to test
*   Eye_Ray            - Current viewing ray
*
* OUTPUT
*
*   Light_Source_Depth - (Remaining) distance to the light source
*   Light_Source_Ray   - (Remaining) ray to the light source
*   Colour             - Color reaching initial point from light source
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   See block_point_light_LBuffer for a description.
*
*   This function uses the hierarchical bounding box volume to
*   speed up shadow testing.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void block_point_light (Light_Source, Light_Source_Depth, Light_Source_Ray, Light_Colour)
LIGHT_SOURCE *Light_Source;
DBL *Light_Source_Depth;
RAY *Light_Source_Ray;
COLOUR Light_Colour;
{
  OBJECT *Blocking_Object;
  int Quit_Looking, Not_Found_Shadow, Cache_Me, Maybe_Found;
  INTERSECTION *Local_Intersection, Bounded_Intersection, Temp_Intersection;
  ISTACK *Local_Stack;

  Local_Stack = open_istack ();

  Quit_Looking = FALSE;

  /* First test the cached object (don't cache semi-transparent objects). */

  if (Light_Source->Shadow_Cached_Object != NULL)
  {
    Increase_Counter(stats[Shadow_Ray_Tests]);

    if (Ray_In_Bound(Light_Source_Ray, Light_Source->Shadow_Cached_Object->Bound))
    {
      if (All_Intersections(Light_Source->Shadow_Cached_Object, Light_Source_Ray, Local_Stack))
      {
        while ((Local_Intersection = pop_entry(Local_Stack)) != NULL)
        {
          if ((!Test_Flag(Local_Intersection->Object, NO_SHADOW_FLAG)) &&
              (Local_Intersection->Depth < *Light_Source_Depth-SHADOW_TOLERANCE) &&
              (Local_Intersection->Depth > SHADOW_TOLERANCE))
          {
            if (do_blocking(Local_Intersection, Light_Source_Ray, Light_Colour, Local_Stack))
            {
              Quit_Looking = TRUE;

              Increase_Counter(stats[Shadow_Cache_Hits]);

              break;
            }
          }
        }
      }
    }

    if (Quit_Looking)
    {
      close_istack (Local_Stack);

      return;
    }
  }

  /* Look for shadows. */

  Not_Found_Shadow = TRUE;

  Cache_Me = FALSE;

  if (!opts.Use_Slabs)
  {
    while (!Quit_Looking)
    {
      /* Use brute force method to get shadows. */

      Maybe_Found = FALSE;

      Bounded_Intersection.Depth = *Light_Source_Depth;

      for (Blocking_Object = Frame.Objects; Blocking_Object != NULL; Blocking_Object = Blocking_Object->Sibling)
      {
        if (Blocking_Object != Light_Source->Shadow_Cached_Object)
        {
          if (!Test_Flag(Blocking_Object, NO_SHADOW_FLAG))
          {
            Increase_Counter(stats[Shadow_Ray_Tests]);

            if (Intersection(&Temp_Intersection, Blocking_Object, Light_Source_Ray))
            {
              if (Temp_Intersection.Depth < Bounded_Intersection.Depth)
              {
                Maybe_Found = TRUE;

                Bounded_Intersection = Temp_Intersection;
              }
            }
          }
        }
      }

      if (Maybe_Found)
      {
        Increase_Counter(stats[Shadow_Rays_Succeeded]);

        filter_shadow_ray(&Bounded_Intersection, Light_Source_Ray, Light_Colour);

        if ((fabs(Light_Colour[RED])   < BLACK_LEVEL) &&
            (fabs(Light_Colour[GREEN]) < BLACK_LEVEL) &&
            (fabs(Light_Colour[BLUE])  < BLACK_LEVEL) &&
            (Test_Flag(Bounded_Intersection.Object, OPAQUE_FLAG)))
        {
          Cache_Me = Not_Found_Shadow;

          break;
        }

        /* Move the ray to the point of intersection, plus some */

        *Light_Source_Depth -= Bounded_Intersection.Depth;

        Assign_Vector(Light_Source_Ray->Initial, Bounded_Intersection.IPoint);

        Not_Found_Shadow = FALSE;
      }
      else
      {
        /* No intersections in the direction of the ray. */

        break;
      }
    }
  }
  else
  {
    /* Use bounding slabs to look for shadows. */

    while (!Quit_Looking)
    {
      Increase_Counter(stats[Shadow_Ray_Tests]);

      Bounded_Intersection.Depth = *Light_Source_Depth;

      if (Intersect_BBox_Tree(Root_Object, Light_Source_Ray, &Bounded_Intersection, &Blocking_Object))
      {
        if (Bounded_Intersection.Depth > *Light_Source_Depth)
        {
          /* Intersection was beyond the light. */

          break;
        }

        if (!Test_Flag(Bounded_Intersection.Object, NO_SHADOW_FLAG))
        {
          if (Blocking_Object != Light_Source->Shadow_Cached_Object)
          {
            Increase_Counter(stats[Shadow_Rays_Succeeded]);

            filter_shadow_ray(&Bounded_Intersection, Light_Source_Ray, Light_Colour);

            if ((fabs(Light_Colour[RED])   < BLACK_LEVEL) &&
                (fabs(Light_Colour[GREEN]) < BLACK_LEVEL) &&
                (fabs(Light_Colour[BLUE])  < BLACK_LEVEL) &&
                (Test_Flag(Blocking_Object, OPAQUE_FLAG)))
            {
              Cache_Me = Not_Found_Shadow;

              break; /* from while */
            }
          }
        }

        /* Move the ray to the point of intersection, plus some */

        *Light_Source_Depth -= Bounded_Intersection.Depth;

        Assign_Vector(Light_Source_Ray->Initial, Bounded_Intersection.IPoint);

        Not_Found_Shadow = FALSE;
      }
      else
      {
        /* No intersections in the direction of the ray */

        break;
      }
    }
  }

  if (Cache_Me)
  {
    Light_Source->Shadow_Cached_Object = Blocking_Object;
  }

  close_istack (Local_Stack);
}



/*****************************************************************************
*
* FUNCTION
*
*   block_area_light
*
* INPUT
*
*   Light_Source       - Light source to test
*   IPoint             -
*   u1, v1, u2, v2     -
*   Level              -
*
* OUTPUT
*
*   Light_Source_Depth - (Remaining) distance to the light source
*   Light_Source_Ray   - (Remaining) ray to the light source
*   Light_Colour       - Color reaching initial point from light source
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Get shadow for given area light source by recursively sampling
*   on the light source area.
*
*   The viewing ray is used to initialize the ray containers of the
*   light source ray.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void block_area_light (Light_Source, Light_Source_Depth,
  Light_Source_Ray, Eye_Ray, IPoint, Light_Colour, u1, v1, u2, v2, Level)
LIGHT_SOURCE *Light_Source;
DBL *Light_Source_Depth;
RAY *Light_Source_Ray, *Eye_Ray;
VECTOR IPoint;
COLOUR Light_Colour;
int u1, v1, u2, v2, Level;
{
  COLOUR Sample_Colour[4], Dummy_Colour;
  VECTOR Center_Save, NewAxis1, NewAxis2;
  int i, j, u, v, New_u1, New_v1, New_u2, New_v2;

  DBL Jitter_u, Jitter_v, ScaleFactor;

  /* First call, initialize */

  if ((u1 == 0) && (v1 == 0) && (u2 == 0) && (v2 == 0))
  {
    /* Flag uncalculated points with a negative value for Red */

    for (i = 0; i < Light_Source->Area_Size1; i++)
    {
      for (j = 0; j < Light_Source->Area_Size2; j++)
      {
        Light_Source->Light_Grid[i][j][RED] = -1.0;
      }
    }

    u1 = 0;
    v1 = 0;
    u2 = Light_Source->Area_Size1 - 1;
    v2 = Light_Source->Area_Size2 - 1;
  }

  /* Save the light source center since we'll be fiddling with it */

  Assign_Vector(Center_Save,Light_Source->Center);

  /* Sample the four corners of the region */

  for (i = 0; i < 4; i++)
  {
    switch (i)
    {
      case 0: u = u1; v = v1; break;
      case 1: u = u2; v = v1; break;
      case 2: u = u1; v = v2; break;
      case 3: u = u2; v = v2; break;
      default: u = v = 0;  /* Should never happen! */
    }

    if (Light_Source->Light_Grid[u][v][RED] >= 0.0)
    {
      /* We've already calculated this point, reuse it */

      Assign_Colour(Sample_Colour[i],Light_Source->Light_Grid[u][v]);
    }
    else
    {
      Jitter_u = (DBL)u;
      Jitter_v = (DBL)v;

      if (Light_Source->Jitter)
      {
        Jitter_u += FRAND() - 0.5;
        Jitter_v += FRAND() - 0.5;
      }

      if (Light_Source->Area_Size1 > 1)
      {
        ScaleFactor = Jitter_u/(DBL)(Light_Source->Area_Size1 - 1) - 0.5;

        VScale (NewAxis1, Light_Source->Axis1, ScaleFactor)
      }
      else
      {
        Make_Vector(NewAxis1, 0.0, 0.0, 0.0);
      }

      if (Light_Source->Area_Size2 > 1)
      {
        ScaleFactor = Jitter_v/(DBL)(Light_Source->Area_Size2 - 1) - 0.5;

        VScale (NewAxis2, Light_Source->Axis2, ScaleFactor)
      }
      else
      {
        Make_Vector(NewAxis2, 0.0, 0.0, 0.0);
      }

      Assign_Vector(Light_Source->Center, Center_Save);

      VAddEq(Light_Source->Center, NewAxis1);
      VAddEq(Light_Source->Center, NewAxis2);

      /* Recalculate the light source ray but not the colour */

      do_light(Light_Source, Light_Source_Depth, Light_Source_Ray, Eye_Ray, IPoint, Dummy_Colour);

      Assign_Colour(Sample_Colour[i], Light_Colour);

      block_point_light(Light_Source, Light_Source_Depth, Light_Source_Ray, Sample_Colour[i]);

      Assign_Colour(Light_Source->Light_Grid[u][v], Sample_Colour[i]);
    }
  }

  Assign_Vector(Light_Source->Center,Center_Save);

  if ((u2 - u1 > 1) || (v2 - v1 > 1))
  {
    if ((Level < Light_Source->Adaptive_Level) ||
        (Colour_Distance(Sample_Colour[0], Sample_Colour[1]) > 0.1) ||
        (Colour_Distance(Sample_Colour[1], Sample_Colour[3]) > 0.1) ||
        (Colour_Distance(Sample_Colour[3], Sample_Colour[2]) > 0.1) ||
        (Colour_Distance(Sample_Colour[2], Sample_Colour[0]) > 0.1))
    {
      for (i = 0; i < 4; i++)
      {
        switch (i)
        {
          case 0:

            New_u1 = u1;
            New_v1 = v1;
            New_u2 = (int)floor ((u1 + u2)/2.0);
            New_v2 = (int)floor ((v1 + v2)/2.0);

            break;

          case 1:

            New_u1 = (int)ceil  ((u1 + u2)/2.0);
            New_v1 = v1;
            New_u2 = u2;
            New_v2 = (int)floor ((v1 + v2)/2.0);

            break;

          case 2:

            New_u1 = u1;
            New_v1 = (int)ceil  ((v1 + v2)/2.0);
            New_u2 = (int)floor ((u1 + u2)/2.0);
            New_v2 = v2;

            break;

          case 3:

            New_u1 = (int)ceil ((u1 + u2)/2.0);
            New_v1 = (int)ceil ((v1 + v2)/2.0);
            New_u2 = u2;
            New_v2 = v2;

            break;

          default:  /* Should never happen! */

            New_u1 = New_u2 = New_v1 = New_v2 = 0;
        }

        /* Recalculate the light source ray but not the colour */

        do_light(Light_Source, Light_Source_Depth, Light_Source_Ray, Eye_Ray, IPoint, Dummy_Colour);

        Assign_Colour(Sample_Colour[i],Light_Colour);

        block_area_light (Light_Source, Light_Source_Depth, Light_Source_Ray, Eye_Ray,
          IPoint, Sample_Colour[i], New_u1, New_v1, New_u2, New_v2, Level+1);
      }
    }
  }

  /* Add up the light contributions */

  Make_Colour (Light_Colour, 0.0, 0.0, 0.0);

  for (i = 0; i < 4; i++)
  {
    Scale_Colour (Sample_Colour[i], Sample_Colour[i], 0.25);

    Add_Colour (Light_Colour, Light_Colour, Sample_Colour[i]);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   do_light
*
* INPUT
*
*   Light_Source       - Light source
*   Light_Source_Depth - Distance from surface to light source
*   Light_Source_Ray   - Ray from surface to light source
*   Eye_Ray            - Current viewing ray
*   IPoint             - Intersection point in surface
*   Colour             - Light's colour
*
* OUTPUT
*
*   Light_Source_Depth, Light_Source_Ray, Colour
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   The viewing ray is used to initialize the ray containers of the
*   light source ray.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void do_light(Light_Source, Light_Source_Depth, Light_Source_Ray, Eye_Ray, IPoint, Light_Colour)
LIGHT_SOURCE *Light_Source;
DBL *Light_Source_Depth;
RAY *Light_Source_Ray, *Eye_Ray;
VECTOR IPoint;
COLOUR Light_Colour;
{
  DBL Attenuation;

  /* Get the light source colour. */

  Assign_Colour(Light_Colour, Light_Source->Colour);

  /*
   * Get the light ray starting at the intersection point and pointing
   * towards the light source.
   */

  Assign_Vector(Light_Source_Ray->Initial, IPoint);

  VSub(Light_Source_Ray->Direction,Light_Source->Center, IPoint);

  VLength(*Light_Source_Depth, Light_Source_Ray->Direction);

  VInverseScaleEq(Light_Source_Ray->Direction, *Light_Source_Depth);

  /* Attenuate light source color. */

  Attenuation = Attenuate_Light(Light_Source, Light_Source_Ray, *Light_Source_Depth);

  /* Now scale the color by the attenuation */

  VScaleEq(Light_Colour, Attenuation);

  /* Init ray containers. */

  Initialize_Ray_Containers(Light_Source_Ray);

  Copy_Ray_Containers(Light_Source_Ray, Eye_Ray);
}



/*****************************************************************************
*
* FUNCTION
*
*   do_diffuse
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Calculate the diffuse color component I_d given by:
*
*     I_d = a * d * I * C * (N . L) ^ b
*
*   where d : surface's diffuse reflection coefficient
*         b : surface's brilliance
*         C : surface's color
*         N : surface's normal vector
*         L : light vector (pointing at the light)
*         I : intensity of the incoming light
*         a : attenuation factor
*
* CHANGES
*
*   -
*
******************************************************************************/

static void do_diffuse(Finish, Light_Source_Ray, Layer_Normal, Colour, Light_Colour, Layer_Pigment_Colour, Attenuation)
FINISH *Finish;
RAY *Light_Source_Ray;
VECTOR Layer_Normal;
COLOUR Colour, Light_Colour, Layer_Pigment_Colour;
DBL Attenuation;
{
  DBL Cos_Angle_Of_Incidence, Intensity;

  VDot(Cos_Angle_Of_Incidence, Layer_Normal, Light_Source_Ray->Direction);

  /* Brilliance is likely to be 1.0 (default value) */

  if (Finish->Brilliance != 1.0)
  {
    Intensity = pow(fabs(Cos_Angle_Of_Incidence), Finish->Brilliance);
  }
  else
  {
    Intensity = fabs(Cos_Angle_Of_Incidence);
  }

  Intensity *= Finish->Diffuse * Attenuation;

  if (Finish->Crand > 0.0)
  {
    Intensity -= FRAND() * Finish->Crand;
  }

  Colour[RED]   += Intensity * Layer_Pigment_Colour[RED]   * Light_Colour[RED];
  Colour[GREEN] += Intensity * Layer_Pigment_Colour[GREEN] * Light_Colour[GREEN];
  Colour[BLUE]  += Intensity * Layer_Pigment_Colour[BLUE]  * Light_Colour[BLUE];
}



/*****************************************************************************
*
* FUNCTION
*
*   do_irid
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dan Farmer
*
* DESCRIPTION
*
*   IRIDESCENCE:
*   -----------
*   Programmed by Dan Farmer.
*
*   Based on Chapter 10.2.4 of Three-Dimensional Computer Graphics
*   by Alan Watt.
*
*   Modulates the diffuse coefficients as a function of wavelength, the angle
*   between the light direction vector, and the surface normal.  It models
*   thin-film interference, as in a soap bubble or oilslick.
*
*   Wavelength at which cancellation offurs is a function of the refractive
*   index of the film, its thickness, and the angle of incidence of the
*   incoming light.  In this implementation, IOR is kept constant, while the
*   thickness of the film is specified, as well as being modulated with a
*   turbulence function.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void do_irid(Finish, Light_Source_Ray, Layer_Normal, IPoint, Colour)
FINISH *Finish;
RAY *Light_Source_Ray;
VECTOR Layer_Normal, IPoint;
COLOUR Colour;
{
  DBL rwl, gwl, bwl;
  DBL Cos_Angle_Of_Incidence, interference;
  DBL film_thickness;
  DBL noise, intensity;
  TURB Turb;

  film_thickness = Finish->Irid_Film_Thickness;

  if (Finish->Irid_Turb != 0)
  {
    /* Uses hardcoded octaves, lambda, omega */
    Turb.Omega=0.5;
    Turb.Lambda=2.0;
    Turb.Octaves=5;

    noise = Turbulence(IPoint, &Turb) * Finish->Irid_Turb;

    film_thickness *= noise;
  }

  /*
   * Approximate dominant wavelengths of primary hues.
   * Source: 3D Computer Graphics by John Vince (Addison Wesely)
   * These are initialized in parse.c (Parse_Frame)
   * and are user-adjustable with the irid_wavelength keyword.
   * Red = 700 nm  Grn = 520 nm Blu = 480 nm
   * Divided by 100 gives: rwl = 0.70;  gwl = 0.52;  bwl = 0.48;
   *
   * However... I originally "guessed" at the values and came up with
   * the following, which I'm using as the defaults, since it seems
   * to work better:  rwl = 0.25;  gwl = 0.18;  bwl = 0.14;
   */

  /* Could avoid these assignments if we want to */

  rwl = Frame.Irid_Wavelengths[RED];
  gwl = Frame.Irid_Wavelengths[GREEN];
  bwl = Frame.Irid_Wavelengths[BLUE];

  /* NOTE: Shouldn't we compute Cos_Angle_Of_Incidence just once? */

  VDot(Cos_Angle_Of_Incidence, Layer_Normal, Light_Source_Ray->Direction);

  /* Calculate phase offset. */

  interference = 4.0 * M_PI * film_thickness * Cos_Angle_Of_Incidence;

  intensity = Cos_Angle_Of_Incidence * Finish->Irid;

  /* Modify color by phase offset for each wavelength. */

  Colour[RED]  += Finish->Irid * (intensity * (1.0 - 0.5 * cos(interference/rwl)));
  Colour[GREEN]+= Finish->Irid * (intensity * (1.0 - 0.5 * cos(interference/gwl)));
  Colour[BLUE] += Finish->Irid * (intensity * (1.0 - 0.5 * cos(interference/bwl)));
}



/*****************************************************************************
*
* FUNCTION
*
*   do_phong
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Calculate the phong reflected color component I_p given by:
*
*     I_p = p * C * (R . L) ^ s
*
*   where p : surface's phong reflection coefficient
*         s : surface's phong size
*         C : surface's color/light color depending on the metallic flag
*         R : reflection vector
*         L : light vector (pointing at the light)
*
*   The reflection vector is calculated from the surface normal and
*   the viewing vector (looking at the surface point):
*
*     R = -2 * (V . N) * N + V, with R . R = 1
*
* CHANGES
*
*   Sep 1994 : Added improved color calculation for metallic surfaces. [DB]
*
******************************************************************************/

static void do_phong(Finish, Light_Source_Ray, Eye, Layer_Normal, Colour, Light_Colour, Layer_Pigment_Colour)
FINISH *Finish;
RAY *Light_Source_Ray;
VECTOR Layer_Normal, Eye;
COLOUR Colour, Light_Colour, Layer_Pigment_Colour;
{
  DBL Cos_Angle_Of_Incidence, Intensity;
  VECTOR Reflect_Direction;
  DBL NdotL, x, F;
  COLOUR Cs;

  VDot(Cos_Angle_Of_Incidence, Eye, Layer_Normal);

  Cos_Angle_Of_Incidence *= -2.0;

  VLinComb2(Reflect_Direction, 1.0, Eye, Cos_Angle_Of_Incidence, Layer_Normal);

  VDot(Cos_Angle_Of_Incidence, Reflect_Direction, Light_Source_Ray->Direction);

  if (Cos_Angle_Of_Incidence > 0.0)
  {
    Intensity = Finish->Phong * pow(Cos_Angle_Of_Incidence, Finish->Phong_Size);

    if (Finish->Metallic > 0.0)
    {
      /*
       * Calculate the reflected color by interpolating between
       * the light source color and the surface color according
       * to the (empirical) Fresnel reflectivity function. [DB 9/94]
       */

      VDot(NdotL, Layer_Normal, Light_Source_Ray->Direction);

      x = fabs(acos(NdotL)) / M_PI_2;

      F = 0.014567225 / Sqr(x - 1.12) - 0.011612903;

      Cs[RED]   = Light_Colour[RED]   * (1.0 + Finish->Metallic * (1.0 - F) * (Layer_Pigment_Colour[RED]   - 1.0));
      Cs[GREEN] = Light_Colour[GREEN] * (1.0 + Finish->Metallic * (1.0 - F) * (Layer_Pigment_Colour[GREEN] - 1.0));
      Cs[BLUE]  = Light_Colour[BLUE]  * (1.0 + Finish->Metallic * (1.0 - F) * (Layer_Pigment_Colour[BLUE]  - 1.0));

      VAddScaledEq(Colour, Intensity, Cs);
    }
    else
    {
      Colour[RED]   += Intensity * Light_Colour[RED];
      Colour[GREEN] += Intensity * Light_Colour[GREEN];
      Colour[BLUE]  += Intensity * Light_Colour[BLUE];
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   do_specular
*
* INPUT
*
* OUTPUT
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Calculate the specular reflected color component I_s given by:
*
*     I_s = s * C * (H . N) ^ (1 / r)
*
*   where s : surface's specular reflection coefficient
*         r : surface's roughness
*         C : surface's color/light color depending on the metallic flag
*         N : surface's normal
*         H : bisection vector between V and L
*
*   The bisecting vector H is calculated by
*
*     H = (L - V) / sqrt((L - V).(L - V))
*
* CHANGES
*
*   Sep 1994 : Added improved color calculation for metallic surfaces. [DB]
*
******************************************************************************/

static void do_specular(Finish, Light_Source_Ray, REye, Layer_Normal, Colour, Light_Colour, Layer_Pigment_Colour)
FINISH *Finish;
RAY *Light_Source_Ray;
VECTOR Layer_Normal, REye;
COLOUR Colour, Light_Colour, Layer_Pigment_Colour;
{
  DBL Cos_Angle_Of_Incidence, Intensity, Halfway_Length;
  VECTOR Halfway;
  DBL NdotL, x, F;
  COLOUR Cs;

  VHalf(Halfway, REye, Light_Source_Ray->Direction);

  VLength(Halfway_Length, Halfway);

  if (Halfway_Length > 0.0)
  {
    VDot(Cos_Angle_Of_Incidence, Halfway, Layer_Normal);

    Cos_Angle_Of_Incidence /= Halfway_Length;

    if (Cos_Angle_Of_Incidence > 0.0)
    {
      Intensity = Finish->Specular * pow(Cos_Angle_Of_Incidence, Finish->Roughness);

      if (Finish->Metallic > 0.0)
      {
        /*
         * Calculate the reflected color by interpolating between
         * the light source color and the surface color according
         * to the (empirical) Fresnel reflectivity function. [DB 9/94]
         */

        VDot(NdotL, Layer_Normal, Light_Source_Ray->Direction);

        x = fabs(acos(NdotL)) / M_PI_2;

        F = 0.014567225 / Sqr(x - 1.12) - 0.011612903;

        Cs[RED]   = Light_Colour[RED]   * (1.0 + Finish->Metallic * (1.0 - F) * (Layer_Pigment_Colour[RED]   - 1.0));
        Cs[GREEN] = Light_Colour[GREEN] * (1.0 + Finish->Metallic * (1.0 - F) * (Layer_Pigment_Colour[GREEN] - 1.0));
        Cs[BLUE]  = Light_Colour[BLUE]  * (1.0 + Finish->Metallic * (1.0 - F) * (Layer_Pigment_Colour[BLUE]  - 1.0));

        VAddScaledEq(Colour, Intensity, Cs);
      }
      else
      {
        Colour[RED]   += Intensity * Light_Colour[RED];
        Colour[GREEN] += Intensity * Light_Colour[GREEN];
        Colour[BLUE]  += Intensity * Light_Colour[BLUE];
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Diffuse
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static void Diffuse (Finish, IPoint, Eye, Layer_Normal, Layer_Pigment_Colour, Colour, Attenuation, Object)
FINISH *Finish;
VECTOR IPoint, Layer_Normal;
COLOUR Layer_Pigment_Colour;
COLOUR Colour;
RAY    *Eye;
DBL    Attenuation;
OBJECT *Object;
{
  int i;
  DBL Light_Source_Depth, Cos_Shadow_Angle;
  RAY Light_Source_Ray;
  LIGHT_SOURCE *Light_Source;
  VECTOR REye;
  COLOUR Light_Colour;

  if ((Finish->Diffuse == 0.0) && (Finish->Specular == 0.0) && (Finish->Phong == 0.0))
  {
    return;
  }

  if (Finish->Specular != 0.0)
  {
    REye[X] = -Eye->Direction[X];
    REye[Y] = -Eye->Direction[Y];
    REye[Z] = -Eye->Direction[Z];
  }

  for (i = 0, Light_Source = Frame.Light_Sources;
       Light_Source != NULL;
       Light_Source = Light_Source->Next_Light_Source, i++)
  {
    /* Get a colour and a ray. */

    do_light(Light_Source, &Light_Source_Depth, &Light_Source_Ray, Eye, IPoint, Light_Colour);

    /* Don't calculate spotlights when outside of the light's cone. */

    if ((fabs(Light_Colour[RED])   < BLACK_LEVEL) &&
        (fabs(Light_Colour[GREEN]) < BLACK_LEVEL) &&
        (fabs(Light_Colour[BLUE])  < BLACK_LEVEL))
    {
      continue;
    }

    /* See if light on far side of surface from camera. */

    if (!(Object->Type & DOUBLE_ILLUMINATE))
    {
      VDot(Cos_Shadow_Angle, Layer_Normal, Light_Source_Ray.Direction);

      if (Cos_Shadow_Angle < EPSILON)
      {
        continue;
      }
    }

    /*
     * If light source was not blocked by any intervening object, then
     * calculate it's contribution to the object's overall illumination.
     */

    if ((opts.Quality_Flags & Q_SHADOW) && (Light_Source->Light_Type != FILL_LIGHT_SOURCE))
    {
      /* If this surface point has already been tested use previous result. */

      if (Light_List[i].Tested)
      {
        Assign_Colour(Light_Colour, Light_List[i].Colour);
      }
      else
      {
        block_light_source(Light_Source, Light_Source_Depth, &Light_Source_Ray, Eye, IPoint, Light_Colour);

        /* Store light colour. */

        Light_List[i].Tested = TRUE;

        Assign_Colour(Light_List[i].Colour, Light_Colour);
      }
    }

    if ((fabs(Light_Colour[RED])   > BLACK_LEVEL) ||
        (fabs(Light_Colour[GREEN]) > BLACK_LEVEL) ||
        (fabs(Light_Colour[BLUE])  > BLACK_LEVEL))
    {
      if (Finish->Diffuse > 0.0)
      {
        do_diffuse(Finish,&Light_Source_Ray,Layer_Normal,Colour,Light_Colour,Layer_Pigment_Colour, Attenuation);
      }

      if (Light_Source->Light_Type!=FILL_LIGHT_SOURCE)
      {
        if (Finish->Phong > 0.0)
        {
          do_phong(Finish,&Light_Source_Ray,Eye->Direction,Layer_Normal,Colour,Light_Colour, Layer_Pigment_Colour);
        }

        if (Finish->Specular > 0.0)
        {
          do_specular(Finish,&Light_Source_Ray,REye,Layer_Normal,Colour,Light_Colour, Layer_Pigment_Colour);
        }
      }

      if (Finish->Irid > 0.0)
      {
        do_irid(Finish,&Light_Source_Ray,Layer_Normal,IPoint,Colour);
      }

    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Reflect
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static void Reflect (Reflection, IPoint, Ray, Layer_Normal, Colour, Weight)
RGB Reflection;
VECTOR IPoint;
RAY *Ray;
VECTOR Layer_Normal;
COLOUR Colour;
DBL Weight;
{
  RAY New_Ray;
  COLOUR Temp_Colour;
  register DBL Normal_Component;

  Increase_Counter(stats[Reflected_Rays_Traced]);

  VDot(Normal_Component, Ray->Direction, Layer_Normal);

  Normal_Component *= -2.0;

  VLinComb2(New_Ray.Direction, Normal_Component, Layer_Normal, 1.0, Ray->Direction);

  Assign_Vector(New_Ray.Initial, IPoint);

  Copy_Ray_Containers (&New_Ray, Ray);

  /* Trace reflected ray. */

  Trace_Level++;

  Make_Colour (Temp_Colour, 0.0, 0.0, 0.0);

  Trace (&New_Ray, Temp_Colour, Weight);

  Trace_Level--;

  Colour[RED]   += Reflection[RED]   * Temp_Colour[RED];
  Colour[GREEN] += Reflection[GREEN] * Temp_Colour[GREEN];
  Colour[BLUE]  += Reflection[BLUE]  * Temp_Colour[BLUE];
}



/*****************************************************************************
*
* FUNCTION
*
*   Refract
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 1995 : Modified to correctly handle the contained texture
*              list in the transmitonly case. [DB]
*
******************************************************************************/

static void Refract (Object, Texture, IPoint, Ray, Top_Normal, Colour, Weight)
OBJECT *Object;
TEXTURE *Texture;
VECTOR IPoint;
RAY *Ray;
VECTOR Top_Normal;
COLOUR Colour;
DBL Weight;
{
  int texture_nr;
  register DBL Normal_Component, Temp_IOR;
  DBL temp, ior;
  VECTOR Local_Normal;
  RGB Reflection;
  COLOUR Temp_Colour;
  RAY New_Ray;

  /* Do we have to bend the ray? */

  if (Top_Normal == NULL)
  {
    /* Only transmit the ray. */

    Assign_Vector(New_Ray.Initial, IPoint);
    Assign_Vector(New_Ray.Direction, Ray->Direction);

    Copy_Ray_Containers (&New_Ray, Ray);

    /* Handle contained textures. */

    if (Ray->Containing_Index == -1)
    {
      /* The ray is entering from the atmosphere */

      Ray_Enter (&New_Ray, Texture, Object);
    }
    else
    {
      /* The ray is currently inside an object */

      if ((texture_nr = Texture_In_Ray_Container(&New_Ray, Texture)) >= 0)
      {
        /* The ray is leaving the current object */

        Ray_Exit (&New_Ray, texture_nr);
      }
      else
      {
        /* The ray is entering a new object */

        Ray_Enter (&New_Ray, Texture, Object);
      }
    }

    /* Trace transmitted ray. */

    Trace_Level++;

    Increase_Counter(stats[Transmitted_Rays_Traced]);

    Make_Colour (Temp_Colour, 0.0, 0.0, 0.0);

    Trace (&New_Ray, Temp_Colour, Weight);

    Trace_Level--;

    VAddEq(Colour, Temp_Colour);
  }
  else
  {
    /* Refract the ray. */

    Increase_Counter(stats[Refracted_Rays_Traced]);

    VDot (Normal_Component, Ray->Direction, Top_Normal);

    if (Normal_Component <= 0.0)
    {
      Assign_Vector(Local_Normal, Top_Normal);

      Normal_Component *= -1.0;
    }
    else
    {
      VScale (Local_Normal, Top_Normal, -1.0);
    }

    Copy_Ray_Containers (&New_Ray, Ray);

    /* Handle contained textures. */

    if (Ray->Containing_Index == -1)
    {
      /* The ray is entering from the atmosphere */

      Ray_Enter (&New_Ray, Texture, Object);

      ior = Frame.Atmosphere_IOR / Texture->Finish->Index_Of_Refraction;
    }
    else
    {
      /* The ray is currently inside an object */

      if ((texture_nr = Texture_In_Ray_Container(&New_Ray, Texture)) >= 0)
      {
        /* The ray is leaving the current object */

        Ray_Exit (&New_Ray, texture_nr);

        if (New_Ray.Containing_Index == -1)
        {
          /* The ray is leaving into the atmosphere */

          Temp_IOR = Frame.Atmosphere_IOR;
        }
        else
        {
          /* The ray is leaving into another object */

          Temp_IOR = New_Ray.Containing_IORs[New_Ray.Containing_Index];
        }

        ior = Texture->Finish->Index_Of_Refraction / Temp_IOR;
      }
      else
      {
        /* The ray is entering a new object */

        Temp_IOR = New_Ray.Containing_IORs[New_Ray.Containing_Index];

        Ray_Enter (&New_Ray, Texture, Object);

        ior = Temp_IOR / Texture->Finish->Index_Of_Refraction;
      }
    }

    /* Compute refrated ray direction using Heckbert's method. */

    temp = 1.0 + Sqr(ior) * (Sqr(Normal_Component) - 1.0);

    if (temp < 0.0)
    {
      /* Total internal reflection occures. */

      Reflection[RED]  = 1.0 - Texture->Finish->Reflection[RED];
      Reflection[GREEN]= 1.0 - Texture->Finish->Reflection[GREEN];
      Reflection[BLUE] = 1.0 - Texture->Finish->Reflection[BLUE];

      Reflect (Reflection, IPoint, Ray, Top_Normal, Colour, Weight);

      return;
    }

    temp = ior * Normal_Component - sqrt(temp);

    VLinComb2(New_Ray.Direction, ior, Ray->Direction, temp, Local_Normal);

    Assign_Vector(New_Ray.Initial,IPoint);

    /* Trace refracted ray. */

    Trace_Level++;

    Make_Colour (Temp_Colour, 0.0, 0.0, 0.0);

    Trace (&New_Ray, Temp_Colour, Weight);

    Trace_Level--;

    VAddScaledEq(Colour, Texture->Finish->Refraction, Temp_Colour);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   create_texture_list
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Chris Young based on Dieter Bayer code
*   
* DESCRIPTION
*
*   Get the list of textures used by current object and the list of
*   appropriate weights for each texture. Only multi-colored objects
*   will have more than one texture.
*
* CHANGES
*
*   Feb 1995 : Added code for triangle mesh texturing. [DB]
*
*   Jul 1995 : Modified code to use pre-allocated lists. [DB]
*
******************************************************************************/

static int create_texture_list(Ray_Intersection)
INTERSECTION *Ray_Intersection;
{
  int Texture_Count;
  BLOB *Blob;
  MESH_TRIANGLE *Triangle;

  /* Test, if object is multi-textured. */

  if (Test_Flag(Ray_Intersection->Object, MULTITEXTURE_FLAG))
  {
    /* Handle blobs. */

    if (Ray_Intersection->Object->Methods == &Blob_Methods)
    {
      Blob = (BLOB *)Ray_Intersection->Object;

      /* Get list of weighted textures. */

      Determine_Blob_Textures(Blob, Ray_Intersection->IPoint, &Texture_Count, Texture_List, Weight_List);
    }

    /* Handle meshes. */

    if (Ray_Intersection->Object->Methods == &Mesh_Methods)
    {
      /* Set texture to triangle's or object's texture. */

      Triangle = (MESH_TRIANGLE *)Ray_Intersection->Pointer;

      if (Triangle->Texture >= 0)
      {
        Texture_List[0] = ((MESH *)Ray_Intersection->Object)->Data->Textures[Triangle->Texture];
      }
      else
      {
        Texture_List[0] = Ray_Intersection->Object->Texture;
      }

      Weight_List[0] = 1.0;

      Texture_Count = 1;
    }
  }
  else
  {
    /* Set texture to object's texture. */

    Texture_List[0] = Ray_Intersection->Object->Texture;
    Weight_List[0]  = 1.0;

    Texture_Count = 1;
  }

  return(Texture_Count);
}



/*****************************************************************************
*
* FUNCTION
*
*   do_texture_map
*
* INPUT
*
*   Texture          - possibly texture_mapped texture to be evaluated
*   IPoint           - point to be evaluated
*   Raw_Normal       - non-purturbed surface normal
*   Ray              - view ray needed for reflection and highlighs
*                      light source ray needed for caustics
*   Weight           - ADC control value
*   Ray_Intersection - only Ray_Int..->Object->Type actually
*                      needed.  Will clean-up later.
*   Shadow_Flag      - tells if computation should use
*                      compute_lighted_texture or compute_shadow_texture
*
* OUTPUT
*
*   Result_Colour    - If Shadow_Flag true then the illuminated
*                      color (RGB only) of IPoint is returned.
*                      If false, the amount by which a shadow ray is
*                      filtered and attenuated is returned.
*                      Includes RGB and T.
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   This routine recursively calls itself until it gets a
*   non-texture_mapped texture that is potentially layered.
*   It then calls compute_lighted_texture or compute_shadow_texture
*   to compute the color which is returned in the argument Result_Colour.
*
* CHANGES
*
******************************************************************************/

static void do_texture_map(Result_Colour, Texture, IPoint, Raw_Normal,
  Ray, Weight, Ray_Intersection, Shadow_Flag)
COLOUR Result_Colour;
TEXTURE *Texture;
VECTOR IPoint, Raw_Normal;
RAY *Ray;
DBL Weight;
INTERSECTION *Ray_Intersection;
int Shadow_Flag;
{
  BLEND_MAP *Blend_Map = Texture->Blend_Map;
  BLEND_MAP_ENTRY *Prev, *Cur;
  DBL value1, value2;
  COLOUR C2;
  VECTOR TPoint;

  if (Texture->Type <= LAST_SPECIAL_PATTERN)
  {
    switch (Texture->Type)
    {
      case NO_PATTERN:

        Make_ColourA(Result_Colour, 1.0, 1.0, 1.0, 1.0, 1.0);

        break;

      case AVERAGE_PATTERN:

        Warp_EPoint(TPoint, IPoint, (TPATTERN *)Texture);

        average_textures(Result_Colour, Texture, TPoint, Raw_Normal, Ray, Weight, Ray_Intersection, Shadow_Flag);

        break;

      case BITMAP_PATTERN:

        Warp_EPoint (TPoint, IPoint, (TPATTERN *)Texture);

        Texture = material_map(TPoint, Texture);

        do_texture_map(Result_Colour, Texture, TPoint, Raw_Normal, Ray, Weight, Ray_Intersection, Shadow_Flag);

        break;

      case PLAIN_PATTERN:

        if (Shadow_Flag)
        {
          compute_shadow_texture(Result_Colour, Texture, IPoint, Raw_Normal, Ray, Ray_Intersection);
        }
        else
        {
          compute_lighted_texture(Result_Colour, Texture, IPoint, Raw_Normal, Ray, Weight, Ray_Intersection);
        }

        break;

      default:

        Error("Bad texture type in do_texture_map()\n");
    }
  }
  else
  {
    value1 = Evaluate_TPat ((TPATTERN *)Texture,IPoint);

    Search_Blend_Map (value1, Blend_Map, &Prev, &Cur);

    Warp_EPoint (TPoint, IPoint, (TPATTERN *)Texture);

    do_texture_map(Result_Colour, Cur->Vals.Texture, TPoint, Raw_Normal, Ray, Weight, Ray_Intersection, Shadow_Flag);

    if (Prev != Cur)
    {
      do_texture_map(C2, Prev->Vals.Texture, TPoint, Raw_Normal, Ray, Weight, Ray_Intersection, Shadow_Flag);

      value1 = (value1 - Prev->value) / (Cur->value - Prev->value);
      value2 = 1.0 - value1;

      CLinComb2(Result_Colour,value1,Result_Colour,value2,C2);
    }
  }
}




/*****************************************************************************
*
* FUNCTION
*
*   compute_lighted_texture
*
* INPUT
*
*   Texture          - a linked list of texture layers
*   IPoint           - point to be evaluated
*   Raw_Normal       - non-purturbed surface normal
*   Ray              - needed for reflection and highlighs
*   Weight           - ADC control value
*   Ray_Intersection - current intersection (need object type and depth)
*
* OUTPUT
*
*   Result_Colour    - illuminated color of IPoint
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   This routine loops through all layers of a texture and computes
*   the appearent color of the point with illumination, shadows,
*   reflection, refraction... everything.  This piece of code was broken out
*   of Determine_Appearent_Colour because texture_map needs to call it twice.
*
* CHANGES
*
*   Jul 1995 : Added code to support alpha channel. [DB]
*
*   Jul 1995 : Moved code for save list allocation. [DB]
*
*   Aug 1995 : Added code for distance based attenuation in translucent
*              objects and halos. [DB]
*
******************************************************************************/

static void compute_lighted_texture(Result_Colour, Texture, IPoint, Raw_Normal, Ray, Weight, Ray_Intersection)
COLOUR Result_Colour;
TEXTURE *Texture;
VECTOR IPoint, Raw_Normal;
RAY *Ray;
DBL Weight;
INTERSECTION *Ray_Intersection;
{
  int i, radiosity_done, radiosity_needed;
  int layer_number;
  int calc_halo;
  int one_colour_found, colour_found;
  DBL w1, w2;
  DBL Normal_Direction, New_Weight, Temp_Weight;
  DBL Attenuation, Transparency, Max_Radiosity_Contribution;
  VECTOR Layer_Normal, Top_Normal;
  COLOUR Layer_Pigment_Colour, Refracted_Colour, Filter_Colour;
  COLOUR Temp_Colour, Gathered_Ambient_Colour, Temp;
  FINISH *Finish;
  HALO *Halo;
  TEXTURE *Layer;

  /*
   * Result_Colour builds up the apparent visible color of the point.
   * Only RGB components are significant.  You can't "see" transparency --
   * you see the color of whatever is behind the transparent surface.
   * This color includes the visible appearence of what is behind the
   * transparency so only RGB is needed.
   */

  Make_ColourA(Result_Colour, 0.0, 0.0, 0.0, 0.0, 0.0);

  /*
   * Filter_Colour serves two purposes.  It accumulates the filter properties
   * of a multi-layer texture so that if a ray makes it all the way through
   * all layers, the color of object behind is filtered by this object.
   * It also is used to attenuate how much of an underlayer you
   * can see in a layered texture.  Note that when computing the reflective
   * properties of a layered texture, the upper layers don't filter the
   * light from the lower layers -- the layer colors add together (even
   * before we added additive transparency via the "transmit" 5th
   * color channel).  However when computing the transmitted rays, all layers
   * filter the light from any objects behind this object. [CY 1/95]
   */

  Make_ColourA(Filter_Colour, 1.0, 1.0, 1.0, 1.0, 1.0);

  Transparency = 1.0;

  /* add in radiosity (stochastic interreflection-based ambient light) if desired */

  radiosity_done = FALSE;

  /* Note that there is no gathering of filter or transparency */

  Make_ColourA(Gathered_Ambient_Colour, 1., 1., 1., 0., 0.);

  if ((opts.Options & RADIOSITY) &&
      (Trace_Level == Radiosity_Trace_Level) &&
      (Radiosity_Trace_Level <= opts.Radiosity_Recursion_Limit))
  {
    /*
     * For "real" (physically-based) diffuse interreflections, the
     * ambient light level is independent of any surface properties, so
     * the light gathering is done only once.  This block just sets up
     * for the code inside the loop, which is first-time-through.
     */

    /* If the surface normal points away, flip its direction. */

    VDot(Normal_Direction, Raw_Normal, Ray->Direction);

    if (Normal_Direction > 0.0)
    {
      VScaleEq(Raw_Normal, -1.0);
    }

    radiosity_needed = 1;
  }
  else
  {
    radiosity_needed = 0;
  }

  /*
   * Loop through the layers and compute the ambient, diffuse, reflection
   * and highlights from these textures.
   */

  one_colour_found = FALSE;

  for (layer_number = 1, Layer = Texture;
      (Layer != NULL) && (Transparency > BLACK_LEVEL);
      layer_number++, Layer = (TEXTURE *)Layer->Next)
  {
    Assign_Vector(Layer_Normal, Raw_Normal);

    if ((opts.Quality_Flags & Q_NORMAL) && (Layer->Tnormal != NULL))
    {
      Perturb_Normal(Layer_Normal, Layer->Tnormal, IPoint);
    }

    /* If the surface normal points away, flip its direction. */

    VDot(Normal_Direction, Layer_Normal, Ray->Direction);

    if (Normal_Direction > 0.0)
    {
      VScaleEq(Layer_Normal, -1.0);
    }

    /* Store top layer normal.*/

    if (layer_number == 1)
    {
      Assign_Vector(Top_Normal,Layer_Normal);
    }

    /* Get surface colour. */

    New_Weight = Weight * Transparency;

    colour_found = Compute_Pigment (Layer_Pigment_Colour, Layer->Pigment, IPoint);

    /*
     * If a valid color was returned set one_colour_found to TRUE.
     * An invalid color is returned if a surface point is outside
     * an image map used just once.
     */

    if (colour_found)
    {
      one_colour_found = TRUE;
    }

    /*
     * This section of code used to be the routine Compute_Reflected_Colour.
     * I copied it in here to rearrange some of it more easily and to
     * see if we could eliminate passing a zillion parameters for no
     * good reason. [CY 1/95]
     */

    if (opts.Quality_Flags & Q_FULL_AMBIENT)
    {
      /* Only use top layer and kill transparency if low quality */

      Assign_Colour(Result_Colour, Layer_Pigment_Colour);

      Result_Colour[FILTER] =
      Result_Colour[TRANSM] = 0.0;
    }
    else
    {
      Make_Colour (Temp_Colour, 0.0, 0.0, 0.0);

      Attenuation = Transparency * (1.0 - min(1.0, Layer_Pigment_Colour[FILTER] + Layer_Pigment_Colour[TRANSM]));

      /* if radiosity calculation needed, but not yet done, do it now */

      if (radiosity_needed && !radiosity_done)
      {
        /* This check eliminates radiosity calculations on "luminous" objects with ambient=1 */

        if ((Layer->Finish->Ambient[RED]   != 1.0) ||
            (Layer->Finish->Ambient[GREEN] != 1.0) ||
            (Layer->Finish->Ambient[BLUE]  != 1.0))
        {
          /* calculate max possible contribution of radiosity, to see if calculating it is worthwhile */

          Temp[RED]   = Attenuation * Layer_Pigment_Colour[RED] *
                        Layer->Finish->Ambient[RED] *
                        Frame.Ambient_Light[RED];

          Temp[GREEN] = Attenuation * Layer_Pigment_Colour[GREEN] *
                        Layer->Finish->Ambient[GREEN] *
                        Frame.Ambient_Light[GREEN];

          Temp[BLUE]  = Attenuation * Layer_Pigment_Colour[BLUE] *
                        Layer->Finish->Ambient[BLUE] *
                        Frame.Ambient_Light[BLUE];

          Max_Radiosity_Contribution = Temp[RED] *.287 + Temp[GREEN] *.589 + Temp[BLUE] * .114;

          if (Max_Radiosity_Contribution > BLACK_LEVEL * 3.0)
          {
            (void)Compute_Ambient(Ray_Intersection->IPoint, Raw_Normal,
              Gathered_Ambient_Colour, Weight * Max_Radiosity_Contribution);

            radiosity_done = TRUE;
          }
        }
      }

      /* Add ambient contribution. */

      Temp_Colour[RED]   += Attenuation *
                            Layer_Pigment_Colour[RED] *
                            Layer->Finish->Ambient[RED] *
                            Frame.Ambient_Light[RED] *
                            Gathered_Ambient_Colour[RED];

      Temp_Colour[GREEN] += Attenuation *
                            Layer_Pigment_Colour[GREEN] *
                            Layer->Finish->Ambient[GREEN] *
                            Frame.Ambient_Light[GREEN] *
                            Gathered_Ambient_Colour[GREEN];

      Temp_Colour[BLUE]  += Attenuation *
                            Layer_Pigment_Colour[BLUE] *
                            Layer->Finish->Ambient[BLUE] *
                            Frame.Ambient_Light[BLUE] *
                            Gathered_Ambient_Colour[BLUE];


      /* Add diffuse, phong, specular, and iridescence contribution. */

      Diffuse (Layer->Finish, Ray_Intersection->IPoint, Ray,
        Layer_Normal, Layer_Pigment_Colour, Temp_Colour, Attenuation,
        Ray_Intersection->Object);

      VAddEq(Result_Colour, Temp_Colour);

      /* Do reflection. */

      if (opts.Quality_Flags & Q_REFLECT)
      {
        if ((Layer->Finish->Reflection[RED]   != 0.0) ||
            (Layer->Finish->Reflection[GREEN] != 0.0) ||
            (Layer->Finish->Reflection[BLUE]  != 0.0))
        {
          Temp_Weight = New_Weight * max3(Layer->Finish->Reflection[RED], Layer->Finish->Reflection[GREEN], Layer->Finish->Reflection[BLUE]);

          Reflect(Layer->Finish->Reflection, Ray_Intersection->IPoint, Ray,
            Layer_Normal, Result_Colour, Temp_Weight);
        }
      }
    }

    /*
     * End of former Compute_Reflected_Colour code.
     */

    if (colour_found)
    {
      Filter_Colour[RED]    *= Layer_Pigment_Colour[RED];
      Filter_Colour[GREEN]  *= Layer_Pigment_Colour[GREEN];
      Filter_Colour[BLUE]   *= Layer_Pigment_Colour[BLUE];
      Filter_Colour[FILTER] *= Layer_Pigment_Colour[FILTER];
      Filter_Colour[TRANSM] *= Layer_Pigment_Colour[TRANSM];
    }

    Transparency = min(1.0, fabs(Filter_Colour[FILTER]) + fabs(Filter_Colour[TRANSM]));
  }

  /*
   * Now see if any transparency remains.  If it does, then fire a transmitted
   * ray and add it's contribution to the total Result_Colour after
   * filtering it by Filter_Colour.
   */

  Finish = Texture->Finish;

  if ((Transparency > BLACK_LEVEL) && (opts.Quality_Flags & Q_REFRACT))
  {
    Make_Colour(Refracted_Colour, 0.0, 0.0, 0.0);

    w1 = fabs(Filter_Colour[FILTER]) * max3(Filter_Colour[RED], Filter_Colour[GREEN], Filter_Colour[BLUE]);
    w2 = fabs(Filter_Colour[TRANSM]);

    New_Weight = Weight * max(w1, w2);

    /*
     * WARNING: The Refract() routine must be passed the un-transformed
     * IPoint from Ray_Intersection->IPoint so that the Ray->Initial for
     * transmitted rays is properly set.  It should not be a TPoint from
     * some transformed or turbulated texture_map pattern.
     */

    if (Finish->Refraction > 0.0)
    {
      Refract (Ray_Intersection->Object, Texture, Ray_Intersection->IPoint, Ray, Top_Normal, Refracted_Colour, New_Weight);
    }
    else
    {
      Refract (Ray_Intersection->Object, Texture, Ray_Intersection->IPoint, Ray, NULL, Refracted_Colour, New_Weight);
    }

    /* Get distance based attenuation. */

    Attenuation = 1.0;

    if (Ray->Containing_Index > -1)
    {
      /* Attenuate light due to light source distance. */

      if (Texture_In_Ray_Container(Ray, Texture) >= 0)
      {
        if ((Finish->Fade_Power > 0.0) && (fabs(Finish->Fade_Distance) > EPSILON))
        {
          Attenuation = 1.0 / (1.0 + pow(Ray_Intersection->Depth / Finish->Fade_Distance, Finish->Fade_Power));
        }
      }
    }

    if (one_colour_found)
    {
      Result_Colour[RED]  += Attenuation * Refracted_Colour[RED]   *
        (Filter_Colour[RED]   * Filter_Colour[FILTER] + Filter_Colour[TRANSM]);

      Result_Colour[GREEN]+= Attenuation * Refracted_Colour[GREEN] *
        (Filter_Colour[GREEN] * Filter_Colour[FILTER] + Filter_Colour[TRANSM]);

      Result_Colour[BLUE] += Attenuation * Refracted_Colour[BLUE]  *
        (Filter_Colour[BLUE]  * Filter_Colour[FILTER] + Filter_Colour[TRANSM]);
    }
    else
    {
      Result_Colour[RED]  += Attenuation * Refracted_Colour[RED];
      Result_Colour[GREEN]+= Attenuation * Refracted_Colour[GREEN];
      Result_Colour[BLUE] += Attenuation * Refracted_Colour[BLUE];
    }

    /* We need to know the transmittance value for the alpha channel. [DB] */

    Result_Colour[TRANSM] = Attenuation * Filter_Colour[TRANSM];
  }

  /* Calculate halo effects. */

  if ((opts.Quality_Flags & Q_VOLUME) && (Ray->Containing_Index > -1))
  {
    calc_halo = TRUE;

    /* Test for any solid object. */

    for (i = 0; i <= Ray->Containing_Index; i++)
    {
      if (!Test_Flag(Ray->Containing_Objects[i], HOLLOW_FLAG))
      {
        calc_halo = FALSE;

        break;
      }
    }

    /* Calculate effects of all halos we're currently in. */

    if (calc_halo)
    {
      for (i = 0; i <= Ray->Containing_Index; i++)
      {
        if ((Halo = Ray->Containing_Textures[i]->Halo) != NULL)
        {
          if (Halo->Type != HALO_NO_HALO)
          {
            Do_Halo(Halo, Ray, Ray_Intersection, Result_Colour, FALSE);
          }
        }
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   compute_shadow_texture
*
* INPUT
*
*   Texture          - layered texture through which shadow ray passes
*   IPoint           - point through which shadow ray passes
*   Raw_Normal       - non-purturbed surface normal
*   Ray              - light source ray
*   Ray_Intersection - current intersection (need intersection depth)
*
* OUTPUT
*
*   Filter_Colour - returned filter for shadow ray
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
*   Dec 1994 : Separated from filter_shadow_ray to do texture_map [CEY]
*
*   May 1995 : Added caustic code by Steve Anger. [DB]
*
*   Aug 1995 : Caustic code moved here from filter_shadow_ray. [CEY]
*
******************************************************************************/

static void compute_shadow_texture (Filter_Colour, Texture, IPoint, Raw_Normal, Ray, Ray_Intersection)
COLOUR Filter_Colour;
TEXTURE *Texture;
VECTOR IPoint;
VECTOR Raw_Normal;
RAY *Ray;
INTERSECTION *Ray_Intersection;
{
  int i, calc_halo, colour_found, one_colour_found;
  DBL Caustics, dot, k, Refraction;
  VECTOR Layer_Normal;
  COLOUR Layer_Pigment_Colour;
  COLOUR Halo_Colour;
  FINISH *Finish;
  HALO *Halo;
  TEXTURE *Layer;

  Make_ColourA(Filter_Colour, 1.0, 1.0, 1.0, 1.0, 1.0);

  one_colour_found = FALSE;

  for (Layer = Texture; (Layer != NULL) &&
       (fabs(Filter_Colour[FILTER]) + fabs(Filter_Colour[TRANSM]) > BLACK_LEVEL);
       Layer = (TEXTURE *)Layer->Next)
  {
    colour_found = Compute_Pigment (Layer_Pigment_Colour, Layer->Pigment, IPoint);

    if (colour_found)
    {
      one_colour_found = TRUE;
    }

    Refraction = (DBL)((Layer->Finish->Refraction > 0.0) ? Layer->Finish->Refraction : 1.0);

    /* Get distance based attenuation. */

    if (Ray->Containing_Index > -1)
    {
      /* Get finish of texture we're currently in. */

      if ((Finish = Ray->Containing_Textures[Ray->Containing_Index]->Finish)!=NULL)
      {
         /* Attenuate light due to light source distance. */

         if (Texture_In_Ray_Container(Ray, Texture) >= 0)
         {
           if ((Finish->Fade_Power > 0.0) && (fabs(Finish->Fade_Distance) > EPSILON))
           {
             Refraction *= 1.0 / (1.0 + pow(Ray_Intersection->Depth / Finish->Fade_Distance, Finish->Fade_Power));
           }
         }
      }
    }

    if (colour_found)
    {
      Filter_Colour[RED]    *= Refraction * Layer_Pigment_Colour[RED];
      Filter_Colour[GREEN]  *= Refraction * Layer_Pigment_Colour[GREEN];
      Filter_Colour[BLUE]   *= Refraction * Layer_Pigment_Colour[BLUE];
      Filter_Colour[FILTER] *= Refraction * Layer_Pigment_Colour[FILTER];
      Filter_Colour[TRANSM] *= Refraction * Layer_Pigment_Colour[TRANSM];
    }
    else
    {
      Filter_Colour[RED]    *= Refraction;
      Filter_Colour[GREEN]  *= Refraction;
      Filter_Colour[BLUE]   *= Refraction;
      Filter_Colour[FILTER] *= Refraction;
      Filter_Colour[TRANSM] *= Refraction;
    }

    /* Get normal for faked caustics. (Will rewrite later to cache) */

    if ((Caustics = Layer->Finish->Caustics) != 0.0)
    {
      Assign_Vector(Layer_Normal, Raw_Normal);

      if ((opts.Quality_Flags & Q_NORMAL) && (Layer->Tnormal != NULL))
      {
        Perturb_Normal(Layer_Normal, Layer->Tnormal, IPoint);
      }

      /* Get new filter/transmit values. */

      VDot (dot, Layer_Normal, Ray->Direction);

      k = (1.0 + pow(fabs(dot), Caustics));

      Filter_Colour[FILTER] *= k;
      Filter_Colour[TRANSM] *= k;
    }
  }

  /*
   * If no valid color was found we set the filtering channel
   * to zero to make sure that no light amplification occures.
   * That would happen if both the filter and transmit channel
   * were used.
   */

  if (!one_colour_found)
  {
    Filter_Colour[FILTER] = 0.0;
  }

  /* Calculate halo. */

  if ((opts.Quality_Flags & Q_VOLUME) && (Ray->Containing_Index > -1))
  {
    calc_halo = TRUE;

    /* Test for any solid object. */

    for (i = 0; i <= Ray->Containing_Index; i++)
    {
      if (!Test_Flag(Ray->Containing_Objects[i], HOLLOW_FLAG))
      {
        calc_halo = FALSE;

        break;
      }
    }

    /* Calculate effects of all halos we're currently in. */

    if (calc_halo)
    {
      for (i = 0; i <= Ray->Containing_Index; i++)
      {
        if ((Halo = Ray->Containing_Textures[i]->Halo) != NULL)
        {
          if (Halo->Type != HALO_NO_HALO)
          {
            Do_Halo(Halo, Ray, Ray_Intersection, Halo_Colour, TRUE);
          }
        }
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   filter_shadow_ray
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 1994 : Code for early exit due to opaque object added. [DB]
*
*   Sep 1994 : Code for multi-textured blobs added. [DB]
*
*   May 1995 : Added caustic code by Steve Anger. [DB]
*
*   Aug 1995 : Added code to attenuate light source color
*              due to atmospheric effects. [DB]
*
******************************************************************************/

static void filter_shadow_ray(Ray_Intersection, Light_Source_Ray, Colour)
INTERSECTION *Ray_Intersection;
RAY *Light_Source_Ray;
COLOUR Colour;
{
  int i, Texture_Count;
  VECTOR IPoint;
  VECTOR Raw_Normal;
  COLOUR FC1, Temp_Colour;
  TEXTURE *Texture = NULL;  /* To remove uninitialized use warning [AED] */

  Assign_Vector(IPoint, Ray_Intersection->IPoint);

  if (!(opts.Quality_Flags & Q_SHADOW))
  {
    return;
  }

  /* If the object is opaque there's no need to go any further. [DB 8/94] */

  if (Test_Flag(Ray_Intersection->Object, OPAQUE_FLAG))
  {
    Make_Colour(Colour, 0.0, 0.0, 0.0);

    return;
  }

  /* Get the normal to the surface */

  Normal(Raw_Normal, Ray_Intersection->Object, Ray_Intersection);

  /* Get texture list and weights. */

  Texture_Count = create_texture_list(Ray_Intersection);

  Make_ColourA(Temp_Colour, 0.0, 0.0, 0.0, 0.0, 0.0);

  for (i = 0; i < Texture_Count; i++)
  {
    /* If contribution of this texture is neglectable skip ahead. */

    if (Weight_List[i] < BLACK_LEVEL)
    {
      continue;
    }

    Texture = Texture_List[i];

    do_texture_map(FC1, Texture, IPoint, Raw_Normal, Light_Source_Ray, 0.0, Ray_Intersection, TRUE);

    Temp_Colour[RED]     += Weight_List[i] * FC1[RED];
    Temp_Colour[GREEN]   += Weight_List[i] * FC1[GREEN];
    Temp_Colour[BLUE]    += Weight_List[i] * FC1[BLUE];
    Temp_Colour[FILTER]  += Weight_List[i] * FC1[FILTER];
    Temp_Colour[TRANSM]  += Weight_List[i] * FC1[TRANSM];
  }

  if (fabs(Temp_Colour[FILTER]) + fabs(Temp_Colour[TRANSM]) < BLACK_LEVEL)
  {
    Make_Colour(Colour, 0.0, 0.0, 0.0);
  }
  else
  {
    Colour[RED]   *= Temp_Colour[FILTER] * Temp_Colour[RED]  + Temp_Colour[TRANSM];
    Colour[GREEN] *= Temp_Colour[FILTER] * Temp_Colour[GREEN]+ Temp_Colour[TRANSM];
    Colour[BLUE]  *= Temp_Colour[FILTER] * Temp_Colour[BLUE] + Temp_Colour[TRANSM];
  }

  /* Get atmospheric attenuation. */

  do_light_ray_atmosphere(Light_Source_Ray, Ray_Intersection, Texture, Colour, TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
*   do_blocking
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static int do_blocking(Local_Intersection, Light_Source_Ray, Light_Colour, Local_Stack)
INTERSECTION *Local_Intersection;
RAY *Light_Source_Ray;
COLOUR Light_Colour;
ISTACK *Local_Stack;
{
  Increase_Counter(stats[Shadow_Rays_Succeeded]);

  filter_shadow_ray(Local_Intersection, Light_Source_Ray, Light_Colour);

  if ((fabs(Light_Colour[RED])   < BLACK_LEVEL) &&
      (fabs(Light_Colour[GREEN]) < BLACK_LEVEL) &&
      (fabs(Light_Colour[BLUE])  < BLACK_LEVEL))
  {
    while ((Local_Intersection = pop_entry(Local_Stack)) != NULL)
    {
    }

    return(TRUE);
  }

  return(FALSE);
}



/*****************************************************************************
*
* FUNCTION
*
*   block_light_source
*
* INPUT
*
*   Light            - Light source
*   Depth            - Distance to light source
*   Light_Source_Ray - Light ray
*   Eye_Ray          - Ray from eye to current intersection point
*   P                - Surface point to shade
*
* OUTPUT
*
*   Colour           - Light color reaching point P
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Determine how much light from the given light source reaches
*   the given point. This includes attenuation due to blocking
*   and translucent objects and atmospheric effects.
*
* CHANGES
*
*   Jan 1995 : Creation (Extracted from common code).
*
*   Aug 1995 : Added code to support atmospheric effects. [DB]
*
******************************************************************************/

static void block_light_source(Light, Depth, Light_Source_Ray, Eye_Ray, P, Colour)
LIGHT_SOURCE *Light;
DBL Depth;
RAY *Light_Source_Ray, *Eye_Ray;
VECTOR P;
COLOUR Colour;
{
  DBL New_Depth;
  INTERSECTION Intersection;
  RAY New_Ray;

  /* Store current depth and ray because they will be modified. */

  New_Depth = Depth;

  New_Ray = *Light_Source_Ray;

  /* Get shadows from current light source. */

  if ((Light->Area_Light) && (opts.Quality_Flags & Q_AREA_LIGHT))
  {
    block_area_light(Light, &New_Depth, &New_Ray, Eye_Ray, P, Colour, 0, 0, 0, 0, 0);
  }
  else
  {
    if (opts.Options & USE_LIGHT_BUFFER)
    {
      block_point_light_LBuffer(Light, &New_Depth, &New_Ray, Colour);
    }
    else
    {
      block_point_light(Light, &New_Depth, &New_Ray, Colour);
    }
  }

  /*
   * If there's some distance left for the ray to reach the light source
   * we have to apply atmospheric stuff to this part of the ray.
   */

  if ((New_Depth > SHADOW_TOLERANCE) &&
      (Light->Atmosphere_Interaction) &&
      (Light->Atmospheric_Attenuation))
  {
    Intersection.Depth = New_Depth;

    do_light_ray_atmosphere(&New_Ray, &Intersection, NULL, Colour, FALSE);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   average_textures
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static void average_textures (Result_Colour, Texture, IPoint, Raw_Normal,
  Ray, Weight, Ray_Intersection, Shadow_Flag)
COLOUR Result_Colour;
TEXTURE *Texture;
VECTOR IPoint, Raw_Normal;
RAY *Ray;
DBL Weight;
INTERSECTION *Ray_Intersection;
int Shadow_Flag;
{
   int i;
   COLOUR LC;
   BLEND_MAP *Map = Texture->Blend_Map;
   SNGL Value;
   SNGL Total = 0.0;

   Make_Colour (Result_Colour, 0.0, 0.0, 0.0);

   for (i = 0; i < Map->Number_Of_Entries; i++)
   {
     Value = Map->Blend_Map_Entries[i].value;

     do_texture_map (LC,Map->Blend_Map_Entries[i].Vals.Texture, IPoint,Raw_Normal,Ray,Weight,Ray_Intersection,Shadow_Flag);

     Result_Colour[RED]   += LC[RED]   *Value;
     Result_Colour[GREEN] += LC[GREEN] *Value;
     Result_Colour[BLUE]  += LC[BLUE]  *Value;
     Result_Colour[FILTER]+= LC[FILTER]*Value;
     Result_Colour[TRANSM]+= LC[TRANSM]*Value;

     Total += Value;
   }

   Result_Colour[RED]   /= Total;
   Result_Colour[GREEN] /= Total;
   Result_Colour[BLUE]  /= Total;
   Result_Colour[FILTER]/= Total;
   Result_Colour[TRANSM]/= Total;
}



/*****************************************************************************
*
* FUNCTION
*
*   do_light_ray_atmosphere
*
* INPUT
*
*   Light_Source_Ray - Current ray towards light source
*   Ray_Intersection - Current intersection with a blocking object
*   Texture          - Current PNFH texture
*   Valid_Object     - Flag: 1=a valid object is in the intersection struct
*
* OUTPUT
*
*   Colour           - Attenuated light source color
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Determine the influence of atmospheric effects on a light source ray.
*
* CHANGES
*
*   Aug 1995 : Creation.
*
******************************************************************************/

static void do_light_ray_atmosphere(Light_Source_Ray, Ray_Intersection, Texture, Colour, Valid_Object)
RAY *Light_Source_Ray;
INTERSECTION *Ray_Intersection;
TEXTURE *Texture;
COLOUR Colour;
int Valid_Object;
{
  int texture_nr;
  int i, all_hollow;

  /* Why are we here? */

  if ((Colour[RED] < BLACK_LEVEL) && (Colour[GREEN] < BLACK_LEVEL) && (Colour[BLUE] < BLACK_LEVEL))
  {
    return;
  }

  all_hollow = TRUE;

  for (i = 0; i <= Light_Source_Ray->Containing_Index; i++)
  {
    if (!Test_Flag(Light_Source_Ray->Containing_Objects[i], HOLLOW_FLAG))
    {
      all_hollow = FALSE;

      break;
    }
  }

  /* Apply atmospheric effects inside and/or outside any object. */

  if (all_hollow || (Valid_Object && Test_Flag(Ray_Intersection->Object, HOLLOW_FLAG)))
  {
    Do_Finite_Atmosphere(Light_Source_Ray, Ray_Intersection, Colour, TRUE);
  }

  /* Handle contained textures. */

  if (Valid_Object)
  {
    if (Light_Source_Ray->Containing_Index == -1)
    {
      /* The ray is entering from the atmosphere */

      Ray_Enter(Light_Source_Ray, Texture, Ray_Intersection->Object);
    }
    else
    {
      /* The ray is currently inside an object */

      if ((texture_nr = Texture_In_Ray_Container(Light_Source_Ray, Texture)) >= 0)
      {
        /* The ray is leaving the current object */

        Ray_Exit(Light_Source_Ray, texture_nr);
      }
      else
      {
        /* The ray is entering a new object */

        Ray_Enter(Light_Source_Ray, Texture, Ray_Intersection->Object);
      }
    }
  }
}
