#include "emulator.hpp"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

bool Emulator::run = true;
Memory Emulator::memory;
Emulator::InstructionRegister Emulator::ir;
Gpr Emulator::gpr;
Csr Emulator::csr;
bool Emulator::invalidInstructionInterrupt = false;
bool Emulator::timerInterrupt = false;
bool Emulator::terminalInterrupt = false;
bool Emulator::softwareInterrupt = false;

void Emulator::initEmulator(FILE* f) {
	const uint32_t endFile = 0xffffffff;
	uint32_t loadAddress = 0;
	uint32_t sectionSize = 0;
	uint8_t byte;
	fread(&loadAddress, sizeof(uint32_t), 1, f);
	fread(&sectionSize, sizeof(uint32_t), 1, f);
	while(loadAddress != endFile && sectionSize != endFile) {
		for(uint32_t i = 0 ; i < sectionSize ; i++) {
			fread(&byte, sizeof(uint8_t), 1, f);
			memory.put8(loadAddress+i, byte);
		}
		fread(&loadAddress, sizeof(uint32_t), 1, f);
		fread(&sectionSize, sizeof(uint32_t), 1, f);
	}
	gpr.set(PC, startAddress);
}

void Emulator::InstructionRegister::operator=(uint32_t value) {
	opCode = (value & 0xff000000) >> 24;
	regA = (value & 0x00f00000) >> 20;
	regB = (value & 0x000f0000) >> 16;
	regC = (value & 0x0000f000) >> 12;
	disp = (value & 0x00000fff);
}

void Emulator::cpuRoutine() {
	while(run) {
		instructionFetch();
		try {
			instructionExecute();
		} catch (std::exception& e) {
			invalidInstructionInterrupt = true;
		}
		interrupt();
	}
}

void Emulator::instructionFetch() {
	ir = (memory.get8(gpr.get(PC)) << 24) | (memory.get8(gpr.get(PC)+1) << 16) | (memory.get8(gpr.get(PC)+2) << 8) | (memory.get8(gpr.get(PC)+3));
	gpr.inc(PC, 4);
}

 void Emulator::instructionExecute() {
	switch(ir.opCode) {
		case 0x00: //halt
			run = false;
			break;
		case 0x10: //int
			softwareInterrupt = true;
			break;
		case 0x20: //call
			gpr.inc(SP, -4);
			memory.put32(gpr.get(SP), gpr.get(PC));
			gpr.set(PC, gpr.get(ir.regA) + gpr.get(ir.regB) + ir.disp);
			break;
		case 0x21: //call
			gpr.inc(SP, -4);
			memory.put32(gpr.get(SP), gpr.get(PC));
			gpr.set(PC, memory.get32(gpr.get(ir.regA) + gpr.get(ir.regB) + ir.disp));
			break;
		case 0x30: //jmp
			gpr.set(PC, gpr.get(ir.regA) + ir.disp);
			break;
		case 0x31: //beq
			if(gpr.get(ir.regB) == gpr.get(ir.regC))gpr.set(PC, gpr.get(ir.regA) + ir.disp);
			break;
		case 0x32: //bne
			if(gpr.get(ir.regB) != gpr.get(ir.regC))gpr.set(PC, gpr.get(ir.regA) + ir.disp);
			break;
		case 0x33: //bgt
			if(gpr.get(ir.regB) > gpr.get(ir.regC))gpr.set(PC, gpr.get(ir.regA) + ir.disp);
			break;
		case 0x38: //jmp
			gpr.set(PC, memory.get32(gpr.get(ir.regA) + ir.disp));
			break;
		case 0x39: //beq
			if(gpr.get(ir.regB) == gpr.get(ir.regC))gpr.set(PC, memory.get32(gpr.get(ir.regA) + ir.disp));
			break;
		case 0x3a: //bne
			if(gpr.get(ir.regB) != gpr.get(ir.regC))gpr.set(PC, memory.get32(gpr.get(ir.regA) + ir.disp));
			break;
		case 0x3b: //bgt
			if(gpr.get(ir.regB) > gpr.get(ir.regC))gpr.set(PC, memory.get32(gpr.get(ir.regA) + ir.disp));
			break;
		case 0x40: //xchg
			uint32_t tmp;
			tmp = gpr.get(ir.regB);
			gpr.set(ir.regB, gpr.get(ir.regC));
			gpr.set(ir.regC, tmp);
			break;
		case 0x50: //add
			gpr.set(ir.regA, gpr.get(ir.regB) + gpr.get(ir.regC));
			break;
		case 0x51: //sub
			gpr.set(ir.regA, gpr.get(ir.regB) - gpr.get(ir.regC));
			break;
		case 0x52: //mul
			gpr.set(ir.regA, gpr.get(ir.regB) * gpr.get(ir.regC));
			break;
		case 0x53: //div
			gpr.set(ir.regA, gpr.get(ir.regB) / gpr.get(ir.regC));
			break;
		case 0x60: //not
			gpr.set(ir.regA, ~gpr.get(ir.regB));
			break;
		case 0x61: //and
			gpr.set(ir.regA, gpr.get(ir.regB) & gpr.get(ir.regC));
			break;
		case 0x62: //or
			gpr.set(ir.regA, gpr.get(ir.regB) | gpr.get(ir.regC));
			break;
		case 0x63: //xor
			gpr.set(ir.regA, gpr.get(ir.regB) ^ gpr.get(ir.regC));
			break;
		case 0x70: //shl
			gpr.set(ir.regA, gpr.get(ir.regB) << static_cast<uint32_t>(gpr.get(ir.regC)));
			break;
		case 0x71: //shr
			gpr.set(ir.regA, gpr.get(ir.regB) >> static_cast<uint32_t>(gpr.get(ir.regC)));
			break;
		case 0x80: //st memdir
			memory.put32(gpr.get(ir.regA) + gpr.get(ir.regB) + ir.disp, gpr.get(ir.regC));
			break;
		case 0x81: //push
			gpr.inc(ir.regA, ir.disp);
			memory.put32(gpr.get(ir.regA), gpr.get(ir.regC));
			break;
		case 0x82: //st memidir
			memory.put32(memory.get32(gpr.get(ir.regA) + gpr.get(ir.regB) + ir.disp), gpr.get(ir.regC));
			break;
		case 0x90: //csrrd
			gpr.set(ir.regA, csr[ir.regB]);
			break;
		case 0x91:
			gpr.set(ir.regA, gpr.get(ir.regB) + ir.disp);
			break;
		case 0x92:
			gpr.set(ir.regA, memory.get32(gpr.get(ir.regB) + gpr.get(ir.regC) + ir.disp));
			break;
		case 0x93: //pop, ret
			gpr.set(ir.regA, memory.get32(gpr.get(ir.regB)));
			gpr.inc(ir.regB, ir.disp);
			break;
		case 0x94:
			csr[ir.regA] = gpr.get(ir.regB);
			break;
		case 0x95:
			csr[ir.regA] = csr[ir.regB] | ir.disp;
			break;
		case 0x96:
			csr[ir.regA] = memory.get32(gpr.get(ir.regB) + gpr.get(ir.regC) + ir.disp);
			break;
		case 0x97:
			csr[ir.regA] = memory.get32(gpr.get(ir.regB));
			gpr.inc(ir.regB, ir.disp);
			break;
		default:
			throw std::exception();
			break;
	}
} 

