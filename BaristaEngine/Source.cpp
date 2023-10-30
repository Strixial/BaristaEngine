#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	OutputDebugStringA("!! - Initialising Barista\n");
	
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		ErrorLogger::CreateErrorBox("Failed to call CoInitialize");
		return -1;
	}

	Engine Engine;
	Engine.Init(hInstance, "Barista Engine - DirectX 11", "BaristaDX11", 1280, 720);

	while (Engine.ProcessMessages() == true) {
		Engine.Update();
		Engine.RenderFrame();
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return 0;
}