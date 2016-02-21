#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "sapi.h"
#include "slib.h"
#include "spyre.h"
#include "scompile-byte.h"

spy_state* spy_newstate() {
	spy_state* S = malloc(sizeof(spy_state));
	
	memset(S->mem, 0, sizeof(S->mem));
	memset(S->reg, 0, sizeof(S->reg));
	S->reg[SP] = START_STACK;
	S->reg[BP] = START_STACK;
	S->flags = 0x00;
	S->nfuncs = 0;

	return S;
}

#define ARITH(op)														\
	switch (mode) {														\
		case 2:															\
			S->reg[(u64)a] op b;										\
			break;														\
		case 3:															\
			S->reg[(u64)a] op S->reg[(u64)b];							\
			break;														\
		case 4:															\
			S->reg[(u64)a] op S->mem[(u64)S->reg[(u64)b] + (u64)c];		\
			break;														\
		case 5:															\
			S->mem[(u64)S->reg[(u64)a] + (u64)b] op c;					\
			break;														\
		case 6:															\
			S->mem[(u64)S->reg[(u64)a] + (u64)b] op S->reg[(u64)c];		\
			break;														\
	}

#define BIT(op)																											\
	switch (mode) {																										\
		case 2:																											\
			S->reg[(u64)a] = (u64)S->reg[(u64)a] op (u64)b;																\
			break;																										\
		case 3:																											\
			S->reg[(u64)a] = (u64)S->reg[(u64)a] op (u64)S->reg[(u64)b];												\
			break;																										\
		case 4:																											\
			S->reg[(u64)a] = (u64)S->reg[(u64)a] op (u64)S->mem[(u64)S->reg[(u64)b] + (u64)c];							\
			break;																										\
		case 5:																											\
			S->mem[(u64)S->reg[(u64)a] + (u64)b] = (u64)S->mem[(u64)S->reg[(u64)a] + (u64)b] op (u64)c;					\
			break;																										\
		case 6:																											\
			S->mem[(u64)S->reg[(u64)a] + (u64)b] = (u64)S->mem[(u64)S->reg[(u64)a] + (u64)b] op (u64)S->reg[(u64)c];	\
			break;																										\
	}

#define COMPARE(op)	{																\
	u8 istrue;																		\
	switch (mode) {																	\
		case 2:																		\
			istrue = S->reg[(u64)a] op b;											\
			break;																	\
		case 3:																		\
			istrue = S->reg[(u64)a] op S->reg[(u64)b];								\
			break;																	\
		case 4:																		\
			istrue = S->reg[(u64)a] op S->mem[(u64)S->reg[(u64)b] + (u64)c];		\
			break;																	\
		case 5:																		\
			istrue = S->mem[(u64)S->reg[(u64)a] + (u64)b] op c;						\
			break;																	\
		case 6:																		\
			istrue = S->mem[(u64)S->reg[(u64)a] + (u64)b] op S->reg[(u64)c];		\
	}																				\
	if (istrue) {																	\
		S->flags |= FLAG_CMP;														\
	} else {																		\
		S->flags &= ~FLAG_CMP;														\
	}																				\
}
			

