/****************************************************************************
*                pvbmp.c
*
*  This module contains the code to read and write the BMP output file
*  format.
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

#define POVWIN_FILE

/****************************************************************************
*
*  Explanation:
*
*    -
*
*  ---
*
*  Jan 1996 : Creation.
*
*****************************************************************************/

#include "frame.h"
#include "povproto.h"
#include "povray.h"
#include "pvbmp.h"

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define BI_RGB  0
#define BI_RLE8 1
#define BI_RLE4 2
#define WIN_OS2_OLD 12
#define WIN_NEW     40
#define OS2_NEW     64
#define MAGIC1 0x15
#define MAGIC2 0x05
#define MAGIC3 0x75

/*****************************************************************************
* Local typedefs
******************************************************************************/

/*****************************************************************************
* Local variables
******************************************************************************/

static int BMP_Line_Number;

/*****************************************************************************
* Static functions
******************************************************************************/

static int Open_BMP_File PARAMS((FILE_HANDLE *handle, char *name, int *width,
				 int *height, int buffer_size, int mode));
static void Write_BMP_Line PARAMS((FILE_HANDLE *handle, COLOUR *line_data,
				   int line_number));
static int Read_BMP_Line PARAMS((FILE_HANDLE *handle, COLOUR *line_data,
				 int *line_number));
static void Close_BMP_File PARAMS((FILE_HANDLE *handle));

static unsigned Read_Short PARAMS((FILE *f));
static unsigned Read_Long PARAMS((FILE *f));
static void Write_Short PARAMS((FILE *f, unsigned n));
static void Write_Long PARAMS((FILE *f, unsigned n));
static unsigned char Safe_Getc PARAMS((FILE *f));

static void Read_BMP1 PARAMS((unsigned char **lines, FILE *f, unsigned width,
			      unsigned height));
static void Read_BMP4 PARAMS((unsigned char **lines, FILE *f,
			      unsigned compression, unsigned width,
			      unsigned height));
static void Read_BMP8 PARAMS((unsigned char **lines, FILE *f,
			      unsigned compression, unsigned width,
			      unsigned height));
static void Read_BMP24 PARAMS((IMAGE_LINE *lines, FILE *f, unsigned width,
			       unsigned height));


