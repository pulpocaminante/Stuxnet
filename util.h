#pragma once
#include <phnt_windows.h>
#include <phnt.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <vector>
#include "config.h"

namespace util
{
	LPVOID GetMainModuleBaseSecure();
	LPVOID GetModuleFunc(LPCWSTR sModuleName, LPCSTR sFuncName);
	PPEB GetPPEB();

	
	// UNUSED:
	//LPVOID GetMainModuleBaseFast_x86();
	//LPVOID GetTEB();
	
	// FLAGGED AS MALICIOUS (some or all):
	// LPVOID byteInjection(const char* bytes, LPVOID address);
	// LPVOID testOpcodeInjection(LPVOID address);
	// LPVOID dumpIAT();
}