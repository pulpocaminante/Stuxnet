#pragma once
#include "ComWrapper.hpp"
#include "Win32Wrapper.hpp"
#include "KernelbaseWrapper.hpp"

static inline ULONG BTE(BOOL f)
{
    return f ? 0 : GetLastError();
}

class UACInterface
{
public:
    UACInterface(COMWrapper& com_wrapper, Win32Wrapper& win32_wrapper, KernelbaseWrapper& kernelbase_wrapper) :
        COM(com_wrapper),
        IWin32(win32_wrapper),
		IKB(kernelbase_wrapper)
    {
        
    }

    const ULONG CheckElevation()
    {
        // Get the process token
        HANDLE hToken;
        ULONG err = BTE(IKB.OpenProcessToken(IWin32.GetCurrentProcess(), TOKEN_QUERY, &hToken));
        if (!err)
        {
            // Get the token elevation type

            ULONG cb = 0, rcb = 0x20;
            union {
                PTOKEN_USER ptu;
                PVOID buf;
            };
            static volatile UCHAR guz;
            PVOID stack = malloc(guz);
            PWSTR SzSid = 0;

            //++ for display user sid only
            //do
            //{
            //    if (cb < rcb)
            //    {
            //        cb = RtlPointerToOffset(buf = malloc(rcb - cb), stack);
            //    }

            //    if (!(err = BTE(IKB.GetTokenInformation(hToken, ::TokenUser, buf, cb, &rcb))))
            //    {
            //        //ConvertSidToStringSidW(ptu->User.Sid, &SzSid);
            //        ILog("Got: %ls\n", SzSid);
            //        break;
            //    }

            //} while (err == ERROR_INSUFFICIENT_BUFFER);
            // -- for display user sid only 

            union {
                TOKEN_ELEVATION te;
                TOKEN_ELEVATION_TYPE tet;
            };

            if (!(err = BTE(IKB.GetTokenInformation(hToken, ::TokenElevationType, &tet, sizeof(tet), &rcb))))
            {
                // Display the elevation type
                switch (tet)
                {
                    case TokenElevationTypeDefault:
                        if (!(err = BTE(IKB.GetTokenInformation(hToken, ::TokenElevation, &te, sizeof(te), &rcb))))
                        {
                            if (te.TokenIsElevated)
                            {
                                // We are the built-in admin or UAC is disabled in the system
                                ILog("Elevation type 1: Admin/No UAC\n");
                                return 0x1;
                            }
                            else
                            {
                                // We are a standard non-admin user
                                ILog("Elevation type 4: Not in admin group\n");
                                return 0x4;
                            }
                        }
                    case TokenElevationTypeFull:
                        // The process is elevated
                        ILog("Elevation type 2: Admin/UAC\n");
                        return 0x2;

                    case TokenElevationTypeLimited:
                        // The user is an admin but the process isn't elevated
                        ILog("Elevation type 3: No Admin\n");
                        return 0x3;

                    default:
                        err = ERROR_GEN_FAILURE;
                        return 0x0;
                }
            }

            if (SzSid) LocalFree(SzSid);

            CloseHandle(hToken);
        }

        return err;
    }
    
private:
    COMWrapper& COM;
	Win32Wrapper& IWin32;
	KernelbaseWrapper& IKB;
};

