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

void spy_pushstring(spy_state* S, const s8* str) {
	// TODO fix this, somehow the writing offset is fucked
	// possible fix: push string in reverse order?
	while (*str) {
		S->mem[(u64)--S->reg[SP]] = *str++;		
	}
	S->mem[(u64)--S->reg[SP]] = 0;
}

void spy_pushchar(spy_state* S, s8 c) {
	S->mem[(u64)--S->reg[SP]] = c;
}

void spy_setmem(spy_state* S, u64 addr, f64 val) {
	S->mem[addr] = val;
}

void spy_setregister(spy_state* S, const s8* name, f64 val) {
	u8 i = 0;
	while (strncmp(name, registers[i++], 3));
	S->reg[i - 1] = val;
}

void spy_regcpy(spy_state* S, const s8* regname, const void* src, size_t size) {
	memcpy(spy_getregaddr(S, regname), &src, size);
}

void* spy_getcptr(spy_state* S, const s8* regname) {
	u64 ptr;
	f64 fptr;
	fptr = spy_getregister(S, regname);
	memcpy(&ptr, &fptr, sizeof(u64));
	return (void*)ptr;
}

f64 spy_getmem(spy_state* S, u64 addr) {
	return S->mem[addr];
}

f64 spy_getregister(spy_state* S, const s8* regname) {
	u8 i = 0;
	while (strncmp(regname, registers[i++], 3));
	return S->reg[i - 1];
}

f64* spy_getregaddr(spy_state* S, const s8* regname) {
	u8 i = 0;
	while (strncmp(regname, registers[i++], 3));
	return &S->reg[i - 1];
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
		*buf++ = (s8)S->mem[(u64)ptr++];
	}
	*buf = 0;
	return start;
}

s8 spy_getchar(spy_state* S, const s8* regname) {
	return (s8)spy_getregister(S, regname);
}

s64 spy_getint(spy_state* S, const s8* regname) {
	return (s64)spy_getregister(S, regname);
}

f64 spy_getfloat(spy_state* S, const s8* regname) {
	return (f64)spy_getregister(S, regname);
}
