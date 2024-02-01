#
# Spearmint Makefile
#
# GNU Make required
#
COMPILE_PLATFORM=$(shell uname | sed -e 's/_.*//' | tr '[:upper:]' '[:lower:]' | sed -e 's/\//_/g')
COMPILE_ARCH=$(shell uname -m | sed -e 's/i.86/x86/' | sed -e 's/^arm.*/arm/')

ifeq ($(COMPILE_PLATFORM),sunos)
  # Solaris uname and GNU uname differ
  COMPILE_ARCH=$(shell uname -p | sed -e 's/i.86/x86/')
endif

ifndef BUILD_GAME_SO
  BUILD_GAME_SO    =
endif
ifndef BUILD_GAME_QVM
  BUILD_GAME_QVM   =
endif
ifndef BUILD_BASEGAME
  BUILD_BASEGAME =
endif
ifndef USE_BASEGAME_MP_HUD
  USE_BASEGAME_MP_HUD =
endif
ifndef BUILD_MISSIONPACK
  BUILD_MISSIONPACK=0
endif
ifndef USE_MISSIONPACK_Q3_UI
  USE_MISSIONPACK_Q3_UI =
endif
ifndef USE_MISSIONPACK_MP_HUD
  USE_MISSIONPACK_MP_HUD = 1
endif
ifndef BUILD_FINAL
  BUILD_FINAL      =0
endif
ifndef USE_YACC
  USE_YACC=0
endif

#############################################################################
#
# If you require a different configuration from the defaults below, create a
# new file named "Makefile.local" in the same directory as this file and define
# your parameters there. This allows you to change configuration without
# causing problems with keeping up to date with the repository.
#
#############################################################################
-include Makefile.local

ifeq ($(COMPILE_PLATFORM),cygwin)
  PLATFORM=mingw32
endif

ifndef PLATFORM
PLATFORM=$(COMPILE_PLATFORM)
endif
export PLATFORM

ifeq ($(PLATFORM),mingw32)
  MINGW=1
endif
ifeq ($(PLATFORM),mingw64)
  MINGW=1
endif

ifeq ($(COMPILE_ARCH),i86pc)
  COMPILE_ARCH=x86
endif

ifeq ($(COMPILE_ARCH),amd64)
  COMPILE_ARCH=x86_64
endif
ifeq ($(COMPILE_ARCH),x64)
  COMPILE_ARCH=x86_64
endif

ifeq ($(COMPILE_ARCH),powerpc)
  COMPILE_ARCH=ppc
endif
ifeq ($(COMPILE_ARCH),powerpc64)
  COMPILE_ARCH=ppc64
endif

ifeq ($(COMPILE_ARCH),axp)
  COMPILE_ARCH=alpha
endif

ifndef ARCH
ARCH=$(COMPILE_ARCH)
endif
export ARCH

ifneq ($(PLATFORM),$(COMPILE_PLATFORM))
  CROSS_COMPILING=1
else
  CROSS_COMPILING=0

  ifneq ($(ARCH),$(COMPILE_ARCH))
    CROSS_COMPILING=1
  endif
endif
export CROSS_COMPILING

ifndef VERSION
VERSION=1.0.3
endif

ifndef VM_PREFIX
VM_PREFIX=mint-
endif

ifndef SOURCE_ARCHIVE
SOURCE_ARCHIVE=mint-metal
endif

ifndef BASEGAME
BASEGAME=fakk
endif

ifndef BASEGAME_CFLAGS
BASEGAME_CFLAGS=

ifeq ($(USE_BASEGAME_MP_HUD), 1)
BASEGAME_CFLAGS+=-DMISSIONPACK_HUD
endif
endif

BASEGAME_CFLAGS+=-DMODDIR=\"$(BASEGAME)\" -DBASETA=\"$(MISSIONPACK)\"

ifndef MISSIONPACK
MISSIONPACK=missionpack
endif

ifndef MISSIONPACK_CFLAGS
MISSIONPACK_CFLAGS=-DMISSIONPACK

ifeq ($(USE_MISSIONPACK_MP_HUD), 1)
MISSIONPACK_CFLAGS+=-DMISSIONPACK_HUD
endif
endif

MISSIONPACK_CFLAGS+=-DMODDIR=\"$(MISSIONPACK)\" -DBASEQ3=\"$(BASEGAME)\"

# Add "-DEXAMPLE" to define EXAMPLE in engine and game/cgame.
ifndef BUILD_DEFINES
BUILD_DEFINES =
endif

ifndef COPYDIR
COPYDIR="/usr/local/games/spearmint"
endif

ifndef COPYBINDIR
COPYBINDIR=$(COPYDIR)
endif

ifndef MOUNT_DIR
MOUNT_DIR=code
endif

ifndef BUILD_DIR
BUILD_DIR=build
endif

ifndef TEMPDIR
TEMPDIR=/tmp
endif

ifndef GENERATE_DEPENDENCIES
GENERATE_DEPENDENCIES=1
endif

ifndef DEBUG_CFLAGS
DEBUG_CFLAGS=-ggdb -O0
endif

#############################################################################

BD=$(BUILD_DIR)/debug-$(PLATFORM)-$(ARCH)
BR=$(BUILD_DIR)/release-$(PLATFORM)-$(ARCH)
CMDIR=$(MOUNT_DIR)/qcommon
BLIBDIR=$(MOUNT_DIR)/botlib
GDIR=$(MOUNT_DIR)/game
CGDIR=$(MOUNT_DIR)/cgame
UIDIR=$(MOUNT_DIR)/ui
Q3UIDIR=$(MOUNT_DIR)/q3_ui
Q3ASMDIR=$(MOUNT_DIR)/tools/asm
LBURGDIR=$(MOUNT_DIR)/tools/lcc/lburg
Q3CPPDIR=$(MOUNT_DIR)/tools/lcc/cpp
Q3LCCETCDIR=$(MOUNT_DIR)/tools/lcc/etc
Q3LCCSRCDIR=$(MOUNT_DIR)/tools/lcc/src

bin_path=$(shell which $(1) 2> /dev/null)

ifneq ($(BUILD_FINAL),1)

# Add git version info
USE_GIT=
ifeq ($(wildcard .git),.git)
  GIT_REV=$(shell git show -s --pretty=format:%ad+%h --date=short | tr -d '-')
  ifneq ($(GIT_REV),)
    VERSION:=$(VERSION)+$(GIT_REV)
    USE_GIT=1
  endif
endif

endif


#############################################################################
# SETUP AND BUILD -- LINUX
#############################################################################

INSTALL=install
MKDIR=mkdir -p

ifneq (,$(findstring "$(COMPILE_PLATFORM)", "linux" "gnu_kfreebsd" "kfreebsd-gnu" "gnu"))
  TOOLS_CFLAGS += -DARCH_STRING=\"$(COMPILE_ARCH)\"
endif

ifneq (,$(findstring "$(PLATFORM)", "linux" "gnu_kfreebsd" "kfreebsd-gnu" "gnu"))
  BASE_CFLAGS = -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes \
    -pipe -DUSE_ICON -DARCH_STRING=\\\"$(ARCH)\\\"

  OPTIMIZEVM = -O3
  OPTIMIZE = $(OPTIMIZEVM) -ffast-math

  ifeq ($(ARCH),x86_64)
    OPTIMIZEVM = -O3
    OPTIMIZE = $(OPTIMIZEVM) -ffast-math
    HAVE_VM_COMPILED = true
  else
  ifeq ($(ARCH),x86)
    OPTIMIZEVM = -O3 -march=i586
    OPTIMIZE = $(OPTIMIZEVM) -ffast-math
    HAVE_VM_COMPILED=true
  else
  ifeq ($(ARCH),ppc)
    ALTIVEC_CFLAGS = -maltivec
    HAVE_VM_COMPILED=true
  endif
  ifeq ($(ARCH),ppc64)
    ALTIVEC_CFLAGS = -maltivec
    HAVE_VM_COMPILED=true
  endif
  ifeq ($(ARCH),sparc)
    OPTIMIZE += -mtune=ultrasparc3 -mv8plus
    OPTIMIZEVM += -mtune=ultrasparc3 -mv8plus
    HAVE_VM_COMPILED=true
  endif
  ifeq ($(ARCH),alpha)
    # According to http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=410555
    # -ffast-math will cause the client to die with SIGFPE on Alpha
    OPTIMIZE = $(OPTIMIZEVM)
  endif
  endif
  endif

  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC -fvisibility=hidden
  SHLIBLDFLAGS=-shared $(LDFLAGS)

  THREAD_LIBS=-lpthread
  LIBS=-ldl -lm

  ifeq ($(ARCH),x86)
    # linux32 make ...
    BASE_CFLAGS += -m32
  else
  ifeq ($(ARCH),ppc64)
    BASE_CFLAGS += -m64
  endif
  endif
else # ifeq Linux

#############################################################################
# SETUP AND BUILD -- MAC OS X
#############################################################################

