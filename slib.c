#include <stdio.h>
#include "sapi.h"
#include "slib.h"

void spyL_loadlibs(spy_state* S) {
	spy_pushcfunction(S, "print", spyL_io_print);	
}

void spyL_io_print(spy_state* S, u8 nargs) {
	printf("%f\n", spy_getregister(S, "RDX"));
}
