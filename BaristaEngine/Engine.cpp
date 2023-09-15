#include "Engine.h"

// TODO: https://www.youtube.com/watch?v=wkwgTBvjfA8 @ 13:49
bool Engine::Init(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height) {
	OutputDebugStringA("!! - Initialising engine\n");

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
	while (!keyboard.CharBufferIsEmpty()) {
		unsigned char Char = keyboard.ReadChar();
	}

	while (!keyboard.KeyBufferIsEmpty()) {
		KeyboardEvent keyBoardEvent = keyboard.ReadKey();
		unsigned char keyCode = keyBoardEvent.GetKeyCode();
	}

	while (!mouse.EventBufferIsEmpty()) {
		MouseEvent mouseEvent = mouse.ReadEvent();

		if (mouseEvent.GetType() == MouseEvent::EventType::RAW_MOVE) {

		}
	}
}

void Engine::RenderFrame()
{
	graphics.RenderFrame();
}
