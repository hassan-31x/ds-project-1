#**************************************************************************************************
#
#   raylib makefile for Desktop platforms, Raspberry Pi, Android and HTML5
#
#   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
#
#   This software is provided "as-is", without any express or implied warranty. In no event
#   will the authors be held liable for any damages arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose, including commercial
#   applications, and to alter it and redistribute it freely, subject to the following restrictions:
#
#     1. The origin of this software must not be misrepresented; you must not claim that you
#     wrote the original software. If you use this software in a product, an acknowledgment
#     in the product documentation would be appreciated but is not required.
#
#     2. Altered source versions must be plainly marked as such, and must not be misrepresented
#     as being the original software.
#
#     3. This notice may not be removed or altered from any source distribution.
#
#**************************************************************************************************

.PHONY: all clean

# Define project name
PROJECT_NAME       ?= class_scheduler

# Define required raylib variables
RAYLIB_VERSION     ?= 4.2.0
RAYLIB_PATH        ?= $(HOME)/raylib

# Define compiler path on Windows
COMPILER_PATH      ?= C:/raylib/w64devkit/bin

# Define default options
# One of PLATFORM_DESKTOP, PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
PLATFORM           ?= PLATFORM_DESKTOP

# Locations of your newly installed library and associated headers. See ../src/Makefile
# On Linux, if you have installed raylib but cannot compile the examples, check that
# the LD_LIBRARY_PATH environment variable includes the directory in which you have
# installed raylib. Use ldd to check if all dependencies are satisfied.
# NOTE: To configure specific linkage of dependencies, review Makefile.Linux
# RAYLIB_INSTALL_PATH ?= /usr/local
RAYLIB_INSTALL_PATH ?= .

# Library type used for raylib: STATIC (.a) or SHARED (.so/.dll)
RAYLIB_LIBTYPE        ?= STATIC

# Build mode for project: DEBUG or RELEASE
BUILD_MODE            ?= RELEASE

# Use external GLFW library instead of rglfw module
# TODO: Review usage on Linux. Target version of choice. Switch on -lglfw or -lglfw3
USE_EXTERNAL_GLFW     ?= FALSE

# Use Wayland display server protocol on Linux desktop
# by default it uses X11 windowing system
USE_WAYLAND_DISPLAY   ?= FALSE

# Determine PLATFORM_OS in case PLATFORM_DESKTOP selected
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    # No uname.exe on MinGW!, but OS=Windows_NT on Windows!
    # ifeq ($(UNAME),Msys) -> Windows
    ifeq ($(OS),Windows_NT)
        PLATFORM_OS=WINDOWS
        export PATH := $(COMPILER_PATH):$(PATH)
    else
        UNAMEOS=$(shell uname)
        ifeq ($(UNAMEOS),Linux)
            PLATFORM_OS=LINUX
        endif
        ifeq ($(UNAMEOS),FreeBSD)
            PLATFORM_OS=BSD
        endif
        ifeq ($(UNAMEOS),OpenBSD)
            PLATFORM_OS=BSD
        endif
        ifeq ($(UNAMEOS),NetBSD)
            PLATFORM_OS=BSD
        endif
        ifeq ($(UNAMEOS),DragonFly)
            PLATFORM_OS=BSD
        endif
        ifeq ($(UNAMEOS),Darwin)
            PLATFORM_OS=OSX
        endif
    endif
endif
ifeq ($(PLATFORM),PLATFORM_RPI)
    UNAMEOS=$(shell uname)
    ifeq ($(UNAMEOS),Linux)
        PLATFORM_OS=LINUX
    endif
endif

# RAYLIB_PATH adjustment for different platforms
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        RAYLIB_PATH    ?= $(CURDIR)/raylib
    endif
endif

# Define raylib release directory for compiled library.
# RAYLIB_RELEASE_PATH points to provided binaries or your freshly built version
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        RAYLIB_RELEASE_PATH = $(RAYLIB_PATH)/src
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        RAYLIB_RELEASE_PATH = $(RAYLIB_PATH)/src
    endif
    ifeq ($(PLATFORM_OS),OSX)
        RAYLIB_RELEASE_PATH = $(RAYLIB_PATH)/src
    endif
    ifeq ($(PLATFORM_OS),BSD)
        RAYLIB_RELEASE_PATH = $(RAYLIB_PATH)/src
    endif
endif
ifeq ($(PLATFORM),PLATFORM_RPI)
    RAYLIB_RELEASE_PATH = $(RAYLIB_PATH)/src
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
    RAYLIB_RELEASE_PATH = $(RAYLIB_PATH)/src
endif
ifeq ($(PLATFORM),PLATFORM_ANDROID)
    RAYLIB_RELEASE_PATH = $(RAYLIB_PATH)/src
endif

# Define default C compiler: gcc
# NOTE: define g++ compiler if using C++
CC = g++

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),OSX)
        # OSX default compiler
        CC = clang++
    endif
    ifeq ($(PLATFORM_OS),BSD)
        # FreeBSD, OpenBSD, NetBSD, DragonFly default compiler
        CC = clang
    endif
