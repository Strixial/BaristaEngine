#pragma once
#include "ErrorLogger.h"

class WindowContainer;

class RenderWindow {
public:
	bool Init(WindowContainer* pWindowContainer, HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height);
	bool ProcessMessages();
	~RenderWindow();
private:
	void RegisterWindowClass();
	HWND handle = NULL; // Handle for this window
	HINSTANCE hInstance = NULL; // Handle to application
	std::string windowTitle = "";
	std::wstring windowTitleWide = L"";
	std::string windowClass = "";
	std::wstring windowClassWide = L"";
	int width = 0;
	int height = 0;
};