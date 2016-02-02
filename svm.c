#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "svm.h"

spy_state* spy_newstate() {
	spy_state* S = malloc(sizeof(spy_state));

	memset(S->mem, 0, sizeof(S->memory));
	memset(S->reg, 0, sizeof(S->reg));

	return S;
}

/*
	OPCODE FORMATS
	iA:	<opcode>	<format>	<a>		<b>		<c>
					0 = a reg, b imm
					1 = a reg, b reg
					2 = a reg, b reg, c offset

	0x00	MOV(iA)
*/

void spy_run(spy_state* S, u8* code) {
	while (1) {
		u8 opcode = code[S->reg[IP]++];
		switch (opcode) {
			// NULL
			case 0x00:
				return;
			// MOV (iA)
			case 0x01: {
				u8 format = code[S->reg[IP]++];
				u8 reg = code[S->reg[IP]++];
				u8 val = code[S->reg[IP]++];
				switch (format) {
					// imm into reg
					case 0: {
						f64 d;
						memcpy(&d, code[S->reg[IP]], 8);
						S->reg[IP] += 8;	
						S->reg[reg] = d;
						break;
					}
					// reg into reg
					case 1:
						S->reg[reg] = S->reg[val];
						break;
					// memory(reg + offset) into reg
					case 2:
						S->reg[reg] = S->mem[S->reg[val] + code[S->reg[IP]++]];
						break;
				}
				break;
			}
		}
	}
}
