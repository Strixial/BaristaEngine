#pragma once
#include "StringConverter.h"
#include <Windows.h>

class ErrorLogger {
public:
	static void CreateErrorBox(std::string message);
	static void CreateErrorBox(HRESULT hResult, std::string message);
	static void Log(std::string message);
};