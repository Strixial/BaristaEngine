#include "WindowContainer.h"

bool RenderWindow::Init(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height) {
	this->hInstance = hInstance;
	this->width = width;
	this->height = height;
	this->windowTitle = windowTitle;
	this->windowTitleWide = StringConverter::StringToWide(windowTitle);
	this->windowClass = windowClass;
	this->windowClassWide = StringConverter::StringToWide(windowClass);

	this->RegisterWindowClass();

	this->handle = CreateWindowEx(
		0,
		this->windowClassWide.c_str(),
		this->windowTitleWide.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		0,
		0,
		this->width,
		this->height,
		NULL,
		NULL,
		this->hInstance,
		nullptr
	);

	if (this->handle == NULL) {
		ErrorLogger::Log(GetLastError(), "CreateWindowEX failed for window " + this->windowTitle);
		return false;
	}

	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);

	return false;
}

bool RenderWindow::ProcessMessages() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	if (PeekMessage(&msg,
		this->handle,
		0,
		0,
		PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL) {
		if (!IsWindow(this->handle)) {
			this->handle = NULL;
			UnregisterClass(this->windowClassWide.c_str(), this->hInstance);
			return false;
		}
	}

	return true;
}

RenderWindow::~RenderWindow() {
	if (this->handle != NULL) {
		UnregisterClass(this->windowClassWide.c_str(), this->hInstance);
		DestroyWindow(handle);
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CHAR:
		unsigned char letter = static_cast<unsigned char>(wParam);
		return 0
	case WM_KEYDOWN:
		unsigned char keycode = static_cast<unsigned char>(wParam);
		return 0

	}
}

void RenderWindow::RegisterWindowClass() {
	WNDCLASSEX windowClass;
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = this->hInstance;
	windowClass.hIcon = NULL;
	windowClass.hIconSm = NULL;
	windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = this->windowClassWide.c_str();
	windowClass.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&windowClass);
}