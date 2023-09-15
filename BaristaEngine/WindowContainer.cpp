#include "WindowContainer.h"

LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		// KEYBOARD MESSAGES //

	case WM_KEYDOWN: {
		unsigned char keyCode = static_cast<unsigned char>(wParam);
		if (keyboard.IsKeysAutoRepeat()) {
			keyboard.OnKeyPressed(keyCode);
		}
		else {
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed) {
				keyboard.OnChar(keyCode);
			}
		}
		return 0; 
	}
	case WM_KEYUP: {
		unsigned char keyCode = static_cast<unsigned char>(wParam);
		keyboard.OnKeyReleased(keyCode);
		return 0; 
	}
	case WM_CHAR: {
		unsigned char Char = static_cast<unsigned char>(wParam);
		if (keyboard.IsCharsAutoRepeat()) {
			keyboard.OnChar(Char);
		}
		else {
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed) {
				keyboard.OnChar(Char);
			}
		}
		return 0;
	}
	
	// MOUSE MESSAGES //
	case WM_MOUSEMOVE: {
		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);
		mouse.OnMouseMove(xPos, yPos);

		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnLeftPressed(x, y);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnRightPressed(x, y);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnMiddlePressed(x, y);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnLeftReleased(x, y);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnRightReleased(x, y);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnMiddleReleased(x, y);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			mouse.OnWheelUp(x, y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			mouse.OnWheelDown(x, y);
		}
		return 0;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}