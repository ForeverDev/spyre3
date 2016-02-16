#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "slex-byte.h"

lexb_state* lexb_newstate() {
	lexb_state* L = malloc(sizeof(lexb_state));
	L->source.handle = NULL;
	L->source.contents = NULL;
	L->source.length = 0;
	L->head = NULL;
	lexb_pushtoken(L, HEAD, "HEAD");
	return L;
}

void lexb_report(lexb_state* L, const s8* format, ...) {
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

void lexb_readAndTokenize(lexb_state* L, const s8* filename) {

	// validate that the file exists and open it

	L->source.handle = fopen(filename, "r");
	if (!L->source.handle) {
		lexb_report(L, "couldn't open file '%s'", filename);
	}

	// get the length of the file, make space for the
	// source string buffer
	fseek(L->source.handle, 0, SEEK_END);
	L->source.length = ftell(L->source.handle);
	fseek(L->source.handle, 0, SEEK_SET);
	L->source.contents = malloc(L->source.length * sizeof(s8));

	
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
	s8  strbuf[1024];
	s8* bp = buf;
	s8* sbp = strbuf;
	while ((c = *p)) {
		// handle strings
		if (c == '"') {
			while ((c = *++p) != '"') {
				*sbp++ = c;
			}
			p++;
			*sbp = 0;
			lexb_pushtoken(L, STRING, strbuf);
		// handle identifiers / keywords / registers
		} else if (isalnum(c)) {
			while (isalnum((c = *p++))) {
				*bp++ = c;
			}
			p--;
			*bp = 0;
			bp = buf;
			lexb_pushtoken(L, IDENTIFIER, buf);
		// handle punctuation (operators, etc)
		} else if (ispunct(c)) {
			buf[0] = c;
			buf[1] = 0;
			lexb_pushtoken(L, (
				c == '+' ? PLUS :
				c == '-' ? MINUS :
				c == '[' ? OPENSQ :
				c == ']' ? CLOSESQ :
				c == ':' ? COLON : 
				c == ',' ? COMMA : 
				UNKNOWN
			), buf);
		}
		p++;	
	}

	lexb_dump(L);

	fclose(L->source.handle);
	free(L->source.contents);

	exit(0);

}

