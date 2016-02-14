#include <stdio.h>
#include <string.h>
#include "sapi.h"
#include "slib.h"

// IO LIBRARY

void spyL_loadlibs(spy_state* S) {
	spy_pushcfunction(S, "print", spyL_io_print);	
	spy_pushcfunction(S, "println", spyL_io_println);	
	spy_pushcfunction(S, "getchar", spyL_io_getchar);
	spy_pushcfunction(S, "getstr", spyL_io_getstr);

	spy_pushcfunction(S, "strlen", spyL_str_strlen);

	spy_pushcfunction(S, "malloc", spyL_mem_malloc);
	spy_pushcfunction(S, "free", spyL_mem_free);
}

// void println(rex=format, rfx=...);
void spyL_io_print(spy_state* S, u8 nargs) {
	static s8 printfmt[1024];
	static s8 genbuf[1024];
	s8* pf = printfmt;
	spy_getstr(S, "REX", printfmt);
	u8 i = 1;
	while (*pf) {
		switch (*pf++) {
			case '%': {
				switch (*pf++) {
					case 'c':
						printf("%c", spy_getchar(S, spy_getarg(S, i++)));
						break;
					case 's':
						printf("%s", spy_getstr(S, spy_getarg(S, i++), genbuf));
						break;
					case 'd':
						printf("%f", spy_getfloat(S, spy_getarg(S, i++)));
						break;
					case 'x':
						printf("%llx", spy_getint(S, spy_getarg(S, i++)));
						break;
				}
				break;
			}
			default:
				printf("%c", *(pf - 1));
				break;
		}
	}
	spy_setregister(S, "RAX", 0);
}

// void println(rex=format, rfx=...);
void spyL_io_println(spy_state* S, u8 nargs) {
	spyL_io_print(S, nargs);
	printf("\n");
}

// char getchar();
void spyL_io_getchar(spy_state* S, u8 nargs) {
	spy_setregister(S, "RAX", (f64)getchar());
}

// ptr getstr();
void spyL_io_getstr(spy_state* S, u8 nargs) {
	s8 buf[1024];
	s8* bp = buf;
	fgets(buf, 1024, stdin);
	u32 len = strlen(buf);
	buf[len] = 0;
	for (u32 i = len - 1; i > 0; i--) {
		buf[i] = buf[i - 1];
	}
	buf[0] = 0;
	while (*++bp);
	bp--;
	spy_pushchar(S, 0);
	while (*bp) {
		spy_pushchar(S, *bp--);
	}
	spy_setregister(S, "RAX", spy_getregister(S, "RSP"));
}

// STRING LIBRARY

void spyL_str_strlen(spy_state* S, u8 nargs) {
	u32 len = 0;
	u64 ptr = spy_getint(S, "REX");
	while (spy_getmem(S, ptr++)) {
		len++;
	}
	spy_setregister(S, "RAX", len);
}

// MEMORY MANAGEMENT LIBRARY

// void* malloc(rex=size);
void spyL_mem_malloc(spy_state* S, u8 nargs) {
	u32 head = START_MEMORY;
	f64 mem;
	while ((mem = spy_getmem(S, head))) {
		head += mem;
	}
	spy_setmem(S, head, spy_getregister(S, "REX"));
	spy_setregister(S, "RAX", head);
}

// void free(rex=ptr);
void spyL_mem_free(spy_state* S, u8 nargs) {
	spy_setmem(S, spy_getregister(S, "REX"), 0);
}
