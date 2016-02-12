#include <string.h>
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

f64 spy_getregister(spy_state* S, const s8* name) {
	u8 i = 0;
	while (strncmp(name, registers[i++], 3));
	return S->reg[i - 1];
}

void spy_setregister(spy_state* S, const s8* name, f64 val) {
	u8 i = 0;
	while (strncmp(name, registers[i++], 3));
	S->reg[i - 1] = val;
}

f64 spy_gettop(spy_state* S) {
	return S->mem[(u64)S->reg[SP]++];
}
