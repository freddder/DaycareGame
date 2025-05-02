#pragma once
#include <glm/glm.hpp>

class cEntity
{
public:
	cEntity();
	~cEntity();

	glm::ivec3 position;
	bool blocksTile = false;

	// Interact
	virtual void PromptInteract();
	virtual void WalkInteract();

	// AI
};