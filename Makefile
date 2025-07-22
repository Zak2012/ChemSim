## Compiler
CCXX = C:\Users\user\scoop\apps\mingw-winlibs-msvcrt\current\bin\g++
CC = C:\Users\user\scoop\apps\mingw-winlibs-msvcrt\current\bin\gcc
# LN = ld -r -b binary

WINDRES = C:\Users\user\scoop\apps\mingw-winlibs-msvcrt\current\bin\windres

# "C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86\signtool" sign /a /fd SHA256 /tr http://timestamp.digicert.com /td SHA256 chemsim.exe

EMXX = em++
EM = emcc

## Delete Commmand
DC = rm -rf

## Standard
STDXX = c++2a
STD = c17

## Executables Name
# EXE = $(BINDIR)/$(notdir $(CURDIR))
EXE = $(BINDIR)/ChemSim
# WEB = $(BINDIR)/ChemSim.js
WEB = $(BINDIR)/index.js

## Project Directories
INCDIR = include ## embed/header embed lib
LIBDIR = lib
WEBLIBDIR = lib/web
OBJDIR = obj
SRCDIR = src
BINDIR = bin
# EMBDIR = src/embed

## Define Source
SOURCE = Application.cpp Shader.cpp Resource.cpp Object.cpp ColorConvert.cpp Physics.cpp Widget.cpp Font.cpp Atoms.cpp Embed.cpp
LIBS = glfw3 freetype BulletDynamics BulletCollision LinearMath Bullet3Common
SLIBS = glw.c tinyxml2.cpp
RESF = src/embed/Res.rc
RESO = obj/Res.o
# EMSRC = Chemsim.png shader/Basic.vert shader/Basic.frag shader/Sprite.vert shader/Sprite.frag shader/Circle.vert shader/Circle.frag shader/Text.vert shader/Text.frag Roboto-Regular.ttf

WEBOBJECT = $(addsuffix .web.o, $(SOURCE)) $(addsuffix .lib.web.o, $(notdir $(SLIBS)))
OBJECT = $(addsuffix .o, $(SOURCE)) $(addsuffix .lib.o, $(notdir $(SLIBS)))
# EMOBJ = $(addsuffix .data.o, $(EMSRC))

## Define File
SRC = $(addprefix $(SRCDIR)/, $(SOURCE))
INC = $(addprefix -I, $(INCDIR))
WBJ = $(addprefix $(OBJDIR)/, $(WEBOBJECT))
OBJ = $(addprefix $(OBJDIR)/, $(OBJECT))
SLB = $(addprefix $(LIBDIR)/, $(SLIBS))
LIB = $(addprefix -l, $(LIBS))
# ERC = $(addprefix $(EMBDIR)/, $(EMSRC))
# EBJ = $(addprefix $(OBJDIR)/, $(EMOBJ))

OPTFLAG = -g3
# OPTFLAG = -O3

## Define Flags
CFLAGSXX = -c $(OPTFLAG) -Wall -std=$(STDXX) $(INC)
CFLAGS = -c $(OPTFLAG) -Wall -std=$(STD) $(INC)
LFLAGS = -L$(LIBDIR) -L$(BINDIR) $(LIB) -lopengl32 -lgdi32 -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive -static-libgcc -static-libstdc++ ##-mwindows 
EFLAGSXX = -c $(OPTFLAG) -Wall -std=$(STDXX) $(INC) -fexceptions -pthread 
EFLAGS = -c $(OPTFLAG) -Wall -std=$(STD) $(INC) -fexceptions -pthread 
WEBLFLAGS = -L$(WEBLIBDIR) $(LIB) --embed-file ./src/embed@/ -pthread -sUSE_PTHREADS=1 -sPTHREAD_POOL_SIZE=3 -sUSE_GLFW=3 -sFULL_ES3 -sWASM=1 -fexceptions -sALLOW_MEMORY_GROWTH -sINITIAL_MEMORY=64mb -sSTACK_SIZE=24mb

## Define Scope
# all : native web
all : native
# all : web

# embed :
# 	ld -r -b binary data.dat -o data.o
# 	$(foreach dir,$(EMSRC),-D$(dir)=R"$(shell xxd -i -c 64 -u $(EMBDIR)/shader/Basic.vert)")
## 	$(foreach dir,$(EMSRC),$(file < $(EMBDIR)/header/$(dir).h, $(shell xxd -i -c 64 -u $(EMBDIR)/$(dir))))
# $(foreach dir,$(TEXT),-D$(dir)=R"$(file < $(EMBDIR)/$(dir))")
# $(foreach dir,$(TEXT),-D$(dir)=R"$(shell type $(EMBDIR)/$(dir))")
# $(ANNOUNCE_BODY)
# type $(EMBDIR)/shader/basic.vert

native : $(SRC) $(SLB) $(EXE) ##$(ERC) 

## Embed
# $(OBJDIR)/%.data.o : $(EMBDIR)/%
# 	$(LN) $< -o $@

## Compile C++ Files
$(OBJDIR)/%.cpp.o : $(SRCDIR)/%.cpp
	$(CCXX) $< $(CFLAGSXX) -o $@
## Compile C Files
$(OBJDIR)/%.c.o : $(SRCDIR)/%.c
	$(CC) $< $(CFLAGS) -o $@

## Compile C++ Libs
$(OBJDIR)/%.cpp.lib.o : $(LIBDIR)/%.cpp
	$(CCXX) $< $(CFLAGSXX) -O2 -o $@

## Compile C Libs
$(OBJDIR)/%.c.lib.o : $(LIBDIR)/%.c
	$(CC) $< $(CFLAGS) -O2 -o $@

## Link Object Files
$(EXE) : $(OBJ) ##$(EBJ)
	$(WINDRES) $(RESF) -O coff -o $(RESO)
	$(CCXX) $(RESO) $^ $(LFLAGS) -o $@

web : $(SRC) $(SLB) $(WEB)

## Embed
# $(OBJDIR)/%.data.o : $(EMBDIR)/%
# 	$(MN) $< -o $@

## Compile Web C++ Files
$(OBJDIR)/%.cpp.web.o : $(SRCDIR)/%.cpp
	$(EMXX) $< $(EFLAGSXX) -o $@

## Compile Web C Files
$(OBJDIR)/%.c.web.o : $(SRCDIR)/%.c
	$(EM) $< $(EFLAGS) -o $@

## Compile Web C++ Libs
$(OBJDIR)/%.cpp.lib.web.o : $(LIBDIR)/%.cpp
	$(EMXX) $< $(EFLAGSXX) -O2 -o $@

## Compile Web C Libs
$(OBJDIR)/%.c.lib.web.o : $(LIBDIR)/%.c
	$(EM) $< $(EFLAGS) -O3 -o $@

## Link Web Object Files
$(WEB) : $(WBJ)
	$(EMXX) $^ $(WEBLFLAGS) -o $@

.PHONY: clean

## Clean Object Files
clean : 
	-$(DC) obj/*.o