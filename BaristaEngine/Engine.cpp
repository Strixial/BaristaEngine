#include "Engine.h"

bool Engine::Init(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height) {
	return this->renderWindow.Init(this, hInstance, windowTitle, windowClass, width, height);
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
	}
}