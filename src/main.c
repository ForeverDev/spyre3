#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "interpreter/slex-byte.h"
#include "interpreter/spyre.h"

int main(int argc, char** argv) {

	spy_state* S = spy_newstate();
	lexb_state* L = lexb_newstate();
	lexb_readAndTokenize(L, argv[1]);

	char newfn[1024];
	int len;
	strcpy(newfn, argv[1]);
	len = strlen(newfn);
	newfn[len] = 0;
	newfn[len - 1] = 'b';

	spy_readAndRun(S, newfn);
	
	remove(newfn);

	return 0;

}
