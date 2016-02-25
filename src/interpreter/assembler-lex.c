#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "assembler.h"
#include "assembler-lex.h"

lexb_state* lexb_newstate() {
	lexb_state* L = malloc(sizeof(lexb_state));
	L->source.handle = NULL;
	L->source.contents = NULL;
	L->source.length = 0;
	L->head = NULL;
	lexb_pushtoken(L, HEAD, "HEAD");
	return L;
}

void lexb_report(const s8* format, ...) {
	s8 buf[1024];
	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	strcat(buf, ".  (abort)");
	printf("%s\n", buf);
	exit(1);
}

void lexb_pushtoken(lexb_state* L, lexb_token_type t, const s8* word) {
	
	lexb_token* token = malloc(sizeof(lexb_token));
	token->type = t;
	token->next = NULL;
	strcpy(token->word, word);
	
	if (!L->head) {
		L->head = token;
		return;
	}

	lexb_token* i = L->head;

	while (i->next) {
		i = i->next;
	}

	i->next = token;
		
}

void lexb_dump(lexb_state* L) {

	lexb_token* i = L->head;
	
	while (i) {
		printf("(%02x, %s)\n", i->type, i->word);
		i = i->next;
	}

}

u8* lexb_readAndTokenize(lexb_state* L, const s8* filename) {

	// validate that the file exists and open it

	L->source.handle = fopen(filename, "r");
	if (!L->source.handle) {
		lexb_report("couldn't open file '%s'", filename);
	}

	// get the length of the file, make space for the
	// source string buffer
	fseek(L->source.handle, 0, SEEK_END);
	L->source.length = ftell(L->source.handle);
	fseek(L->source.handle, 0, SEEK_SET);
	L->source.contents = malloc(L->source.length * sizeof(s8) + 1);

	
	// read the contents of the file
	s8	c;
	u32 i = 0;
	while ((c = fgetc(L->source.handle)) != EOF) {
		L->source.contents[i++] = c;
	}
	L->source.contents[i] = 0;

	// handle tokens
	s8* p = L->source.contents;
	s8  buf[1024];
	s8* bp = buf;
	u8	next_num_neg = 0;
	while ((c = *p)) {
		if (c == '\n') {
			lexb_pushtoken(L, NEWLINE, "");
		} else if (c == ';') {
			while ((c = *++p) != '\n');
		// handle strings
		} else if (c == '"') {
			while ((c = *++p) != '"') {
				*bp++ = c;
			}
			*bp = 0;
			bp = buf;
			lexb_pushtoken(L, STRING, buf);
		// handle numbers
		} else if (isdigit(c)) {
			if (next_num_neg) {
				*bp++ = '-';
				next_num_neg = 0;
			}
			while (isdigit(c) || c == '.') {
				*bp++ = c;
				c = *++p;
			}
			p--;
			*bp = 0;
			bp = buf;
			lexb_pushtoken(L, NUMBER, buf);
		// handle identifiers / keywords / registers
		} else if (isalnum(c) || c == '_') {
			while (isalnum((c = *p++)) || c == '_') {
				*bp++ = c;
			}
			p -= 2;
			*bp = 0;
			bp = buf;
			lexb_pushtoken(L, IDENTIFIER, buf);
		// handle punctuation (operators, etc)
		} else if (c == ':') {
			buf[0] = c;
			buf[1] = 0;
			lexb_pushtoken(L, (
				c == ':' ? COLON : 
				UNKNOWN
			), buf);
		} else if (c == '-') {
			next_num_neg = 1;
		}
		p++;	
	}

	lexb_pushtoken(L, TAIL, "");

	fclose(L->source.handle);

	// now that we've lexed everything, pass the tokens
	// to the bytecode compiler
	compb_state* C = compb_newstate();
	
	return compb_compileTokens(C, L->head, filename);

}

