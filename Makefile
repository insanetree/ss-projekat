SRCDIR=src
INCDIR=inc
OBJDIR=build

CPP=g++
CPPFLAGS=-I${INCDIR} -std=c++11 -MMD -MP

SRC=$(wildcard $(SRCDIR)/*.cpp)
SRC+=$(SRCDIR)/lexer.cpp
SRC+=$(SRCDIR)/parser.cpp
OBJ=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

ASSEMBLER=build/assembler
LINKER=build/linker
EMULATOR=build/emulator

all: assembler linker emulator

assembler: $(ASSEMBLER)

linker: $(LINKER)

emulator: $(EMULATOR)

$(ASSEMBLER): $(filter-out $(OBJDIR)/linker.o $(OBJDIR)/emulator.o, $(OBJ))
	$(CPP) $(CPPFLAGS) -o $@ $^

$(LINKER): $(filter-out $(OBJDIR)/assembler.o $(OBJDIR)/emulator.o, $(OBJ))
	$(CPP) $(CPPFLAGS) -o $@ $^

$(EMULATOR): $(filter-out $(OBJDIR)/assembler.o $(OBJDIR)/linker.o, $(OBJ))
	$(CPP) $(CPPFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CPP) $(CPPFLAGS) -c -o $@ $<

-include $(wildcard $(OBJDIR)/*.d)

$(SRCDIR)/lexer.cpp: misc/lexer.l
	flex misc/lexer.l

$(SRCDIR)/parser.cpp: misc/parser.y misc/lexer.l
	bison misc/parser.y

clean:
	rm -rf build
	rm $(SRCDIR)/lexer.cpp
	rm $(SRCDIR)/parser.cpp
	
#.PHONY: all clean assembler linker emulator