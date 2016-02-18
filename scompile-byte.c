#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "scompile-byte.h"

const s8* compb_registers[13] = {
	"rip", "rsp", "rbp",
	"rax", "rbx", "rcx",
	"rdx", "rex", "rfx",
	"rgx", "rhx", "rix",
	"rjx"
};

compb_state* compb_newstate() {
	
	compb_state* C = malloc(sizeof(compb_state));
	C->handle = NULL;

	memset(C->rom, 0, sizeof(C->rom));
	C->romp = C->rom;

	memset(C->code, 0, sizeof(C->code));
	C->codep = C->code;

	return C;

}

s32 compb_isValidRegister(compb_state* C, const s8* regname) {

	for (u32 i = 0; i < 13; i++) {
		if (!strncmp(compb_registers[i], regname, 3)) {
			return i;
		}
	}

	return -1;

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

	// this is the current section we're in
	s8* current_section;

	while (t->type != TAIL) {
		switch (t->type) {
			case HEAD:
			case TAIL:
				break;
			case IDENTIFIER:
				for (u32 i = 0; t->word[i]; i++) {
					t->word[i] = tolower(t->word[i]);
				}
				if (compb_isValidRegister(C, t->word) > -1) {
					printf("found register %s\n", t->word);	
				} else if (!strncmp(t->word, "section", 7)) {
					t = t->next;
					current_section = t->word;
					t = t->next;
				}
				break;
			// to silence annoying compiler warning
			default:
				break;
		}
		t = t->next;
	}

	exit(1);

}
