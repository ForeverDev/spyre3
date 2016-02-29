#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "spyre.h"
#include "assembler.h"

const s8* compb_registers[13] = {
	"RIP", "RSP", "RBP",
	"RAX", "RBX", "RCX",
	"RDX", "REX", "RFX",
	"RGX", "RHX", "RIX",
	"RJX"
};

const s8* compb_opcodenames[0xff] = {
	[EMPTY] = "EMPTY",
	[EXIT] = "EXIT",
	[RET] = "RET",

	[MOV] = "MOV",
	[ADD] = "ADD",
	[SUB] = "SUB",
	[MUL] = "MUL",
	[DIV] = "DIV",
	[NEG] = "NEG",
	[OR] = "OR",
	[AND] = "AND",
	[XOR] = "XOR",
	[NOT] = "NOT",
	[SHL] = "SHL",
	[SHR] = "SHR",
	[LT] = "LT",
	[LE] = "LE",
	[GT] = "GT",
	[GE] = "GE",
	[CMP] = "CMP",
	[LAND] = "LAND",
	[LOR] = "LOR",
	[LNOT] = "LNOT",
	[LEA] = "LEA",

	[PUSH] = "PUSH",
	[POP] = "POP",

	[CALL] = "CALL",
	[CCALL] = "CCALL",
	[JMP] = "JMP",
	[JIF] = "JIF",
	[JIT] = "JIT",

	[DLOG] = "DLOG"
};

compb_state* compb_newstate() {

	compb_state* C = malloc(sizeof(compb_state));

	memset(C->rom, 0, sizeof(C->rom));
	C->romp = C->rom;

	memset(C->code, 0, sizeof(C->code));
	C->codep = C->code;
	
	C->labels = malloc(sizeof(compb_label));	
	C->labels->value = 0;
	C->labels->next = NULL;

	return C;

}

s8 compb_getRegister(compb_state* C, const s8* regname) {
	for (u32 i = 0; i < 13; i++) {
		if (!strncmp(compb_registers[i], regname, 3)) {
			return i;
		}
	}
	return -1;
}

void compb_newLabel(compb_state* C, const s8* identifier, u32 value) {
	compb_label* label = malloc(sizeof(compb_label));
	strcpy(label->identifier, identifier);
	label->value = value;
	label->next = NULL;

	compb_label* at = C->labels;
	while (at->next) {
		at = at->next;
	}
	at->next = label;
}

s32 compb_getLabel(compb_state* C, const s8* identifier) {
	compb_label* at = C->labels;
	while (at) {
		if (!strcmp(at->identifier, identifier)) {
			return at->value;
		}
		at = at->next;
	}
	return -1;
}

s32 compb_getOpcode(compb_state* C, const s8* identifier) {
	for (u32 i = 0; i < 0xff; i++) {
		// check if i is a valid opcode
		if ((			  i <= 0x02) ||
			(i >= 0x20 && i <= 0x34) ||
			(i >= 0x40 && i <= 0x41) ||
			(i >= 0x60 && i <= 0x64) ||
			(i == 0xf0)) {
			if (!strcmp(compb_opcodenames[i], identifier)) {
				return i;
			}
		}
	}
	return -1;
}

