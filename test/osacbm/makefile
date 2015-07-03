###############################################################################
##         (c) Copyright, Real-Time Innovations, All rights reserved.        ##
##                                                                           ##
##         Permission to modify and use for internal purposes granted.       ##
## This software is provided "as is", without warranty, express or implied.  ##
##                                                                           ##
###############################################################################

# This Makefile contains the common rules to build for all the architectures.
# It is included from the architecture-specific Makefile.
# This Makefile requires the following variables:
# - ARCH: Architecture to build (for example i86Linux2.6gcc4.1.1)
#
# Optional variables:
# - DEBUG: If set to '1', it turns on debugging information
# - SHAREDLIB: If set to '1', shared libraries will be used
# - CXX: compiler name.
# - CXXFLAGS: compiler flags: will be appended to $CXX command-line
# - CXXLD: linker name.
# - CXXLDFLAGS: linker flags: will be inserted at the beginning of CXXLD cmdline
# - SYSLIBS: additional system libraries to append to the CXXLD command-line

###############################################################################
# Ensure this Makefile is invoked with the right variable set
###############################################################################
ifeq ($(ARCH), )
all:
	@echo "***************************************************************"
	@echo "The environment variable 'ARCH' is not set!"
	@echo "***************************************************************"
	@false
else
###############################################################################
# Ensure $NDDSHOME is defined
###############################################################################
ifeq ($(NDDSHOME), )
all:
	@echo "***************************************************************"
	@echo "The environment variable 'NDDSHOME' is not set!"
	@echo "***************************************************************"
	@false
else
###############################################################################
# Ensure $BOOSTHOME is defined
###############################################################################
ifeq ($(BOOSTHOME), )
all:
	@echo "***************************************************************"
	@echo "The environment variable 'BOOSTHOME' is not set!"
	@echo "***************************************************************"
	@false
else
###############################################################################
# Ensure $REFLEXHOME is defined
###############################################################################
ifeq ($(REFLEXHOME), )
all:
	@echo "***************************************************************"
	@echo "The environment variable 'REFLEXHOME' is not set!"
	@echo "***************************************************************"
	@false
###############################################################################
# Ensure $XSDCXXHOME is defined
###############################################################################
ifeq ($(XSDCXXHOME), )
all:
	@echo "***************************************************************"
	@echo "The environment variable 'XSDCXXHOME' is not set!"
	@echo "***************************************************************"
	@false
endif
endif
endif
endif
endif

ifeq ($(DEBUG), 1)
  CXXFLAGS   := -W -Wall -g -pg -O0 -DREFLEX_DEBUG
  CXXLDFLAGS := -g -pg -O0 
  REFLEXLIB  := -lreflexd
  NDDSLIBS   := -lnddscore -lnddscd -lnddscppd  
else
  CXXFLAGS  := -W -Wall -O3
  REFLEXLIB := -lreflex
  NDDSLIBS  := -lnddscore -lnddsc -lnddscpp 
endif 

XSDCXX    := $(XSDCXXHOME)/bin/xsd # path to the XSD compiler
LIBXSD    := $(XSDCXXHOME)/libxsd # path to the LIBXSD
CXX       := g++ -Wl,--no-as-needed 
XSDSTD    := --std c++11
CXXSTD    := --std=c++11
CXXFLAGS  += -m32 -D HAVE_CXA_DEMANGLE=1 \
             -Wno-deprecated-declarations \
             -Wno-unused-local-typedefs \
             -Wno-unused-parameter \
             -DRTI_UNIX -DRTI_LINUX \
             #-ftemplate-backtrace-limit=0

SYSLIBS := -ldl -lnsl -lm -lpthread -lrt
INCLUDEPATHS    := -I$(NDDSHOME)/include/ndds \
                   -I$(NDDSHOME)/include \
                   -I$(REFLEXHOME)/include \
                   -I$(BOOSTHOME)\
                   -I$(LIBXSD)   \
                   -Igen

LIBPATHS    := -L$(NDDSHOME)/lib/$(ARCH) \
               -L$(REFLEXHOME)/objs/$(ARCH) \

TOPXSD    := mimosa-osa-cbm-v03-3-1/V3.3.1/02-XSD/\[OSA-CBM-XSD-V03-3\]all.xsd

src_nodir := $(shell ls gen/*.cxx)
src_dir := $(addprefix gen/, $(src_nodir))
obj_dir := $(src_nodir:gen/%.cxx=objs/%.o)

driver: driver.cxx $(obj_dir)  
	$(CXX) $(CXXSTD) $(CXXFLAGS) $(CXXLDFLAGS) $(INCLUDEPATHS) -o $@ $^ $(LIBPATHS) $(NDDSLIBS) $(REFLEXLIB) -lxerces-c 

objs/%.o: gen/%.cxx 
	mkdir -p objs
	$(CXX) $(CXXFLAGS) $(CXXLDFLAGS)  $(CXXSTD) -I$(LIBXSD) -c $< -o $@  

gen: $(TOPXSD)       
	mkdir -p gen
	mkdir -p objs
	$(XSDCXX) cxx-tree --hxx-prologue-file common.h $(XSDSTD) \
        --output-dir gen --file-per-type --generate-ostream \
        --generate-comparison --generate-default-ctor \
        --namespace-map http://www.mimosa.org/OSACBMV3.3.1=osacbm $(TOPXSD)
