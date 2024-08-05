## Compiler
CCXX = g++
CC = gcc

## Delete Commmand
DC = rm -rf

## Standard
STDXX = c++2a
STD = c17

## Executables Name
EXE = $(BINDIR)/$(notdir $(CURDIR))

## Project Directories
INCDIR = include
LIBDIR = lib
OBJDIR = obj
SRCDIR = src
BINDIR = bin
RESDIR = res

## Define Source
SOURCE = Application.cpp File.cpp Shader.cpp Resource.cpp Object.cpp ColorConvert.cpp Font.cpp Physics.cpp Widget.cpp
LIBS = winmm glfw3 opengl32 harfbuzz usp10 rpcrt4 gdi32 freetype tinyxml2
SLIBS = glew.c
RESF = res/Res.rc
RESO = res/Res.o

OBJECT = $(addsuffix .o, $(SOURCE))
FOROBJ = $(addsuffix .o, $(SLIBS))

## Define File
SRC = $(addprefix $(SRCDIR)/, $(SOURCE))
INC = $(addprefix -I, $(INCDIR))
OBJ = $(addprefix $(OBJDIR)/, $(OBJECT))
SLB = $(addprefix $(LIBDIR)/, $(SLIBS))
FBJ = $(addprefix $(OBJDIR)/, $(FOROBJ))
LIB = $(addprefix -l, $(LIBS))

## Define Flags
CFLAGSXX = -c -g3 -Wall -std=$(STDXX) $(INC)
CFLAGS = -c -g3 -Wall -std=$(STD) $(INC)
LFLAGS = -L$(LIBDIR) -L$(BINDIR) $(LIB) -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive -static-libgcc -static-libstdc++ 

## Define Scope
# all : embed $(SRC) $(EXE)
all : $(SRC) $(SLB) $(EXE)

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
$(EXE) : $(OBJ) $(FBJ)
	-windres $(RESF) -O coff -o $(RESO)
	$(CCXX) $(RESO) $^ $(LFLAGS) -o $@

## Clean Object Files
clean : $(subst /,\,$(OBJ) $(FBJ))
	-$(DC) obj/*.o
