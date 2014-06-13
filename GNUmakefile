# --------------------------------------------------------------
# GNUmakefile for ACTARSIM module.  Hector Alvarez Pol, 01/06/05.
# Following the examples module GNUMakefile.
# ACTARSIM
# --------------------------------------------------------------

name := actarsim
G4TARGET := $(name)
G4EXLIB := true

G4TMPDIR = $(G4TMP)/$(G4SYSTEM)/$(name)

ifndef G4INSTALL
  G4INSTALL = ../../..
endif

#Checking OS
UNAME_S := $(shell uname -s)


.PHONY: all
all: lib bin sharedlib

include $(G4INSTALL)/config/binmake.gmk

#######  ROOT includes and libs (in case G4UIROOT is not defined) ########
ifndef INC_ROOT
   CPPFLAGS += $(shell $(ROOTSYS)/bin/root-config --cflags)
endif
ifndef LOAD_ROOT
   ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lHtml
   ROOTLIBS      := $(filter-out -lNew,$(ROOTLIBS))
#   ROOTLIBS      := $(filter-out -lThread,$(ROOTLIBS))
   ROOTLIBS      := $(filter-out -lpthread,$(ROOTLIBS))
   INTYLIBS      += $(ROOTLIBS)
endif
##########################################################################

visclean:
	rm -f g4*.prim g4*.eps g4*.wrl
	rm -f .DAWN_*

outclean:
	rm -f *~ *.rndm simFile.root


######################## ROOT dictionary ################################
#Inspired in Isidro Gonzalez GNUmakefile in his G4UIROOT interface!!

# Root Headers
ROOTHDRS := ActarSimData.hh ActarSimTrack.hh ActarSimSimpleTrack.hh ActarSimSciHit.hh ActarSimSilHit.hh ActarSimParisHit.hh ActarSimPrimaryInfo.hh ActarSimBeamInfo.hh
ROOTHDRSWITHPATH := $(patsubst %.hh,include/%.hh,$(ROOTHDRS))

# Make library depend on the root dictionary object file also
$(G4LIBDIR)/lib$(name).$(SHEXT): $(G4TMPDIR)/obj.last \
			         $(G4TMPDIR)/ActarSimDict.o

$(G4TMPDIR)/ActarSimDict.o: src/ActarSimDict.cc $(G4TMPDIR)/ActarSimDict.d

# Produce the dictionary file
src/ActarSimDict.cc: $(ROOTHDRSWITHPATH) \
		     src/ActarSimLinkDef.hh \
		     $(ROOTSYS)/include/RVersion.h
	@echo Making Root Dictionary...
ifdef CPPVERBOSE
	cd src; rootcint -f ActarSimDict.cc -c -p -I../include $(ROOTHDRS) ActarSimLinkDef.hh
else
	@cd src; rootcint -f ActarSimDict.cc -c -p -I../include $(ROOTHDRS) ActarSimLinkDef.hh
endif

# Clean up root dictionary
cleandict: src/ActarSimDict.cc
	@echo Removing Root dictionary and shared library...
	@rm -f src/ActarSimDict.*
	@rm -f $(G4TMPDIR)/ActarSimDict.*
	@rm -f actarsim.sl

# Make clean target clean de dictionary files...
#clean: cleandict

################ A SHARED LIBRARY WITH THE DATA DEFINITION FOR ROOT ######
#Hector Alvarez GNUmakefile: TESTING A SHARED LIBRARY ONLY FOR ROOT

CXXSHAREDFLAGS	= -g -I$(ROOTSYS)/include

ifeq ($(UNAME_S),Linux)
LDSHAREDFLAGS= -Wl,-soname, -shared -o
endif
ifeq ($(UNAME_S),Darwin)
LDSHAREDFLAGS= -dynamiclib -single_module -undefined dynamic_lookup -o 
endif

#LDSHAREDFLAGS   = -g

LDSHARED        = g++

SHAREDLIB       = actarsim.sl

SHAREDLIBHDRS	= $(ROOTHDRS) ActarSimDict.hh
SHAREDSRCS      = $(SHAREDLIBHDRS:.hh=.cc)
SHAREDOBJS	= $(SHAREDSRCS:.cc=.o)
SHAREDOBJSWITHPATH := $(patsubst %.o,$(G4TMP)/$(G4SYSTEM)/$(name)/%.o,$(SHAREDOBJS))

sharedlib:	$(SHAREDLIB)

$(SHAREDLIB):
		echo "Linking $(SHAREDLIB)"
		/bin/rm -f $(SHAREDLIB)
		$(LDSHARED) $(LDSHAREDFLAGS) $(SHAREDLIB) $(SHAREDOBJSWITHPATH)
##$(CXX) -Wl,-soname,$(@F) -shared -o $$libdir/$(@F) $(INTYLIBS) *.o
		chmod 555 $(SHAREDLIB)
		echo "done for $(UNAME_S)"


