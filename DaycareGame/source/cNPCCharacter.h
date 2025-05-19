#pragma once

#include "cCharacterEntity.h"

struct lua_State;

class cNPCCharacter : public cCharacterEntity
{
public:
	cNPCCharacter(std::string textureName, glm::vec3 pos);
	~cNPCCharacter();

private:
	lua_State* L;
public:
	virtual void PromptInteract(int input = 0) override;
};