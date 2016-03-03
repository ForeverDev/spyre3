#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "lib.h"

// IO LIBRARY

void spyL_loadlibs(spy_state* S) {
	spy_pushcfunction(S, "printf", spyL_io_printf);	
	spy_pushcfunction(S, "getchar", spyL_io_getchar);
	spy_pushcfunction(S, "getstr", spyL_io_getstr);
	spy_pushcfunction(S, "fopen", spyL_io_fopen);
	spy_pushcfunction(S, "fclose", spyL_io_fclose);
	spy_pushcfunction(S, "fprintf", spyL_io_fprintf);
	spy_pushcfunction(S, "fputstr", spyL_io_fputstr);
	spy_pushcfunction(S, "fputchar", spyL_io_fputchar);
	spy_pushcfunction(S, "fputnum", spyL_io_fputnum);
	spy_pushcfunction(S, "fprintf", spyL_io_fprintf);

	spy_pushcfunction(S, "strlen", spyL_str_strlen);
	spy_pushcfunction(S, "strcpy", spyL_str_strcpy);
	spy_pushcfunction(S, "strcmp", spyL_str_strcmp);
	spy_pushcfunction(S, "sprintf", spyL_str_sprintf);

	spy_pushcfunction(S, "malloc", spyL_mem_malloc);
	spy_pushcfunction(S, "free", spyL_mem_free);
	spy_pushcfunction(S, "memcpy", spyL_mem_memcpy);
	spy_pushcfunction(S, "memset", spyL_mem_memset);
}