ifeq ($(PLATFORM),darwin)
  HAVE_VM_COMPILED=true
  LIBS = -framework Cocoa
  OPTIMIZEVM = -O3

  # Default minimum Mac OS X version
  ifeq ($(MACOSX_VERSION_MIN),)
    MACOSX_VERSION_MIN=10.7
  endif

  MACOSX_MAJOR=$(shell echo $(MACOSX_VERSION_MIN) | cut -d. -f1)
  MACOSX_MINOR=$(shell echo $(MACOSX_VERSION_MIN) | cut -d. -f2)
  ifeq ($(shell test $(MACOSX_MINOR) -gt 9; echo $$?),0)
    # Multiply and then remove decimal. 10.10 -> 101000.0 -> 101000
    MAC_OS_X_VERSION_MIN_REQUIRED=$(shell echo "$(MACOSX_MAJOR) * 10000 + $(MACOSX_MINOR) * 100" | bc | cut -d. -f1)
  else
    # Multiply by 100 and then remove decimal. 10.7 -> 1070.0 -> 1070
    MAC_OS_X_VERSION_MIN_REQUIRED=$(shell echo "$(MACOSX_VERSION_MIN) * 100" | bc | cut -d. -f1)
  endif

  LDFLAGS += -mmacosx-version-min=$(MACOSX_VERSION_MIN)
  BASE_CFLAGS += -mmacosx-version-min=$(MACOSX_VERSION_MIN) \
                 -DMAC_OS_X_VERSION_MIN_REQUIRED=$(MAC_OS_X_VERSION_MIN_REQUIRED)

  ifeq ($(ARCH),ppc)
    BASE_CFLAGS += -arch ppc
    ALTIVEC_CFLAGS = -faltivec
  endif
  ifeq ($(ARCH),ppc64)
    BASE_CFLAGS += -arch ppc64
    ALTIVEC_CFLAGS = -faltivec
  endif
  ifeq ($(ARCH),x86)
    OPTIMIZEVM += -march=prescott -mfpmath=sse
    # x86 vm will crash without -mstackrealign since MMX instructions will be
    # used no matter what and they corrupt the frame pointer in VM calls
    BASE_CFLAGS += -arch i386 -m32 -mstackrealign
  endif
  ifeq ($(ARCH),x86_64)
    OPTIMIZEVM += -mfpmath=sse
    BASE_CFLAGS += -arch x86_64
  endif

  # When compiling on OSX for OSX, we're not cross compiling as far as the
  # Makefile is concerned, as target architecture is specified as a compiler
  # argument
  ifeq ($(COMPILE_PLATFORM),darwin)
    CROSS_COMPILING=0
  endif

  ifeq ($(CROSS_COMPILING),1)
    ifeq ($(ARCH),x86_64)
      CC=x86_64-apple-darwin13-cc
      RANLIB=x86_64-apple-darwin13-ranlib
    else
      ifeq ($(ARCH),x86)
        CC=i386-apple-darwin13-cc
        RANLIB=i386-apple-darwin13-ranlib
      else
        $(error Architecture $(ARCH) is not supported when cross compiling)
      endif
    endif
  endif

  BASE_CFLAGS += -fno-strict-aliasing -fno-common -pipe

  BASE_CFLAGS += -D_THREAD_SAFE=1

  OPTIMIZE = $(OPTIMIZEVM) -ffast-math

  SHLIBEXT=dylib
  SHLIBCFLAGS=-fPIC -fno-common
  SHLIBLDFLAGS=-dynamiclib $(LDFLAGS) -Wl,-U,_com_altivec

  NOTSHLIBCFLAGS=-mdynamic-no-pic

else # ifeq darwin


#############################################################################
# SETUP AND BUILD -- MINGW32
#############################################################################

ifdef MINGW

  ifeq ($(CROSS_COMPILING),1)
    # If CC is already set to something generic, we probably want to use
    # something more specific
    ifneq ($(findstring $(strip $(CC)),cc gcc),)
      CC=
    endif

    # We need to figure out the correct gcc and windres
    ifeq ($(ARCH),x86_64)
      MINGW_PREFIXES=x86_64-w64-mingw32 amd64-mingw32msvc
    endif
    ifeq ($(ARCH),x86)
      MINGW_PREFIXES=i686-w64-mingw32 i586-mingw32msvc i686-pc-mingw32
    endif

    ifndef CC
      CC=$(firstword $(strip $(foreach MINGW_PREFIX, $(MINGW_PREFIXES), \
         $(call bin_path, $(MINGW_PREFIX)-gcc))))
    endif

    ifndef WINDRES
      WINDRES=$(firstword $(strip $(foreach MINGW_PREFIX, $(MINGW_PREFIXES), \
         $(call bin_path, $(MINGW_PREFIX)-windres))))
    endif
  else
    # Some MinGW installations define CC to cc, but don't actually provide cc,
    # so check that CC points to a real binary and use gcc if it doesn't
    ifeq ($(call bin_path, $(CC)),)
      CC=gcc
    endif

  endif

  # using generic windres if specific one is not present
  ifndef WINDRES
    WINDRES=windres
  endif

  BASE_CFLAGS = -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes \
    -DUSE_ICON

  # In the absence of wspiapi.h, require Windows XP or later
  ifeq ($(shell test -e $(CMDIR)/wspiapi.h; echo $$?),1)
    BASE_CFLAGS += -DWINVER=0x501
  endif

  ifeq ($(ARCH),x86_64)
    OPTIMIZEVM = -O3
    OPTIMIZE = $(OPTIMIZEVM) -ffast-math
    HAVE_VM_COMPILED = true
  endif
  ifeq ($(ARCH),x86)
    OPTIMIZEVM = -O3 -march=i586
    OPTIMIZE = $(OPTIMIZEVM) -ffast-math
    HAVE_VM_COMPILED = true
  endif

  SHLIBEXT=dll
  SHLIBCFLAGS=
  SHLIBLDFLAGS=-shared $(LDFLAGS)

  BINEXT=.exe

  ifeq ($(CROSS_COMPILING),0)
    TOOLS_BINEXT=.exe
  endif

  ifeq ($(COMPILE_PLATFORM),cygwin)
    TOOLS_BINEXT=.exe

    # Under cygwin the default of using gcc for TOOLS_CC won't work, so
    # we need to figure out the appropriate compiler to use, based on the
    # host architecture that we're running under (as tools run on the host)
    ifeq ($(COMPILE_ARCH),x86_64)
      TOOLS_MINGW_PREFIXES=x86_64-w64-mingw32 amd64-mingw32msvc
    endif
    ifeq ($(COMPILE_ARCH),x86)
      TOOLS_MINGW_PREFIXES=i686-w64-mingw32 i586-mingw32msvc i686-pc-mingw32
    endif

    TOOLS_CC=$(firstword $(strip $(foreach TOOLS_MINGW_PREFIX, $(TOOLS_MINGW_PREFIXES), \
      $(call bin_path, $(TOOLS_MINGW_PREFIX)-gcc))))
  endif

  LIBS= -lws2_32 -lwinmm -lpsapi

  ifeq ($(ARCH),x86)
    # build 32bit
    BASE_CFLAGS += -m32
  else
    BASE_CFLAGS += -m64
  endif

else # ifdef MINGW

#############################################################################
# SETUP AND BUILD -- FREEBSD
#############################################################################

ifeq ($(PLATFORM),freebsd)

  # flags
  BASE_CFLAGS = \
    -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes \
    -DUSE_ICON -DMAP_ANONYMOUS=MAP_ANON
  HAVE_VM_COMPILED = true

  OPTIMIZEVM =
  OPTIMIZE = $(OPTIMIZEVM) -ffast-math

  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC
  SHLIBLDFLAGS=-shared $(LDFLAGS)

  THREAD_LIBS=-lpthread
  # don't need -ldl (FreeBSD)
  LIBS=-lm

  # cross-compiling tweaks
  ifeq ($(ARCH),x86)
    ifeq ($(CROSS_COMPILING),1)
      BASE_CFLAGS += -m32
    endif
  endif
  ifeq ($(ARCH),x86_64)
    ifeq ($(CROSS_COMPILING),1)
      BASE_CFLAGS += -m64
    endif
  endif
else # ifeq freebsd

#############################################################################
# SETUP AND BUILD -- OPENBSD
#############################################################################

