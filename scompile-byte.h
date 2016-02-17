#ifndef __SCOMPILE_BYTE_H
#define __SCOMPILE_BYTE_H

#include "slex-byte.h"
#include "info.h"

struct 	compb_state;
enum	compb_opcodes;
enum	compb_registers;

typedef enum compb_opcodes {
	
	EMPTY	= 0x00,
	EXIT	= 0x01,
	RET		= 0x02,

	MOV		= 0x20,
	ADD		= 0x21,
	SUB		= 0x22,
	MUL		= 0x23,
	DIV		= 0x24,
	NEG		= 0x25,
	OR		= 0x26,
	AND		= 0x27,
	XOR		= 0x28,
	NOT		= 0x29,
	SHL		= 0x2a,
	SHR		= 0x2b,
	LT		= 0x2c,
	LE		= 0x2d,
	GT		= 0x2e,
	GE		= 0x2f,
	CMP		= 0x30,
	LAND	= 0x31,
	LOR		= 0x32,
	LNOT	= 0x33,

	PUSH	= 0x40,
	POP		= 0x41,

	CALL	= 0x60,
	CCALL	= 0x61,
	JMP		= 0x62,
	JIF		= 0x63,
	JIT		= 0x64

} compb_opcodes;

typedef enum compb_registers {
	
	RIP		= 0x01,
	RSP		= 0x02,
	RBP		= 0x03,
	RAX		= 0x04,
	RBX		= 0x05,
	RCX		= 0x06,
	RDX		= 0x07,
	REX		= 0x08,
	RFX		= 0x09,
	RGX		= 0x0a,
	RHX		= 0x0b,
	RIX		= 0x0c,
	RJX		= 0x0d

} compb_registers;

typedef struct compb_state {
	
	FILE*	handle;

	s8		rom[SIZE_ROM];
	s8*		romp;

	s8		code[65536];
	s8*		codep;
	
} compb_state;

compb_state* 	compb_newstate();
void			compb_writeCodeByte(compb_state*, u8);
void			compb_writeDataByte(compb_state*, u8);
void			compb_compileTokens(compb_state*, lexb_token*, const s8*);

#endif