// void println(format, ...);
void spyL_io_printf(spy_state* S, u8 nargs) {
	s8* pf = S->gen0; 
	spy_getstr(S, "REX", S->gen0);
	u8 i = 1;
	while (*pf) {
		switch (*pf++) {
			case '%': {
				switch (*pf++) {
					case 'c':
						printf("%c", spy_getchar(S, spy_getarg(S, i++)));
						break;
					case 's':
						printf("%s", spy_getstr(S, spy_getarg(S, i++), S->gen1));
						break;
					case 'd':
						printf("%lld", spy_getint(S, spy_getarg(S, i++)));
						break;
					case 'f':
						printf("%f", spy_getfloat(S, spy_getarg(S, i++)));
						break;
					case 'x':
						printf("%llx", spy_getint(S, spy_getarg(S, i++)));
						break;
					case 'p':
						printf("0x%08llx", spy_getint(S, spy_getarg(S, i++)));
						break;
					case 't':
						printf("0x%08llx\n", (u64)spy_getcptr(S, spy_getarg(S, i++)));
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

// char getchar();
void spyL_io_getchar(spy_state* S, u8 nargs) {
	spy_setregister(S, "RAX", (f64)getchar());
}

// ptr getstr();
void spyL_io_getstr(spy_state* S, u8 nargs) {
	s8* bp = S->gen0;
	fgets(S->gen0, 1024, stdin);
	u32 len = strlen(S->gen0);
	S->gen0[len] = 0;
	for (u32 i = len - 1; i > 0; i--) {
		S->gen0[i] = S->gen0[i - 1];
	}
	S->gen0[0] = 0;
	while (*++bp);
	bp--;
	spy_pushchar(S, 0);
	while (*bp) {
		spy_pushchar(S, *bp--);
	}
	spy_setregister(S, "RAX", spy_getregister(S, "RSP"));
}

void spyL_io_fopen(spy_state* S, u8 nargs) {
	FILE* f;
	s8 fname[1024];
	s8 fmode[4];
	spy_getstr(S, "REX", fname);
	spy_getstr(S, "RFX", fmode);
	f = fopen(fname, fmode);
	if (f) {
		spy_regcpy(S, "RAX", f, sizeof(FILE*));
	} else {
		spy_setregister(S, "RAX", 0);
	}
}

void spyL_io_fclose(spy_state* S, u8 nargs) {
	FILE* f = (FILE*)spy_getcptr(S, "REX");
	fclose(f);
}

void spyL_io_fprintf(spy_state* S, u8 nargs) {
	// here we use a little trick with sprintf
	// if we pass a NULL pointer to the destination (REX)
	// sprintf will not write the string.  however, the
	// string will be in the general purpose buffer S->gen2

	// save the file argument
	void* file = spy_getcptr(S, "REX");
	
	spy_setregister(S, "REX", 0);
	spyL_str_sprintf(S, nargs);

	// the string to write is now in S->gen2
	fwrite(S->gen2, 1, strlen(S->gen2), file);

	spy_setregister(S, "RAX", 0);
}

void spyL_io_fputstr(spy_state* S, u8 nargs) {
	FILE* f;
	s8 str[1024];
	f = (FILE*)spy_getcptr(S, "REX");
	spy_getstr(S, "RFX", str);
	fputs(str, f);
	spy_setregister(S, "RAX", 0);
}

void spyL_io_fputchar(spy_state* S, u8 nargs) {
	FILE* f;
	s8 c;
	f = (FILE*)spy_getcptr(S, "REX");
	c = spy_getchar(S, "RFX");
	fputc(c, f);
	spy_setregister(S, "RAX", 0);
}

void spyL_io_fputnum(spy_state* S, u8 nargs) {
	FILE* f;
	f64 n;
	s8 word[128];
	s8* bp = word;
	f = (FILE*)spy_getcptr(S, "REX");
	n = spy_getregister(S, "RFX");
	sprintf(word, "%lld", (s64)n);
	fputs(word, f);
	spy_setregister(S, "RAX", 0);
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

void spyL_str_strcpy(spy_state* S, u8 nargs) {
	u64 dest = (u64)spy_getregister(S, "REX");
	u64 src = (u64)spy_getregister(S, "RFX");
	// get strlen of source
	spy_setregister(S, "REX", (f64)src);
	spyL_str_strlen(S, 1);
	u64 len = (u64)spy_getregister(S, "RAX");
	for (u64 i = 0; i < len; i++) {
		spy_setmem(S, dest + i, spy_getmem(S, src + i));
	}
	spy_setmem(S, dest + len, 0);
	spy_setregister(S, "RAX", 0);
}

void spyL_str_strcmp(spy_state* S, u8 nargs) {

	spy_getstr(S, "REX", S->gen0);
	spy_getstr(S, "RFX", S->gen1);

	spy_setregister(S, "RAX", (f64)(!strcmp(S->gen0, S->gen1)));
	
	// note we do NOT free stra and strb because
	// they are static and we reuse them
}

void spyL_str_sprintf(spy_state* S, u8 nargs) {
	s8* pf = S->gen0; 
	s8* bp = S->gen2;
	u64 addr = spy_getint(S, "REX");
	spy_getstr(S, "RFX", S->gen0);
	u8 i = 2;
	while (*pf) {
		switch (*pf++) {
			case '%': {
				switch (*pf++) {
					case 'c':
						bp += sprintf(bp, "%c", spy_getchar(S, spy_getarg(S, i++)));
						break;
					case 's':
						bp += sprintf(bp, "%s", spy_getstr(S, spy_getarg(S, i++), S->gen1));
						break;
					case 'd':
						bp += sprintf(bp, "%lld", spy_getint(S, spy_getarg(S, i++)));
						break;
					case 'f':
						bp += sprintf(bp, "%f", spy_getfloat(S, spy_getarg(S, i++)));
						break;
					case 'x':
						bp += sprintf(bp, "%llx", spy_getint(S, spy_getarg(S, i++)));
						break;
					case 'p':
						bp += sprintf(bp, "0x%08llx", spy_getint(S, spy_getarg(S, i++)));
						break;
				}
				break;
			}
			default:
				bp += sprintf(bp, "%c", *(pf - 1));
				break;
		}
	}
	*bp++ = 0;
	if (addr) {
		for (u64 i = 0; i < strlen(S->gen2); i++) {
			spy_setmem(S, addr + i, S->gen2[i]);	
		}
		spy_setmem(S, addr + strlen(S->gen2), 0);
	}
	spy_setregister(S, "RAX", 0);
}

// MEMORY MANAGEMENT LIBRARY

// void* malloc(size);
void spyL_mem_malloc(spy_state* S, u8 nargs) {
	u64 head = START_MEMORY;
	f64 mem;
	while ((mem = spy_getmem(S, head))) {
		head += (u64)mem;
	}
	spy_setmem(S, head, spy_getregister(S, "REX") + 1);
	spy_setregister(S, "RAX", head + 1);
}

// void free(ptr);
void spyL_mem_free(spy_state* S, u8 nargs) {
	spy_setmem(S, spy_getregister(S, "REX"), 0);
	spy_setregister(S, "RAX", 0);
}

void spyL_mem_memcpy(spy_state* S, u8 nargs) {
	u64 dest = (u64)spy_getregister(S, "REX");
	u64 src = (u64)spy_getregister(S, "RFX");
	u64 len = (u64)spy_getregister(S, "RGX");
	for (u64 i = 0; i < len; i++) {
		spy_setmem(S, dest + i, src + i);
	}
	spy_setregister(S, "RAX", 0);
	 
}

void spyL_mem_memset(spy_state* S, u8 nargs) {
	u64 dest = (u64)spy_getregister(S, "REX");
	u64 imm = (u64)spy_getregister(S, "RFX");
	u64 len = (u64)spy_getregister(S, "RGX");
	for (u64 i = 0; i < len; i++) {
		spy_setmem(S, dest + i, imm);
	}
	spy_setregister(S, "RAX", 0);
}
