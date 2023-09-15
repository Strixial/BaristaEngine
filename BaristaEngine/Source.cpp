#include "RenderWindow.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	RenderWindow Window;
	RenderWindow Window2;
	Window.Init(hInstance, "Barista Engine - DirectX 11", "BaristaDX11", 800, 600);
	Window2.Init(hInstance, "Barista Engine (Debugger) - DirectX 11", "BaristaDebugDX11", 800, 600);

	while (Window.ProcessMessages() == true && Window2.ProcessMessages() == true) {
		Sleep(50);
	}

	return 0;
}