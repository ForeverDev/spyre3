#ifndef __SLEX_H
#define __SLEX_H

#include <stdio.h>
#include "types.h"

enum	lexc_token_type;
struct	lexc_token;
struct	lexc_state;

typedef enum lexc_token_type {
	
	UNKNOWN		= 0x00,
	HEAD		= 0x01,
	TAIL		= 0x02,
	
	IDENTIFIER	= 0x03,
	OPENSQ		= 0x04,
	CLOSESQ		= 0x05,
	COLON		= 0x06,
	COMMA		= 0x07,
	PLUS		= 0x08,
	MINUS		= 0x09,
	SEMICOLON 	= 0x0a,
	NUMBER		= 0x0b,
	STRING		= 0x0c,
	NEWLINE		= 0x0d

} lexc_token_type;

typedef struct lexc_token {
	
	lexc_token_type		type;
	s8					word[64];
	struct lexc_token*	next;			

} lexc_token;

typedef struct lexc_state {
	
	lexc_token*	head;
	struct {
		FILE*	handle;
		s8*		contents;
		u64		length;
	} source;
	
} lexc_state;

lexc_state*	lexc_newstate();
static void lexc_pushtoken(lexc_state*, lexc_token_type, const s8*);
void 		lexc_report(const s8*, ...);
void		lexc_readAndTokenize(lexc_state*, const s8*);

#endif