endif
ifeq ($(PLATFORM),PLATFORM_RPI)
    ifeq ($(USE_RPI_CROSS_COMPILER),TRUE)
        # Define RPI cross-compiler
        #CC = armv6j-hardfloat-linux-gnueabi-gcc
        CC = $(RPI_TOOLCHAIN)/bin/arm-linux-gnueabihf-gcc
    endif
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
    # HTML5 emscripten compiler
    # WARNING: To compile to HTML5, code must be redesigned
    # to use emscripten.h and emscripten_set_main_loop()
    CC = emcc
endif

# Define default make program: Mingw32-make
MAKE = make

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        MAKE = mingw32-make
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        MAKE = make
    endif
endif

# Define compiler flags:
#  -g                        generates debug information
#  -O0                       no code optimization
#  -O1                       moderate optimization
#  -O2                       moderate optimization
#  -O3                       performs level 3 optimization
#  -Wall                     turns on most warnings
#  -std=c99                  uses C99 standard
#  -std=gnu99                uses GNU99 standard
#  -Wno-missing-braces      ignore invalid warning (GCC bug 53119)
#  -D_DEFAULT_SOURCE         use with -std=c99 on Linux and PLATFORM_WEB, required for timespec
#  -Werror=implicit-function-declaration   catch function calls without previous declaration
#  -fno-strict-aliasing     disable dangerous optimization that broke glTexImage2D on Android 8
CFLAGS = -Wall -std=c++11 -D_DEFAULT_SOURCE -Wno-missing-braces -Werror=implicit-function-declaration -fno-strict-aliasing

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -O0
else
    CFLAGS += -O2
endif

# Additional flags for compiler (if desired)
#CFLAGS += -Wextra -Wmissing-prototypes -Wstrict-prototypes
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        # resource file contains windows executable icon and properties
        # -Wl,--subsystem,windows hides the console window
        CFLAGS += $(RAYLIB_PATH)/src/raylib.rc.data
        ifeq ($(BUILD_MODE),RELEASE)
            CFLAGS += -Wl,--subsystem,windows
        endif
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        ifeq ($(RAYLIB_LIBTYPE),STATIC)
            CFLAGS += -D_DEFAULT_SOURCE
        endif
        ifeq ($(RAYLIB_LIBTYPE),SHARED)
            # Explicitly enable runtime link to libraylib.so
            CFLAGS += -Wl,-rpath,$(RAYLIB_RELEASE_PATH)
        endif
    endif
endif
ifeq ($(PLATFORM),PLATFORM_RPI)
    CFLAGS += -std=gnu99
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
    # -Os                        # size optimization
    # -O2                        # optimization level 2, if used, also set --memory-init-file 0
    # -s USE_GLFW=3              # Use glfw3 library (context/input management)
    # -s ALLOW_MEMORY_GROWTH=1   # Allow memory resizing
    # -s TOTAL_MEMORY=16777216   # 16 MB, default = 16MB
    # -s USE_PTHREADS=1          # multithreading support
    # -s WASM=0                  # disable Web Assembly, emitted by default
    # -s ASYNCIFY                # lets synchronous C/C++ code interact with asynchronous JS
    # -s FORCE_FILESYSTEM=1      # force filesystem to load/save files data
    # -s ASSERTIONS=1            # enable runtime checks for common memory allocation errors (-O1 and above turn it off)
    # --profiling                # include information for code profiling
    # --memory-init-file 0       # to avoid an external memory initialization code file (.mem)
    # --preload-file resources   # specify a resources folder for data compilation
    # --source-map-base          # allow debugging in browser with source map
    CFLAGS += -Os -s USE_GLFW=3 -s TOTAL_MEMORY=16777216 --preload-file resources
    ifeq ($(BUILD_MODE), DEBUG)
        CFLAGS += -s ASSERTIONS=1 --profiling
    endif

    # Define a custom shell .html and output extension
    CFLAGS += --shell-file $(RAYLIB_PATH)/src/shell.html
    EXT = .html
endif

# Define include paths for required headers
# NOTE: Several external required libraries (stb and others)
INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external -Isrc

# Define additional directories containing required header files
ifeq ($(PLATFORM),PLATFORM_RPI)
    # RPI required libraries
    INCLUDE_PATHS += -I/opt/vc/include
    INCLUDE_PATHS += -I/opt/vc/include/interface/vmcs_host/linux
    INCLUDE_PATHS += -I/opt/vc/include/interface/vcos/pthreads
endif
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),BSD)
        # Consider -L$(RAYLIB_INSTALL_PATH)
        INCLUDE_PATHS += -I/usr/local/include
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        # Reset everything.
        # Precedence: immediately local, installed version, raysan5 provided libs
        #INCLUDE_PATHS = -I$(RAYLIB_INSTALL_PATH)/include -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/release/include -I$(RAYLIB_PATH)/src/external
        INCLUDE_PATHS += -I$(RAYLIB_INSTALL_PATH)/include -I$(RAYLIB_PATH)/release/include
    endif
endif

