#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "svm.h"

int main(int argc, char** argv) {

	spy_state* S = spy_newstate();
	
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaL_dofile(L, "sfinal.lua");
	lua_getglobal(L, "sfinal_main");
	lua_pushstring(L, "test.spys");
	lua_pcall(L, 1, 0, 0);

	FILE* f = fopen("test.spyb", "rb");
	fseek(f, 0, SEEK_END);
	unsigned long long len = ftell(f);
	fseek(f, 0, SEEK_SET);
	unsigned char* contents = malloc(len + 1);
	fread(contents, len, 1, f);
	contents[len] = 0;

	spy_run(S, contents);
	spy_debug(S);

	return 0;

}
