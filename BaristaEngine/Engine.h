#pragma once
#include "WindowContainer.h"

class Engine : WindowContainer {
public:
	bool Init(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height);
	bool ProcessMessages();
};