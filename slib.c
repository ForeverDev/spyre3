#include <stdio.h>
#include "sapi.h"
#include "slib.h"

void spyL_loadlibs(spy_state* S) {
	spy_pushcfunction(S, "print", spyL_io_print);	
	spy_pushcfunction(S, "println", spyL_io_println);	
}

void spyL_io_print(spy_state* S, u8 nargs) {
	printf("%f", spy_getregister(S, "REX"));
}

void spyL_io_println(spy_state* S, u8 nargs) {
	spyL_io_print(S, nargs);
	printf("\n");
}
