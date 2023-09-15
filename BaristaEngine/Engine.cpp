#include "Engine.h"

bool Engine::Init(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height) {
	return this->renderWindow.Init(this, hInstance, windowTitle, windowClass, width, height);
}

bool Engine::ProcessMessages() {
	return this->renderWindow.ProcessMessages();
}

void Engine::Update() {

}