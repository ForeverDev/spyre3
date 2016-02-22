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
	
	/*	
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	
	if (luaL_dofile(L, "scompile2.lua")) {
		printf("lua error: %s\n", lua_tostring(L, -1));
		return 1;
	}
	lua_getglobal(L, "scompile2_main");
	lua_pushstring(L, argv[1]);
	if (lua_pcall(L, 1, 0, 0)) {
		printf("lua error: %s\n", lua_tostring(L, -1));
		return 1;
	}
	*/
	lexb_state* L = lexb_newstate();
	lexb_readAndTokenize(L, argv[1]);

	char newfn[1024];
	int len;
	strcpy(newfn, argv[1]);
	len = strlen(newfn);
	newfn[len] = 0;
	newfn[len - 1] = 'b';

	spy_readAndRun(S, newfn);

	return 0;

}