// this is possibly the ugliest function I've ever written
s32 compb_getInstructionMode(compb_state* C, lexb_token** instruction, u32 nargs) {
	if (nargs == 0)
		return 0;	
	else if ( 	
				(nargs == 1) && 
				(compb_getRegister(C, instruction[0]->word) > -1)
			)
		return 1;
	else if (	
				(nargs == 2) && 
				(compb_getRegister(C, instruction[0]->word) > -1) &&
				(instruction[1]->type == NUMBER)				
			)
		return 2;
	else if (	// data is what we use to wrap 8 bytes
	// into a double using memcpy
	// memcpy(data, &code[S->reg[IP]], sizeof(f64));
				(nargs == 2) &&
				(compb_getRegister(C, instruction[0]->word) > -1) &&
				(compb_getRegister(C, instruction[1]->word) > -1)
			)
		return 3;
	else if (
				(nargs == 3) &&
				(compb_getRegister(C, instruction[0]->word) > -1) &&
				(compb_getRegister(C, instruction[1]->word) > -1) &&
				(instruction[2]->type == NUMBER)
			)
		return 4;
	else if (
				(nargs == 3) &&
				(compb_getRegister(C, instruction[0]->word) > -1) &&
				(instruction[1]->type == NUMBER) &&
				(instruction[2]->type == NUMBER)
			)
		return 5;
	else if (
				(nargs == 3) &&
				(compb_getRegister(C, instruction[0]->word) > -1) &&
				(instruction[1]->type == NUMBER) &&
				(compb_getRegister(C, instruction[2]->word) > -1)
			)
		return 6;
	else if (
				(nargs == 1) &&
				(instruction[0]->type == NUMBER)
			)
		return 7;
	else if (
				(nargs == 4) &&
				(compb_getRegister(C, instruction[0]->word) > -1) &&
				(instruction[1]->type == NUMBER) &&
				(compb_getRegister(C, instruction[2]->word) > -1) &&
				(instruction[3]->type == NUMBER)
			)
		return 8;
		return -1;
}

void compb_strupper(compb_state* C, s8* str) {
	for (u32 i = 0; i < strlen(str); i++) {
		str[i] = toupper(str[i]);
	}
}