# Define library paths containing required libs.
LDFLAGS = -L.

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        # resource file contains windows executable icon and properties
        LDFLAGS += $(RAYLIB_RELEASE_PATH)/raylib.rc.data -lopengl32 -lgdi32 -lwinmm
        # -lpthread -lopenal32
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        LDFLAGS += -L$(RAYLIB_INSTALL_PATH)/lib -L$(RAYLIB_PATH)/release/libs/linux
        # Check for external GLFW library
        ifeq ($(USE_EXTERNAL_GLFW),TRUE)
            LDFLAGS += -lglfw
        endif
        # Check for Wayland display system
        ifeq ($(USE_WAYLAND_DISPLAY),TRUE)
            LDFLAGS += -lwayland-client -lwayland-cursor -lwayland-egl -lxkbcommon
        endif
        # Explicitly enable runtime link to libraylib.so.
        LDFLAGS += -Wl,-rpath,$(RAYLIB_INSTALL_PATH)/lib
    endif
    ifeq ($(PLATFORM_OS),OSX)
        LDFLAGS += -L$(RAYLIB_INSTALL_PATH)/lib -L$(RAYLIB_PATH)/release/libs/osx
        # Check for external GLFW library
        ifeq ($(USE_EXTERNAL_GLFW),TRUE)
            LDFLAGS += -lglfw
        endif
    endif
    ifeq ($(PLATFORM_OS),BSD)
        # Consider -L$(RAYLIB_INSTALL_PATH)/lib
        LDFLAGS += -L/usr/local/lib
        # Check for external GLFW library
        ifeq ($(USE_EXTERNAL_GLFW),TRUE)
            LDFLAGS += -lglfw
        endif
    endif
endif
ifeq ($(PLATFORM),PLATFORM_RPI)
    LDFLAGS += -L/opt/vc/lib
    # Check for OpenGL ES version to be used (2.0 by default)
    # Use GRAPHICS_API_OPENGL_ES3 if RPI supports it (#define GLFW_INCLUDE_ES3)
    ifeq ($(USE_RPI_CROSS_COMPILER),TRUE)
        LDFLAGS += -L$(RPI_TOOLCHAIN)/lib
    endif
    ifeq ($(GRAPHICS_API_OPENGL_ES3),TRUE)
        LDFLAGS += -lGLESv2
    else
        LDFLAGS += -lGLESv2
    endif
    LDFLAGS += -lEGL -lpthread -lrt -lm -lbcm_host -lvcos -lvchiq_arm
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
    # No specific Link flags for PLATFORM_WEB
endif

# Define all object files required
SRC_DIR = src
OBJ_DIR = obj

# Define all source files required
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Define libraries required for linking
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        ifeq ($(RAYLIB_LIBTYPE),STATIC)
            LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
        endif
        ifeq ($(RAYLIB_LIBTYPE),SHARED)
            LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
        endif
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        ifeq ($(RAYLIB_LIBTYPE),STATIC)
            LIBS = -static -pthread -lraylib -lm -ldl -lX11
            # Check for OpenGL library
            ifeq ($(shell $(CC) -lGL -o $(OBJ_DIR)/tmp 2>&1 || echo 1), 1)
                LIBS += -lGL
            endif
            # Check if GLEW library is required (OpenGL extension handling)
            ifeq ($(USE_EXTERNAL_GLFW),TRUE)
                LIBS += -lglfw
            endif
            # Check if Wayland display system is required
            ifeq ($(USE_WAYLAND_DISPLAY),TRUE)
                LIBS += -lwayland-client -lwayland-cursor -lwayland-egl -lxkbcommon
            endif
        endif
        ifeq ($(RAYLIB_LIBTYPE),SHARED)
            LIBS = -lraylib
        endif
    endif
    ifeq ($(PLATFORM_OS),OSX)
        LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
        # Check if GLFW external library is required
        ifeq ($(USE_EXTERNAL_GLFW),TRUE)
            LIBS += -lglfw
        endif
        # add system frameworks
        ifeq ($(RAYLIB_LIBTYPE),SHARED)
            LIBS += -lraylib
        endif
        ifeq ($(RAYLIB_LIBTYPE),STATIC)
            LIBS += -lraylib
        endif
    endif
    ifeq ($(PLATFORM_OS),BSD)
        # Consider -L$(RAYLIB_INSTALL_PATH)/lib
        LDLIBS = -lraylib -lGL -lpthread -lm

        ifeq ($(RAYLIB_LIBTYPE),SHARED)
            LDLIBS = -lraylib -lGL -lpthread -lm
        endif
    endif
endif
ifeq ($(PLATFORM),PLATFORM_RPI)
    LIBS = -lraylib -lGLESv2 -lEGL -lpthread -lrt -lm -lbcm_host -lvcos -lvchiq_arm -ldl
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
    LIBS = $(RAYLIB_RELEASE_PATH)/libraylib.a
endif

# Creating required directories for object files
$(shell mkdir -p $(OBJ_DIR) >/dev/null)

# Build all sources
all: $(PROJECT_NAME)

# Compile source files
# NOTE: We construct a list of object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

# Link all object files
$(PROJECT_NAME): $(OBJ_FILES)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(PROJECT_NAME)