#include "cCharacterEntity.h"

#include "Engine.h"
#include "cMapManager.h"

#include <iostream>

cCharacterEntity::cCharacterEntity(glm::vec3 pos)
{
	position = pos;
	follower = nullptr;

	L = luaL_newstate();
	luaL_openlibs(L);

	if (luaL_dofile(L, "source/LuaScripts/NPC.lua") != LUA_OK) {
		std::cerr << "Error loading script: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
}

cCharacterEntity::~cCharacterEntity()
{
	Manager::map.RemoveEntityFromTile(position);

	if (spriteModel)
		delete spriteModel;

	lua_close(L);
}

void cCharacterEntity::AttemptMovement(eDirection dir, bool run)
{
	if (!spriteModel->modelAnimation->isDone) return;

	eEntityMoveResult moveResult = Manager::map.TryMoveEntity(this, dir);

	glm::vec3 prevPos = position;
	position = spriteModel->AnimateMovement(dir, run, moveResult);

	if (follower && moveResult != eEntityMoveResult::FAILURE)
	{
		MoveFollower(prevPos, run);
	}
}

void cCharacterEntity::SetFollower(cCharacterEntity* newFollower)
{
	if (follower) return;

	follower = newFollower;
}

void cCharacterEntity::UnsetFollower()
{
	follower = nullptr;
}

void cCharacterEntity::MoveFollower(glm::vec3 newPos, bool run)
{
	eDirection dir = UP;
	if ((newPos - follower->spriteModel->model->position).z == 1.f) dir = RIGHT;
	else if ((newPos - follower->spriteModel->model->position).z == -1.f) dir = LEFT;
	else if ((newPos - follower->spriteModel->model->position).x == 1.f) dir = UP;
	else if ((newPos - follower->spriteModel->model->position).x == -1.f) dir = DOWN;
	else return;

	follower->spriteModel->modelAnimation->isDone = true;

	follower->AttemptMovement(dir, run);
}
