#include <stdio.h>
#include "sapi.h"
#include "slib.h"

void spyL_loadlibs(spy_state* S) {
	spy_pushcfunction(S, "print", spyL_io_print);	
	spy_pushcfunction(S, "println", spyL_io_println);	
}

void spyL_io_print(spy_state* S, u8 nargs) {
	static s8 printfmt[1024];
	static s8 genbuf[1024];
	s8* pf = printfmt;
	spy_getstr(S, "REX", printfmt);
	while (*pf) {
		switch (*pf++) {
			case '%': {
				s8 code = *pf++;	
				switch (code) {
					case 'c':
						printf("%c", spy_getchar(S, "RFX"));
						break;
					case 's':
						// TODO fixme
						printf("%s", spy_getstr(S, "RFX", genbuf));
						break;
				}
				break;
			}
			default:
				printf("%c", *pf);
				break;
		}
	}
	spy_setregister(S, "RAX", 0);
}

void spyL_io_println(spy_state* S, u8 nargs) {
	spyL_io_print(S, nargs);
	printf("\n");
}

void spyL_io_getc(spy_state* S, u8 nargs) {
	
}
