#ifndef __SAPI_H
#define __SAPI_H

#include "spyre.h"

// TODO make some of these functions inline

void 			spy_pushcfunction(spy_state*, const s8*, void (*)(spy_state*, u8));
void			spy_pushstring(spy_state*, const s8*);
void			spy_pushchar(spy_state*, s8);

void			spy_setmem(spy_state*, u64, f64);
void			spy_setregister(spy_state*, const s8*, f64 val);

f64				spy_getmem(spy_state*, u64);
f64 			spy_getregister(spy_state*, const s8*);
const s8*		spy_getarg(spy_state*, u8);
f64				spy_gettop(spy_state*);
s8*				spy_getstr(spy_state*, const s8*, s8*);
s8				spy_getchar(spy_state*, const s8*);
s64				spy_getint(spy_state*, const s8*);
f64				spy_getfloat(spy_state*, const s8*);

#endif
