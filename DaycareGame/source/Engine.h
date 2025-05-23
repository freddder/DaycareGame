#pragma once

struct GLFWwindow;

class cCameraManager;
class cLightManager;
class cAnimationManager;
class cRenderManager;
class cMapManager;
class cSceneManager;
class cUIManager;
class cInputManager;
class cEntity;

namespace Manager
{
	extern cCameraManager camera;
	extern cLightManager light;
	extern cAnimationManager animation;
	extern cRenderManager render;
	extern cMapManager map;
	extern cSceneManager scene;
	extern cUIManager ui;
	extern cInputManager input;
}

enum eGameMode
{
	MAP,
	//BATTLE,
	MENU
};

namespace Engine
{
	extern eGameMode currGameMode;
	extern cEntity* currInteractingEntity;

	bool InitializeGLFW();

	void StartUpManagers();
	void ShutdownManagers();

	void GameLoop(bool renderDebugInfo);

	void ShutdownGLFW();

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
}