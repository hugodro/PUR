/****************************************************************************
*                express.c
*
*  This module implements an expression parser for the floats, vectors and
*  colours in scene description files.
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

#include <ctype.h>
#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "parse.h"
#include "parstxtr.h"
#include "colour.h"
#include "express.h"
#include "matrices.h"
#include "povray.h"
#include "tokenize.h"
#include "pattern.h"
#include "pigment.h"
#include "normal.h"
#include "texture.h"


/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define ftrue(f) ((int)(fabs(f)>EPSILON))



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/

static unsigned int Number_Of_Random_Generators;
static unsigned long *next_rand;


/*****************************************************************************
* Static functions
******************************************************************************/

static void Parse_Vector_Param PARAMS((VECTOR Vector));
static void Parse_Vector_Param2 PARAMS((VECTOR Vect1, VECTOR Vect2));
static void Parse_Num_Factor PARAMS((EXPRESS Express, int *Terms));
static void Parse_Num_Term PARAMS((EXPRESS Express, int *Terms));
static void Parse_Rel_Factor PARAMS((EXPRESS Express, int *Terms));
static void Parse_Rel_Term PARAMS((EXPRESS Express, int *Terms));
static void Parse_Logical PARAMS((EXPRESS Express, int *Terms));
static void Parse_Express PARAMS((EXPRESS Express, int *Terms));
static void Promote_Express PARAMS((EXPRESS Express,int *Old_Terms,int New_Terms));
static void POV_strupr PARAMS((char *s));
static void POV_strlwr PARAMS((char *s));

