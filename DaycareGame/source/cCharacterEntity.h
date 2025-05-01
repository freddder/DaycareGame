#pragma once

#include "cEntity.h"
#include "CharacterSprite.h"

extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

class cCharacterEntity : public cEntity
{
public:
	cCharacterEntity(glm::vec3 pos);
	~cCharacterEntity();

	// Move
	cCharacterSprite* spriteModel;
	void AttemptMovement(eDirection dir, bool run);

	// Follow
private:
	cCharacterEntity* follower;
public:
	void SetFollower(cCharacterEntity* newFollower);
	void UnsetFollower();
	void MoveFollower(glm::vec3 newPos, bool run);

private:
	lua_State* L;
};