ifeq ($(PLATFORM),openbsd)

  BASE_CFLAGS = -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes \
    -pipe -DUSE_ICON -DMAP_ANONYMOUS=MAP_ANON
  CLIENT_CFLAGS += $(SDL_CFLAGS)

  OPTIMIZEVM = -O3
  OPTIMIZE = $(OPTIMIZEVM) -ffast-math

  ifeq ($(ARCH),x86_64)
    OPTIMIZEVM = -O3
    OPTIMIZE = $(OPTIMIZEVM) -ffast-math
    HAVE_VM_COMPILED = true
  else
  ifeq ($(ARCH),x86)
    OPTIMIZEVM = -O3 -march=i586
    OPTIMIZE = $(OPTIMIZEVM) -ffast-math
    HAVE_VM_COMPILED=true
  else
  ifeq ($(ARCH),ppc)
    ALTIVEC_CFLAGS = -maltivec
    HAVE_VM_COMPILED=true
  endif
  ifeq ($(ARCH),ppc64)
    ALTIVEC_CFLAGS = -maltivec
    HAVE_VM_COMPILED=true
  endif
  ifeq ($(ARCH),sparc64)
    OPTIMIZE += -mtune=ultrasparc3 -mv8plus
    OPTIMIZEVM += -mtune=ultrasparc3 -mv8plus
    HAVE_VM_COMPILED=true
  endif
  ifeq ($(ARCH),alpha)
    # According to http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=410555
    # -ffast-math will cause the client to die with SIGFPE on Alpha
    OPTIMIZE = $(OPTIMIZEVM)
  endif
  endif
  endif

  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC
  SHLIBLDFLAGS=-shared $(LDFLAGS)

  THREAD_LIBS=-lpthread
  LIBS=-lm
else # ifeq openbsd

#############################################################################
# SETUP AND BUILD -- NETBSD
#############################################################################

ifeq ($(PLATFORM),netbsd)

  LIBS=-lm
  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC
  SHLIBLDFLAGS=-shared $(LDFLAGS)
  THREAD_LIBS=-lpthread

  BASE_CFLAGS = -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes

  ifeq ($(ARCH),x86)
    HAVE_VM_COMPILED=true
  endif
else # ifeq netbsd

#############################################################################
# SETUP AND BUILD -- IRIX
#############################################################################

ifeq ($(PLATFORM),irix64)
  LIB=lib

  ARCH=mips

  CC = c99

  BASE_CFLAGS=-Dstricmp=strcasecmp -Xcpluscomm -woff 1185 \
    -I. -I$(ROOT)/usr/include
  OPTIMIZE = -O3

  SHLIBEXT=so
  SHLIBCFLAGS=
  SHLIBLDFLAGS=-shared

  LIBS=-ldl -lm -lgen

else # ifeq IRIX

#############################################################################
# SETUP AND BUILD -- SunOS
#############################################################################

ifeq ($(PLATFORM),sunos)

  CC=gcc
  INSTALL=ginstall
  MKDIR=gmkdir -p
  COPYDIR="/usr/local/share/games/spearmint"

  ifneq ($(ARCH),x86)
    ifneq ($(ARCH),sparc)
      $(error arch $(ARCH) is currently not supported)
    endif
  endif

  BASE_CFLAGS = -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes \
    -pipe -DUSE_ICON
  CLIENT_CFLAGS += $(SDL_CFLAGS)

  OPTIMIZEVM = -O3 -funroll-loops

  ifeq ($(ARCH),sparc)
    OPTIMIZEVM += -O3 \
      -fstrength-reduce -falign-functions=2 \
      -mtune=ultrasparc3 -mv8plus -mno-faster-structs
    HAVE_VM_COMPILED=true
  else
  ifeq ($(ARCH),x86)
    OPTIMIZEVM += -march=i586 -fomit-frame-pointer \
      -falign-functions=2 -fstrength-reduce
    HAVE_VM_COMPILED=true
    BASE_CFLAGS += -m32
    CLIENT_CFLAGS += -I/usr/X11/include/NVIDIA
    CLIENT_LDFLAGS += -L/usr/X11/lib/NVIDIA -R/usr/X11/lib/NVIDIA
  endif
  endif

  OPTIMIZE = $(OPTIMIZEVM) -ffast-math

  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC
  SHLIBLDFLAGS=-shared $(LDFLAGS)

  THREAD_LIBS=-lpthread
  LIBS=-lsocket -lnsl -ldl -lm

  BOTCFLAGS=-O0

else # ifeq sunos

#############################################################################
# SETUP AND BUILD -- GENERIC
#############################################################################
  BASE_CFLAGS=
  OPTIMIZE = -O3

  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC
  SHLIBLDFLAGS=-shared

endif #Linux
endif #darwin
endif #MINGW
endif #FreeBSD
endif #OpenBSD
endif #NetBSD
endif #IRIX
endif #SunOS

ifndef CC
  CC=gcc
endif

ifndef RANLIB
  RANLIB=ranlib
endif

ifneq ($(HAVE_VM_COMPILED),true)
  BASE_CFLAGS += -DNO_VM_COMPILED
endif

TARGETS =

ifndef FULLBINEXT
  FULLBINEXT=_$(ARCH)$(BINEXT)
endif

ifndef SHLIBNAME
  SHLIBNAME=$(ARCH).$(SHLIBEXT)
endif

ifneq ($(BUILD_GAME_SO),0)
  ifneq ($(BUILD_BASEGAME),0)
    TARGETS += \
      $(B)/$(BASEGAME)/$(VM_PREFIX)cgame_$(SHLIBNAME) \
      $(B)/$(BASEGAME)/$(VM_PREFIX)game_$(SHLIBNAME)
  endif
  ifneq ($(BUILD_MISSIONPACK),0)
    TARGETS += \
      $(B)/$(MISSIONPACK)/$(VM_PREFIX)cgame_$(SHLIBNAME) \
      $(B)/$(MISSIONPACK)/$(VM_PREFIX)game_$(SHLIBNAME)
  endif
endif

ifneq ($(BUILD_GAME_QVM),0)
  ifneq ($(BUILD_BASEGAME),0)
    TARGETS += \
      $(B)/$(BASEGAME)/vm/$(VM_PREFIX)cgame.qvm \
      $(B)/$(BASEGAME)/vm/$(VM_PREFIX)game.qvm
  endif
  ifneq ($(BUILD_MISSIONPACK),0)
    TARGETS += \
      $(B)/$(MISSIONPACK)/vm/$(VM_PREFIX)cgame.qvm \
      $(B)/$(MISSIONPACK)/vm/$(VM_PREFIX)game.qvm
  endif
endif

ifeq ("$(CC)", $(findstring "$(CC)", "clang" "clang++"))
  BASE_CFLAGS += -Qunused-arguments
endif

ifeq ($(GENERATE_DEPENDENCIES),1)
  DEPEND_CFLAGS = -MMD
else
  DEPEND_CFLAGS =
endif

ifeq ($(NO_STRIP),1)
  STRIP_FLAG =
else
  STRIP_FLAG = -s
endif

# https://reproducible-builds.org/specs/source-date-epoch/
ifdef SOURCE_DATE_EPOCH
  BASE_BUILD_DEFINES += -DPRODUCT_DATE=\\\"$(shell date --date="@$$SOURCE_DATE_EPOCH" "+%b %_d %Y" | sed -e 's/ /\\\ /g')\\\"
endif

BASE_BUILD_DEFINES += -DPRODUCT_VERSION=\\\"$(VERSION)\\\"
ifeq ($(USE_GIT),1)
  BASE_BUILD_DEFINES += -DPRODUCT_VERSION_HAS_DATE
endif

BASE_CFLAGS += -Wformat=2 -Wno-format-zero-length -Wformat-security -Wno-format-nonliteral
BASE_CFLAGS += -Wstrict-aliasing=2 -Wmissing-format-attribute
BASE_CFLAGS += -Wdisabled-optimization
BASE_CFLAGS += -Werror-implicit-function-declaration

ifeq ($(V),1)
echo_cmd=@:
Q=
else
echo_cmd=@echo
Q=@
endif

ifeq ($(GENERATE_DEPENDENCIES),1)
  DO_QVM_DEP=cat $(@:%.o=%.d) | sed -e 's/\.o/\.asm/g' >> $(@:%.o=%.d)
endif

define DO_SHLIB_CC
$(echo_cmd) "SHLIB_CC $<"
$(Q)$(CC) $(BASEGAME_CFLAGS) $(SHLIBCFLAGS) $(CFLAGS) $(OPTIMIZEVM) -o $@ -c $<
$(Q)$(DO_QVM_DEP)
endef

define DO_GAME_CC
$(echo_cmd) "GAME_CC $<"
$(Q)$(CC) $(BASEGAME_CFLAGS) -DGAME $(SHLIBCFLAGS) $(CFLAGS) $(OPTIMIZEVM) -o $@ -c $<
$(Q)$(DO_QVM_DEP)
endef

define DO_CGAME_CC
$(echo_cmd) "CGAME_CC $<"
$(Q)$(CC) $(BASEGAME_CFLAGS) -DCGAME $(SHLIBCFLAGS) $(CFLAGS) $(OPTIMIZEVM) -o $@ -c $<
$(Q)$(DO_QVM_DEP)
endef

define DO_SHLIB_CC_MISSIONPACK
$(echo_cmd) "SHLIB_CC_MISSIONPACK $<"
$(Q)$(CC) $(MISSIONPACK_CFLAGS) $(SHLIBCFLAGS) $(CFLAGS) $(OPTIMIZEVM) -o $@ -c $<
$(Q)$(DO_QVM_DEP)
endef

