#ifndef __SLEX_BYTE_H
#define __SLEX_BYTE_H

#include <stdio.h>
#include "info.h"

enum	lexb_token_type;
struct	lexb_token;
struct	lexb_state;

typedef enum lexb_token_type {
	
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

} lexb_token_type;

typedef struct lexb_token {
	
	lexb_token_type		type;
	s8					word[1024];
	struct lexb_token*	next;			

} lexb_token;

typedef struct lexb_state {
	
	lexb_token*	head;
	struct {
		FILE*	handle;
		s8*		contents;
		u64		length;
	} source;
	
} lexb_state;

lexb_state*	lexb_newstate();
static void lexb_pushtoken(lexb_state*, lexb_token_type, const s8*);
void 		lexb_report(const s8*, ...);
u8*			lexb_readAndTokenize(lexb_state*, const s8*);

#endif
