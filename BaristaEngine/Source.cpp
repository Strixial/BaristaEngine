#include "RenderWindow.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	RenderWindow Window;
	Window.Init(hInstance, "Testing", "MyWindowClass", 800, 600);

	while (Window.ProcessMessages() == true) {
		Sleep(50);
	}

	return 0;
}