u8* compb_compileTokens(compb_state* C, lexb_token* token_head, const s8* filename) {

	// this is the first token in the sequence of tokens
	lexb_token* t = token_head;

	// headers
	u32 data_start = sizeof(u32)*2;
	u32 code_start = 0;

	// temporary storage before we write to the file
	u8* bytecode = malloc(65536 * sizeof(u8));
	if (!bytecode) {
		printf("Couldn't allocate space for bytecode\n");
		exit(1);
	}
	u8* bp = bytecode;
	
	u32 ip = 0;

	// make all identifiers upper and remove useless punctuation (brackets, commas, etc)
	while (t->type != TAIL) {
		if (t->type == IDENTIFIER) {
			compb_strupper(C, t->word);
		} 
		t = t->next;
	}
	t = token_head;

	// first, scan for labels and create them
	while (t->type != TAIL) {
		if (t->type != STRING) {
			compb_strupper(C, t->word);
		}
		if (t->next->type == COLON) {
			compb_newLabel(C, t->word, ip);
		} else if (!strncmp(t->word, "LET", 3)) {
			s8* identifier;
			s8* wptr;
			s8 word[1024];
			s8* writeword = word;
			u32 value;
			t = t->next;
			identifier = t->word;
			ip += strlen(t->word) + 1;
			t = t->next;
			wptr = t->word;
			do {
				*writeword++ = *wptr;
			} while (*++wptr);
			*writeword = 0;
			compb_newLabel(C, identifier, (u32)(bp - bytecode));
			writeword = word;
			for (; *writeword; writeword++) {
				switch (*writeword) {
					case '\\':
						switch (*++writeword) {
							case 'n':
								*bp++ = '\n';
								break;
							case 't':
								*bp++ = '\t';
								break;
						}
						break;
					default:
						*bp++ = *writeword;
						break;
				}
			}
			*bp++ = 0;
		} else {
			switch (t->type) {
				case IDENTIFIER:
					if (!strncmp(t->word, "SECTION", 7)) {
						t = t->next;
						if (!strncmp(t->word, "DATA", 4)) {
							ip = 0;
						} else if (!strncmp(t->word, "CODE", 4)) {
							code_start = (u32)(bp - bytecode + sizeof(u32)*2);
							ip = 0;
						}
					} else if (compb_getLabel(C, t->word) > -1) {
						ip += 8;	
					} else if (compb_getRegister(C, t->word) > -1) {
						ip += 1;
					} else if (compb_getOpcode(C, t->word) > -1) {
						ip += 2;
					} else {
						ip += 8;
					}
					break;
				case NUMBER:
					ip += 8;
					break;
				// to silence the annoying compiler warning
				default:
					break;
			}
		}	
		t = t->next;
	}
	t = token_head;
	


	// now, do a find and replace for labels
	// find: 	label name
	// replace:	label location
	while (t->type != TAIL) {
		if (t->type == IDENTIFIER) {
			if (!strncmp(t->word, "LET", 3)) {
				t = t->next->next;
			} else {
				if (compb_getLabel(C, t->word) > -1) {
					sprintf(t->word, "%lf", (f64)compb_getLabel(C, t->word));
					t->type = NUMBER;
				}
				// if we find a label definition, remove it from the list.
				// note, we have to peek so that we can properly remove
				// two elements (the identifier and the colon) from
				// the list of tokens
				if (t->next->type == IDENTIFIER && t->next->next->type == COLON) {
					lexb_token* a = t->next;
					lexb_token* b = t->next->next;
					t->next = b->next;
					free(a);
					free(b);
				}
			}
		}
		t = t->next;
	}
	t = token_head;

	while (t->type != TAIL) {
		switch (t->type) {
			case HEAD:
			case TAIL:
				break;
			case IDENTIFIER: {
				s32 opcode = compb_getOpcode(C, t->word);
				if (opcode > -1) {
					t = t->next;
					lexb_token* instruction[3];
					u32 i = 0;
					while (t->type != NEWLINE && t->type != TAIL) {
						instruction[i++] = t;
						t = t->next;
					}
					u32 mode = compb_getInstructionMode(C, instruction, i);
					*bp++ = opcode;
					*bp++ = mode;
					// these are the variables we are going to
					// asign to based on the mode
					f64 a64;
					f64 b64;
					u8 	a8;
					u8	b8;
					switch (mode) {
						case 0:
							break;
						case 1:
							*bp++ = (u8)compb_getRegister(C, instruction[0]->word);
							break;
						case 2: {
							*bp++ = (u8)compb_getRegister(C, instruction[0]->word);
							a64 = strtod(instruction[1]->word, NULL);
							memcpy(bp, &a64, sizeof(f64));
							bp += sizeof(f64);
							break;
						}
						case 3: {
							*bp++ = (u8)compb_getRegister(C, instruction[0]->word);
							*bp++ = (u8)compb_getRegister(C, instruction[1]->word);
							break;
						}
						case 4: {
							*bp++ = (u8)compb_getRegister(C, instruction[0]->word);
							*bp++ = (u8)compb_getRegister(C, instruction[1]->word);
							a64 = strtod(instruction[2]->word, NULL);
							memcpy(bp, &a64, sizeof(f64));
							bp += sizeof(f64);
							break;
						}
						case 5: {
							*bp++ = (u8)compb_getRegister(C, instruction[0]->word);
							a64 = strtod(instruction[1]->word, NULL);
							memcpy(bp, &a64, sizeof(f64));
							bp += sizeof(f64);
							a64 = strtod(instruction[2]->word, NULL);
							memcpy(bp, &a64, sizeof(f64));
							bp += sizeof(f64);
							break;
						}
						case 6: {
							*bp++ = (u8)compb_getRegister(C, instruction[0]->word);
							a64 = strtod(instruction[1]->word, NULL);
							memcpy(bp, &a64, sizeof(f64));
							bp += sizeof(f64);
							*bp++ = (u8)compb_getRegister(C, instruction[2]->word);
							break;
						}
						case 7: {
							a64 = strtod(instruction[0]->word, NULL);
							memcpy(bp, &a64, sizeof(f64));
							bp += sizeof(f64);
							break;
						}
						case 8: {
							*bp++ = (u8)compb_getRegister(C, instruction[0]->word);
							a64 = strtod(instruction[1]->word, NULL);
							memcpy(bp, &a64, sizeof(f64));
							bp += sizeof(f64);
							*bp++ = (u8)compb_getRegister(C, instruction[2]->word);
							a64 = strtod(instruction[3]->word, NULL);
							memcpy(bp, &a64, sizeof(f64));
							bp += sizeof(f64);
							break;
						}
					}
				}
				break;
			}
			// to silence annoying compiler warning
			default:
				break;
		}
		t = t->next;
	}

	u8* final = malloc(sizeof(bytecode) + sizeof(data_start) + sizeof(code_start) + 1);
	memset(&final[0], 0, sizeof(final));
	memcpy(&final[0], &data_start, sizeof(data_start));
	memcpy(&final[4], &code_start, sizeof(code_start));
	memcpy(&final[8], bytecode, bp - bytecode);

	free(bytecode);

	return final;

}
