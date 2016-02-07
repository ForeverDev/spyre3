#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "svm.h"

/*
	OPCODE FORMATS
		MOV: 	format(0, 1, 2, 3, 4)
				0 = mov reg0, imm
				1 = mov reg0, reg1
				2 = mov reg0, [reg1 + offset]
				3 = mov [reg0 + offset], imm
				4 = mov [reg0 + offset], reg1

	0x00	MOV(iA)
*/

spy_state* spy_newstate() {
	spy_state* S = malloc(sizeof(spy_state));

	memset(S->mem, 0, sizeof(S->mem));
	memset(S->reg, 0, sizeof(S->reg));

	return S;
}

void spy_run(spy_state* S, const u8* code) {
	// data is what we use to wrap 8 bytes
	// into a double using memcpy
	// memcpy(data, &code[S->reg[IP]], sizeof(f64));
	u64 data;
	while (1) {
		u8 opcode = code[S->reg[IP]++];
		switch (opcode) {
			// NULL
			case 0x00:
				return;
			// MOV
			case 0x01: {
				u8 format = code[S->reg[IP]++];
				u8 reg = code[S->reg[IP]++];
				switch (format) {
					// mov reg0, imm 
					case 0: 
						memcpy(&data, &code[S->reg[IP]], sizeof(u64));
						S->reg[IP] += sizeof(u64);	
						S->reg[reg] = data;
						break;
					// mov reg0, reg1
					case 1: 
						S->reg[reg] = S->reg[code[S->reg[IP]++]];
						break;
					// mov reg0, [reg1 + offset]
					case 2: { 
						u8 rightreg = S->reg[S->reg[IP]++];
						S->reg[reg] = S->mem[rightreg + code[S->reg[IP]++]];
						printf("%d\n", rightreg + code[S->reg[IP] - 1]);
						break;
					}
					// mov [reg0 + offset], imm
					case 3: {
						u8 offset = code[S->reg[IP]++];
						memcpy(&data, &code[S->reg[IP]], sizeof(u64));
						S->reg[IP] += sizeof(u64);
						S->mem[S->reg[reg] + offset] = data;
					}
					// mov [reg0 + offset], reg1
					case 4: {
						u8 offset = code[S->reg[IP]++];
						S->mem[S->reg[reg] + offset] = S->reg[code[S->reg[IP]++]];
						break;
					}
				}
				break;
			}
		}
	}
}

void spy_toWords(spy_state* S, const u8* code) {
	
}

void spy_debug(spy_state* S) {
	for (u32 i = 0; i < 12; i++) {
		printf("REG %02d: %llu\n", i, S->reg[i]);
	}	
	printf("\n");
	for (u32 i = 0; i < 65536; i++) {
		if (S->mem[i] != 0) {
			printf("MEM %08x: %llu\n", i, S->mem[i]);
		}	
	}
}
