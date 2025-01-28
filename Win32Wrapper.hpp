#pragma once
#include "ExportInterface.hpp"

static const volatile __declspec(noinline) HANDLE _GetCurrentProcess(void)
{
    return (HANDLE)0xFFFFFFFFFFFFFFFF;
}

// Mirrors the functionality of the same Win32 API functions
// without the need for an API import
const class Win32Wrapper
{
public:
    static const __forceinline HANDLE GetCurrentProcess()
    {
        return _GetCurrentProcess();
    }

    static const __declspec(noinline) uintptr_t GetNtVersion(void) {
        // KUSER_SHARED_DATA
        // 0x7FFE026C corresponds to the NtMajorVersion field
        static uintptr_t version = *(uintptr_t*)0x7FFE026C;
    }

    static const __declspec(noinline) bool _Is64BitOS(void) {
        // KUSER_SHARED_DATA
        static uintptr_t version = *(uintptr_t*)0x7FFE026C;

        // Access SysCall field with the correct offset for the OS version
        static uintptr_t address = (version == 10 || version == 11) ? 0x7FFE0308 : 0x7FFE0300;

        // On AMD64, this value is initialized to a nonzero value if the system 
        // operates with an altered view of the system service call mechanism.
        ILog("Running %u-bit system\n", *(uintptr_t**)address ? 32 : 64);

        return (*(uintptr_t**)address ? false : true);
    };
};