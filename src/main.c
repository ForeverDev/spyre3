#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "interpreter/assembler.h"
#include "interpreter/assembler-lex.h"
#include "interpreter/spyre.h"
#include "interpreter/api.h"

void print_usage() {

}

int main(int argc, char** argv) {
	
	int flags[256];
	memset(flags, 0, sizeof(flags));
	int c;

	char* flag_compile_name = NULL;
	char* flag_output_name = NULL;
	char* flag_input_name = NULL;

	int i = 1;
	for (; i < argc; i++) {
		if (argv[i][0] == '-') {
			flags[argv[i][1]] = 1;
		} else {
			flag_input_name = argv[i];
		}
	}
	
	if (flags['c'] && flags['r']) {
		printf("Can't compile and run at the same time.  If you want to compile on the fly, just do spy -g <filename>\n");
		exit(1);
	}

	char input_name[1024];
	char wd[1024];

	getcwd(wd, 1024);
	strcpy(input_name, wd);
	strcpy(input_name, "/");
	strcpy(input_name, flag_input_name);

	if (flags['g']) {
		
		spy_state* S = spy_newstate();
		lexb_state* L = lexb_newstate();
		FILE* src = fopen(argv[1], "r");
		unsigned long long flen;
		char* fsrc;
		char output_name[1024];

		if (!src) {
			printf("Couldn't open file '%s'\n", argv[1]);
			return 1;
		}

		strcpy(output_name, input_name);
		strcat(output_name, "s");

		fseek(src, 0, SEEK_END);
		flen = ftell(src);
		fseek(src, 0, SEEK_SET);
		fsrc = malloc(flen + 1);
		fsrc[flen] = 0;
		fread(fsrc, flen, 1, src);
	
		// TODO once the compiler is done, obviously we don't want to reassemble it every time
		// the compiler should already be in bytecode format
		const unsigned char* compilesrc = lexb_readAndTokenize(L, "/usr/local/include/spyre/tiny.spys");
		const unsigned char* code = spy_prepare(S, &compilesrc[4]);
		u64 fcontents = spy_pushstring(S, fsrc);
		u64 fname = spy_pushstring(S, input_name);
		f64 foutput = spy_pushstring(S, output_name);
		/*
			ARGS:
				REX = file contents;
				RGX = file name;
				RHX = file output name;
		*/
		spy_setregister(S, "REX", (f64)fcontents);
		spy_setregister(S, "RFX", (f64)flen);
		spy_setregister(S, "RGX", (f64)fname);
		spy_setregister(S, "RHX", (f64)foutput);
		spy_run(S, code);

	} else if (flags['r']) {
	
		// spy file.spy -g
		// spy file.spy -c
		// spy file.spys -r
		
		// no need to compile the source, just run it
		spy_state* S = spy_newstate();
		lexb_state* L = lexb_newstate();
		const unsigned char* assembly = lexb_readAndTokenize(L, flag_input_name);
		const unsigned char* code = spy_prepare(S, assembly);
		spy_run(S, code);

	}
	
	return 0;

}
