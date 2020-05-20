#/************************************************
#* File: makefile.
#* Desc: Building definitions for the VrmlSup base support lib.

PROJECT= VrmlSup
#include $(AKRAPROJ)/geneMake.lnx


PLZEN_LEVELS= -DPLZEN_LEVEL_2 

CPPSRCS= baseVrml.cpp containerStack.cpp generalVrml.cpp \
	geometriesVrml.cpp geometry3D.cpp lightingVrml.cpp \
	lowSFNode.cpp materialVrml.cpp nativeError.cpp \
	rendererVrml.cpp transfoStack.cpp utility.cpp


all: libVrmlSup.a


libVrmlSup.a: $(OBJS)
	( cd $(OBJ_PATH) && \
	    $(AR) cru $@ $(OBJS) && \
	    $(RANLIB) $@ && \
	    cp $@ $(LIB_PATH) \
	)


clean:
	( cd $(OBJ_PATH) && \
	  rm *$(OBJ_EXT) $(STATNAMELIB) \
	)