static DBL stream_rand PARAMS((int stream));
static int stream_seed PARAMS((int seed));



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL Parse_Float_Param()
{
  DBL Local;
  EXPRESS Express;
  int Terms = 1;

  GET(LEFT_PAREN_TOKEN);
  Parse_Express(Express,&Terms);

  if (Terms>1)
  {
    Error ("Float expected but vector or color expression found.");
  }

  Local = Express[0];

  GET(RIGHT_PAREN_TOKEN);

  return (Local);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parse_Float_Param2(Val1,Val2)
  DBL *Val1;
  DBL *Val2;
  {
   GET (LEFT_PAREN_TOKEN);
   *Val1 = Parse_Float();
   Parse_Comma();
   *Val2 = Parse_Float();
   GET (RIGHT_PAREN_TOKEN);
  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void Parse_Vector_Param(Vector)
  VECTOR Vector;
  {
  GET(LEFT_PAREN_TOKEN);
  Parse_Vector(Vector);
  GET(RIGHT_PAREN_TOKEN);
  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void Parse_Vector_Param2(Val1,Val2)
  VECTOR Val1;
  VECTOR Val2;
  {
   GET (LEFT_PAREN_TOKEN);
   Parse_Vector(Val1);
   Parse_Comma();
   Parse_Vector(Val2);
   GET (RIGHT_PAREN_TOKEN);
  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void Parse_Num_Factor (Express,Terms)
EXPRESS Express;
int *Terms;
{
  int i = 0;
  DBL Val,Val2;
  VECTOR Vect,Vect2,Vect3;
  TRANSFORM Trans;
  char *Local_String, *Local_String2;
  FILE *f;

  EXPECT
    CASE (FLOAT_FUNCT_TOKEN)
       /* All of these functions return a DBL result */
       switch(Token.Function_Id)
         {
          case ABS_TOKEN:
            Val = Parse_Float_Param();
            Val = fabs(Val);
            break;

          case ACOS_TOKEN:
            Val = acos(Parse_Float_Param());
            break;

          case VAL_TOKEN:
            GET (LEFT_PAREN_TOKEN);
            Local_String=Parse_String();
            Val = atof(Local_String);
            POV_FREE(Local_String);
            GET (RIGHT_PAREN_TOKEN);
            break;

          case ASC_TOKEN:
            GET (LEFT_PAREN_TOKEN);
            Local_String=Parse_String();
            Val = (DBL)Local_String[0];
            POV_FREE(Local_String);
            GET (RIGHT_PAREN_TOKEN);
            break;

          case ASIN_TOKEN:
            Val = asin(Parse_Float_Param());
            break;

          case ATAN2_TOKEN:
            Parse_Float_Param2(&Val,&Val2);
            Val=atan2(Val,Val2);
            break;

          case CEIL_TOKEN:
            Val = ceil(Parse_Float_Param());
            break;

          case CLOCK_TOKEN:
            Val = opts.FrameSeq.Clock_Value;
            break;

          case COS_TOKEN:
            Val = cos(Parse_Float_Param());
            break;

          case DEGREES_TOKEN:
            Val = Parse_Float_Param()/M_PI_180;
            break;

          case DIV_TOKEN:
            Parse_Float_Param2(&Val,&Val2);
            Val=(DBL) ( (int)(Val/Val2) );
            break;

          case EXP_TOKEN:
            Val = exp(Parse_Float_Param());
            break;

          case FILE_EXISTS_TOKEN:
            GET (LEFT_PAREN_TOKEN);
            Local_String=Parse_String();
            Val = ((f=Locate_File(Local_String,READ_FILE_STRING,"","",FALSE))==NULL) ? 0.0 : 1.0;
            fclose(f);
            POV_FREE(Local_String);
            GET (RIGHT_PAREN_TOKEN);
            break;

          case FLOAT_ID_TOKEN:
            Val = *((DBL *) Token.Constant_Data);
            break;

          case FLOAT_TOKEN:
            Val = Token.Token_Float;
            break;

          case FLOOR_TOKEN:
            Val = floor(Parse_Float_Param());
            break;

          case INT_TOKEN:
            Val = (DBL) ((int) Parse_Float_Param());
            break;

          case LOG_TOKEN:
            Val = Parse_Float_Param();
            if (Val<=0.0)
            {
              Error("Log of negative number %lf\n",Val);
            }
            else
            {
              Val = log(Val);
            }
            break;

          case MAX_TOKEN:
            Parse_Float_Param2(&Val,&Val2);
            Val = max(Val,Val2);
            break;

          case MIN_TOKEN:
            Parse_Float_Param2(&Val,&Val2);
            Val = min(Val,Val2);
            break;

          case MOD_TOKEN:
            Parse_Float_Param2(&Val,&Val2);
            Val = fmod(Val,Val2);
            break;

          case PI_TOKEN:
            Val = M_PI;
            break;

          case POW_TOKEN:
            Parse_Float_Param2(&Val,&Val2);
            Val=pow(Val,Val2);
            break;

          case RADIANS_TOKEN:
            Val = Parse_Float_Param()*M_PI_180;
            break;

          case SIN_TOKEN:
            Val = sin(Parse_Float_Param());
            break;

          case SQRT_TOKEN:
            Val = Parse_Float_Param();
            if (Val<0.0)
            {
              Error("sqrt of negative number %lf\n",Val);
            }
            else
            {
              Val = sqrt(Val);
            }
            break;

          case STRCMP_TOKEN:
            GET (LEFT_PAREN_TOKEN);
            Local_String=Parse_String();
            Parse_Comma();
            Local_String2=Parse_String();
            Val = (DBL)strcmp(Local_String,Local_String2);
            POV_FREE(Local_String);
            POV_FREE(Local_String2);
            GET (RIGHT_PAREN_TOKEN);
            break;

          case STRLEN_TOKEN:
            GET (LEFT_PAREN_TOKEN);
            Local_String=Parse_String();
            Val = (DBL)strlen(Local_String);
            POV_FREE(Local_String);
            GET (RIGHT_PAREN_TOKEN);
            break;

          case TAN_TOKEN:
            Val = tan(Parse_Float_Param());
            break;

          case VDOT_TOKEN:
            Parse_Vector_Param2(Vect,Vect2);
            VDot(Val,Vect,Vect2);
            break;

          case VLENGTH_TOKEN:
            Parse_Vector_Param(Vect);
            VLength(Val,Vect);
            break;

          case VERSION_TOKEN:
            Val = opts.Language_Version;
            break;

          case TRUE_TOKEN:
          case YES_TOKEN:
          case ON_TOKEN:
            Val = 1.0;
            break;

          case FALSE_TOKEN:
          case NO_TOKEN:
          case OFF_TOKEN:
            Val = 0.0;
            break;

          case SEED_TOKEN:
            Val = stream_seed((int)Parse_Float_Param());
            break;

          case RAND_TOKEN:
            i = (int)Parse_Float_Param();
            if ((i < 0) || (i >= Number_Of_Random_Generators))
            {
              Error("Illegal random number generator.");
            }
            Val = stream_rand(i);
            break;

         }
       for (i=0; i < *Terms; i++)
         Express[i]=Val;
       EXIT
     END_CASE

     CASE (VECTOR_FUNCT_TOKEN)
       /* All of these functions return a VECTOR result */
       switch(Token.Function_Id)
         {
          case VAXIS_ROTATE_TOKEN:
            GET (LEFT_PAREN_TOKEN);
            Parse_Vector(Vect2);
            Parse_Comma();
            Parse_Vector(Vect3);
            Parse_Comma();
            Val=Parse_Float()*M_PI_180;
            GET (RIGHT_PAREN_TOKEN);
            Compute_Axis_Rotation_Transform(&Trans,Vect3,Val);
            MTransPoint(Vect, Vect2, &Trans);
            break;

          case VCROSS_TOKEN:
            Parse_Vector_Param2(Vect2,Vect3);
            VCross(Vect,Vect2,Vect3);
            break;

          case VECTOR_ID_TOKEN:
            Assign_Vector(Vect,Token.Constant_Data);
            break;

          case VNORMALIZE_TOKEN:
            Parse_Vector_Param(Vect2);
            VNormalize(Vect,Vect2);
            break;

          case VROTATE_TOKEN:
            Parse_Vector_Param2(Vect2,Vect3);
            Compute_Rotation_Transform (&Trans, Vect3);
            MTransPoint(Vect, Vect2, &Trans);
            break;

          case X_TOKEN:
            Make_Vector(Vect,1.0,0.0,0.0)
            break;

          case Y_TOKEN:
            Make_Vector(Vect,0.0,1.0,0.0)
            break;

          case Z_TOKEN:
            Make_Vector(Vect,0.0,0.0,1.0)
            break;
         }

       /* If it was expecting a DBL, promote it to a VECTOR.
          I haven't yet figured out what to do if it was expecting
          a COLOUR value with Terms>3
       */
       if (*Terms==1)
         *Terms=3;

       for (i=0; i < 3; i++)
         Express[i]=Vect[i];
       EXIT
     END_CASE

     CASE (COLOUR_ID_TOKEN)
       *Terms=5;
       for (i=0; i<5; i++)
         Express[i]=(DBL)(  ((COLC *)(Token.Constant_Data))[i]  );
       EXIT
     END_CASE

     CASE (T_TOKEN)
       *Terms=4;
       Express[0]=0.0;
       Express[1]=0.0;
       Express[2]=0.0;
       Express[3]=1.0;
       EXIT
     END_CASE

     CASE (U_TOKEN)
       *Terms=2;
       Express[0]=1.0;
       Express[1]=0.0;
       EXIT
     END_CASE

     CASE (V_TOKEN)
       *Terms=2;
       Express[0]=0.0;
       Express[1]=1.0;
       EXIT
     END_CASE

     CASE (PLUS_TOKEN)
     END_CASE

     CASE (DASH_TOKEN)
       Parse_Num_Factor(Express,Terms);
       for (i=0; i<*Terms; i++)
         Express[i]=-Express[i];
       EXIT
     END_CASE

     CASE (EXCLAMATION_TOKEN)
       Parse_Num_Factor(Express,Terms);
       for (i=0; i<*Terms; i++)
         Express[i] = ftrue(Express[i])?0.0:1.0;
       EXIT
     END_CASE

     CASE (LEFT_PAREN_TOKEN)
       Parse_Express(Express,Terms);
       GET(RIGHT_PAREN_TOKEN);
       EXIT
     END_CASE

/* This case parses a 2, 3, 4, or 5 term vector.  First parse 2 terms.
   Note Parse_Comma won't crash if it doesn't find one.
 */

     CASE (LEFT_ANGLE_TOKEN)
       Express[X] = Parse_Float();   Parse_Comma();
       Express[Y] = Parse_Float();   Parse_Comma();

       EXPECT
         CASE_EXPRESS
           /* If a 3th float is found, parse it. */
           Express[2] = Parse_Float(); Parse_Comma();
           *Terms=3;
           EXPECT
             CASE_EXPRESS
               /* If a 4th float is found, parse it. */
               Express[3] = Parse_Float(); Parse_Comma();
               *Terms=4;
               EXPECT
                 CASE_EXPRESS
                   /* If a 5th float is found, parse it. */
                   Express[4] = Parse_Float();
                   *Terms=5;
                 END_CASE

                 OTHERWISE
                   /* Only 4 found. */
                   UNGET
                   GET (RIGHT_ANGLE_TOKEN)
                   EXIT
                 END_CASE
               END_EXPECT
               EXIT
             END_CASE

             OTHERWISE
               /* Only 3 found. */
               UNGET
               GET (RIGHT_ANGLE_TOKEN)
               EXIT
             END_CASE
           END_EXPECT
           EXIT
         END_CASE

         OTHERWISE
           /* Only 2 found. */
           UNGET
           GET (RIGHT_ANGLE_TOKEN)
           EXIT
         END_CASE
       END_EXPECT
       EXIT
     END_CASE

     OTHERWISE
       Parse_Error_Str ("numeric expression");
     END_CASE
   END_EXPECT
 
   /* Parse VECTOR.x or COLOR.red type things */
   EXPECT
     CASE(PERIOD_TOKEN)
       EXPECT
         CASE (VECTOR_FUNCT_TOKEN)
           switch(Token.Function_Id)
           {
             case X_TOKEN:
               i=X;
               break;

             case Y_TOKEN:
               i=Y;
               break;

             case Z_TOKEN:
               i=Z;
               break;
               
             default:
               Parse_Error_Str ("x, y, or z");
           }
           EXIT
         END_CASE

         CASE (COLOUR_KEY_TOKEN)
           switch(Token.Function_Id)
           {
             case RED_TOKEN:
               i=RED;
               break;
               
             case GREEN_TOKEN:
               i=GREEN;
               break;

             case BLUE_TOKEN:
               i=BLUE;
               break;

             case FILTER_TOKEN:
               i=FILTER;
               break;
               
             case TRANSMIT_TOKEN:
               i=TRANSM;
               break;
               
             default:
               Parse_Error_Str ("red, green, blue, filter, or transmit");
           }
           EXIT
         END_CASE

         CASE(U_TOKEN)
           i=U;
           EXIT
         END_CASE

         CASE(V_TOKEN)
           i=V;
           EXIT
         END_CASE

         CASE(T_TOKEN)
           i=T;
           EXIT
         END_CASE

         OTHERWISE
           Parse_Error_Str ("x, y, z or color component");
         END_CASE
       END_EXPECT

       if (i>=*Terms)
       {
          Error("Bad operands for period operator.");
       }
       *Terms=1;
       Express[0]=Express[i];
       EXIT
     END_CASE
     
     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT
  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

/* If first operand of a 2-operand function had more terms than the second,
   then the parsing of the 2nd operand would have automatically promoted it.
   But if 2nd operand has more terms then we must go back promote the 1st
   operand before combining them  Promote_Express does it.  If Old_Terms=1
   then set all terms to Express[0].  Otherwise pad extra terms with 0.0.
*/

static void Promote_Express(Express,Old_Terms,New_Terms)
  EXPRESS Express;
  int *Old_Terms;
  int New_Terms;
  {
   register int i;

   if (*Old_Terms >= New_Terms)
     return;

   if (*Old_Terms==1)
   {
     for(i=1;i<New_Terms;i++)
     {
        Express[i]=Express[0];
     }
   }
   else
   {
     for(i=(*Old_Terms);i<New_Terms;i++)
     {
        Express[i]=0.0;
     }
   }

   *Old_Terms=New_Terms;
  }




/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void Parse_Num_Term (Express,Terms)
  EXPRESS Express;
  int *Terms;
  {
   register int i;
   EXPRESS Local_Express;
   int Local_Terms;

   Parse_Num_Factor(Express,Terms);

   Local_Terms=*Terms;

   EXPECT
     CASE (STAR_TOKEN)
       Parse_Num_Factor(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] *= Local_Express[i];
     END_CASE

     CASE (SLASH_TOKEN)
       Parse_Num_Factor(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] /= Local_Express[i];
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT

  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void Parse_Rel_Factor (Express,Terms)
  EXPRESS Express;
  int *Terms;
  {
   register int i;
   EXPRESS Local_Express;
   int Local_Terms;

   Parse_Num_Term(Express,Terms);

   Local_Terms=*Terms;

   EXPECT
     CASE (PLUS_TOKEN)
       Parse_Num_Term(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] += Local_Express[i];
     END_CASE

     CASE (DASH_TOKEN)
       Parse_Num_Term(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] -= Local_Express[i];
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT

  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void Parse_Rel_Term (Express,Terms)
  EXPRESS Express;
  int *Terms;
  {
   register int i;
   EXPRESS Local_Express;
   int Local_Terms;

   Parse_Rel_Factor(Express,Terms);

   Local_Terms=*Terms;

   EXPECT
     CASE (LEFT_ANGLE_TOKEN)
       Parse_Rel_Factor(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] = (DBL)(Express[i] < Local_Express[i]);
     END_CASE

     CASE (REL_LE_TOKEN)
       Parse_Rel_Factor(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] = (DBL)(Express[i] <= Local_Express[i]);
     END_CASE

     CASE (EQUALS_TOKEN)
       Parse_Rel_Factor(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] = (DBL)(!ftrue(Express[i]-Local_Express[i]));
     END_CASE

     CASE (REL_NE_TOKEN)
       Parse_Rel_Factor(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] = (DBL)ftrue(Express[i]-Local_Express[i]);
     END_CASE

     CASE (REL_GE_TOKEN)
       Parse_Rel_Factor(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] = (DBL)(Express[i] >= Local_Express[i]);
     END_CASE

     CASE (RIGHT_ANGLE_TOKEN)
       Parse_Rel_Factor(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] = (DBL)(Express[i] > Local_Express[i]);
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT

  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void Parse_Logical (Express,Terms)
  EXPRESS Express;
  int *Terms;
  {
   register int i;
   EXPRESS Local_Express;
   int Local_Terms;

   Parse_Rel_Term(Express,Terms);

   Local_Terms=*Terms;

   EXPECT
     CASE (AMPERSAND_TOKEN)
       Parse_Rel_Term(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] = (DBL)(ftrue(Express[i]) && ftrue(Local_Express[i]));
     END_CASE

     CASE (BAR_TOKEN)
       Parse_Rel_Term(Local_Express,&Local_Terms);
       Promote_Express(Express,Terms,Local_Terms);

       for(i=0;i<*Terms;i++)
         Express[i] = (DBL)(ftrue(Express[i]) || ftrue(Local_Express[i]));
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT

  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void Parse_Express (Express,Terms)
  EXPRESS Express;
  int *Terms;
  {
   EXPRESS Local_Express1, Local_Express2;
   EXPRESS *Chosen;
   int Local_Terms1, Local_Terms2;

   Local_Terms1 = 1;

   Parse_Logical(Express,&Local_Terms1);

   EXPECT
     CASE (QUESTION_TOKEN)
       if (Local_Terms1 != 1)
         Error("Conditional must evaluate to a float.");
       Local_Terms1 = Local_Terms2 = *Terms;
       Parse_Express(Local_Express1,&Local_Terms1);
       GET(COLON_TOKEN);
       Parse_Express(Local_Express2,&Local_Terms2);
       if (ftrue(Express[0]))
         {
          Chosen = (EXPRESS *)&Local_Express1;
          *Terms = Local_Terms1;
         }
       else
         {
          Chosen = (EXPRESS *)&Local_Express2;
          *Terms = Local_Terms2;
         }
       memcpy(Express,Chosen,sizeof(EXPRESS));
       EXIT
     END_CASE

     OTHERWISE
       /* Not a (c)?a:b expression.  Since Express was parsed with
          Local_Terms1=1 then we may have to promote this.  Suppose
          Terms=3 but Local_Terms1=1.  If this had been a (c)?a:b
          then a float is ok but since it is not a condition then
          it must be promoted to Terms=3.  Note that the parameters
          below look wrong but they are not.
        */
       Promote_Express (Express,&Local_Terms1,*Terms);
       /* On the other hand, Local_Terms1 may be bigger than Terms.
          If so, Express already is promoted and Terms must reflect that.
        */
       *Terms=Local_Terms1;
       UNGET
       EXIT
     END_CASE
   END_EXPECT

  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL Parse_Raw_Number()
{
  int Terms;
  DBL Val;
  EXPRESS Express;

  Terms = 1;

  Parse_Num_Factor(Express, &Terms);

  Val = Express[0];

  if (Terms != 1)
  {
    Error("Raw float expected but vector found instead.");
  }

  return (Val);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL Parse_Float ()
  {
   EXPRESS Express;
   int Terms;

   Terms=1;

   if (opts.Language_Version < 1.5)
          Parse_Num_Factor(Express,&Terms);
   else
      Parse_Rel_Factor(Express,&Terms);

   if (Terms>1)
      Error ("Float expected but vector or color expression found.");

   return (Express[0]);
  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL Allow_Float (defval)
  DBL defval;
  {
  DBL retval;

  EXPECT
    CASE_EXPRESS
      retval = Parse_Float();
      EXIT
    END_CASE

    OTHERWISE
      UNGET
      retval = defval;
      EXIT
    END_CASE
  END_EXPECT

  return (retval);
  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parse_Vector (Vector)
  VECTOR Vector;
  {
   EXPRESS Express;
   int Terms;

   /* Initialize expression. [DB 12/94] */

   for (Terms = 0; Terms < 5; Terms++)
   {
     Express[Terms] = 0.0;
   }

   Terms=3;

   if (opts.Language_Version < 1.5)
          Parse_Num_Factor(Express,&Terms);
   else
      Parse_Rel_Factor(Express,&Terms);

   if (Terms>3)
      Error ("Vector expected but color expression found.");

   for(Terms=0;Terms<3;Terms++)
      Vector[Terms]=Express[Terms];
  }


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parse_Vector4D (Vector)
  VECTOR Vector;
  {
   EXPRESS Express;
   int Terms;
   int Dim = 4;
   /* Initialize expression. [DB 12/94] */

   for (Terms = 0; Terms < 5; Terms++)
   {
     Express[Terms] = 0.0;
   }

   Terms=Dim;

   if (opts.Language_Version < 1.5)
          Parse_Num_Factor(Express,&Terms);
   else
      Parse_Rel_Factor(Express,&Terms);

   if (Terms>Dim)
      Error ("Vector expected but color expression found.");

   for(Terms=0;Terms<Dim;Terms++)
      Vector[Terms]=Express[Terms];
  }




/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parse_UV_Vect (UV_Vect)
  UV_VECT UV_Vect;
  {
   EXPRESS Express;
   int Terms;

   /* Initialize expression. [DB 12/94] */

   for (Terms = 0; Terms < 5; Terms++)
   {
     Express[Terms] = 0.0;
   }

   Terms=2;

   if (opts.Language_Version < 1.5)
          Parse_Num_Factor(Express,&Terms);
   else
      Parse_Rel_Factor(Express,&Terms);

   if (Terms>2)
      Error ("UV_Vector expected but vector or color expression found.");

   for(Terms=0;Terms<2;Terms++)
      UV_Vect[Terms]=Express[Terms];
  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parse_Vector_Float(Vector)
  VECTOR Vector;
  {
   EXPRESS Express;
   int Terms;

   /* Initialize expression. [DB 12/94] */

   for (Terms = 0; Terms < 5; Terms++)
   {
     Express[Terms] = 0.0;
   }

   Terms=1;

   if (opts.Language_Version < 1.5)
          Parse_Num_Factor(Express,&Terms);
   else
      Parse_Rel_Factor(Express,&Terms);

   if (Terms>3)
      Error ("Vector or float expected but color expression found.");

   Have_Vector=(Terms==3);

   if (Have_Vector)
     for(Terms=0;Terms<3;Terms++)
       Vector[Terms]=Express[Terms];
   else
     for(Terms=0;Terms<3;Terms++)
       Vector[Terms]=Express[0];
  }


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parse_Scale_Vector (Vector)
  VECTOR Vector;
  {
   Parse_Vector(Vector);

   if (Vector[X] == 0.0)
    {
     Vector[X] = 1.0;
     Warn(0.0, "Illegal Value: Scale X by 0.0. Changed to 1.0.");
    }
   if (Vector[Y] == 0.0)
    {
     Vector[Y] = 1.0;
     Warn(0.0, "Illegal Value: Scale Y by 0.0. Changed to 1.0.");
    }
   if (Vector[Z] == 0.0)
    {
     Vector[Z] = 1.0;
     Warn(0.0, "Illegal Value: Scale Z by 0.0. Changed to 1.0.");
    }
  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parse_Colour (Colour)
  COLOUR Colour;
  {
  EXPRESS Express;
  int Terms;
  register int i;

  /* Initialize expression. [DB 12/94] */

  for (Terms = 0; Terms < 5; Terms++)
  {
    Express[Terms] = 0.0;
  }

  Make_Colour (Colour, 0.0, 0.0, 0.0);

  ALLOW(COLOUR_TOKEN)

  EXPECT
    CASE (COLOUR_KEY_TOKEN)
      switch(Token.Function_Id)
        {
         case ALPHA_TOKEN:
           Warn(1.55, "Keyword ALPHA discontinued. Use FILTER instead.");
           /* missing break deliberate */

         case FILTER_TOKEN:
                   Colour[FILTER] = (COLC)Parse_Float();
                   break;

         case BLUE_TOKEN:
                   Colour[BLUE] = (COLC)Parse_Float();
                   break;

         case GREEN_TOKEN:
                   Colour[GREEN] = (COLC)Parse_Float();
                   break;

         case RED_TOKEN:
                   Colour[RED] = (COLC)Parse_Float();
                   break;

         case TRANSMIT_TOKEN:
                   Colour[TRANSM] = (COLC)Parse_Float();
                   break;

         case RGB_TOKEN:
           Terms=3;
           Parse_Express(Express,&Terms);
           if (Terms != 3)
             Warn(0.0, "Suspicious expression after rgb.");
           for (i=0;i<Terms;i++)
             Colour[i]=(COLC)Express[i];
           break;

         case RGBF_TOKEN:
           Terms=4;
           Parse_Express(Express,&Terms);
           if (Terms != 4)
             Warn(0.0, "Suspicious expression after rgbf.");
           for (i=0;i<Terms;i++)
             Colour[i]=(COLC)Express[i];
           break;

         case RGBT_TOKEN:
           Terms=4;
           Parse_Express(Express,&Terms);
           if (Terms != 4)
             Warn(0.0, "Suspicious expression after rgbt.");
           for (i=0;i<Terms;i++)
             Colour[i]=(COLC)Express[i];
           Colour[TRANSM]=Colour[FILTER];
           Colour[FILTER]=0.0;
           break;

         case RGBFT_TOKEN:
           Terms=5;
           Parse_Express(Express,&Terms);
           if (Terms != 5)
             Warn(0.0, "Suspicious expression after rgbft.");
           for (i=0;i<Terms;i++)
             Colour[i]=(COLC)Express[i];
           break;
        }
    END_CASE

    CASE (COLOUR_ID_TOKEN)
      UNGET
      Terms=5;
      Parse_Express(Express,&Terms);
      for (i=0;i<Terms;i++)
        Colour[i]=(COLC)Express[i];
    END_CASE

    CASE_EXPRESS
      UNGET
      Terms=5;
      Parse_Express(Express,&Terms);
      if (Terms != 5)
        Error("Color expression expected but float or vector expression found.");
      for (i=0;i<Terms;i++)
        Colour[i]=(COLC)Express[i];
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT
}

/*****************************************************************************
*
* FUNCTION
*
*   Parse_Blend_Map
*
* INPUT
*
*   Type of map to parse: pigment_map, normal_map etc
*   
* OUTPUT
*   
* RETURNS
*
*   Pointer to created blend map
*   
* AUTHOR
*
*   Chris Young 11/94
*   
* DESCRIPTION   :
*
* CHANGES
*
******************************************************************************/

BLEND_MAP *Parse_Blend_Map (Blend_Type,Pat_Type)
int Blend_Type,Pat_Type;
  {
   BLEND_MAP *New = NULL;
   BLEND_MAP_ENTRY *Temp_Ent;
   int i;

   Parse_Begin ();

   EXPECT
     CASE2 (COLOUR_MAP_ID_TOKEN, PIGMENT_MAP_ID_TOKEN)
     CASE3 (NORMAL_MAP_ID_TOKEN, TEXTURE_MAP_ID_TOKEN, SLOPE_MAP_ID_TOKEN)
       New = Copy_Blend_Map ((BLEND_MAP *) Token.Constant_Data);
       EXIT
     END_CASE

     OTHERWISE
       UNGET
       Temp_Ent = Create_BMap_Entries(MAX_BLEND_MAP_ENTRIES);
       i = 0;

       EXPECT
         CASE (LEFT_SQUARE_TOKEN)
           switch (Pat_Type)
           {
              case AVERAGE_PATTERN:
                Temp_Ent[i].value = Allow_Float(1.0);
                Parse_Comma();
                break;
              
              default:
                Temp_Ent[i].value = Parse_Float();
                Parse_Comma();
                break;
           }

           switch (Blend_Type)
           {
            case PIGMENT_TYPE:
              Temp_Ent[i].Vals.Pigment=Copy_Pigment(Default_Texture->Pigment);
              Parse_Pigment(&(Temp_Ent[i].Vals.Pigment));
              break;

            case NORMAL_TYPE:
              Temp_Ent[i].Vals.Tnormal=Copy_Tnormal(Default_Texture->Tnormal);
              Parse_Tnormal(&(Temp_Ent[i].Vals.Tnormal));
              break;

            case SLOPE_TYPE:
              Parse_UV_Vect(Temp_Ent[i].Vals.Point_Slope);
              break;

            case TEXTURE_TYPE:
              Temp_Ent[i].Vals.Texture=Parse_Texture();
              break;

            default:
              Error("Type not implemented yet.");
           }
           if (++i > MAX_BLEND_MAP_ENTRIES)
             Error ("Blend_Map too long");

           GET (RIGHT_SQUARE_TOKEN);
         END_CASE

         OTHERWISE
           UNGET
           if (i < 1)
             Error ("Must have at least one entry in map.");
           New = Create_Blend_Map ();
           New->Number_Of_Entries = i;
           New->Type=Blend_Type;
           New->Transparency_Flag=TRUE; /*Temp fix.  Really set in Post_???*/
           New->Blend_Map_Entries = POV_REALLOC(Temp_Ent,sizeof(BLEND_MAP_ENTRY)*i,"blend map entries");
           EXIT
         END_CASE
       END_EXPECT
       EXIT
     END_CASE
   END_EXPECT

   Parse_End ();

   return (New);
  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

BLEND_MAP *Parse_Blend_List (Count,Def_Map,Blend_Type)
  int Count;
  BLEND_MAP *Def_Map;
  int Blend_Type;
  {
   BLEND_MAP *New;
   BLEND_MAP_ENTRY *Temp_Ent;
   int Type, i;

   i = 0;

   if(Blend_Type == PIGMENT_TYPE)
   {
     EXPECT
       CASE(PIGMENT_TOKEN)
         UNGET
         Type=PIGMENT_TYPE;
         EXIT
       END_CASE

       OTHERWISE
         UNGET
         Type=COLOUR_TYPE;
         EXIT
       END_CASE
     END_EXPECT
   }
   else
   {
     Type=Blend_Type;
   }

   Temp_Ent = Create_BMap_Entries(Count);

   switch(Type)
   {
     case COLOUR_TYPE:
       EXPECT
         CASE_COLOUR
         CASE_EXPRESS
           Parse_Colour (Temp_Ent[i].Vals.Colour);
           Parse_Comma ();
           Temp_Ent[i].value = (SNGL)i;
           if (++i >= Count)
             EXIT
         END_CASE

         OTHERWISE
           UNGET
           EXIT
         END_CASE
       END_EXPECT
       break;

     case PIGMENT_TYPE:
       EXPECT
         CASE(PIGMENT_TOKEN)
           Parse_Begin ();
           Temp_Ent[i].Vals.Pigment=Copy_Pigment(Default_Texture->Pigment);
           Parse_Pigment(&(Temp_Ent[i].Vals.Pigment));
           Parse_End ();
           Parse_Comma ();
           Temp_Ent[i].value = (SNGL)i;
           if (++i >= Count)
             EXIT
         END_CASE

         OTHERWISE
           UNGET
           EXIT
         END_CASE
       END_EXPECT
       break;

     case NORMAL_TYPE:
       EXPECT
         CASE(TNORMAL_TOKEN)
           Parse_Begin ();
           Temp_Ent[i].Vals.Tnormal=Copy_Tnormal(Default_Texture->Tnormal);
           Parse_Tnormal(&(Temp_Ent[i].Vals.Tnormal));
           Parse_End ();
           Parse_Comma ();
           Temp_Ent[i].value = (SNGL)i;
           if (++i >= Count)
             EXIT
         END_CASE

         OTHERWISE
           UNGET
           EXIT
         END_CASE
       END_EXPECT
       break;

     case TEXTURE_TYPE:
       EXPECT
         CASE(TEXTURE_TOKEN)
           Parse_Begin ();
           Temp_Ent[i].Vals.Texture=Parse_Texture();
           Parse_End ();
           Parse_Comma ();
           Temp_Ent[i].value = (SNGL)i;
           if (++i >= Count)
             EXIT
         END_CASE

         OTHERWISE
           UNGET
           EXIT
         END_CASE
       END_EXPECT
       break;

   }
   
   if ((Type==NORMAL_TYPE) && (i==0))
   {
     POV_FREE(Temp_Ent);
     return (NULL);
   }

   while (i < Count)
   {
     switch (Type)
     {
        case COLOUR_TYPE:
          Assign_Colour(Temp_Ent[i].Vals.Colour,Def_Map->Blend_Map_Entries[i].Vals.Colour);
          break;

        case PIGMENT_TYPE:
          Temp_Ent[i].Vals.Pigment=Copy_Pigment(Default_Texture->Pigment);
          break;

        case NORMAL_TYPE:
          Temp_Ent[i].Vals.Tnormal=Copy_Tnormal(Default_Texture->Tnormal);
          break;

        case TEXTURE_TYPE:
          Temp_Ent[i].Vals.Texture=Copy_Textures(Default_Texture);
          break;

     }
     Temp_Ent[i].value = (SNGL)i;
     i++;
   }

   New = Create_Blend_Map ();
   New->Number_Of_Entries = Count;
   New->Type=Type;
   New->Transparency_Flag=TRUE; /*Temp fix.  Really set in Post_???*/
   New->Blend_Map_Entries = Temp_Ent;

   return (New);
  }


/*****************************************************************************
*
* FUNCTION
*
*   Parse_Colour_Map
*
* INPUT
*   
* OUTPUT
*
* RETURNS
*
*   Pointer to newly created BLEND_MAP that has colors as all
*   its entries.
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION   : This seperate routine parses color_maps only.  It
*                 cannot be used for pigment_maps because it accomidates
*                 the old double entry color maps from vers 1.0
*
* CHANGES
*
******************************************************************************/

BLEND_MAP *Parse_Colour_Map ()
  {
   BLEND_MAP *New = NULL;
   int i,j,c,p,ii;
   EXPRESS Express;
   int Terms;
   BLEND_MAP_ENTRY *Temp_Ent;

   Parse_Begin ();

   EXPECT
     CASE (COLOUR_MAP_ID_TOKEN)
       New = Copy_Blend_Map ((BLEND_MAP *) Token.Constant_Data);
       EXIT
     END_CASE

     OTHERWISE
       UNGET
       Temp_Ent = Create_BMap_Entries(MAX_BLEND_MAP_ENTRIES);
       i = 0;
       j = 1;

       EXPECT
         CASE (LEFT_SQUARE_TOKEN)
           Temp_Ent[i].value = Parse_Float();  Parse_Comma();

           EXPECT
             /* After [ must be a float. If 2nd thing found is another
                float then this is an old style color_map.
              */
             CASE_FLOAT
               Terms=1;
               Parse_Express(Express,&Terms);
               if (Terms==1)
                 {
                  Temp_Ent[j].value = Express[0];
                  Parse_Colour (Temp_Ent[i].Vals.Colour);

                  GET (COLOUR_TOKEN);
                  Parse_Colour (Temp_Ent[j].Vals.Colour);
                  i += 2;
                  j += 2;
                 }
               else
                 if (Terms==5)
                   {
                    for (ii=0;ii<5;ii++)
                      Temp_Ent[i].Vals.Colour[ii]=(COLC)Express[ii];
                    i++;
                    j++;
                   }
                 else
                   Error("Illegal expression syntax in color_map.");
               EXIT
             END_CASE

             CASE_COLOUR
               Parse_Colour (Temp_Ent[i].Vals.Colour);
               i++;
               j++;
               EXIT
             END_CASE
             
             OTHERWISE
               Parse_Error_Str ("float or color");
             END_CASE

           END_EXPECT

           if (j > MAX_BLEND_MAP_ENTRIES)
             Error ("Blend_Map too long.");

           GET (RIGHT_SQUARE_TOKEN);
         END_CASE

         OTHERWISE
           UNGET
           if (i < 1)
             Error ("Must have at least one color in color map.");

           /* Eliminate duplicates */
           for (c = 1, p = 0; c<i; c++)
             {
              if (memcmp(&(Temp_Ent[p]),
                         &(Temp_Ent[c]),sizeof(BLEND_MAP_ENTRY)) == 0)
                p--;

              Temp_Ent[++p] = Temp_Ent[c];
             }
           p++;
           New = Create_Blend_Map ();
           New->Number_Of_Entries = p;
           New->Type=COLOUR_TYPE;
           New->Transparency_Flag=TRUE; /*Temp fix.  Really set in Post_???*/
           New->Blend_Map_Entries = POV_REALLOC(Temp_Ent,sizeof(BLEND_MAP_ENTRY)*p,"blend map entries");
           EXIT
         END_CASE
       END_EXPECT
       EXIT
     END_CASE
   END_EXPECT

   Parse_End ();

   return (New);
  }



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

char *Parse_String()
{
  char *temp1, *temp2, *New = NULL, *p;
  char temp3[64];
  char temp4[64];
  DBL val;
  int l,l2,d;

  EXPECT
    CASE(STRING_LITERAL_TOKEN)
      New=POV_MALLOC(strlen(Token.Token_String) + 1, "temporary string");
      strcpy (New, Token.Token_String);
      EXIT
    END_CASE

    CASE(STR_TOKEN)
      GET (LEFT_PAREN_TOKEN);
      val = Parse_Float();
      Parse_Comma();
      l = (int)Parse_Float();
      Parse_Comma();
      d = (int)Parse_Float();
      GET (RIGHT_PAREN_TOKEN);

      p=temp3;
      *(p++) = '%';
      if (l>0)
      {
        sprintf(p,"%d",l);
        while (*(++p));
        /* Could also be written for clarity as:
        while (*p != '\0')
          p++;
        */
      }
      else
      {
        if (l)
        {
          sprintf(p,"0%d",abs(l));
          while (*(++p));
        }
      }
      
      if (d>=0)
      {
        *(p++) = '.';
        sprintf(p,"%d",d);
        while (*(++p));
      }
      *(p++) = 'f';
      *p     = '\0';
            
      sprintf(temp4,temp3,val);
      
      New=POV_MALLOC(strlen(temp4) + 1, "temporary string");
      strcpy (New, temp4);
      EXIT
    END_CASE

    CASE(CONCAT_TOKEN)
      GET (LEFT_PAREN_TOKEN);
      
      New=Parse_String();
      EXPECT
        CASE(RIGHT_PAREN_TOKEN)
          EXIT
        END_CASE
        
        OTHERWISE
          Parse_Comma();
          temp1=New;
          temp2=Parse_String();
          l2=strlen(temp1)+strlen(temp2)+2;
          New=POV_MALLOC(l2, "temporary string");
          strcpy(New,temp1);
          strcat(New,temp2);
          POV_FREE(temp1);
          POV_FREE(temp2);
        END_CASE
      END_EXPECT       
      EXIT
    END_CASE
    
    CASE(CHR_TOKEN)
      New=POV_MALLOC(2, "temporary string");
      d=(int)Parse_Float_Param();
      if ((d<0)||(d>255))
      {
        Error("Value %d cannot be used in chr(...).\n",d);
      }
      New[0]=d;
      New[1]='\0';
      EXIT
    END_CASE
    
    CASE(SUBSTR_TOKEN)
      GET (LEFT_PAREN_TOKEN);
      
      temp1=Parse_String();
      Parse_Comma();
      l=(int)Parse_Float();
      Parse_Comma();
      d=(int)Parse_Float();
      if ((l+d-1) > strlen(temp1))
      {
         Error("Illegal params in substr(%s,%d,%d).\n",temp1,l,d);
      }
      New=POV_MALLOC((size_t)(d+1), "temporary string");
      strncpy(New,&(temp1[l-1]),(unsigned)d);
      New[d]='\0';
      POV_FREE(temp1);
      GET (RIGHT_PAREN_TOKEN);
      EXIT
    END_CASE
    
    CASE(STRUPR_TOKEN)
      GET (LEFT_PAREN_TOKEN);
      New=Parse_String();
      POV_strupr(New);
      GET (RIGHT_PAREN_TOKEN);
      EXIT
    END_CASE
    
    CASE(STRLWR_TOKEN)
      GET (LEFT_PAREN_TOKEN);
      New=Parse_String();
      POV_strlwr(New);
      GET (RIGHT_PAREN_TOKEN);
      EXIT
    END_CASE
    
    CASE(STRING_ID_TOKEN)
      New=POV_MALLOC(strlen(Token.Constant_Data) + 1, "temporary string");
      strcpy (New, Token.Constant_Data);
      EXIT
    END_CASE
      
    OTHERWISE
      Parse_Error_Str ("string expression");
    END_CASE
  END_EXPECT

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

char *Parse_Formatted_String()
{
  char *New, *src, *dest;

  New = src = dest = Parse_String();

  while (*src != '\0')
  {
    if (*src=='\\')
    {
      switch(*(++src))
      {
        case 'a': *dest=0x07; break;
           
        case 'b': *dest=0x08; break;

        case 'f': *dest=0x0c; break;

        case 'n': *dest=0x0a; break;

        case 'r': *dest=0x0d; break;

        case 't': *dest=0x09; break;

        case 'v': *dest=0x0b; break;

        case '\0': *dest=0x5c; break;

        case '\'': *dest=0x27; break;
           
        default: *dest='\\'; dest++; *dest=*src; break;
      }
    }
    else
    {
      *dest=*src;
    }
    src++;
    dest++;
  }
  *dest='\0';
  return (New);
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void POV_strupr(s)
char *s;
{
  int i;
  
  for (i = 0; i < strlen(s); i++)
  {
    s[i] = (char)toupper((int)s[i]);
  }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void POV_strlwr(s)
char *s;
{
  int i;
  
  for (i = 0; i < strlen(s); i++)
  {
    s[i] = (char)tolower((int)s[i]);
  }
}


/*****************************************************************************
*
* FUNCTION
*
*   stream_rand
*
* INPUT
*
*   stream - number of random stream
*
* OUTPUT
*
* RETURNS
*
*   DBL - random value
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Standard pseudo-random function.
*
* CHANGES
*
*   Feb 1996 : Creation.
*   Mar 1996 : Return 2^32 random values instead of 2^16 [AED]
*
******************************************************************************/

static DBL stream_rand(stream)
int stream;
{
  next_rand[stream] = next_rand[stream] * 1812433253L + 12345L;

  return((DBL)(next_rand[stream] & 0xFFFFFFFFUL) / 0xFFFFFFFFUL);
}



/*****************************************************************************
*
* FUNCTION
*
*   stream_seed
*
* INPUT
*
*   seed - Pseudo-random generator start value
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
*   Set start value for pseudo-random generator.
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

static int stream_seed(seed)
int seed;
{
  next_rand = (unsigned long *)POV_REALLOC(next_rand, (Number_Of_Random_Generators+1)*sizeof(unsigned long), "random number generator");

  next_rand[Number_Of_Random_Generators] = (unsigned long int)seed;

  Number_Of_Random_Generators++;

  return(Number_Of_Random_Generators-1);
}



/*****************************************************************************
*
* FUNCTION
*
*   Init_Random_Generators
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
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Init_Random_Generators()
{
  Number_Of_Random_Generators = 0;

  next_rand = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Random_Generators
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
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Destroy_Random_Generators()
{
  if (next_rand != NULL)
  {
    POV_FREE(next_rand);
  }

  next_rand = NULL;

  Number_Of_Random_Generators = 0;
}

