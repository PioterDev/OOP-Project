CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c17 $(INCLIB) -g

CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++23 $(INCLIB) -g

SRCDIR = src
INCDIR = include
BUILDDIR = build
LIBDIR = G:/Projects/CLibs

INCLIB = -I$(INCDIR) -I$(SDLINCLUDE) -L$(SDLLIB) -I$(SDLIMAGEINCLUDE) -L$(SDLIMAGELIB) -I$(SDLMIXERINCLUDE) -L$(SDLMIXERLIB) -I$(SDLTTFINCLUDE) -L$(SDLTTFLIB)
LDFLAGS = -lmingw32 -l$(LIBRARYSDLMAIN) -l$(LIBRARYSDL) -l$(LIBRARYSDLIMAGE) -l$(LIBRARYSDLMIXER) -l$(LIBRARYSDLTTF) -mwindows -lwinmm

SDLINCLUDE 		= $(LIBDIR)/SDL2/include/SDL2
SDLIMAGEINCLUDE	= $(LIBDIR)/SDL2_image/include/SDL2
SDLMIXERINCLUDE = $(LIBDIR)/SDL2_mixer/include/SDL2
SDLTTFINCLUDE	= $(LIBDIR)/SDL2_ttf/include/SDL2

SDLLIB 			= $(LIBDIR)/SDL2/lib
SDLIMAGELIB 	= $(LIBDIR)/SDL2_image/lib
SDLMIXERLIB 	= $(LIBDIR)/SDL2_mixer/lib
SDLTTFLIB 		= $(LIBDIR)/SDL2_ttf/lib

SRCS_C 		= $(wildcard $(SRCDIR)/*.c)
SRCS_CPP 	= $(wildcard $(SRCDIR)/*.cpp)
OBJS_C 		= $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS_C))
OBJS_CPP 	= $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRCS_CPP))
OBJS = $(OBJS_C) $(OBJS_CPP)
#OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
EXEC = ./out/Cetris

LIBRARYSDL = SDL2
LIBRARYSDLMAIN = SDL2main
LIBRARYSDLIMAGE = SDL2_image
LIBRARYSDLMIXER = SDL2_mixer
LIBRARYSDLTTF = SDL2_ttf

# Default target
all: $(EXEC)

# Rule to build executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -g -o $(EXEC)

# Rule to compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -Dmain=SDL_main -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -Dmain=SDL_main -c $< -o $@

#Rule to compile main.c | DOESN'T WORK!
#$(BUILDDIR)/main.o: ./main.c
#	$(CC) $(CFLAGS) -Dmain=SDL_main -c $< -o $@

# Clean rule
clean:
	rm -f $(EXEC) $(OBJS)

cleanWin:
	del /S build\*.o