#pragma once
#include <phnt_windows.h>
#include <phnt.h>
#include <stdio.h>
#include <time.h>
#include <SetupAPI.h>
#include <strsafe.h>
#include <iostream>
#include <vector>
#include <sddl.h>
#include <shellapi.h>
#include "config.h"
#include "util.h"
#include <comdef.h>
#include "wmi_defs.h"

// class for locating exported functions in a DLL and 
// retrieving their addresses
class IExport
{
public:
    template<typename... Args, typename = std::enable_if_t<(std::is_same_v<LPCSTR, Args> && ...)>>
    
    // Unused, needs to be rewritten
    static const std::vector<LPVOID> LoadAndFindExports(LPCSTR lpDll, Args ... args)
    {
        // Declarations
        std::vector vArgs = { args ... };        // unpack variadic list
        std::vector<LPVOID>              fnMap;  // our function map for return
        HMODULE                     hModule;// Module handle
        PIMAGE_EXPORT_DIRECTORY     pIED;   // Image export directory
        IMAGE_EXPORT_DIRECTORY      IED;
        PBYTE                       pBase;  // base address of module
        char* cDll;
        wchar_t* lpwDll;

        // Initializations
        pIED = &IED;
        cDll = new CHAR[strlen(lpDll) + sizeof(CHAR)];
        lpwDll = new WCHAR[strlen(lpDll) * sizeof(wchar_t) + sizeof(wchar_t)];

        // Decipher DLL name
        railfence_decipher(5, lpDll, cDll);
		mbstowcs(lpwDll, cDll, strlen(cDll) + sizeof(wchar_t));

        // Load DLL
        hModule = (HMODULE)LoadLibrary(lpwDll);
        //ILog("Loading %ls\n", lpwDll);

        // Initialize pBase
        pBase = (PBYTE)hModule;

        // Make sure library was loaded
        if (!hModule)
        {
            // LoadLibrary failed
            ILog("Failed to load module: %d\n", GetLastError());
            delete[] lpwDll;
            delete[] cDll;
            return fnMap;
        }

        // Get PIMAGE_EXPORT_DIRECTORY
        pIED = GetExportDirectoryTable(hModule);
        if (pIED == NULL)
        {
            delete[] lpwDll;
            delete[] cDll;
            return fnMap;
        }

        //ILog("%ls: %u named exports\n", lpwDll, pIED->NumberOfNames);

        // Get pointers to the name and function arrays
        PDWORD pNames = (PDWORD)(pBase + pIED->AddressOfNames);
        PDWORD pFunctions = (PDWORD)(pBase + pIED->AddressOfFunctions);
        PWORD pOrdinals = (PWORD)(pBase + pIED->AddressOfNameOrdinals);
        for (DWORD i = 0; i < pIED->NumberOfNames; i++) {

            // Get the name and function address for this entry
            PSTR pName = (PSTR)(pBase + pNames[i]);

            // Look up the ordinal value for the function in the address lookup table
            WORD ordinal = pIED->Base + pOrdinals[i];

            //ILog("%s: 0x%d\n", pName, ordinal);
            for (auto& fn : vArgs)
            {
                // Declarations & initializations
                char* lpszFn = new char[strlen(pName) + 1];
                railfence_encipher(5, pName, lpszFn);

                // Compare strings to enciphered module name
                if (strncmp(fn, lpszFn, strlen(lpszFn)) == 0)
                {
                    LPVOID fnAddress = util::GetModuleFunc(lpwDll, (LPCSTR)ordinal);
                    //ILog("Found %s @ 0x%llx\n", pName, fnAddress);

                    fnMap.push_back(fnAddress);
                }

                // Call destructor
                delete[] lpszFn;
            }
        }

        // Destructors
        delete[] cDll;
        delete[] lpwDll;

        return fnMap;
    }

    static const LPVOID LoadAndFindSingleExport(LPCSTR lpDll, LPCSTR function)
    {
        // Declarations
        HMODULE                     hModule;// Module handle
        PIMAGE_EXPORT_DIRECTORY     pIED;   // Image export directory
        IMAGE_EXPORT_DIRECTORY      IED;
        PBYTE                       pBase;  // base address of module
        LPVOID                      fnAddress;
        char* cDll;
        wchar_t* lpwDll;

        // Initializations
        pIED = &IED;
        cDll = new CHAR[strlen(lpDll) + sizeof(CHAR)];
        lpwDll = new WCHAR[( (strlen(lpDll) * sizeof(wchar_t)) ) + sizeof(wchar_t)];

        // Decipher DLL name
        railfence_decipher(5, lpDll, cDll);
        if (MultiByteToWideChar(CP_ACP, 0, cDll, -1, lpwDll, (strlen(cDll) + 1) * sizeof(wchar_t)) == 0)
        {
            ILog("Failed to convert verb to wide char\n");
            delete[] cDll;
            delete[] lpwDll;
            return nullptr;
        }

        // Load DLL
        hModule = (HMODULE)LoadLibrary(lpwDll);
        //ILog("Loading %ls\n", lpwDll);

        // Initialize pBase
        pBase = (PBYTE)hModule;

        // Make sure library was loaded
        if (!hModule)
        {
            // LoadLibrary failed
            ILog("Failed to load module: %d\n", GetLastError());
            delete[] cDll;
            delete[] lpwDll;
            return nullptr;
        }

        // Get PIMAGE_EXPORT_DIRECTORY
        pIED = GetExportDirectoryTable(hModule);
        if (pIED == NULL)
        {
            delete[] cDll;
            delete[] lpwDll;
            return nullptr;
        }

        //ILog("%ls: %u named exports\n", lpwDll, pIED->NumberOfNames);

        // Get pointers to the name and function arrays
        PDWORD pNames = (PDWORD)(pBase + pIED->AddressOfNames);
        PDWORD pFunctions = (PDWORD)(pBase + pIED->AddressOfFunctions);
        PWORD pOrdinals = (PWORD)(pBase + pIED->AddressOfNameOrdinals);
        for (DWORD i = 0; i < pIED->NumberOfNames; i++) {

            // Get the name and function address for this entry
            PSTR pName = (PSTR)(pBase + pNames[i]);

            // Look up the ordinal value for the function in the address lookup table
            DWORD ordinal = pIED->Base + pOrdinals[i];

            // There was a ninja segfault here because of a zero length
            // or corrupted export name
            if ((strlen(pName) != 0) && (strlen(pName) == strlen(function)))
            {
                // Declarations & initializations
                char* lpszFn = new char[strlen(pName) + 2];
                railfence_encipher(5, pName, lpszFn);

                // Compare strings to enciphered module name
                if (strncmp(function, lpszFn, strlen(lpszFn)) == 0)
                {
                    if (ordinal != NULL)
                    {
                        if ((fnAddress = util::GetModuleFunc(lpwDll, (LPCSTR)ordinal)) != NULL)
                        {
                            //ILog("Found %s @ 0x%llx\n", pName, fnAddress);
                            delete[] lpszFn;
                            delete[] cDll;
                            delete[] lpwDll;
                            return fnAddress;
                        }
                    }
                    else
                    {
						if ((fnAddress = util::GetModuleFunc(lpwDll, pName)) != NULL)
						{
							//ILog("Found %s @ 0x%llx\n", pName, fnAddress);
							delete[] lpszFn;
							delete[] cDll;
							delete[] lpwDll;
							return fnAddress;
						}
                    }
                }

                // Destructor
                delete[] lpszFn;
            }
        }

        // Destructors
        delete[] cDll;
        delete[] lpwDll;

		ILog("Failed to find export %s in %s\n", function, lpDll);
        
        return nullptr;
    }

