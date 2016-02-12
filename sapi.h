#ifndef __SAPI_H
#define __SAPI_H

#include "spyre.h"

void 	spy_pushcfunction(spy_state*, const s8*, void (*)(spy_state*, u8));
u64		spy_pushstring(spy_state*, const s8*);
u64		spy_pushchar(spy_state*, s8);
f64 	spy_getregister(spy_state*, const s8*);
void	spy_setregister(spy_state*, const s8*, f64 val);
f64		spy_gettop(spy_state*);
s8*		spy_getstr(spy_state*, const s8*, s8*);
s8		spy_getchar(spy_state*, const s8*);

#endif