/*****************************************************************************
*
* FUNCTION
*
*   Read_Short
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Tim Rowley
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static unsigned Read_Short(f)
FILE *f;
{
  return (getc(f) | (getc(f) << 8));
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_Long
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Tim Rowley
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static unsigned Read_Long(f)
FILE *f;
{
  return (getc(f) | (getc(f) << 8) | (getc(f) << 16) | (getc(f) << 24));
}

/*****************************************************************************
*
* FUNCTION
*
*   Write_Short
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Tim Rowley
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static void Write_Short(f, n)
FILE *f;
unsigned n;
{
  putc(n&0xff, f);
  putc((n>>8)&0xff, f);
}

/*****************************************************************************
*
* FUNCTION
*
*   Write_Long
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Tim Rowley
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static void Write_Long(f, n)
FILE *f;
unsigned n;
{
  putc(n&0xff, f);
  putc((n>>8)&0xff, f);
  putc((n>>16)&0xff, f);
  putc((n>>24)&0xff, f);
}

/*****************************************************************************
*
* FUNCTION
*
*   Safe_Getc
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Tim Rowley
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static unsigned char Safe_Getc(f)
FILE *f;
{
  int data;
  if ((data=getc(f)) == EOF)
    Error("Error reading data from BMP image.\n");
  return data;
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP1
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Tim Rowley
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static void Read_BMP1(lines, f, width, height)
unsigned char **lines;
FILE *f;
unsigned width;
unsigned height;
{
  int pwidth, bit, i, y;
  int c;

  pwidth = ((width+31)/32)*32; /* 4 byte boundary */

  for (y=height-1; y>=0; y--)
    for (i=bit=0; i<pwidth; i++, bit++)
    {
      if ((bit&7) == 0)
      {
	c = Safe_Getc(f);
	bit = 0;
      }
      if (i<width)
      {
	lines[y][i] = (c & 0x80) ? 1 : 0;
	c <<= 1;
      }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP4
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Tim Rowley
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static void Read_BMP4(lines, f, compression, width, height)
unsigned char **lines;
FILE *f;
unsigned compression;
unsigned width;
unsigned height;
{
  int i, pwidth, nibble, x, y;
  int c, cc;

  if (compression == BI_RGB)
  {
    pwidth = ((width+7)/8)*8;
    for (y=height-1; y>=0; y--)
      for (i=nibble=0; i<pwidth; i++, nibble++)
      {
	if ((nibble&1)==0)
	{
	  c = Safe_Getc(f);
	  nibble = 0;
	}
	if (i<width)
	{
	  lines[y][i] = (c&0xf0)>>4;
	  c <<= 4;
	}
      }
  }
  else if (compression == BI_RLE4)
  {
    x = 0;
    y = height-1;
    while (1)
    {
      c = Safe_Getc(f);
      if (c)
      {
	cc = Safe_Getc(f);
	for (i=0; i<c; i++, x++)
	  if ((y>=0) && (y<height) && (x>=0) && (x<width))
	    lines[y][x] = (i&1) ? (cc &0x0f) : ((cc>>4)&0x0f);
      }
      else
      {
	c = Safe_Getc(f);
	if (c==0)
	{
	  x=0;
	  y--;
	}
	else if (c==1)
	  return;
	else if (c==2)
	{
	  x += Safe_Getc(f);
	  y -= Safe_Getc(f);
	}
	else
	{
	  for (i=0; i<c; i++, x++)
	  {
	    if ((i&1)==0)
	      cc = Safe_Getc(f);
	    if ((y>=0) && (y<height) && (x>=0) && (x<width))
	      lines[y][x] = (i&1) ? (cc&0x0f) : ((cc>>4)&0x0f);
	  }
	  if (((c&3)==1) || ((c&3)==2))
	    Safe_Getc(f);
	}
      }
    }
  }
  else
    Error ("Unknown compression scheme in BMP image.\n");
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP8
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Tim Rowley
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static void Read_BMP8(lines, f, compression, width, height)
unsigned char **lines;
FILE *f;
unsigned compression;
unsigned width;
unsigned height;
{
  int i, pwidth, x, y;
  int c, cc;

  if (compression == BI_RGB)
  {
    pwidth = ((width+3)/4)*4;
    for (y=0; y<height; y++)
      for (i=0; i<pwidth; i++)
      {
	c = Safe_Getc(f);
	if (i<width)
	  lines[y][i] = c;
      }
  }
  else if (compression == BI_RLE8)
  {
    x = 0;
    y = height-1;
    while (1)
    {
      c = Safe_Getc(f);
      if (c)
      {
	cc = Safe_Getc(f);
	for (i=0; i<c; i++, x++)
	  if ((y>=0) && (y<height) && (x>=0) && (x<width))
	    lines[y][x] = cc;
      }
      else
      {
	c = Safe_Getc(f);
	if (c==0)
	{
	  x = 0;
	  y--;
	}
	else if (c==1)
	  return;
	else if (c==2)
	{
	  x += Safe_Getc(f);
	  y -= Safe_Getc(f);
	}
	else
	  for (i=0; i<c; i++, x++)
	    if ((y>=0) && (y<height) && (x>=0) && (x<width))
	      lines[y][x] = Safe_Getc(f);
      }
    }
  }
  else
    Error ("Unknown compression scheme in BMP image.\n");
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP24
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Tim Rowley
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static void Read_BMP24(lines, f, width, height)
IMAGE_LINE *lines;
FILE *f;
unsigned width;
unsigned height;
{
  int pad, i, y;
  pad = (4-((width*3)%4)) &0x03;
  for (y=height-1; y>=0; y--)
  {
    for (i=0; i<width; i++)
    {
      lines[y].blue[i] = Safe_Getc(f);
      lines[y].green[i] = Safe_Getc(f);
      lines[y].red[i] = Safe_Getc(f);
    }
    if (fseek(f, pad, SEEK_CUR)==-1)
	Error("Error reading data from BMP image.\n");
  }
}

  
/*****************************************************************************
*
* FUNCTION
*
*   Get_BMP_File_Handle
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Tim Rowley
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

FILE_HANDLE *Get_BMP_File_Handle()
{
  FILE_HANDLE *handle;

  handle = (FILE_HANDLE *)POV_MALLOC(sizeof(FILE_HANDLE), "BMP file handle");

  handle->Open_File_p = Open_BMP_File;
  handle->Write_Line_p = Write_BMP_Line;
  handle->Read_Line_p = Read_BMP_Line;
  handle->Read_Image_p = Read_BMP_Image;
  handle->Close_File_p = Close_BMP_File;

  handle->file = NULL;
  handle->buffer = NULL;
  handle->buffer_size = 0;
  
  return handle;
}

/*****************************************************************************
*
* FUNCTION
*
*   Open_BMP_File
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Tim Rowley
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static int Open_BMP_File (handle, name, width, height, buffer_size, mode)
FILE_HANDLE *handle;
char *name;
int *width;
int *height;
int buffer_size;
int mode;
{
  unsigned dataLocation, depth, compression, planes, i;
  FILE *f;
  unsigned char *magic;

  handle->mode = mode;
  handle->filename = name;

  switch (mode) 
  {
    case READ_MODE:
      BMP_Line_Number = 0;
      if ((handle->file = fopen(name, READ_FILE_STRING)) == NULL)
      {
	return 0;
      }
      if (buffer_size != 0)
      {
	handle->buffer = POV_MALLOC((size_t)buffer_size, "BMP file buffer");
	setvbuf(handle->file, handle->buffer, _IOFBF, buffer_size);
      }

      f = handle->file;
      if ((getc(f)!='B') || (getc(f)!='M'))
      {
	Error ("Error reading magic number of BMP image.\n");
      }
      
      Read_Long(f);                   /* File size */
      Read_Short(f);  Read_Short(f);  /* Reserved fields */
      dataLocation = Read_Long(f);
      
      if (Read_Long(f)!=WIN_NEW)
      {
	Error ("Wrong BMP image format for completion.\n");
      }

      handle->width = *width = Read_Long(f);
      handle->height = *height = Read_Long(f);
      planes = Read_Short(f);
      depth = Read_Short(f);
      compression = Read_Long(f);
      Read_Long(f); /* image size in bytes */
      Read_Long(f); /* horizontal pixels per meter */
      Read_Long(f); /* vertical pixels per meter */
      Read_Long(f); /* Colors */
      Read_Long(f); /* Number of colors absolutely necessary to display image */

      if ((depth!=24) || (compression!=BI_RGB) || (planes!=1))
      {
        Error ("Wrong BMP image format for completion.\n");
      }

      if (feof(f) || (fseek(f, dataLocation, SEEK_SET)==-1))
      return 0;

      handle->buffer_size = buffer_size;
      break;

  case WRITE_MODE:
      BMP_Line_Number = 0;
      if (!strcmp(name, "-"))
      {
	      buffer_size = 0;
        handle->file = stdout;
      }
      else if ((handle->file = fopen(name, WRITE_FILE_STRING)) == NULL)
      {
        return 0;
      }

      if (buffer_size != 0)
      {
        handle->buffer = POV_MALLOC((size_t)buffer_size, "BMP file buffer");
        setvbuf(handle->file, handle->buffer, _IOFBF, buffer_size);
      }

      f = handle->file;
      putc('B', f);
      putc('M', f);
      Write_Long(f, 14+40+((*width*24+31)/32)*4*(*height));
      Write_Short(f, 0);
      Write_Short(f, 0);
      Write_Long(f, 14+40);
      Write_Long(f, 40);
      Write_Long(f, *width);
      Write_Long(f, *height);
      Write_Short(f, 1);
      Write_Short(f, 24);
      Write_Long(f, BI_RGB);
      Write_Long(f, ((*width*24+31)/32)*4*(*height));
      Write_Long(f, 0);
      Write_Long(f, 0);
      Write_Long(f, 0);
      Write_Long(f, 0);

      handle->width = *width;
      handle->height = *height;
      handle->buffer_size = buffer_size;

      magic = POV_MALLOC(((*width*24+31)/32)*4, "BMP magic");
      memset(magic, 0, ((*width*24+31)/32)*4);
      magic[0] = MAGIC1;
      magic[1] = MAGIC2;
      magic[2] = MAGIC3;
      for (i=0; i<*height; i++)
        fwrite(magic, ((*width*24+31)/32)*4, 1, f);
      break;
    case APPEND_MODE:
      BMP_Line_Number--;
      if (!strcmp(name,"-"))
      {
        buffer_size = 0;
        handle->file = stdout;
      }
      else if ((handle->file = fopen (name, "rb+")) == NULL)
      {
        return(0);
      }

      if (buffer_size != 0)
      {
        handle->buffer = POV_MALLOC((size_t)buffer_size, "BMP file buffer");
        setvbuf (handle->file, handle->buffer, _IOFBF, buffer_size);
      }

      break;
    }

  return 1;
}


/*****************************************************************************
*
* FUNCTION
*
*   Write_BMP_Line
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Tim Rowley
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static void Write_BMP_Line (handle, line_data, line_number)
FILE_HANDLE *handle;
COLOUR *line_data;
int line_number;
{
  int pad, i;
  pad = (4-((handle->width*3)%4))&0x03;

  fseek(handle->file, 
	14+40+(handle->height-1-BMP_Line_Number)*(3*handle->width+pad), 
	SEEK_SET);
  for (i=0; i<handle->width; i++)
  {
    putc((int)floor(line_data[i][BLUE]*255.0), handle->file);
    putc((int)floor(line_data[i][GREEN]*255.0), handle->file);
    putc((int)floor(line_data[i][RED]*255.0), handle->file);
  }
  for (i=0; i<pad; i++)
    putc(0, handle->file);
  BMP_Line_Number++;

  if (handle->buffer_size == 0)
  {
    fflush(handle->file); 
    if (strcmp(handle->filename, "-"))
      handle->file = freopen(handle->filename, "rb+", handle->file);
  }
}

      
/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_Line
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Tim Rowley
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static int Read_BMP_Line (handle, line_data, line_number)
FILE_HANDLE *handle;
COLOUR *line_data;
int *line_number;
{
  int pad, i, data, info;
  pad = (4-((handle->width*3)%4)) &0x03;

  fseek(handle->file, 
	-(BMP_Line_Number+1)*(3*handle->width+pad), SEEK_END);

  info=0;
  for (i=0; i<handle->width; i++)
  {
    if ((data = getc(handle->file)) == EOF)
      return -1;
    line_data[i][BLUE] = (DBL) data/255.0;
    if ((!i && (data!=MAGIC1)) || (i && data)) info=1;

    if ((data = getc(handle->file)) == EOF)
      return -1;
    line_data[i][GREEN] = (DBL) data/255.0;
    if ((!i && (data!=MAGIC2)) || (i && data)) info=1;

    if ((data = getc(handle->file)) == EOF)
      return -1;
    line_data[i][RED] = (DBL) data/255.0;
    if ((!i && (data!=MAGIC3)) || (i && data)) info=1;
  }

  if (info)
  {
    *line_number = BMP_Line_Number++;
    return 1;
  }
  else
  {
    *line_number = BMP_Line_Number++-1;
    return 0;
  }
}


/*****************************************************************************
*
* FUNCTION
*
*   Close_BMP_File
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Tim Rowley
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

static void Close_BMP_File (handle)
FILE_HANDLE *handle;
{
  if (handle->file)
  {
    fflush(handle->file);
    fclose(handle->file);
  }

  if (handle->buffer != NULL)
  {
    POV_FREE(handle->buffer);
  }

  handle->file = NULL;
  handle->buffer = NULL;
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_Image
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Tim Rowley
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1996 : Creation.
*
******************************************************************************/

void Read_BMP_Image(Image, name)
IMAGE *Image;
char *name;
{
  FILE *f;
  int i;
  unsigned width, height, depth, colors, dataLocation, planes, compression;
  unsigned infoSize;
  IMAGE_COLOUR *cmap;
  IMAGE_LINE *line_data;
  unsigned char *map_line;

  if ((f = Locate_File(name, READ_FILE_STRING, ".bmp", ".BMP", FALSE)) == NULL)
  {
    Error ("Error opening BMP image.\n");
  }

  if ((getc(f)!='B') || (getc(f)!='M'))
  {
    Error ("Error reading magic number of BMP image.\n");
  }

  Read_Long(f);                   /* File size */
  Read_Short(f);  Read_Short(f);  /* Reserved fields */
  dataLocation = Read_Long(f);

  infoSize = Read_Long(f);
  if (infoSize!=WIN_OS2_OLD)
  {
    width = Read_Long(f);
    height = Read_Long(f);
    planes = Read_Short(f);
    depth = Read_Short(f);
    compression = Read_Long(f);
    Read_Long(f); /* image size in bytes */
    Read_Long(f); /* horizontal pixels per meter */
    Read_Long(f); /* vertical pixels per meter */
    colors = Read_Long(f);
    Read_Long(f); /* Number of colors absolutely necessary to display image */
  }
  else  /* Old style */
  {
    width = Read_Short(f);
    height = Read_Short(f);
    planes = Read_Short(f);
    depth = Read_Short(f);
    compression = BI_RGB;
    colors = 0;
  }

  if (feof(f))
    Error("Error reading data from BMP image.\n");

  /* sanity check */
  if (((depth!=1) && (depth!=4) && (depth!=8) && (depth!=24)) ||
      (planes!=1) || (compression>BI_RLE4) ||
      (((depth==1) || (depth==24)) && (compression!=BI_RGB)) ||
      ((depth==4) && (compression==BI_RLE8)) ||
      ((depth==8) && (compression==BI_RLE4)))
  {
    Error ("Invalid BMP image (depth=%d planes=%d compression=%d).\n",
	   depth, planes, compression);
  }

  Image->iwidth = width;
  Image->iheight = height;
  Image->width = (DBL)width;
  Image->height = (DBL)height;
  Image->Colour_Map = NULL;

  /* Move to colormap */
  if (infoSize!=WIN_OS2_OLD)
  {
    int bytes;
    bytes = infoSize-40;
    for (i=0; i<bytes; i++)
      getc(f);
  }
  
  /* Load colormap */
  if (depth!=24)
  {
    int length;
    if (colors)
      length = colors;
    else
      length = 1<<depth;

    cmap = (IMAGE_COLOUR *)POV_CALLOC((size_t)length, 
						sizeof(IMAGE_COLOUR),
						"BMP color map");

    for (i=0; i<length; i++)
    {
      cmap[i].Blue    = getc(f);
      cmap[i].Green  = getc(f);
      cmap[i].Red   = getc(f);
      if (infoSize != WIN_OS2_OLD)
	getc(f);
    }

    if (feof(f))
      Error("Error reading data from BMP image.\n");

    Image->Colour_Map_Size = length;
    Image->Colour_Map = cmap;
    Image->data.map_lines = 
      (unsigned char **)POV_MALLOC(height*sizeof(unsigned char **),
				   "BMP image");
  }
  else
    Image->data.rgb_lines = (IMAGE_LINE *)POV_MALLOC(height*sizeof(IMAGE_LINE),
						     "BMP image");
  fseek(f, dataLocation, SEEK_SET);

  if (depth!=24)
  {
    for (i=height-1; i>=0; i--)
    {
      map_line = (unsigned char *)POV_MALLOC(width*sizeof(unsigned char),
					     "BMP image line");
      Image->data.map_lines[i] = map_line;
    }
    switch (depth)
    {
    case 1:
      Read_BMP1(Image->data.map_lines, f, width, height);
      break;
    case 4:
      Read_BMP4(Image->data.map_lines, f, compression, width, height);
      break;
    case 8:
      Read_BMP8(Image->data.map_lines, f, compression, width, height);
      break;
    }
  }
  else
  {
    for (i=height-1; i>=0; i--)
    {
      line_data = &Image->data.rgb_lines[i];
      line_data->red =
	(unsigned char *)POV_MALLOC(width*sizeof(unsigned char),
						   "BMP image line");
      line_data->green = 
	(unsigned char *)POV_MALLOC(width*sizeof(unsigned char),
						     "BMP image line");
      line_data->blue = 
	(unsigned char *)POV_MALLOC(width*sizeof(unsigned char),
						    "BMP image line");
    }
    Read_BMP24(Image->data.rgb_lines, f, width, height);
  }

  /* Whew.... we're done */
  fclose(f);
}