define DO_GAME_CC_MISSIONPACK
$(echo_cmd) "GAME_CC_MISSIONPACK $<"
$(Q)$(CC) $(MISSIONPACK_CFLAGS) -DGAME $(SHLIBCFLAGS) $(CFLAGS) $(OPTIMIZEVM) -o $@ -c $<
$(Q)$(DO_QVM_DEP)
endef

define DO_CGAME_CC_MISSIONPACK
$(echo_cmd) "CGAME_CC_MISSIONPACK $<"
$(Q)$(CC) $(MISSIONPACK_CFLAGS) -DCGAME $(SHLIBCFLAGS) $(CFLAGS) $(OPTIMIZEVM) -o $@ -c $<
$(Q)$(DO_QVM_DEP)
endef


#############################################################################
# MAIN TARGETS
#############################################################################

default: release
all: debug release

debug:
	@$(MAKE) targets B=$(BD) CFLAGS="$(CFLAGS) $(BASE_BUILD_DEFINES) $(BASE_CFLAGS) $(BUILD_DEFINES) $(DEPEND_CFLAGS)" \
	  OPTIMIZE="$(DEBUG_CFLAGS)" OPTIMIZEVM="$(DEBUG_CFLAGS)" \
	  CLIENT_CFLAGS="$(CLIENT_CFLAGS)" SERVER_CFLAGS="$(SERVER_CFLAGS)" V=$(V) \
	  QVM_CFLAGS="$(BASE_BUILD_DEFINES) $(BUILD_DEFINES)"

release:
	@$(MAKE) targets B=$(BR) CFLAGS="$(CFLAGS) $(BASE_BUILD_DEFINES) $(BASE_CFLAGS) $(BUILD_DEFINES) $(DEPEND_CFLAGS)" \
	  OPTIMIZE="-DNDEBUG $(OPTIMIZE)" OPTIMIZEVM="-DNDEBUG $(OPTIMIZEVM)" \
	  CLIENT_CFLAGS="$(CLIENT_CFLAGS)" SERVER_CFLAGS="$(SERVER_CFLAGS)" V=$(V) \
	  QVM_CFLAGS="-DNDEBUG $(BASE_BUILD_DEFINES) $(BUILD_DEFINES)"

ifneq ($(call bin_path, tput),)
  TERM_COLUMNS=$(shell if c=`tput cols`; then echo $$(($$c-4)); else echo 76; fi)
else
  TERM_COLUMNS=76
endif

NAKED_TARGETS=$(shell echo $(TARGETS) | sed -e "s!$(B)/!!g")

print_list=-@for i in $(1); \
     do \
             echo "    $$i"; \
     done

ifneq ($(call bin_path, fmt),)
  print_wrapped=@echo $(1) | fmt -w $(TERM_COLUMNS) | sed -e "s/^\(.*\)$$/    \1/"
else
  print_wrapped=$(print_list)
endif

# Create the build directories, check libraries and print out
# an informational message, then start building
targets: makedirs
	@echo ""
	@echo "Building in $(B):"
	@echo "  PLATFORM: $(PLATFORM)"
	@echo "  ARCH: $(ARCH)"
	@echo "  VERSION: $(VERSION)"
	@echo "  COMPILE_PLATFORM: $(COMPILE_PLATFORM)"
	@echo "  COMPILE_ARCH: $(COMPILE_ARCH)"
	@echo "  CC: $(CC)"
ifeq ($(PLATFORM),mingw32)
	@echo "  WINDRES: $(WINDRES)"
endif
	@echo ""
	@echo "  CFLAGS:"
	$(call print_wrapped, $(CFLAGS) $(OPTIMIZE))
	@echo ""
	@echo "  TOOLS_CFLAGS:"
	$(call print_wrapped, $(TOOLS_CFLAGS))
	@echo ""
	@echo "  LDFLAGS:"
	$(call print_wrapped, $(LDFLAGS))
	@echo ""
	@echo "  LIBS:"
	$(call print_wrapped, $(LIBS))
	@echo ""
	@echo "  Output:"
	$(call print_list, $(NAKED_TARGETS))
	@echo ""
ifneq ($(TARGETS),)
  ifndef DEBUG_MAKEFILE
	@$(MAKE) $(TARGETS) $(B).zip V=$(V)
  endif
endif

$(B).zip: $(TARGETS)
ifdef ARCHIVE
	@rm -f $@
	@(cd $(B) && zip -r9 ../../$@ $(NAKED_TARGETS))
endif

makedirs:
	@$(MKDIR) $(B)/$(BASEGAME)/cgame
	@$(MKDIR) $(B)/$(BASEGAME)/botlib
	@$(MKDIR) $(B)/$(BASEGAME)/game
	@$(MKDIR) $(B)/$(BASEGAME)/ui
	@$(MKDIR) $(B)/$(BASEGAME)/mpui
	@$(MKDIR) $(B)/$(BASEGAME)/qcommon
	@$(MKDIR) $(B)/$(BASEGAME)/vm
	@$(MKDIR) $(B)/$(MISSIONPACK)/cgame
	@$(MKDIR) $(B)/$(MISSIONPACK)/botlib
	@$(MKDIR) $(B)/$(MISSIONPACK)/game
	@$(MKDIR) $(B)/$(MISSIONPACK)/ui
	@$(MKDIR) $(B)/$(MISSIONPACK)/qcommon
	@$(MKDIR) $(B)/$(MISSIONPACK)/q3ui
	@$(MKDIR) $(B)/$(MISSIONPACK)/vm
	@$(MKDIR) $(B)/tools/asm
	@$(MKDIR) $(B)/tools/etc
	@$(MKDIR) $(B)/tools/rcc
	@$(MKDIR) $(B)/tools/cpp
	@$(MKDIR) $(B)/tools/lburg

#############################################################################
# QVM BUILD TOOLS
#############################################################################

ifndef TOOLS_CC
  # A compiler which probably produces native binaries
  TOOLS_CC = gcc
endif

ifndef YACC
  YACC = yacc
endif

TOOLS_OPTIMIZE = -g -Wall -fno-strict-aliasing
TOOLS_CFLAGS += $(TOOLS_OPTIMIZE) \
                -DTEMPDIR=\"$(TEMPDIR)\" -DSYSTEM=\"\" \
                -I$(Q3LCCSRCDIR) \
                -I$(LBURGDIR)
TOOLS_LIBS =
TOOLS_LDFLAGS =

ifeq ($(GENERATE_DEPENDENCIES),1)
  TOOLS_CFLAGS += -MMD
endif

define DO_YACC
$(echo_cmd) "YACC $<"
$(Q)$(YACC) $<
$(Q)mv -f y.tab.c $@
endef

define DO_TOOLS_CC
$(echo_cmd) "TOOLS_CC $<"
$(Q)$(TOOLS_CC) $(TOOLS_CFLAGS) -o $@ -c $<
endef

define DO_TOOLS_CC_DAGCHECK
$(echo_cmd) "TOOLS_CC_DAGCHECK $<"
$(Q)$(TOOLS_CC) $(TOOLS_CFLAGS) -Wno-unused -o $@ -c $<
endef

LBURG       = $(B)/tools/lburg/lburg$(TOOLS_BINEXT)
DAGCHECK_C  = $(B)/tools/rcc/dagcheck.c
Q3RCC       = $(B)/tools/q3rcc$(TOOLS_BINEXT)
Q3CPP       = $(B)/tools/q3cpp$(TOOLS_BINEXT)
Q3LCC       = $(B)/tools/q3lcc$(TOOLS_BINEXT)
Q3ASM       = $(B)/tools/q3asm$(TOOLS_BINEXT)

LBURGOBJ= \
  $(B)/tools/lburg/lburg.o \
  $(B)/tools/lburg/gram.o

# override GNU Make built-in rule for converting gram.y to gram.c
%.c: %.y
ifeq ($(USE_YACC),1)
	$(DO_YACC)
endif

$(B)/tools/lburg/%.o: $(LBURGDIR)/%.c
	$(DO_TOOLS_CC)

