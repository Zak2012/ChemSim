## Compiler
CCXX = g++
CC = gcc

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
WEB = $(BINDIR)/ChemSim.js

## Project Directories
INCDIR = include embed/header embed
LIBDIR = lib
WEBLIBDIR = lib/web
OBJDIR = obj
SRCDIR = src
BINDIR = bin
RESDIR = res
EMBDIR = embed


## Define Source
SOURCE = Application.cpp Shader.cpp Resource.cpp Object.cpp ColorConvert.cpp Physics.cpp Widget.cpp Font.cpp
LIBS = glfw3 freetype
SLIBS = glew.c
RESF = embed/Res.rc
RESO = obj/Res.o
EMSRC = Basic.frag Basic.vert Circle.frag Circle.vert Sprite.frag Sprite.vert Text.frag Text.vert Chemsim.png ARIAL.ttf

WEBOBJECT = $(addsuffix .o, $(SOURCE))
OBJECT = $(addsuffix .o, $(SOURCE)) $(addsuffix .o, $(SLIBS))
EMOBJ = $(addsuffix .h, $(EMSRC))

## Define File
SRC = $(addprefix $(SRCDIR)/, $(SOURCE))
INC = $(addprefix -I, $(INCDIR))
WBJ = $(addprefix $(OBJDIR)/web/, $(WEBOBJECT))
OBJ = $(addprefix $(OBJDIR)/, $(OBJECT))
SLB = $(addprefix $(LIBDIR)/, $(SLIBS))
LIB = $(addprefix -l, $(LIBS))
ERC = $(addprefix $(EMBDIR)/, $(EMSRC))
EBJ = $(addprefix $(EMBDIR)/header/, $(EMOBJ))

## Define Flags
CFLAGSXX = -c -g3 -Wall -std=$(STDXX) $(INC)
CFLAGS = -c -g3 -Wall -std=$(STD) $(INC)
LFLAGS = -L$(LIBDIR) -L$(BINDIR) $(LIB) -lopengl32 -lgdi32 -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive -static-libgcc -static-libstdc++ ##-mwindows 
EFLAGSXX = -c -g3 -Wall -std=$(STDXX) $(INC) -fexceptions
EFLAGS = -c -g3 -Wall -std=$(STD) $(INC) -fexceptions
WEBLFLAGS = -L$(WEBLIBDIR) $(LIB) -sUSE_GLFW=3 -sFULL_ES3 -sWASM=1 -fexceptions -sALLOW_MEMORY_GROWTH -sINITIAL_MEMORY=2147483648

## Define Scope
all : native web


embed :
	$(foreach dir,$(EMSRC),$(file > $(EMBDIR)/header/$(dir).h, $(shell xxd -i -c 64 -u $(EMBDIR)/$(dir))))

native : embed $(SRC) $(SLB) $(EXE)

## Compile C++ Files
$(OBJDIR)/%.cpp.o : $(SRCDIR)/%.cpp
	$(CCXX) $< $(CFLAGSXX) -o $@

## Compile C Files
$(OBJDIR)/%.c.o : $(SRCDIR)/%.c
	$(CC) $< $(CFLAGS) -o $@

## Compile C++ Libs
$(OBJDIR)/%.cpp.o : $(LIBDIR)/%.cpp
	$(CCXX) $< $(CFLAGSXX) -o $@

## Compile C Libs
$(OBJDIR)/%.c.o : $(LIBDIR)/%.c
	$(CC) $< $(CFLAGS) -o $@

## Link Object Files
$(EXE) : $(OBJ)
	-windres $(RESF) -O coff -o $(RESO)
	$(CCXX) $(RESO) $^ $(LFLAGS) -o $@

web : embed $(SRC) $(WEB)

## Compile Web C++ Files
$(OBJDIR)/web/%.cpp.o : $(SRCDIR)/%.cpp
	$(EMXX) $< $(EFLAGSXX) -o $@

## Compile Web C Files
$(OBJDIR)/web/%.c.o : $(SRCDIR)/%.c
	$(EM) $< $(EFLAGS) -o $@

## Compile Web C++ Libs
$(OBJDIR)/web/%.cpp.o : $(LIBDIR)/%.cpp
	$(EMXX) $< $(EFLAGSXX) -o $@

## Compile Web C Libs
$(OBJDIR)/web/%.c.o : $(LIBDIR)/%.c
	$(EM) $< $(EFLAGS) -o $@

## Link Web Object Files
$(WEB) : $(WBJ)
	$(EMXX) $^ $(WEBLFLAGS) -o $@

.PHONY: clean embed src/Application.cpp

## Clean Object Files
clean : 
	-$(DC) obj/*.o
	-$(DC) obj/web/*.o
	-$(DC) embed/header/*.h




