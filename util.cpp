#pragma once
#include "util.h"

#define MAX_ORDINAL 0xffff

using namespace std;

/*
*
*  Header file for secure module mapping functions
*  All functions use zero hookable function calls
*
*  Header file can be compiled for x86, x64 & ARM
*
*/


LPVOID util::GetMainModuleBaseSecure()
{
	// GET POINTER TO MAIN (.EXE) MODULE BASE
	// Slightly slower (splitting milliseconds), works on x86, ARM, x84
	// Get pointer to the TEB
#if defined(_M_X64) // x64
	auto pTeb = reinterpret_cast<PTEB>(__readgsqword(reinterpret_cast<DWORD>(&static_cast<PNT_TIB>(nullptr)->Self)));
#elif defined(_M_ARM) // ARM
	auto pTeb = reinterpret_cast<PTEB>(_MoveFromCoprocessor(15, 0, 13, 0, 2)); // CP15_TPIDRURW
#else // x86
	auto pTeb = reinterpret_cast<PTEB>(__readfsdword(reinterpret_cast<DWORD>(&static_cast<PNT_TIB>(nullptr)->Self)));
#endif

	// Get pointer to the PEB
	auto pPeb = pTeb->ProcessEnvironmentBlock;
	const auto base = pPeb->ImageBaseAddress;
	return base;
}

PPEB util::GetPPEB()
{
	// GET POINTER TO MAIN (.EXE) MODULE BASE
	// Slightly slower (splitting milliseconds), works on x86, ARM, x84
	// Get pointer to the TEB
#if defined(_M_X64) // x64
	auto pTeb = reinterpret_cast<PTEB>(__readgsqword(reinterpret_cast<DWORD>(&static_cast<PNT_TIB>(nullptr)->Self)));
#elif defined(_M_ARM) // ARM
	auto pTeb = reinterpret_cast<PTEB>(_MoveFromCoprocessor(15, 0, 13, 0, 2)); // CP15_TPIDRURW
#else // x86
	auto pTeb = reinterpret_cast<PTEB>(__readfsdword(reinterpret_cast<DWORD>(&static_cast<PNT_TIB>(nullptr)->Self)));
#endif

	// Get pointer to the PEB
	auto pPeb = pTeb->ProcessEnvironmentBlock;
	return pPeb;
}


LPVOID util::GetModuleFunc(LPCWSTR sModuleName, LPCSTR sFuncName)
{
	// Get address of exported function (procedure) within a specific module in memory
	// Returns nullptr if failed
	// 
	// Structure definitions are taken from winnt.h and winternl.h Microsoft SDK 7.1 files and slightly modified when necessary for simplicity.
	// Main reason is to create compatibility for the code compiled with different SDKs, that might have changed definitions.
	typedef struct _NT_TIB {
		PVOID ExceptionList;
		PVOID StackBase;
		PVOID StackLimit;
		PVOID SubSystemTib;
		PVOID FiberData;
		PVOID ArbitraryUserPointer;
		PNT_TIB Self;
	} NT_TIB, * PNT_TIB;

	typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR  Buffer;
	} UNICODE_STRING;

	typedef struct _PEB_LDR_DATA {
		BYTE Reserved1[8];
		PVOID Reserved2[3];
		LIST_ENTRY InMemoryOrderModuleList;
	} PEB_LDR_DATA, * PPEB_LDR_DATA;

	typedef struct _RTL_USER_PROCESS_PARAMETERS {
		BYTE Reserved1[16];
		PVOID Reserved2[10];
		UNICODE_STRING ImagePathName;
		UNICODE_STRING CommandLine;
	} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

	typedef struct _PEB {
		BYTE Reserved1[2];
		BYTE BeingDebugged;
		BYTE Reserved2[1];
		PVOID Reserved3[2];
		PPEB_LDR_DATA Ldr;
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
		PVOID Reserved4[3];
		PVOID AtlThunkSListPtr;
		PVOID Reserved5;
		ULONG Reserved6;
		PVOID Reserved7;
		ULONG Reserved8;
		ULONG AtlThunkSListPtr32;
		PVOID Reserved9[45];
		BYTE Reserved10[96];
		PVOID PostProcessInitRoutine;
		BYTE Reserved11[128];
		PVOID Reserved12[1];
		ULONG SessionId;
	} PEB, * PPEB;

	typedef struct _TEB {
		PVOID Reserved1[12];
		PPEB ProcessEnvironmentBlock;
		PVOID Reserved2[399];
		BYTE Reserved3[1952];
		PVOID TlsSlots[64];
		BYTE Reserved4[8];
		PVOID Reserved5[26];
		PVOID ReservedForOle;  // Windows 2000 only
		PVOID Reserved6[4];
		PVOID TlsExpansionSlots;
	} TEB, * PTEB;

	// Modified LDR_DATA_TABLE_ENTRY definition (this one includes BaseDllName field and has InMemoryOrderLinks at the top for easier processing)
	typedef struct _LDR_DATA_TABLE_ENTRY {
		/*LIST_ENTRY InLoadOrderLinks;*/
		LIST_ENTRY InMemoryOrderLinks;
		LIST_ENTRY InInitializationOrderList;
		PVOID DllBase;
		PVOID EntryPoint;
		PVOID Reserved3;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
	} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;


	// Get pointer to the TEB
