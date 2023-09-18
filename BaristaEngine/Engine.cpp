#include "Engine.h"

// TODO: https://www.youtube.com/watch?v=wkwgTBvjfA8 @ 13:49
bool Engine::Init(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height) {
	timer.Start();
	keyboard.EnableAutoRepeatKeys();

	auto renderWindowResult = this->renderWindow.Init(this, hInstance, windowTitle, windowClass, width, height);
	
	if (!renderWindowResult) {
		OutputDebugStringA("!! - Render window failed to initialise for whatever reason.\n");
		return false;
	}
	else {
		OutputDebugStringA("!! - Render window seems to be fine, initialising graphics system...\n");
	}
	
	auto DXInitResult = graphics.Init(this->renderWindow.getHWND(), width, height);
	if (!DXInitResult) {
		OutputDebugStringA("!! - Graphics system didn't initialise properly.\n");
		return false;
	}
	else {
		OutputDebugStringA("!! - Graphics system seems to be fine, finished engine init.\n");
	}

	return true;
}

bool Engine::ProcessMessages() {
	return this->renderWindow.ProcessMessages();
}

void Engine::Update() {
	float dt = timer.GetMillisecondsElapsed();
	timer.Restart();

	while (!keyboard.CharBufferIsEmpty()) {
		unsigned char Char = keyboard.ReadChar();
	}

	while (!keyboard.KeyBufferIsEmpty()) {
		KeyboardEvent keyBoardEvent = keyboard.ReadKey();
		unsigned char keyCode = keyBoardEvent.GetKeyCode();
		/*
		if (keyCode == 'W') {
			this->graphics.camera.AdjustPosition(this->graphics.camera.GetForwardVector() * cameraSpeed * dt);
		}
		if (keyCode == 'S') {
			this->graphics.camera.AdjustPosition(this->graphics.camera.GetForwardVector() * -1.0f * cameraSpeed * dt);
		}
		if (keyCode == 'A') {
			this->graphics.camera.AdjustPosition(this->graphics.camera.GetRightVector() * -1.0f * cameraSpeed * dt);
		}
		if (keyCode == 'D') {
			this->graphics.camera.AdjustPosition(this->graphics.camera.GetRightVector() * cameraSpeed * dt);
		}
		if (keyCode == VK_SPACE) {
			this->graphics.camera.AdjustPosition(this->graphics.camera.GetUpVector() * cameraSpeed * dt);
		}
		if (keyCode == VK_CONTROL) {
			this->graphics.camera.AdjustPosition(this->graphics.camera.GetUpVector() * -1.0f * cameraSpeed * dt);
		}*/
	}

	while (!mouse.EventBufferIsEmpty()) {
		MouseEvent mouseEvent = mouse.ReadEvent();

		if (mouseEvent.GetType() == MouseEvent::EventType::RAW_MOVE && mouse.IsRightDown()) {
			this->graphics.camera.AdjustRotation((float)mouseEvent.GetPosY() * 0.005f, (float)mouseEvent.GetPosX() * 0.01f, 0.0f);
		}
	}

	const float cameraSpeed = 0.006f;

	if (keyboard.KeyIsPressed('W'))
	{
		this->graphics.camera.AdjustPosition(this->graphics.camera.GetForwardVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		this->graphics.camera.AdjustPosition(this->graphics.camera.GetForwardVector() * -1.0f * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		this->graphics.camera.AdjustPosition(this->graphics.camera.GetRightVector() * -1.0f * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('D'))
	{
		this->graphics.camera.AdjustPosition(this->graphics.camera.GetRightVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed(VK_SPACE))
	{
		this->graphics.camera.AdjustPosition(this->graphics.camera.GetUpVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed(VK_CONTROL))
	{
		this->graphics.camera.AdjustPosition(this->graphics.camera.GetUpVector() * -1.0f * cameraSpeed * dt);
	}
}

void Engine::RenderFrame()
{
	graphics.RenderFrame();
}
