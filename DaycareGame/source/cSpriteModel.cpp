#include "cSpriteModel.h"

#include "Engine.h"
#include "cRenderManager.h"

cSpriteModel::cSpriteModel()
{
	currSpriteId = 0;
	shaderHash = ComputeHash("sprite");
}

void cSpriteModel::SetUpUniforms()
{
	Manager::render.SetupSpriteSheet(textureHash, currSpriteId);
}