void spy_run(spy_state* S, const u8* code) {
	while (1) {
		u8 opcode = code[(u64)S->reg[IP]++];
		//printf("op %s 0x%02x\n", compb_opcodenames[opcode], opcode);
		u8 mode = code[(u64)S->reg[IP]++];
		f64 a = 0;					
		f64 b = 0;
		f64 c = 0;
		// first, assign (a, b, c) correctly based on the mode
		switch (mode) {
			case 0:
				break;
			case 1:
				a = code[(u64)S->reg[IP]++];
				break;
			case 2:
				a = code[(u64)S->reg[IP]++];
				memcpy(&b, &code[(u64)S->reg[IP]], sizeof(f64));
				S->reg[IP] += sizeof(f64);
				break;
			case 3:
				a = code[(u64)S->reg[IP]++];
				b = code[(u64)S->reg[IP]++];
				break;
			case 4:
				a = code[(u64)S->reg[IP]++];
				b = code[(u64)S->reg[IP]++];
				memcpy(&c, &code[(u64)S->reg[IP]], sizeof(u64));
				S->reg[IP] += sizeof(u64);
				break;
			case 5:
				a = code[(u64)S->reg[IP]++];
				memcpy(&b, &code[(u64)S->reg[IP]], sizeof(u64));
				S->reg[IP] += sizeof(u64);
				memcpy(&c, &code[(u64)S->reg[IP]], sizeof(f64));
				S->reg[IP] += sizeof(f64);
				break;
			case 6:
				a = code[(u64)S->reg[IP]++];
				memcpy(&b, &code[(u64)S->reg[IP]], sizeof(u64));
				S->reg[IP] += sizeof(u64);
				c = code[(u64)S->reg[IP]++];
				break;
			case 7:
				memcpy(&a, &code[(u64)S->reg[IP]], sizeof(u64));
				S->reg[IP] += sizeof(u64);
				break;
		}
		// now, test opcode again and use a, b and c accordingly
		switch (opcode) {
			case 0x00:	// NULL
				return;
			case 0x01:	// EXIT
				return;
			case 0x02:	// RET
				S->reg[IP] = S->mem[(u64)S->reg[SP]++];
				break;
			case 0x20: 	// MOV
				ARITH(=);
				break;
			case 0x21:	// ADD
				ARITH(+=);
				break;
			case 0x22:	// SUB 
				ARITH(-=);
				break;
			case 0x23:	// MUL
				ARITH(*=);
				break;
			case 0x24:	// DIV
				ARITH(/=);
				break;
			case 0x25:	// NEG
				switch(mode) {
					case 1:
						S->reg[(u64)a] *= -1;
						break;
					case 8:
						S->mem[(u64)S->reg[(u64)a] + (u64)b] *= -1;
						break;
				}
				break;
			case 0x26:	// OR 
				BIT(|);
				break;
			case 0x27:	// AND
				BIT(&);
				break;
			case 0x28:	// XOR
				BIT(^);
				break;
			case 0x29:	// NOT
				switch(mode) {
					case 1:
						S->reg[(u64)a] = ~(u64)S->reg[(u64)a];
						break;
					case 8:
						S->mem[(u64)S->reg[(u64)a] + (u64)b] = ~(u64)S->mem[(u64)S->reg[(u64)a] + (u64)b];
						break;
				}
				break;
			case 0x2a:	// SHL
				BIT(<<);
				break;
			case 0x2b:	// SHR
				BIT(>>);
				break;
			case 0x2c:	// LT
				COMPARE(<);
				break;
			case 0x2d:	// LE
				COMPARE(<=);
				break;
			case 0x2e:	// GT
				COMPARE(>);
				break;
			case 0x2f:	// GE
				COMPARE(>=);
				break;
			case 0x30:	// CMP
				COMPARE(==);
				break;
			case 0x31:	// LAND
			case 0x32:	// LOR
			case 0x33:	// LNOT
				break;
			case 0x40: 	// PUSH
				switch (mode) {
					case 1:
						S->mem[(u64)(--S->reg[SP])] = S->reg[(u64)a];
						break;
					case 7:
						S->mem[(u64)(--S->reg[SP])] = a;
						break;
				}
				break;
			case 0x41:	// POP
				switch (mode) {
					case 0:
						S->reg[SP]++;
						break;
					case 1:
						S->reg[(u64)a] = S->mem[(u64)S->reg[SP]++];
						break;
				}
				break;
			case 0x60:	// CALL
				S->mem[(u64)(--S->reg[SP])] = S->reg[IP]; 
				S->reg[IP] = a;
				break;
			case 0x61: { // CCALL 
				s8 buf[128];
				s8* bp = buf;
				u8 foundfunc = 0;
				u8 nargs = (u8)S->mem[(u64)S->reg[SP]++];
				while (S->mem[(u64)a]) {
					*bp++ = S->mem[(u64)a++];
				}
				*bp = 0;
				for (u8 i = 0; i < S->nfuncs; i++) {
					if (!strcmp(S->cfuncs[i].identifier, buf)) {
						S->cfuncs[i].fptr(S, nargs);
						foundfunc = 1;
						break;
					}
				}
				if (!foundfunc) {
					spy_runtimeError(S, "Attempt to call non-existant Spyre or C function '%s'", buf);
					exit(1);
				}
				break;
			} 
			case 0x62:	// JMP
				S->reg[IP] = a;
				break;
			case 0x63:	// JIF
				if (!S->flags & FLAG_CMP) {
					S->reg[IP] = a;
				}
				break;
			case 0x64:	// JIT
				if (S->flags & FLAG_CMP) {
					S->reg[IP] = a;
				}
				break;
		}
	}
}

void spy_readAndRun(spy_state* S, const s8* filename) {
	FILE* f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	u64 len = ftell(f);
	fseek(f, 0, SEEK_SET);
	u8* contents = malloc(len + 1);
	fread(contents, len, 1, f);
	contents[len] = 0;

	u32 datastart;
	u32 codestart;

	// read the file headers (see opdocs.txt for details)
	memcpy(&datastart, &contents[0], sizeof(u32));
	memcpy(&codestart, &contents[4], sizeof(u32));

	for (u32 i = START_ROM; i < START_ROM + SIZE_ROM; i++) {
		if (datastart + i >= codestart) {
			break;
		}
		S->mem[i] = (f64)contents[(u64)datastart + i];
	}

	spyL_loadlibs(S);	
	spy_run(S, &contents[(u64)codestart]);
	//spy_debug(S);
}

void spy_runtimeError(spy_state* S, const s8* format, ...) {
	
	va_list args;
	va_start(args, format);	

	printf("Spyre runtime error: ");
	vprintf(format, args);
	printf("\n");

	va_end(args);

}

void spy_debug(spy_state* S) {
	printf("----REGISTERS----\n");
	printf("RIP: 0x%04x\nRSP: 0x%04x\nRBP: 0x%04x\n", (u32)S->reg[IP], (u32)S->reg[SP], (u32)S->reg[BP]);
	for (u32 i = 3; i < 12; i++) {
		printf("R%cX: %lf\n", 'A' + (i - 3), S->reg[i]);
	}	
	printf("\n----STACK----\n");
	for (u32 i = SIZE_STACK + SIZE_ROM - 1; i >= S->reg[SP]; i--) {
		printf("0x%08x: %lf\n", i, S->mem[i]);
	}
	printf("\n----FLAGS----\n");
	u8 copyflag = S->flags;
	for (u8 i = 0; i < 8; i++) {
		printf("0x%02x: %d\n", i, copyflag & 0x01); 
		copyflag >>= 1;
	}
}
