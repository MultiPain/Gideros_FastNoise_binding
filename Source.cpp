#include "gideros.h"
#include "lua.h"
#include "lauxlib.h"
#include "FastNoise.h"

// some Lua helper functions
#ifndef abs_index
#define abs_index(L, i) ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)
#endif

static void luaL_newweaktable(lua_State* L, const char* mode)
{
	lua_newtable(L);			// create table for instance list
	lua_pushstring(L, mode);
	lua_setfield(L, -2, "__mode");	  // set as weak-value table
	lua_pushvalue(L, -1);             // duplicate table
	lua_setmetatable(L, -2);          // set itself as metatable
}

static void luaL_rawgetptr(lua_State* L, int idx, void* ptr)
{
	idx = abs_index(L, idx);
	lua_pushlightuserdata(L, ptr);
	lua_rawget(L, idx);
}

static void luaL_rawsetptr(lua_State* L, int idx, void* ptr)
{
	idx = abs_index(L, idx);
	lua_pushlightuserdata(L, ptr);
	lua_insert(L, -2);
	lua_rawset(L, idx);
}

static char keyWeak = ' ';

class GNoise : public GProxy
{
public:

	GNoise() {}
	~GNoise() {}
};

static int destructNoise(lua_State* L)
{
	void* ptr = *(void**)lua_touserdata(L, 1);
	GReferenced* object = static_cast<GReferenced*>(ptr);
	GNoise* n = static_cast<GNoise*>(object->proxy());

	n->unref();

	return 0;
}

static int initNoise(lua_State* L)
{
	GNoise* n = new GNoise();
	g_pushInstance(L, "Noise", n->object());

	luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	lua_pushvalue(L, -2);
	luaL_rawsetptr(L, -2, n);
	lua_pop(L, 1);

	lua_pushvalue(L, -1);
	return 1;
}

static int loader(lua_State* L)
{
	const luaL_Reg functionlist[] = {
		{"new", initNoise},
		{NULL,NULL}
	};
	g_createClass(L, "Noise", NULL, NULL, destructNoise, functionlist);
	luaL_newweaktable(L, "v");
	luaL_rawsetptr(L, LUA_REGISTRYINDEX, &keyWeak);

	lua_getglobal(L, "Noise");
	lua_pushnumber(L, 1);
	lua_setfield(L, -2, "MY_GLOBAL_CLASS_FIELD");
	lua_pop(L, 1);

	return 0;
}

static void g_initializePlugin(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");

	lua_pushcfunction(L, loader);
	lua_setfield(L, -2, "FastNoise");
	
	lua_pop(L, 2);
}

static void g_deinitializePlugin(lua_State* L)
{
	
}

REGISTER_PLUGIN("FastNoise", "0.1a");
