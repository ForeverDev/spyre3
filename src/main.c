#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "interpreter/assembler.h"
#include "interpreter/assembler-lex.h"
#include "interpreter/spyre.h"
#include "interpreter/api.h"


int main(int argc, char** argv) {
	
	spy_state* S = spy_newstate();
	lexb_state* L = lexb_newstate();
	FILE* src = fopen(argv[1], "r");
	unsigned long long flen;
	char* fsrc;
	char output_name[256];

	if (!src) {
		printf("Couldn't open file '%s'\n", argv[1]);
		return 1;
	}
	
	memset(output_name, 0, sizeof(output_name));
	strcpy(output_name, argv[1]);
	output_name[strlen(output_name) - 1] = 'b';

	fseek(src, 0, SEEK_END);
	flen = ftell(src);
	fseek(src, 0, SEEK_SET);
	fsrc = malloc(flen + 1);
	fsrc[flen] = 0;
	fread(fsrc, flen, 1, src);

	const unsigned char* compilesrc = lexb_readAndTokenize(L, "/usr/local/include/spyre/tiny.spys");
	const unsigned char* code = spy_prepare(S, compilesrc);
	u64 fcontents = spy_pushstring(S, fsrc);
	u64 fname = spy_pushstring(S, argv[1]);
	f64 foutput = spy_pushstring(S, output_name);
	/*
		ARGS:
			REX = file contents;
			RGX = file name;
			RHX = file output name;
	*/
	spy_setregister(S, "REX", (f64)fcontents);
	spy_setregister(S, "RFX", (f64)fname);
	spy_setregister(S, "RGX", (f64)foutput);
	spy_run(S, code);
	
	return 0;

}
