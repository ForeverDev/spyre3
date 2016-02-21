#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "scompile-byte.h"

#undef WAS_INIT_INSTRUCTIONS

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

	[PUSH] = "PUSH",
	[POP] = "POP",

	[CALL] = "CALL",
	[CCALL] = "CCALL",
	[JMP] = "JMP",
	[JIF] = "JIF",
	[JIT] = "JIT",
	[POP] = "EXIT"
};

compb_state* compb_newstate() {

	compb_state* C = malloc(sizeof(compb_state));
	C->handle = NULL;

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
		if ((i >= 0x00 && i <= 0x02) ||
			(i >= 0x20 && i <= 0x33) ||
			(i >= 0x40 && i <= 0x41) ||
			(i >= 0x60 && i <= 0x64)) {
			if (!strcmp(compb_opcodenames[i], identifier)) {
				return i;
			}
		}
	}
	return -1;
}

void compb_strupper(compb_state* C, s8* str) {
	for (u32 i = 0; i < strlen(str); i++) {
		str[i] = toupper(str[i]);
	}
}

void compb_compileTokens(compb_state* C, lexb_token* token_head, const s8* filename) {
	
	// the filename passed to us has the form *.spys
	// we want to write to *.spyb, so replace the
	// last character with 'b'
	s8 writefile[1024];
	strcpy(writefile, filename);
	writefile[strlen(writefile) - 1] = 'b';

	C->handle = fopen(writefile, "w");
	if (!C->handle) {
		// we can just report the error using
		// the lexer's function because it doesn't
		// take a lex state as a parameter
		lexb_report("Couldn't write to file '%s'. (abort)", writefile);
	}
	
	// this is the first token in the sequence of tokens
	lexb_token* t = token_head;

	// headers
	u32 data_start = 0;
	u32 code_start = 0;

	// temporary storage before we write to the file
	u8* bytecode = malloc(65536 * sizeof(u8));
	u8* bp = bytecode;
	
	u32 ip = 0;

	// first, scan for labels and create them
	while (t->type != TAIL) {
		compb_strupper(C, t->word);
		if (t->next->type == COLON) {
			compb_newLabel(C, t->word, c);
		} else {
			switch (t->type) {
				case IDENTIFIER:
					if (compb_getLabel(C, t->word) > -1) {
						ip += 8;	
					} else if (compb_getRegister(C, t->word) > -1) {
						ip += 1;
					} else if (compb_getOpcode(C, t->word) > -1) {
						ip += 2;
					}
					break;
				case NUMBER:
					c += 8;
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
			// otherwise, if we found a label being used.  we will
			// replace the label identifier with the location of the label
			} else if (compb_getLabel(C, t->word) > -1) {
				sprintf(t->word, "%d", compb_getLabel(C, t->word));
				t->type = NUMBER;
				printf("%s\n", t->word);
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
			case IDENTIFIER:
				// found a section
				if (!strncmp(t->word, "section", 7)) {
					t = t->next;
					if (!strncmp(t->word, "data", 4)) {
						data_start = c;
					} else if (!strncmp(t->word, "code", 4)) {
						code_start = c;
					}

				// found a const variable declaration
				} else if (!strncmp(t->word, "let", 3)) {
					s8* identifier;
					s8* wptr;
					s8 word[64];
					s8* writeword = word;
					u32 value;
					t = t->next;
					identifier = t->word;
					t = t->next;
					wptr = t->word;
					do {
						*writeword++ = *wptr;
					} while (*++wptr);
					compb_newLabel(C, identifier, (u32)(bp - bytecode + sizeof(u32)*2));
					writeword = word;
					for (; *writeword; writeword++) {
						*bp++ = *writeword;
					}
					*bp++ = 0;

				// found a label
				} else if (t->next->type == COLON) {
					compb_newLabel(C, t->word, (u32)(bp - bytecode + sizeof(u32)*2));
					t = t->next;

				// found an instruction
				} else {
					printf("unknown %s %d\n", t->word, t->next->type);
				}
				break;
			// to silence annoying compiler warning
			default:
				break;
		}
		t = t->next;
	}

	fwrite((u8*)&data_start, sizeof(data_start), 1, C->handle);
	fwrite((u8*)&code_start, sizeof(code_start), 1, C->handle);
	fwrite(bytecode, bp - bytecode, 1, C->handle);

	free(bytecode);

	exit(1);

}
