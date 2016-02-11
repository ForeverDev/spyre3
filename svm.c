#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "svm.h"

spy_state* spy_newstate() {
	spy_state* S = malloc(sizeof(spy_state));
	
	memset(S->mem, 0, sizeof(S->mem));
	memset(S->reg, 0, sizeof(S->reg));
	S->reg[SP] = SIZE_STACK;
	S->reg[BP] = SIZE_STACK;
	S->flags = 0x00;

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
			istrue = S->reg[(u64)a] == b;											\
			break;																	\
		case 3:																		\
			istrue = S->reg[(u64)a] == S->reg[(u64)b];								\
			break;																	\
		case 4:																		\
			istrue = S->reg[(u64)a] == S->mem[(u64)S->reg[(u64)b] + (u64)c];		\
			break;																	\
		case 5:																		\
			istrue = S->mem[(u64)S->reg[(u64)a] + (u64)b] == c;						\
			break;																	\
		case 6:																		\
			istrue = S->mem[(u64)S->reg[(u64)a] + (u64)b] == S->reg[(u64)c];		\
	}																				\
	if (istrue) {																	\
		S->flags |= FLAG_CMP;														\
	} else {																		\
		S->flags &= ~FLAG_CMP;														\
	}																				\
}
			

void spy_run(spy_state* S, const u8* code) {
	// data is what we use to wrap 8 bytes
	// into a double using memcpy
	// memcpy(data, &code[S->reg[IP]], sizeof(f64));
	f64 data;
	while (1) {
		u8 opcode = code[(u64)S->reg[IP]++];
		u8 mode = code[(u64)S->reg[IP]++];
		f64 a;					
		f64 b;
		f64 c;
		// first, assign (a, b, c) correctly based on the mode
		switch (mode) {
			case 0:
				break;
			case 1:
				a = code[(u64)S->reg[IP]++];
				break;
			case 2:
				a = code[(u8)S->reg[IP]++];
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
			case 0x01:	// RET
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
				switch (mode) {
					case 7:
						S->mem[(u64)(--S->reg[SP])] = S->reg[IP]; 
						S->reg[IP] = a;
						break;
				}
				break;
			case 0x61:	// JMP
				S->reg[IP] = a;
				break;
			case 0x62:	// JIT
				if (S->flags & FLAG_CMP) {
					S->reg[IP] = a;
				}
			case 0x63:	// JIF
				if (!S->flags & FLAG_CMP) {
					S->reg[IP] = a;
				}
				break;
		}
	}
}

void spy_debug(spy_state* S) {
	printf("RIP: 0x%04x\nRSP: 0x%04x\nRBP: 0x%04x\n", (u32)S->reg[IP], (u32)S->reg[SP], (u32)S->reg[BP]);
	for (u32 i = 3; i < 12; i++) {
		printf("R%cX: %lf\n", 'A' + (i - 3), S->reg[i]);
	}	
	printf("\nSTACK\n");
	for (u32 i = SIZE_STACK - 1; i >= S->reg[SP]; i--) {
		printf("0x%08x: %lf\n", i, S->mem[i]);
	}
}