void Emulator::interrupt() {
	bool interrupt = false;
	uint32_t status = ~(csr[STATUS]);
	if(invalidInstructionInterrupt) {
		interrupt = true;
		invalidInstructionInterrupt = false;
		csr[CAUSE] = 1;
	} else if (timerInterrupt && ((status & 0b101) == 0b101)) {
		interrupt = true;
		timerInterrupt = false;
		csr[CAUSE] = 2;
	} else if (terminalInterrupt && ((status & 0b110) == 0b110)) {
		interrupt = true;
		terminalInterrupt = false;
		csr[CAUSE] = 3;
	} else if (softwareInterrupt) {
		interrupt = true;
		softwareInterrupt = false;
		csr[CAUSE] = 4;
	}
	if(interrupt && (csr[HANDLER] != 0xffffffff)) {
		gpr.inc(SP, 4);
		memory.put32(gpr.get(SP), csr[STATUS]);
		gpr.inc(SP, 4);
		memory.put32(gpr.get(SP), gpr.get(PC));
		csr[STATUS] &= (~0x1);
		gpr.set(PC, csr[HANDLER]);
	}
}

void Emulator::terminalRoutine() {
	termios oldSettings, newSettings;
	uint8_t c;
	tcgetattr(STDIN_FILENO, &oldSettings);
	newSettings = oldSettings;
	newSettings.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
	while(run) {
		if((c = memory.get32(TERM_OUT)) != 0){
			write(STDOUT_FILENO, &c, 1);
		}

		if(read(STDIN_FILENO, &c, 1) > 0 && c != '\n') {
			memory.put32(TERM_IN, c);
			terminalInterrupt = true;
		}
	}
	fcntl(STDIN_FILENO, F_SETFL, flags);
	tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
}

void Emulator::timerRoutine() {

}

void Emulator::printRegisters() {
	printf("\n-----------------------------------------------------------------\n");
	printf("Emulated processor executed halt instruction");
	for(uint32_t i = 0 ; i < 16 ; i++) {
		if(i%4)
			printf("   ");
		else
			printf("\n");
		if(i<10)
			printf("%2s%d=0x%08x","r", i, gpr.get(i));
		else
			printf("%1s%d=0x%08x","r", i, gpr.get(i));
	}
	printf("\n");
}