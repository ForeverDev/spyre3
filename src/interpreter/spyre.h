#ifndef __SVM_H
#define __SVM_H

#include "info.h"

#define IP 0
#define SP 1
#define BP 2

#define	FLAG_CMP 0x01

struct spy_state;
struct spy_cfunc;

typedef struct spy_cfunc {
	
	s8 identifier[128];
	void (*fptr)(struct spy_state*, u8);

} spy_cfunc;

typedef struct spy_state {
	
	f64				mem[SIZE_MEMORY];
	f64				reg[13];
	u8				flags;
	u8				nfuncs;
	spy_cfunc 		cfuncs[128];

} spy_state;

spy_state*		spy_newstate();
const u8*		spy_prepare(spy_state*, const u8*);
static void		spy_runtimeError(spy_state*, const s8*, ...);
void			spy_run(spy_state*, const u8*);
static void		spy_debug(spy_state*);

#endif
