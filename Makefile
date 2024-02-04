SRCDIR=src
INCDIR=inc
OBJDIR=build
EXEDIR=bin

CPP=g++
CPPFLAGS=-I${INCDIR} -MMD -MP -Wall -g -pthread

SRC=$(wildcard $(SRCDIR)/*.cpp)
SRC+=$(SRCDIR)/lexer.cpp
SRC+=$(SRCDIR)/parser.cpp

OBJ=$(addprefix $(OBJDIR)/, $(notdir $(SRC:.cpp=.o)))

ASSEMBLER=bin/assembler
LINKER=bin/linker
EMULATOR=bin/emulator

all: assembler linker emulator

assembler: $(ASSEMBLER)

linker: $(LINKER)

emulator: $(EMULATOR)

$(ASSEMBLER): $(filter-out $(OBJDIR)/linkerMain.o $(OBJDIR)/emulatorMain.o, $(OBJ)) | $(EXEDIR)
	$(CPP) $(CPPFLAGS) -o $@ $^

$(LINKER): $(filter-out $(OBJDIR)/assemblerMain.o $(OBJDIR)/emulatorMain.o, $(OBJ)) | $(EXEDIR)
	$(CPP) $(CPPFLAGS) -o $@ $^

$(EMULATOR): $(filter-out $(OBJDIR)/assemblerMain.o $(OBJDIR)/linkerMain.o, $(OBJ)) | $(EXEDIR)
	$(CPP) $(CPPFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/lexer.cpp $(SRCDIR)/parser.cpp | $(OBJDIR)
	$(CPP) $(CPPFLAGS) -c -o $@ $<

-include $(wildcard $(OBJDIR)/*.d)

$(SRCDIR)/lexer.cpp: misc/lexer.l
	flex misc/lexer.l

$(SRCDIR)/parser.cpp: misc/parser.y misc/lexer.l
	bison misc/parser.y

$(OBJDIR):
	mkdir $@

$(EXEDIR):
	mkdir $@

clean:
	rm -rf build
	rm -f *.txt
	rm -f *.o
	rm -f *.hex
	rm -f $(SRCDIR)/lexer.cpp
	rm -f $(SRCDIR)/parser.cpp
	rm -f $(INCDIR)/lexer.hpp
	rm -f $(INCDIR)/parser.hpp
	
.PHONY: all clean assembler linker emulator
