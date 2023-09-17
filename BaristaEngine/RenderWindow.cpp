#include "WindowContainer.h"

bool RenderWindow::Init(WindowContainer* pWindowContainer, HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height) {
	OutputDebugStringA("!! - Initialising render window...\n");
	this->hInstance = hInstance;
	this->width = width;
	this->height = height;
	this->windowTitle = windowTitle;
	this->windowTitleWide = StringConverter::StringToWide(windowTitle);
	this->windowClass = windowClass;
	this->windowClassWide = StringConverter::StringToWide(windowClass);

	this->RegisterWindowClass();

	RECT windowRect;
	windowRect.left = GetSystemMetrics(SM_CXSCREEN) / 2 - this->width / 2;
	windowRect.top = GetSystemMetrics(SM_CYSCREEN) / 2 - this->height / 2;
	windowRect.right = windowRect.left + this->width;
	windowRect.bottom = windowRect.top + this->height;
	AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	this->handle = CreateWindowEx(
		0,
		this->windowClassWide.c_str(),
		this->windowTitleWide.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		this->hInstance,
		pWindowContainer
	);

	if (this->handle == NULL) {
		ErrorLogger::CreateErrorBox(GetLastError(), "CreateWindowEX failed for window " + this->windowTitle);
		return false;
	}

	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);

	return true;
}

bool RenderWindow::ProcessMessages() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (PeekMessage(&msg,
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

HWND RenderWindow::getHWND() const
{
	return this->handle;
}

RenderWindow::~RenderWindow() {
	if (this->handle != NULL) {
		UnregisterClass(this->windowClassWide.c_str(), this->hInstance);
		DestroyWindow(handle);
	}
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	default: {
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_NCCREATE: {
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
		if (pWindow == nullptr) {
			ErrorLogger::CreateErrorBox("fucking dead, pointer to window container is null during WM_NCCREATE lmfaoooo");
			exit(-1);
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
		//return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	default: {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

void RenderWindow::RegisterWindowClass() {
	WNDCLASSEX windowClass;
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = HandleMessageSetup;
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