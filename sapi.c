#include <string.h>
#include <stdio.h>
#include "sapi.h"

static const s8 registers[][3] = {
	"RIP", "RSP", "RBP",
	"RAX", "RBX", "RCX",
	"RDX", "REX", "RFX",
	"RGX", "RHX", "RIX",
	"RJX"
};

void spy_pushcfunction(spy_state* S, const s8* name, void (*f)(spy_state*, u8)) {
	spy_cfunc func;
	func.fptr = f;
	strcpy(func.identifier, name);
	S->cfuncs[S->nfuncs++] = func;
}

u64 spy_pushstring(spy_state* S, const s8* str) {
	u64 head = (u64)S->reg[SP] - 1;
	while (*str) {
		S->mem[(u64)--S->reg[SP]] = *str++;		
	}
	S->mem[(u64)--S->reg[SP]] = 0;
	return head;
}

u64 spy_pushchar(spy_state* S, s8 c) {
	u64 head = (u64)S->reg[SP] - 1;
	S->mem[(u64)--S->reg[SP]] = c;
	S->mem[(u64)--S->reg[SP]] = 0;
	return head;
}

void spy_setregister(spy_state* S, const s8* name, f64 val) {
	u8 i = 0;
	while (strncmp(name, registers[i++], 3));
	S->reg[i - 1] = val;
}

f64 spy_getregister(spy_state* S, const s8* name) {
	u8 i = 0;
	while (strncmp(name, registers[i++], 3));
	return S->reg[i - 1];
}

const s8* spy_getarg(spy_state* S, u8 argn) {
	return registers[argn + 7];
}

f64 spy_gettop(spy_state* S) {
	return S->mem[(u64)S->reg[SP]++];
}

s8* spy_getstr(spy_state* S, const s8* regname, s8* buf) {
	s8* start = buf;
	u64 ptr = (u64)spy_getregister(S, regname);
	while (S->mem[(u64)ptr]) {
		*buf++ = (s8)S->mem[(u64)ptr];
		ptr += 8;
	}
	*buf++ = 0;
	return start;
}

s8 spy_getchar(spy_state* S, const s8* regname) {
	return (s8)spy_getregister(S, regname);
}
