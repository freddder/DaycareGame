#include "cNPCCharacter.h"

#include "Lua.hpp"
#include <iostream>

#include "Engine.h"
#include "LuaCallable.h"

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

	lua_register(L, "ShowDialogLine", ShowDialogLine);
	lua_register(L, "ShowDialogOptions", ShowDialogOptions);
	lua_register(L, "EndDialog", EndDialog);
}

cNPCCharacter::~cNPCCharacter()
{
	lua_close(L);
}

void cNPCCharacter::PromptInteract(int input)
{
	// CAREFUL: if doning this way, make sure this is set back to null when done interacting
	Engine::currInteractingEntity = this;

	lua_getglobal(L, "interact");
	lua_pushinteger(L, input);
	lua_pcall(L, 1, 0, 0);
}