$(LBURG): $(LBURGOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(TOOLS_CC) $(TOOLS_CFLAGS) $(TOOLS_LDFLAGS) -o $@ $^ $(TOOLS_LIBS)

Q3RCCOBJ = \
  $(B)/tools/rcc/alloc.o \
  $(B)/tools/rcc/bind.o \
  $(B)/tools/rcc/bytecode.o \
  $(B)/tools/rcc/dag.o \
  $(B)/tools/rcc/dagcheck.o \
  $(B)/tools/rcc/decl.o \
  $(B)/tools/rcc/enode.o \
  $(B)/tools/rcc/error.o \
  $(B)/tools/rcc/event.o \
  $(B)/tools/rcc/expr.o \
  $(B)/tools/rcc/gen.o \
  $(B)/tools/rcc/init.o \
  $(B)/tools/rcc/inits.o \
  $(B)/tools/rcc/input.o \
  $(B)/tools/rcc/lex.o \
  $(B)/tools/rcc/list.o \
  $(B)/tools/rcc/main.o \
  $(B)/tools/rcc/null.o \
  $(B)/tools/rcc/output.o \
  $(B)/tools/rcc/prof.o \
  $(B)/tools/rcc/profio.o \
  $(B)/tools/rcc/simp.o \
  $(B)/tools/rcc/stmt.o \
  $(B)/tools/rcc/string.o \
  $(B)/tools/rcc/sym.o \
  $(B)/tools/rcc/symbolic.o \
  $(B)/tools/rcc/trace.o \
  $(B)/tools/rcc/tree.o \
  $(B)/tools/rcc/types.o

$(DAGCHECK_C): $(LBURG) $(Q3LCCSRCDIR)/dagcheck.md
	$(echo_cmd) "LBURG $(Q3LCCSRCDIR)/dagcheck.md"
	$(Q)$(LBURG) $(Q3LCCSRCDIR)/dagcheck.md $@

$(B)/tools/rcc/dagcheck.o: $(DAGCHECK_C)
	$(DO_TOOLS_CC_DAGCHECK)

$(B)/tools/rcc/%.o: $(Q3LCCSRCDIR)/%.c
	$(DO_TOOLS_CC)

$(Q3RCC): $(Q3RCCOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(TOOLS_CC) $(TOOLS_CFLAGS) $(TOOLS_LDFLAGS) -o $@ $^ $(TOOLS_LIBS)

Q3CPPOBJ = \
  $(B)/tools/cpp/cpp.o \
  $(B)/tools/cpp/lex.o \
  $(B)/tools/cpp/nlist.o \
  $(B)/tools/cpp/tokens.o \
  $(B)/tools/cpp/macro.o \
  $(B)/tools/cpp/eval.o \
  $(B)/tools/cpp/include.o \
  $(B)/tools/cpp/hideset.o \
  $(B)/tools/cpp/getopt.o \
  $(B)/tools/cpp/unix.o

$(B)/tools/cpp/%.o: $(Q3CPPDIR)/%.c
	$(DO_TOOLS_CC)

$(Q3CPP): $(Q3CPPOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(TOOLS_CC) $(TOOLS_CFLAGS) $(TOOLS_LDFLAGS) -o $@ $^ $(TOOLS_LIBS)

Q3LCCOBJ = \
	$(B)/tools/etc/lcc.o \
	$(B)/tools/etc/bytecode.o

$(B)/tools/etc/%.o: $(Q3LCCETCDIR)/%.c
	$(DO_TOOLS_CC)

$(Q3LCC): $(Q3LCCOBJ) $(Q3RCC) $(Q3CPP)
	$(echo_cmd) "LD $@"
	$(Q)$(TOOLS_CC) $(TOOLS_CFLAGS) $(TOOLS_LDFLAGS) -o $@ $(Q3LCCOBJ) $(TOOLS_LIBS)

define DO_Q3LCC
$(echo_cmd) "Q3LCC $<"
$(Q)$(Q3LCC) $(BASEGAME_CFLAGS) $(QVM_CFLAGS) -o $@ $<
endef

define DO_CGAME_Q3LCC
$(echo_cmd) "CGAME_Q3LCC $<"
$(Q)$(Q3LCC) $(BASEGAME_CFLAGS) -DCGAME $(QVM_CFLAGS) -o $@ $<
endef

define DO_GAME_Q3LCC
$(echo_cmd) "GAME_Q3LCC $<"
$(Q)$(Q3LCC) $(BASEGAME_CFLAGS) -DGAME $(QVM_CFLAGS) -o $@ $<
endef

define DO_Q3LCC_MISSIONPACK
$(echo_cmd) "Q3LCC_MISSIONPACK $<"
$(Q)$(Q3LCC) $(MISSIONPACK_CFLAGS) $(QVM_CFLAGS) -o $@ $<
endef

define DO_CGAME_Q3LCC_MISSIONPACK
$(echo_cmd) "CGAME_Q3LCC_MISSIONPACK $<"
$(Q)$(Q3LCC) $(MISSIONPACK_CFLAGS) -DCGAME $(QVM_CFLAGS) -o $@ $<
endef

define DO_GAME_Q3LCC_MISSIONPACK
$(echo_cmd) "GAME_Q3LCC_MISSIONPACK $<"
$(Q)$(Q3LCC) $(MISSIONPACK_CFLAGS) -DGAME $(QVM_CFLAGS) -o $@ $<
endef


Q3ASMOBJ = \
  $(B)/tools/asm/q3asm.o \
  $(B)/tools/asm/cmdlib.o

$(B)/tools/asm/%.o: $(Q3ASMDIR)/%.c
	$(DO_TOOLS_CC)

$(Q3ASM): $(Q3ASMOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(TOOLS_CC) $(TOOLS_CFLAGS) $(TOOLS_LDFLAGS) -o $@ $^ $(TOOLS_LIBS)


#############################################################################
## BASEGAME CGAME
#############################################################################

Q3CGOBJ = \
  $(B)/$(BASEGAME)/cgame/cg_main.o \
  $(B)/$(BASEGAME)/cgame/bg_misc.o \
  $(B)/$(BASEGAME)/cgame/bg_pmove.o \
  $(B)/$(BASEGAME)/cgame/bg_slidemove.o \
  $(B)/$(BASEGAME)/cgame/bg_lib.o \
  $(B)/$(BASEGAME)/cgame/bg_tracemap.o \
  $(B)/$(BASEGAME)/cgame/cg_atmospheric.o \
  $(B)/$(BASEGAME)/cgame/cg_console.o \
  $(B)/$(BASEGAME)/cgame/cg_consolecmds.o \
  $(B)/$(BASEGAME)/cgame/cg_newdraw.o \
  $(B)/$(BASEGAME)/cgame/cg_draw.o \
  $(B)/$(BASEGAME)/cgame/cg_drawtools.o \
  $(B)/$(BASEGAME)/cgame/cg_effects.o \
  $(B)/$(BASEGAME)/cgame/cg_ents.o \
  $(B)/$(BASEGAME)/cgame/cg_event.o \
  $(B)/$(BASEGAME)/cgame/cg_field.o \
  $(B)/$(BASEGAME)/cgame/cg_info.o \
  $(B)/$(BASEGAME)/cgame/cg_input.o \
  $(B)/$(BASEGAME)/cgame/cg_localents.o \
  $(B)/$(BASEGAME)/cgame/cg_marks.o \
  $(B)/$(BASEGAME)/cgame/cg_particles.o \
  $(B)/$(BASEGAME)/cgame/cg_players.o \
  $(B)/$(BASEGAME)/cgame/cg_playerstate.o \
  $(B)/$(BASEGAME)/cgame/cg_polybus.o \
  $(B)/$(BASEGAME)/cgame/cg_predict.o \
  $(B)/$(BASEGAME)/cgame/cg_ritualfont.o \
  $(B)/$(BASEGAME)/cgame/cg_scoreboard.o \
  $(B)/$(BASEGAME)/cgame/cg_servercmds.o \
  $(B)/$(BASEGAME)/cgame/cg_snapshot.o \
  $(B)/$(BASEGAME)/cgame/cg_spawn.o \
  $(B)/$(BASEGAME)/cgame/cg_surface.o \
  $(B)/$(BASEGAME)/cgame/cg_syscalls.o \
  $(B)/$(BASEGAME)/cgame/cg_text.o \
  $(B)/$(BASEGAME)/cgame/cg_unlagged.o \
  $(B)/$(BASEGAME)/cgame/cg_view.o \
  $(B)/$(BASEGAME)/cgame/cg_weapons.o \
  \
  $(B)/$(BASEGAME)/ui/ui_main.o \
  $(B)/$(BASEGAME)/ui/ui_addbots.o \
  $(B)/$(BASEGAME)/ui/ui_atoms.o \
  $(B)/$(BASEGAME)/ui/ui_cinematics.o \
  $(B)/$(BASEGAME)/ui/ui_confirm.o \
  $(B)/$(BASEGAME)/ui/ui_connect.o \
  $(B)/$(BASEGAME)/ui/ui_controls2.o \
  $(B)/$(BASEGAME)/ui/ui_credits.o \
  $(B)/$(BASEGAME)/ui/ui_demo2.o \
  $(B)/$(BASEGAME)/ui/ui_display.o \
  $(B)/$(BASEGAME)/ui/ui_gameinfo.o \
  $(B)/$(BASEGAME)/ui/ui_ingame.o \
  $(B)/$(BASEGAME)/ui/ui_ingame_selectplayer.o \
  $(B)/$(BASEGAME)/ui/ui_joystick.o \
  $(B)/$(BASEGAME)/ui/ui_loadconfig.o \
  $(B)/$(BASEGAME)/ui/ui_menu.o \
  $(B)/$(BASEGAME)/ui/ui_mfield.o \
  $(B)/$(BASEGAME)/ui/ui_mods.o \
  $(B)/$(BASEGAME)/ui/ui_network.o \
  $(B)/$(BASEGAME)/ui/ui_options.o \
  $(B)/$(BASEGAME)/ui/ui_playermodel.o \
  $(B)/$(BASEGAME)/ui/ui_players.o \
  $(B)/$(BASEGAME)/ui/ui_playersettings.o \
  $(B)/$(BASEGAME)/ui/ui_preferences.o \
  $(B)/$(BASEGAME)/ui/ui_qmenu.o \
  $(B)/$(BASEGAME)/ui/ui_removebots.o \
  $(B)/$(BASEGAME)/ui/ui_saveconfig.o \
  $(B)/$(BASEGAME)/ui/ui_selectplayer.o \
  $(B)/$(BASEGAME)/ui/ui_serverinfo.o \
  $(B)/$(BASEGAME)/ui/ui_servers2.o \
  $(B)/$(BASEGAME)/ui/ui_setup.o \
  $(B)/$(BASEGAME)/ui/ui_sound.o \
  $(B)/$(BASEGAME)/ui/ui_sparena.o \
  $(B)/$(BASEGAME)/ui/ui_specifyserver.o \
  $(B)/$(BASEGAME)/ui/ui_splevel.o \
  $(B)/$(BASEGAME)/ui/ui_sppostgame.o \
  $(B)/$(BASEGAME)/ui/ui_spskill.o \
  $(B)/$(BASEGAME)/ui/ui_startserver.o \
  $(B)/$(BASEGAME)/ui/ui_team.o \
  $(B)/$(BASEGAME)/ui/ui_teamorders.o \
  $(B)/$(BASEGAME)/ui/ui_video.o \
  \
  $(B)/$(BASEGAME)/qcommon/q_math.o \
  $(B)/$(BASEGAME)/qcommon/q_shared.o \
  $(B)/$(BASEGAME)/qcommon/q_unicode.o

ifeq ($(USE_BASEGAME_MP_HUD), 1)
Q3CGOBJ += \
  $(B)/$(BASEGAME)/mpui/ui_shared.o
endif

Q3CGVMOBJ = $(Q3CGOBJ:%.o=%.asm)

$(B)/$(BASEGAME)/$(VM_PREFIX)cgame_$(SHLIBNAME): $(Q3CGOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(Q3CGOBJ)

$(B)/$(BASEGAME)/vm/$(VM_PREFIX)cgame.qvm: $(Q3CGVMOBJ) $(GDIR)/bg_syscalls.asm $(Q3ASM)
	$(echo_cmd) "Q3ASM $@"
	$(Q)$(Q3ASM) -o $@ $(Q3CGVMOBJ) $(GDIR)/bg_syscalls.asm

#############################################################################
## MISSIONPACK CGAME
#############################################################################

MPCGOBJ = \
  $(B)/$(MISSIONPACK)/cgame/cg_main.o \
  $(B)/$(MISSIONPACK)/cgame/bg_misc.o \
  $(B)/$(MISSIONPACK)/cgame/bg_pmove.o \
  $(B)/$(MISSIONPACK)/cgame/bg_slidemove.o \
  $(B)/$(MISSIONPACK)/cgame/bg_lib.o \
  $(B)/$(MISSIONPACK)/cgame/bg_tracemap.o \
  $(B)/$(MISSIONPACK)/cgame/cg_atmospheric.o \
  $(B)/$(MISSIONPACK)/cgame/cg_console.o \
  $(B)/$(MISSIONPACK)/cgame/cg_consolecmds.o \
  $(B)/$(MISSIONPACK)/cgame/cg_newdraw.o \
  $(B)/$(MISSIONPACK)/cgame/cg_draw.o \
  $(B)/$(MISSIONPACK)/cgame/cg_drawtools.o \
  $(B)/$(MISSIONPACK)/cgame/cg_effects.o \
  $(B)/$(MISSIONPACK)/cgame/cg_ents.o \
  $(B)/$(MISSIONPACK)/cgame/cg_event.o \
  $(B)/$(MISSIONPACK)/cgame/cg_field.o \
  $(B)/$(MISSIONPACK)/cgame/cg_info.o \
  $(B)/$(MISSIONPACK)/cgame/cg_input.o \
  $(B)/$(MISSIONPACK)/cgame/cg_localents.o \
  $(B)/$(MISSIONPACK)/cgame/cg_marks.o \
  $(B)/$(MISSIONPACK)/cgame/cg_particles.o \
  $(B)/$(MISSIONPACK)/cgame/cg_players.o \
  $(B)/$(MISSIONPACK)/cgame/cg_playerstate.o \
  $(B)/$(MISSIONPACK)/cgame/cg_polybus.o \
  $(B)/$(MISSIONPACK)/cgame/cg_predict.o \
  $(B)/$(MISSIONPACK)/cgame/cg_ritualfont.o \
  $(B)/$(MISSIONPACK)/cgame/cg_scoreboard.o \
  $(B)/$(MISSIONPACK)/cgame/cg_servercmds.o \
  $(B)/$(MISSIONPACK)/cgame/cg_snapshot.o \
  $(B)/$(MISSIONPACK)/cgame/cg_spawn.o \
  $(B)/$(MISSIONPACK)/cgame/cg_surface.o \
  $(B)/$(MISSIONPACK)/cgame/cg_syscalls.o \
  $(B)/$(MISSIONPACK)/cgame/cg_text.o \
  $(B)/$(MISSIONPACK)/cgame/cg_unlagged.o \
  $(B)/$(MISSIONPACK)/cgame/cg_view.o \
  $(B)/$(MISSIONPACK)/cgame/cg_weapons.o \
  \
  $(B)/$(MISSIONPACK)/qcommon/q_math.o \
  $(B)/$(MISSIONPACK)/qcommon/q_shared.o \
  $(B)/$(MISSIONPACK)/qcommon/q_unicode.o

ifeq ($(USE_MISSIONPACK_Q3_UI), 1)
MPCGOBJ += \
  $(B)/$(MISSIONPACK)/q3ui/ui_main.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_addbots.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_atoms.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_cinematics.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_confirm.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_connect.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_controls2.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_credits.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_demo2.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_display.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_gameinfo.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_ingame.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_ingame_selectplayer.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_joystick.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_loadconfig.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_menu.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_mfield.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_mods.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_network.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_options.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_playermodel.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_players.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_playersettings.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_preferences.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_qmenu.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_removebots.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_saveconfig.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_selectplayer.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_serverinfo.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_servers2.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_setup.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_sound.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_sparena.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_specifyserver.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_splevel.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_sppostgame.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_spskill.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_startserver.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_team.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_teamorders.o \
  $(B)/$(MISSIONPACK)/q3ui/ui_video.o

ifeq ($(USE_MISSIONPACK_MP_HUD), 1)
MPCGOBJ += \
  $(B)/$(MISSIONPACK)/ui/ui_shared.o
endif
else
MPCGOBJ += \
  $(B)/$(MISSIONPACK)/ui/ui_main.o \
  $(B)/$(MISSIONPACK)/ui/ui_atoms.o \
  $(B)/$(MISSIONPACK)/ui/ui_gameinfo.o \
  $(B)/$(MISSIONPACK)/ui/ui_players.o \
  $(B)/$(MISSIONPACK)/ui/ui_shared.o
endif

MPCGVMOBJ = $(MPCGOBJ:%.o=%.asm)

$(B)/$(MISSIONPACK)/$(VM_PREFIX)cgame_$(SHLIBNAME): $(MPCGOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(MPCGOBJ)

$(B)/$(MISSIONPACK)/vm/$(VM_PREFIX)cgame.qvm: $(MPCGVMOBJ) $(GDIR)/bg_syscalls.asm $(Q3ASM)
	$(echo_cmd) "Q3ASM $@"
	$(Q)$(Q3ASM) -o $@ $(MPCGVMOBJ) $(GDIR)/bg_syscalls.asm



#############################################################################
## BASEGAME GAME
#############################################################################

Q3GOBJ = \
  $(B)/$(BASEGAME)/game/g_main.o \
  $(B)/$(BASEGAME)/game/ai_char.o \
  $(B)/$(BASEGAME)/game/ai_chat.o \
  $(B)/$(BASEGAME)/game/ai_chat_sys.o \
  $(B)/$(BASEGAME)/game/ai_cmd.o \
  $(B)/$(BASEGAME)/game/ai_dmnet.o \
  $(B)/$(BASEGAME)/game/ai_dmq3.o \
  $(B)/$(BASEGAME)/game/ai_ea.o \
  $(B)/$(BASEGAME)/game/ai_gen.o \
  $(B)/$(BASEGAME)/game/ai_goal.o \
  $(B)/$(BASEGAME)/game/ai_main.o \
  $(B)/$(BASEGAME)/game/ai_move.o \
  $(B)/$(BASEGAME)/game/ai_team.o \
  $(B)/$(BASEGAME)/game/ai_weap.o \
  $(B)/$(BASEGAME)/game/ai_weight.o \
  $(B)/$(BASEGAME)/game/ai_vcmd.o \
  $(B)/$(BASEGAME)/game/bg_misc.o \
  $(B)/$(BASEGAME)/game/bg_pmove.o \
  $(B)/$(BASEGAME)/game/bg_slidemove.o \
  $(B)/$(BASEGAME)/game/bg_lib.o \
  $(B)/$(BASEGAME)/game/bg_tracemap.o \
  $(B)/$(BASEGAME)/game/g_active.o \
  $(B)/$(BASEGAME)/game/g_arenas.o \
  $(B)/$(BASEGAME)/game/g_bot.o \
  $(B)/$(BASEGAME)/game/g_botlib.o \
  $(B)/$(BASEGAME)/game/g_client.o \
  $(B)/$(BASEGAME)/game/g_cmds.o \
  $(B)/$(BASEGAME)/game/g_combat.o \
  $(B)/$(BASEGAME)/game/g_items.o \
  $(B)/$(BASEGAME)/game/g_misc.o \
  $(B)/$(BASEGAME)/game/g_missile.o \
  $(B)/$(BASEGAME)/game/g_mover.o \
  $(B)/$(BASEGAME)/game/g_session.o \
  $(B)/$(BASEGAME)/game/g_spawn.o \
  $(B)/$(BASEGAME)/game/g_svcmds.o \
  $(B)/$(BASEGAME)/game/g_syscalls.o \
  $(B)/$(BASEGAME)/game/g_target.o \
  $(B)/$(BASEGAME)/game/g_team.o \
  $(B)/$(BASEGAME)/game/g_trigger.o \
  $(B)/$(BASEGAME)/game/g_unlagged.o \
  $(B)/$(BASEGAME)/game/g_utils.o \
  $(B)/$(BASEGAME)/game/g_weapon.o \
  \
  $(B)/$(BASEGAME)/botlib/be_aas_bspq3.o \
  $(B)/$(BASEGAME)/botlib/be_aas_cluster.o \
  $(B)/$(BASEGAME)/botlib/be_aas_debug.o \
  $(B)/$(BASEGAME)/botlib/be_aas_entity.o \
  $(B)/$(BASEGAME)/botlib/be_aas_file.o \
  $(B)/$(BASEGAME)/botlib/be_aas_main.o \
  $(B)/$(BASEGAME)/botlib/be_aas_move.o \
  $(B)/$(BASEGAME)/botlib/be_aas_optimize.o \
  $(B)/$(BASEGAME)/botlib/be_aas_reach.o \
  $(B)/$(BASEGAME)/botlib/be_aas_route.o \
  $(B)/$(BASEGAME)/botlib/be_aas_routealt.o \
  $(B)/$(BASEGAME)/botlib/be_aas_sample.o \
  $(B)/$(BASEGAME)/botlib/be_interface.o \
  $(B)/$(BASEGAME)/botlib/l_crc.o \
  $(B)/$(BASEGAME)/botlib/l_libvar.o \
  $(B)/$(BASEGAME)/botlib/l_log.o \
  $(B)/$(BASEGAME)/botlib/l_memory.o \
  \
  $(B)/$(BASEGAME)/qcommon/q_math.o \
  $(B)/$(BASEGAME)/qcommon/q_shared.o

Q3GVMOBJ = $(Q3GOBJ:%.o=%.asm)

$(B)/$(BASEGAME)/$(VM_PREFIX)game_$(SHLIBNAME): $(Q3GOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(Q3GOBJ)

$(B)/$(BASEGAME)/vm/$(VM_PREFIX)game.qvm: $(Q3GVMOBJ) $(GDIR)/bg_syscalls.asm $(Q3ASM)
	$(echo_cmd) "Q3ASM $@"
	$(Q)$(Q3ASM) -o $@ $(Q3GVMOBJ) $(GDIR)/bg_syscalls.asm

#############################################################################
## MISSIONPACK GAME
#############################################################################

MPGOBJ = \
  $(B)/$(MISSIONPACK)/game/g_main.o \
  $(B)/$(MISSIONPACK)/game/ai_char.o \
  $(B)/$(MISSIONPACK)/game/ai_chat.o \
  $(B)/$(MISSIONPACK)/game/ai_chat_sys.o \
  $(B)/$(MISSIONPACK)/game/ai_cmd.o \
  $(B)/$(MISSIONPACK)/game/ai_dmnet.o \
  $(B)/$(MISSIONPACK)/game/ai_dmq3.o \
  $(B)/$(MISSIONPACK)/game/ai_ea.o \
  $(B)/$(MISSIONPACK)/game/ai_gen.o \
  $(B)/$(MISSIONPACK)/game/ai_goal.o \
  $(B)/$(MISSIONPACK)/game/ai_main.o \
  $(B)/$(MISSIONPACK)/game/ai_move.o \
  $(B)/$(MISSIONPACK)/game/ai_team.o \
  $(B)/$(MISSIONPACK)/game/ai_weap.o \
  $(B)/$(MISSIONPACK)/game/ai_weight.o \
  $(B)/$(MISSIONPACK)/game/ai_vcmd.o \
  $(B)/$(MISSIONPACK)/game/bg_misc.o \
  $(B)/$(MISSIONPACK)/game/bg_pmove.o \
  $(B)/$(MISSIONPACK)/game/bg_slidemove.o \
  $(B)/$(MISSIONPACK)/game/bg_lib.o \
  $(B)/$(MISSIONPACK)/game/bg_tracemap.o \
  $(B)/$(MISSIONPACK)/game/g_active.o \
  $(B)/$(MISSIONPACK)/game/g_arenas.o \
  $(B)/$(MISSIONPACK)/game/g_bot.o \
  $(B)/$(MISSIONPACK)/game/g_botlib.o \
  $(B)/$(MISSIONPACK)/game/g_client.o \
  $(B)/$(MISSIONPACK)/game/g_cmds.o \
  $(B)/$(MISSIONPACK)/game/g_combat.o \
  $(B)/$(MISSIONPACK)/game/g_items.o \
  $(B)/$(MISSIONPACK)/game/g_misc.o \
  $(B)/$(MISSIONPACK)/game/g_missile.o \
  $(B)/$(MISSIONPACK)/game/g_mover.o \
  $(B)/$(MISSIONPACK)/game/g_session.o \
  $(B)/$(MISSIONPACK)/game/g_spawn.o \
  $(B)/$(MISSIONPACK)/game/g_svcmds.o \
  $(B)/$(MISSIONPACK)/game/g_syscalls.o \
  $(B)/$(MISSIONPACK)/game/g_target.o \
  $(B)/$(MISSIONPACK)/game/g_team.o \
  $(B)/$(MISSIONPACK)/game/g_trigger.o \
  $(B)/$(MISSIONPACK)/game/g_unlagged.o \
  $(B)/$(MISSIONPACK)/game/g_utils.o \
  $(B)/$(MISSIONPACK)/game/g_weapon.o \
  \
  $(B)/$(MISSIONPACK)/botlib/be_aas_bspq3.o \
  $(B)/$(MISSIONPACK)/botlib/be_aas_cluster.o \
  $(B)/$(MISSIONPACK)/botlib/be_aas_debug.o \
  $(B)/$(MISSIONPACK)/botlib/be_aas_entity.o \
  $(B)/$(MISSIONPACK)/botlib/be_aas_file.o \
  $(B)/$(MISSIONPACK)/botlib/be_aas_main.o \
  $(B)/$(MISSIONPACK)/botlib/be_aas_move.o \
  $(B)/$(MISSIONPACK)/botlib/be_aas_optimize.o \
  $(B)/$(MISSIONPACK)/botlib/be_aas_reach.o \
  $(B)/$(MISSIONPACK)/botlib/be_aas_route.o \
  $(B)/$(MISSIONPACK)/botlib/be_aas_routealt.o \
  $(B)/$(MISSIONPACK)/botlib/be_aas_sample.o \
  $(B)/$(MISSIONPACK)/botlib/be_interface.o \
  $(B)/$(MISSIONPACK)/botlib/l_crc.o \
  $(B)/$(MISSIONPACK)/botlib/l_libvar.o \
  $(B)/$(MISSIONPACK)/botlib/l_log.o \
  $(B)/$(MISSIONPACK)/botlib/l_memory.o \
  \
  $(B)/$(MISSIONPACK)/qcommon/q_math.o \
  $(B)/$(MISSIONPACK)/qcommon/q_shared.o

MPGVMOBJ = $(MPGOBJ:%.o=%.asm)

$(B)/$(MISSIONPACK)/$(VM_PREFIX)game_$(SHLIBNAME): $(MPGOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(MPGOBJ)

$(B)/$(MISSIONPACK)/vm/$(VM_PREFIX)game.qvm: $(MPGVMOBJ) $(GDIR)/bg_syscalls.asm $(Q3ASM)
	$(echo_cmd) "Q3ASM $@"
	$(Q)$(Q3ASM) -o $@ $(MPGVMOBJ) $(GDIR)/bg_syscalls.asm


# Extra dependencies to ensure the git version is incorporated
ifeq ($(USE_GIT),1)
  GITVEROBJ = \
    $(B)/$(BASEGAME)/cgame/cg_main.o \
    $(B)/$(BASEGAME)/cgame/cg_console.o \
    $(B)/$(BASEGAME)/game/g_main.o \
    $(B)/$(MISSIONPACK)/cgame/cg_main.o \
    $(B)/$(MISSIONPACK)/cgame/cg_console.o \
    $(B)/$(MISSIONPACK)/game/g_main.o

  GITVERVMOBJ = $(GITVEROBJ:%.o=%.asm)

  $(GITVEROBJ) : .git
  $(GITVERVMOBJ) : .git
endif


#############################################################################
## GAME MODULE RULES
#############################################################################

$(B)/$(BASEGAME)/cgame/bg_%.o: $(GDIR)/bg_%.c
	$(DO_CGAME_CC)

$(B)/$(BASEGAME)/cgame/%.o: $(CGDIR)/%.c
	$(DO_CGAME_CC)

$(B)/$(BASEGAME)/ui/%.o: $(Q3UIDIR)/%.c
	$(DO_CGAME_CC)

$(B)/$(BASEGAME)/mpui/%.o: $(UIDIR)/%.c
	$(DO_CGAME_CC)

$(B)/$(BASEGAME)/cgame/bg_%.asm: $(GDIR)/bg_%.c $(Q3LCC)
	$(DO_CGAME_Q3LCC)

$(B)/$(BASEGAME)/cgame/%.asm: $(CGDIR)/%.c $(Q3LCC)
	$(DO_CGAME_Q3LCC)

$(B)/$(BASEGAME)/ui/%.asm: $(Q3UIDIR)/%.c $(Q3LCC)
	$(DO_CGAME_Q3LCC)

$(B)/$(BASEGAME)/mpui/%.asm: $(UIDIR)/%.c $(Q3LCC)
	$(DO_CGAME_Q3LCC)

$(B)/$(MISSIONPACK)/cgame/bg_%.o: $(GDIR)/bg_%.c
	$(DO_CGAME_CC_MISSIONPACK)

$(B)/$(MISSIONPACK)/cgame/%.o: $(CGDIR)/%.c
	$(DO_CGAME_CC_MISSIONPACK)

$(B)/$(MISSIONPACK)/ui/%.o: $(UIDIR)/%.c
	$(DO_CGAME_CC_MISSIONPACK)

$(B)/$(MISSIONPACK)/q3ui/%.o: $(Q3UIDIR)/%.c
	$(DO_CGAME_CC_MISSIONPACK)

$(B)/$(MISSIONPACK)/cgame/bg_%.asm: $(GDIR)/bg_%.c $(Q3LCC)
	$(DO_CGAME_Q3LCC_MISSIONPACK)

$(B)/$(MISSIONPACK)/cgame/%.asm: $(CGDIR)/%.c $(Q3LCC)
	$(DO_CGAME_Q3LCC_MISSIONPACK)

$(B)/$(MISSIONPACK)/ui/%.asm: $(UIDIR)/%.c $(Q3LCC)
	$(DO_CGAME_Q3LCC_MISSIONPACK)

$(B)/$(MISSIONPACK)/q3ui/%.asm: $(Q3UIDIR)/%.c $(Q3LCC)
	$(DO_CGAME_Q3LCC_MISSIONPACK)


$(B)/$(BASEGAME)/game/%.o: $(GDIR)/%.c
	$(DO_GAME_CC)

$(B)/$(BASEGAME)/game/%.asm: $(GDIR)/%.c $(Q3LCC)
	$(DO_GAME_Q3LCC)

$(B)/$(MISSIONPACK)/game/%.o: $(GDIR)/%.c
	$(DO_GAME_CC_MISSIONPACK)

$(B)/$(MISSIONPACK)/game/%.asm: $(GDIR)/%.c $(Q3LCC)
	$(DO_GAME_Q3LCC_MISSIONPACK)

$(B)/$(BASEGAME)/botlib/%.o: $(BLIBDIR)/%.c
	$(DO_GAME_CC)

$(B)/$(BASEGAME)/botlib/%.asm: $(BLIBDIR)/%.c $(Q3LCC)
	$(DO_GAME_Q3LCC)

$(B)/$(MISSIONPACK)/botlib/%.o: $(BLIBDIR)/%.c
	$(DO_GAME_CC_MISSIONPACK)

$(B)/$(MISSIONPACK)/botlib/%.asm: $(BLIBDIR)/%.c $(Q3LCC)
	$(DO_GAME_Q3LCC_MISSIONPACK)

$(B)/$(BASEGAME)/qcommon/%.o: $(CMDIR)/%.c
	$(DO_SHLIB_CC)

$(B)/$(BASEGAME)/qcommon/%.asm: $(CMDIR)/%.c $(Q3LCC)
	$(DO_Q3LCC)

$(B)/$(MISSIONPACK)/qcommon/%.o: $(CMDIR)/%.c
	$(DO_SHLIB_CC_MISSIONPACK)

$(B)/$(MISSIONPACK)/qcommon/%.asm: $(CMDIR)/%.c $(Q3LCC)
	$(DO_Q3LCC_MISSIONPACK)


#############################################################################
# MISC
#############################################################################

OBJ = $(MPGOBJ) $(Q3GOBJ) $(Q3CGOBJ) $(MPCGOBJ) $(Q3UIOBJ) $(MPUIOBJ) \
  $(MPGVMOBJ) $(Q3GVMOBJ) $(Q3CGVMOBJ) $(MPCGVMOBJ) $(Q3UIVMOBJ) $(MPUIVMOBJ)
TOOLSOBJ = $(LBURGOBJ) $(Q3CPPOBJ) $(Q3RCCOBJ) $(Q3LCCOBJ) $(Q3ASMOBJ)


copyfiles: release
ifneq ($(BUILD_GAME_SO),0)
  ifneq ($(BUILD_BASEGAME),0)
	-$(MKDIR) -m 0755 $(COPYDIR)/$(BASEGAME)
  endif
  ifneq ($(BUILD_MISSIONPACK),0)
	-$(MKDIR) -m 0755 $(COPYDIR)/$(MISSIONPACK)
  endif
endif

ifneq ($(BUILD_GAME_SO),0)
  ifneq ($(BUILD_BASEGAME),0)
	$(INSTALL) $(STRIP_FLAG) -m 0755 $(BR)/$(BASEGAME)/$(VM_PREFIX)cgame_$(SHLIBNAME) \
					$(COPYDIR)/$(BASEGAME)/.
	$(INSTALL) $(STRIP_FLAG) -m 0755 $(BR)/$(BASEGAME)/$(VM_PREFIX)game_$(SHLIBNAME) \
					$(COPYDIR)/$(BASEGAME)/.
  endif
  ifneq ($(BUILD_MISSIONPACK),0)
	$(INSTALL) $(STRIP_FLAG) -m 0755 $(BR)/$(MISSIONPACK)/$(VM_PREFIX)cgame_$(SHLIBNAME) \
					$(COPYDIR)/$(MISSIONPACK)/.
	$(INSTALL) $(STRIP_FLAG) -m 0755 $(BR)/$(MISSIONPACK)/$(VM_PREFIX)game_$(SHLIBNAME) \
					$(COPYDIR)/$(MISSIONPACK)/.
  endif
endif

clean: clean-debug clean-release

clean-debug:
	@$(MAKE) clean2 B=$(BD)

clean-release:
	@$(MAKE) clean2 B=$(BR)

clean2:
	@echo "CLEAN $(B)"
	@rm -f $(OBJ)
	@rm -f $(OBJ_D_FILES)
	@rm -f $(TARGETS)

toolsclean: toolsclean-debug toolsclean-release

toolsclean-debug:
	@$(MAKE) toolsclean2 B=$(BD)

toolsclean-release:
	@$(MAKE) toolsclean2 B=$(BR)

toolsclean2:
	@echo "TOOLS_CLEAN $(B)"
	@rm -f $(TOOLSOBJ)
	@rm -f $(TOOLSOBJ_D_FILES)
	@rm -f $(LBURG) $(DAGCHECK_C) $(Q3RCC) $(Q3CPP) $(Q3LCC) $(Q3ASM)

distclean: clean toolsclean
	@rm -rf $(BUILD_DIR)

dist:
	git archive --format zip --output $(SOURCE_ARCHIVE)-$(VERSION).zip HEAD

#############################################################################
# DEPENDENCIES
#############################################################################

ifneq ($(B),)
  OBJ_D_FILES=$(filter %.d,$(OBJ:%.o=%.d))
  TOOLSOBJ_D_FILES=$(filter %.d,$(TOOLSOBJ:%.o=%.d))
  -include $(OBJ_D_FILES) $(TOOLSOBJ_D_FILES)
endif

.PHONY: all clean clean2 clean-debug clean-release copyfiles \
	debug default dist distclean makedirs \
	release targets \
	toolsclean toolsclean2 toolsclean-debug toolsclean-release \
	$(OBJ_D_FILES) $(TOOLSOBJ_D_FILES)

# If the target name contains "clean", don't do a parallel build
ifneq ($(findstring clean, $(MAKECMDGOALS)),)
.NOTPARALLEL:
endif