#if defined(_M_X64) // x64
	auto pTeb = reinterpret_cast<PTEB>(__readgsqword(reinterpret_cast<DWORD>(&static_cast<PNT_TIB>(nullptr)->Self)));
#elif defined(_M_ARM) // ARM
	auto pTeb = reinterpret_cast<PTEB>(_MoveFromCoprocessor(15, 0, 13, 0, 2)); // CP15_TPIDRURW
#else // x86
	auto pTeb = reinterpret_cast<PTEB>(__readfsdword(reinterpret_cast<DWORD>(&static_cast<PNT_TIB>(nullptr)->Self)));
#endif

	// Get pointer to the PEB
	auto pPeb = pTeb->ProcessEnvironmentBlock;

	// Now get pointer to the PEB loader list data
	auto pLdrData = pPeb->Ldr;
	// And then pointer to the in-memory-order loader list
	auto pModListHdr = &pLdrData->InMemoryOrderModuleList;

	// Calculate the size of the input string
	int iLenModule = 0;
	for (; sModuleName[iLenModule]; ++iLenModule);

	// Loop over all modules in list
	for (auto pModListCurrent = pModListHdr->Flink; pModListCurrent != pModListHdr; pModListCurrent = pModListCurrent->Flink)
	{
		// Get current module in list
		auto pModEntry = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(pModListCurrent);

		for (int i = 0; i < pModEntry->BaseDllName.Length / 2 /* length is in bytes */; ++i)
		{
			if (sModuleName[i] == '\0') // the end of the string
				break;
			else if ((sModuleName[i] & ~' ') != (pModEntry->BaseDllName.Buffer[i] & ~' ')) // all upper case for case-insensitive comparisson
				break;
			else if (i == iLenModule - 1) // gone through all characters and they all matched
			{
				int iLenFuncName = 0;
				DWORD iOrdinal = reinterpret_cast<DWORD>(sFuncName);
				if (iOrdinal > MAX_ORDINAL) // check to see if passed data is really an ordinal value. Otherwise it's function name
				{
					iOrdinal = 0; // it's not ordinal then
					// Calculate the size of the wanted function's name
					for (; sFuncName[iLenFuncName]; ++iLenFuncName);
				}

				// Parse the PE in order to find exports section
				auto pImageDOSHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(pModEntry->DllBase);
				auto pImageNtHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<ULONG_PTR>(pImageDOSHeader) + pImageDOSHeader->e_lfanew);
				auto pExport = reinterpret_cast<PIMAGE_DATA_DIRECTORY>(&pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);

				// Maybe there aren't any exports?
				if (pExport->VirtualAddress == 0 || pExport->Size == 0)
					return nullptr;

				// Finally
				auto pExports = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(reinterpret_cast<ULONG_PTR>(pImageDOSHeader) + pExport->VirtualAddress);

				// Check to see if there are exports, or someone is being a smart ass
				if (pExports->NumberOfFunctions && pExports->AddressOfFunctions && (iOrdinal || (pExports->NumberOfNames && pExports->AddressOfNames && pExports->AddressOfNameOrdinals)))
				{
					// Okie dokie, now reinterpret offsets (RVAs).
					// Array of functions addresses
					auto pdwBufferAddress = reinterpret_cast<LPDWORD>(reinterpret_cast<ULONG_PTR>(pImageDOSHeader) + pExports->AddressOfFunctions);

					DWORD dwExportRVA = 0; // to save the exported function's RVA to

					if (iOrdinal) // function is wanted by its ordinal value
					{
						// Check to see if valid ordinal value is specified
						if (iOrdinal >= pExports->Base && iOrdinal < pExports->Base + pExports->NumberOfFunctions)
							dwExportRVA = pdwBufferAddress[iOrdinal - pExports->Base];
					}
					else // function is wanted by its name
					{
						// Array of functions names
						auto pdwBufferNames = reinterpret_cast<LPDWORD>(reinterpret_cast<ULONG_PTR>(pImageDOSHeader) + pExports->AddressOfNames);
						// Array of functions indexes into array of addresses
						auto pwBufferNamesOrdinals = reinterpret_cast<LPWORD>(reinterpret_cast<ULONG_PTR>(pImageDOSHeader) + pExports->AddressOfNameOrdinals);

						// Loop through all functions exported by name
						for (DWORD j = 0; j < pExports->NumberOfNames; ++j)
						{
							// Read the listed function name
							auto sFunc = reinterpret_cast<LPCSTR>(reinterpret_cast<ULONG_PTR>(pImageDOSHeader) + pdwBufferNames[j]);
							// Calculate the size of the listed function's name
							int iLenFunc = 0;
							for (; sFunc[iLenFunc]; ++iLenFunc);

							// Check only if the length of the names matches, otherwise the function is wrong for sure
							if (iLenFuncName == iLenFunc)
							{
								for (int k = 0; k < iLenFuncName; ++k)
								{
									if (sFuncName[k] != sFunc[k])
										break;
									else if (k == iLenFuncName - 1)
									{
										// Excellent! This is the function, read RVA
										dwExportRVA = pdwBufferAddress[pwBufferNamesOrdinals[j]];
										break;
									}
								}
							}
							if (dwExportRVA) break; // found, get out of the loop
						}
					}

					if (dwExportRVA) // if function is found
					{
						//Check if export is forwarded (the hint is that address points to a place inside the exports)
						if (dwExportRVA > pExport->VirtualAddress && dwExportRVA < pExport->VirtualAddress + pExport->Size)
						{
							// Read forwarded data. Null-terminated ASCII string in format of ModuleName.FunctionName or ModuleName.#OrdinalValue
							auto sForwarder = reinterpret_cast<LPCSTR>(reinterpret_cast<ULONG_PTR>(pImageDOSHeader) + dwExportRVA);
							// Allocate big enough buffer for the new module name
							WCHAR sForwarderDll[MAX_PATH];
							LPCSTR sForwarderFunc = nullptr;
							DWORD dwForwarderOrdinal = 0;
							// Reinterpret WCHAR buffer as CHAR one
							auto sForwarderDll_A = reinterpret_cast<CHAR*>(sForwarderDll);
							// Now go through all characters
							for (int iPos = 0; sForwarder[iPos]; ++iPos)
							{
								// Fill WCHAR buffer reading/copying from CHAR one (someone could call this way lame)
								sForwarderDll_A[2 * iPos] = sForwarder[iPos]; // copy character
								sForwarderDll_A[2 * iPos + 1] = '\0';

								if (sForwarder[iPos] == '.')
								{
									sForwarderDll[iPos] = '\0'; // null-terminate the ModuleName string
									++iPos; // skip . character
									if (sForwarder[iPos] == '#')
									{
										++iPos; // skip # character
										// OrdinalValue is hashtag, convert ASCII string to integer value
										for (; sForwarder[iPos]; ++iPos)
										{
											dwForwarderOrdinal *= 10;
											dwForwarderOrdinal += (sForwarder[iPos] - '0');
										}
										if (dwForwarderOrdinal > MAX_ORDINAL) // something is wrong
											return nullptr;
										// Reinterpret the ordinal value as string
										sForwarderFunc = reinterpret_cast<LPSTR>(dwForwarderOrdinal);
										break;
									}
									else
									{
										sForwarderFunc = &sForwarder[iPos]; // FunctionName follows the dot
										break;
									}
								}
							}
							// Call again with forwarded data
							return GetModuleFunc(sForwarderDll, sForwarderFunc);
						}
						else
							// That's pretty much it. Return address of the function
							return reinterpret_cast<LPVOID>(reinterpret_cast<ULONG_PTR>(pImageDOSHeader) + dwExportRVA);
					}
					// Wanted export in this module doesn't exist
					return nullptr;
				}
				// No exports
				return nullptr;
			}
		}
	}
	// Not such module found
	return nullptr;
}

