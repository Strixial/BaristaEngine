#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	OutputDebugStringA("!! - Initialising Barista\n");
	
	Engine Engine;
	Engine.Init(hInstance, "Barista Engine - DirectX 11", "BaristaDX11", 1280, 720);

	while (Engine.ProcessMessages() == true) {
		Engine.Update();
		Engine.RenderFrame();
	}

	return 0;
}