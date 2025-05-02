#include "cNPCCharacter.h"

#include "Lua.hpp"
#include <iostream>

cNPCCharacter::cNPCCharacter(std::string textureName, glm::vec3 pos) : cCharacterEntity(pos)
{
	blocksTile = true;

	spriteModel = new cCharacterSprite(textureName, pos);

	L = luaL_newstate();
	luaL_openlibs(L);

	if (luaL_dofile(L, "source/LuaScripts/TestNPC.lua") != LUA_OK) {
		std::cerr << "Error loading script: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}

	if (luaL_dofile(L, "source/LuaScripts/NPCLogic.lua") != LUA_OK) {
		std::cerr << "Error loading script: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
}

cNPCCharacter::~cNPCCharacter()
{
	lua_close(L);
}

void cNPCCharacter::PromptInteract()
{
	lua_getglobal(L, "interact");
	lua_pushinteger(L, 0);
	lua_pcall(L, 1, 1, 0);

	std::string result = lua_tostring(L, -1);
	std::cout << result << std::endl;
	lua_pop(L, 1);
}
