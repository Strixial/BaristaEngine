#include "ErrorLogger.h"
#include <comdef.h>

void ErrorLogger::Log(std::string message) {
	auto errorMessage = "Error: " + message;
	MessageBoxA(NULL, errorMessage.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hResult, std::string message) {
	_com_error error(hResult);

	auto errorMessage = L"Error: " + StringConverter::StringToWide(message) + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
}