    // Railfence cipher encode
    static void railfence_encipher(int key, const char* plaintext, char* ciphertext)
    {
        int line, i, skip, length = strlen(plaintext), j = 0, k = 0;
        for (line = 0; line < key - 1; line++) {
            skip = 2 * (key - line - 1);
            k = 0;
            for (i = line; i < length;) {
                ciphertext[j] = plaintext[i];
                if ((line == 0) || (k % 2 == 0)) i += skip;
                else i += 2 * (key - 1) - skip;
                j++;   k++;
            }
        }
        for (i = line; i < length; i += 2 * (key - 1)) ciphertext[j++] = plaintext[i];
        ciphertext[j] = '\0'; // Null terminate
    }

    // Railfence cipher decode
    static void railfence_decipher(int key, const char* ciphertext, char* plaintext)
    {
        int i, length = strlen(ciphertext), skip, line, j, k = 0;
        for (line = 0; line < key - 1; line++) {
            skip = 2 * (key - line - 1);
            j = 0;
            for (i = line; i < length;) {
                plaintext[i] = ciphertext[k++];
                if ((line == 0) || (j % 2 == 0)) i += skip;
                else i += 2 * (key - 1) - skip;
                j++;
            }
        }
        for (i = line; i < length; i += 2 * (key - 1)) plaintext[i] = ciphertext[k++];
        plaintext[length] = '\0'; /* Null terminate */
    }

    // Railfence cipher wchar_t* decode
    static void railfence_wdecipher(int key, const wchar_t* ciphertext, wchar_t* plaintext)
    {
        int i, length = wcslen(ciphertext), skip, line, j, k = 0;
        for (line = 0; line < key - 1; line++) {
            skip = 2 * (key - line - 1);
            j = 0;
            for (i = line; i < length;) {
                plaintext[i] = ciphertext[k++];
                if ((line == 0) || (j % 2 == 0)) i += skip;
                else i += 2 * (key - 1) - skip;
                j++;
            }
        }
        for (i = line; i < length; i += 2 * (key - 1)) plaintext[i] = ciphertext[k++];
        plaintext[length] = '\00'; /* Null terminate */
    }

    // Get the export directory table from the module
    static const PIMAGE_EXPORT_DIRECTORY GetExportDirectoryTable(HMODULE hModule)
    {
        PBYTE                   pBase;  // base address of module
        PIMAGE_FILE_HEADER      pIFH;   // COFF file header
        PIMAGE_EXPORT_DIRECTORY pIED;   // export directory table (EDT)
        DWORD                   RVA;    // relative virtual address of EDT
        PIMAGE_DOS_HEADER       pIDH;   // MS-DOS stub
        PIMAGE_OPTIONAL_HEADER  pIOH;   // so-called "optional" header
        PDWORD                  peSig;  // PE signature

        //ILog("Module base: 0x%llx\n", (HMODULE)hModule);

        pBase = (PBYTE)hModule;
        pIDH = (PIMAGE_DOS_HEADER)hModule;
        peSig = (PDWORD)(pBase + pIDH->e_lfanew);

        if (IMAGE_NT_SIGNATURE != *peSig)
        {
            // PE signature is invalid
            ILog("Incorrect PE signature, got: %d\n", *peSig);
            return NULL;
        }

        //ILog("PE Signature: %d\n", *peSig);

        pIFH = (PIMAGE_FILE_HEADER)(peSig + 1);
        pIOH = (PIMAGE_OPTIONAL_HEADER)(pIFH + 1);

        if (IMAGE_DIRECTORY_ENTRY_EXPORT >= pIOH->NumberOfRvaAndSizes) {
            // This image doesn't have an export directory table.
            return NULL;
        }

        RVA = pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
        pIED = (PIMAGE_EXPORT_DIRECTORY)(pBase + RVA);

        return pIED;
    }

};