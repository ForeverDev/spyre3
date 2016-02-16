#ifndef __SLEX_BYTE_H
#define __SLEX_BYTE_H

#include <stdio.h>
#include "spyre.h"

enum	lexb_token_type;
struct	lexb_token;
struct	lexb_state;

typedef enum lexb_token_type {
	
	UNKNOWN		= 0,
	HEAD		= 1,
	TAIL		= 2,
	
	IDENTIFIER,
	OPENSQ,
	CLOSESQ,
	COLON,
	NUMBER,
	STRING

} lexb_token_type;

typedef struct lexb_token {
	
	lexb_token_type		type;
	s8					word[64];
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
static void lexb_report(lexb_state*, const s8*, ...);
static void lexb_pushtoken(lexb_state*, lexb_token_type, const s8*);
void		lexb_readAndTokenize(lexb_state*, const s8*);

#endif
