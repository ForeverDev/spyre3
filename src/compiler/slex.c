#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "slex.h"

lexc_state* lexc_newstate() {
	lexc_state* L = malloc(sizeof(lexc_state));
	L->source.handle = NULL;
	L->source.contents = NULL;
	L->source.length = 0;
	L->head = NULL;
	lexc_pushtoken(L, HEAD, "HEAD");
	return L;
}

void lexc_report(const s8* format, ...) {
	s8 buf[1024];
	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	strcat(buf, ".  (abort)");
	printf("%s\n", buf);
	exit(1);
}

void lexc_pushtoken(lexc_state* L, lexc_token_type t, const s8* word) {
	
	lexc_token* token = malloc(sizeof(lexc_token));
	token->type = t;
	token->next = NULL;
	strcpy(token->word, word);
	
	if (!L->head) {
		L->head = token;
		return;
	}

	lexc_token* i = L->head;

	while (i->next) {
		i = i->next;
	}

	i->next = token;
		
}

void lexc_dump(lexc_state* L) {

	lexc_token* i = L->head;
	
	while (i) {
		printf("(%02x, %s)\n", i->type, i->word);
		i = i->next;
	}

}

void lexc_readAndTokenize(lexc_state* L, const s8* filename) {

	// validate that the file exists and open it

	L->source.handle = fopen(filename, "r");
	if (!L->source.handle) {
		lexc_report("couldn't open file '%s'", filename);
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
			lexc_pushtoken(L, NEWLINE, "");
		} else if (c == ';') {
			while ((c = *++p) != '\n');
		// handle strings
		} else if (c == '"') {
			while ((c = *++p) != '"') {
				*bp++ = c;
			}
			*bp = 0;
			bp = buf;
			lexc_pushtoken(L, STRING, buf);
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
			lexc_pushtoken(L, NUMBER, buf);
		// handle identifiers / keywords / registers
		} else if (isalnum(c) || c == '_') {
			while (isalnum((c = *p++)) || c == '_') {
				*bp++ = c;
			}
			p -= 2;
			*bp = 0;
			bp = buf;
			lexc_pushtoken(L, IDENTIFIER, buf);
		// handle punctuation (operators, etc)
		} else if (c == ':') {
			buf[0] = c;
			buf[1] = 0;
			lexc_pushtoken(L, (
				c == ':' ? COLON : 
				UNKNOWN
			), buf);
		} else if (c == '-') {
			next_num_neg = 1;
		}
		p++;	
	}

	lexc_pushtoken(L, TAIL, "");

	fclose(L->source.handle);
	
}

