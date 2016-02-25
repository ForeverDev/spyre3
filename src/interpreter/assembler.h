#ifndef __SCOMPILE_BYTE_H
#define __SCOMPILE_BYTE_H

#include "assembler-lex.h"
#include "info.h"

extern const s8* compb_registers[13];
extern const s8* compb_opcodenames[0xff];	

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

typedef struct compb_label {
	
	s8					identifier[64];
	u32					value;
	struct compb_label* next;

} compb_label;


typedef struct compb_state {
	
	s8				rom[SIZE_ROM];
	s8*				romp;

	s8				code[65536];
	s8*				codep;

	compb_label* 	labels;
	
} compb_state;

compb_state* 	compb_newstate();

s8				compb_getRegister(compb_state*, const s8*);

s32				compb_getLabel(compb_state*, const s8*);
void			compb_newLabel(compb_state*, const s8*, u32);

s32				compb_getOpcode(compb_state*, const s8*);

s32				compb_getInstructionMode(compb_state*, lexb_token**, u32);

void			compb_strupper(compb_state*, s8*);
u8*				compb_compileTokens(compb_state*, lexb_token*, const s8*);

#endif
