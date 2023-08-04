SRCDIR=src
INCDIR=inc
DEPDIR=build/dep
OBJDIR=build/obj

CPP=g++
CPPFLAGS=-I${INCDIR} -std=c++11

SRC=$(wildcard $(SRCDIR)/*.cpp)
OBJ=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))
DEP=$(patsubst $(SRCDIR)/%.cpp,$(DEPDIR)/%.d,$(SRC))

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

$(DEPDIR)/%.d: $(SRCDIR)/%.cpp
	mkdir -p $(DEPDIR)
	$(CPP) $(CPPFLAGS) -MM -MT $(OBJDIR)/$*.o $< > $@

include $(DEP)

clean:
	rm -rf build
	
.PHONY: all clean assembler linker emulator