#include "Graphics.h"

bool Graphics::Init(HWND hwnd, int width, int height) {
	if (!InitDirectX(hwnd, width, height))
		return false;
	
	return true
}

bool Graphics::InitDirectX(HWND hwnd, int width, int height) {
	HRESULT hResult = D3D11CreateDeviceAndSwapChain();
	
	return true;
}