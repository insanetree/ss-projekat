#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include "global.hpp"
#include "memory.hpp"
#include "gpr.hpp"
#include "csr.hpp"

class Emulator {
public:
	static void initEmulator(FILE*);
	static void cpuRoutine();
	static void terminalRoutine();
	static void timerRoutine();
	static void printRegisters();
private:
	struct InstructionRegister {
		uint32_t opCode : 8;
		uint32_t regA : 4;
		uint32_t regB : 4;
		uint32_t regC : 4;
		int32_t disp : 12;
		void operator=(uint32_t);
	};

	static const uint32_t startAddress = 0x40000000;
	static bool run;
	static Memory memory;
	static InstructionRegister ir;
	static Gpr gpr;
	static Csr csr;

	static bool invalidInstructionInterrupt;
	static bool timerInterrupt;
	static bool terminalInterrupt;
	static bool softwareInterrupt;

	static void instructionFetch();
	static void instructionExecute();
	static void interrupt();
};

#endif