#pragma once
#include <phnt_windows.h>
#include "ExportInterface.hpp"

// Interface for setupapi.dll functions
class SetupAPIWrapper {
public:

    BOOL IReady = FALSE;

    SetupAPIWrapper() :
        pSetupDiGetClassDevsExW(IExport::LoadAndFindSingleExport("s.eidtplualp", "SeeeGtDvtiCssuDlsEWpax")),
        pSetupDiEnumDeviceInfo(IExport::LoadAndFindSingleExport("s.eidtplualp", "SneeEucItiminuDDvfpeo")),
        pSetupDiGetDeviceRegistryPropertyW(IExport::LoadAndFindSingleExport("s.eidtplualp", "SeRPWeGteeyrytiDcgrotuDeiitprpvse")),
        pSetupDiDestroyDeviceInfoList(IExport::LoadAndFindSingleExport("s.eidtplualp", "SevLeDseioititDcfsuDryentpoI")),
        pSetupDiGetDeviceInstanceIdW(IExport::LoadAndFindSingleExport("s.eidtplualp", "SeIIeGtenedtiDcscWuDeitnpva"))
    {
        if (pSetupDiGetClassDevsExW == nullptr || pSetupDiEnumDeviceInfo == nullptr || pSetupDiGetDeviceRegistryPropertyW == nullptr ||
            pSetupDiDestroyDeviceInfoList == nullptr || pSetupDiGetDeviceInstanceIdW == nullptr)
        {
            ILog("Failed to find all required functions\n");
        }
        else
        {
            IReady = TRUE;
        }
    }

    const HDEVINFO WINAPI SetupDiGetClassDevsExW(
        _In_opt_ CONST GUID* ClassGuid,
        _In_opt_ PCWSTR Enumerator,
        _In_opt_ HWND hwndParent,
        _In_ DWORD Flags,
        _In_opt_ HDEVINFO DeviceInfoSet,
        _In_opt_ PCWSTR MachineName,
        _Reserved_ PVOID Reserved
    )
    {
        return _SafeSetupDiGetClassDevsExW(ClassGuid, Enumerator, hwndParent, Flags, DeviceInfoSet, MachineName, Reserved);
    }

    const BOOL WINAPI SetupDiEnumDeviceInfo(
        _In_ HDEVINFO DeviceInfoSet,
        _In_ DWORD MemberIndex,
        _Out_ PSP_DEVINFO_DATA DeviceInfoData
    )
    {

        return _SafeSetupDiEnumDeviceInfo(DeviceInfoSet, MemberIndex, DeviceInfoData);
    }

    const BOOL WINAPI SetupDiGetDeviceRegistryPropertyW(
        _In_ HDEVINFO DeviceInfoSet,
        _In_ PSP_DEVINFO_DATA DeviceInfoData,
        _In_ DWORD Property,
        _Out_opt_ PDWORD PropertyRegDataType,
        _Out_writes_bytes_to_opt_(PropertyBufferSize, *RequiredSize) PBYTE PropertyBuffer,
        _In_ DWORD PropertyBufferSize,
        _Out_opt_ PDWORD RequiredSize)
    {
        return _SafeSetupDiGetDeviceRegistryPropertyW(DeviceInfoSet, DeviceInfoData, Property, PropertyRegDataType, PropertyBuffer, PropertyBufferSize, RequiredSize);
    }

    const BOOL WINAPI SetupDiDestroyDeviceInfoList(
        _In_ HDEVINFO DeviceInfoSet
    )
    {
        return _SafeSetupDiDestroyDeviceInfoList(DeviceInfoSet);
    }

    const BOOL WINAPI SafeSetupDiGetDeviceInstanceIdW(
        _In_ HDEVINFO DeviceInfoSet,
        _In_ PSP_DEVINFO_DATA DeviceInfoData,
        _Out_writes_opt_(DeviceInstanceIdSize) PWSTR DeviceInstanceId,
        _In_ DWORD DeviceInstanceIdSize,
        _Out_opt_ PDWORD RequiredSize
    )
    {
        return _SafeSetupDiGetDeviceInstanceIdW(DeviceInfoSet, DeviceInfoData, DeviceInstanceId, DeviceInstanceIdSize, RequiredSize);
    }

private:

    LPVOID pSetupDiGetClassDevsExW = nullptr;
    LPVOID pSetupDiEnumDeviceInfo = nullptr;
    LPVOID pSetupDiGetDeviceRegistryPropertyW = nullptr;
    LPVOID pSetupDiDestroyDeviceInfoList = nullptr;
    LPVOID pSetupDiGetDeviceInstanceIdW = nullptr;

    LPVOID slpSetupDiGetClassDevsEx = (LPVOID)((uintptr_t)pSetupDiGetClassDevsExW + 0x0);
    LPVOID slpSetupDiEnumDeviceInfo = (LPVOID)((uintptr_t)pSetupDiEnumDeviceInfo + 0x0);
    LPVOID slpSetupDiGetDeviceRegistryPropertyW = (LPVOID)((uintptr_t)pSetupDiGetDeviceRegistryPropertyW + 0x0);
    LPVOID slpSetupDiDestroyDeviceInfoList = (LPVOID)((uintptr_t)pSetupDiDestroyDeviceInfoList + 0x0);
    LPVOID slpSetupDiGetDeviceInstanceIdW = (LPVOID)((uintptr_t)pSetupDiGetDeviceInstanceIdW + 0x0);

    // Redefine the functions
    HDEVINFO(WINAPI* _SafeSetupDiGetClassDevsExW)(
        _In_opt_ CONST GUID* ClassGuid,
        _In_opt_ PCWSTR Enumerator,
        _In_opt_ HWND hwndParent,
        _In_ DWORD Flags,
        _In_opt_ HDEVINFO DeviceInfoSet,
        _In_opt_ PCWSTR MachineName,
        _Reserved_ PVOID Reserved
        ) = (HDEVINFO(WINAPI*)(
            _In_opt_ CONST GUID * ClassGuid,
            _In_opt_ PCWSTR Enumerator,
            _In_opt_ HWND hwndParent,
            _In_ DWORD Flags,
            _In_opt_ HDEVINFO DeviceInfoSet,
            _In_opt_ PCWSTR MachineName,
            _Reserved_ PVOID Reserved
            ))slpSetupDiGetClassDevsEx;

    BOOL(WINAPI* _SafeSetupDiEnumDeviceInfo)(_In_ HDEVINFO DeviceInfoSet, _In_ DWORD MemberIndex, _Out_ PSP_DEVINFO_DATA DeviceInfoData) =
        (BOOL(WINAPI*)(_In_ HDEVINFO DeviceInfoSet, _In_ DWORD MemberIndex, _Out_ PSP_DEVINFO_DATA DeviceInfoData))pSetupDiEnumDeviceInfo;

    BOOL(WINAPI* _SafeSetupDiGetDeviceRegistryPropertyW)(
        _In_ HDEVINFO DeviceInfoSet,
        _In_ PSP_DEVINFO_DATA DeviceInfoData,
        _In_ DWORD Property,
        _Out_opt_ PDWORD PropertyRegDataType,
        _Out_writes_bytes_to_opt_(PropertyBufferSize, *RequiredSize) PBYTE PropertyBuffer,
        _In_ DWORD PropertyBufferSize,
        _Out_opt_ PDWORD RequiredSize
        ) = (BOOL(WINAPI*)(_In_ HDEVINFO DeviceInfoSet,
            _In_ PSP_DEVINFO_DATA DeviceInfoData,
            _In_ DWORD Property,
            _Out_opt_ PDWORD PropertyRegDataType,
            _Out_writes_bytes_to_opt_(PropertyBufferSize, *RequiredSize) PBYTE PropertyBuffer,
            _In_ DWORD PropertyBufferSize,
            _Out_opt_ PDWORD RequiredSize))pSetupDiGetDeviceRegistryPropertyW;

    BOOL(WINAPI* _SafeSetupDiDestroyDeviceInfoList)(
        _In_ HDEVINFO DeviceInfoSet) = (BOOL(WINAPI*)(
            _In_ HDEVINFO DeviceInfoSet))pSetupDiDestroyDeviceInfoList;

    BOOL(WINAPI* _SafeSetupDiGetDeviceInstanceIdW)(
        _In_ HDEVINFO DeviceInfoSet,
        _In_ PSP_DEVINFO_DATA DeviceInfoData,
        _Out_writes_opt_(DeviceInstanceIdSize) PWSTR DeviceInstanceId,
        _In_ DWORD DeviceInstanceIdSize,
        _Out_opt_ PDWORD RequiredSize
        ) =
        (BOOL(WINAPI*)(_In_ HDEVINFO DeviceInfoSet,
            _In_ PSP_DEVINFO_DATA DeviceInfoData,
            _Out_writes_opt_(DeviceInstanceIdSize) PWSTR DeviceInstanceId,
            _In_ DWORD DeviceInstanceIdSize,
            _Out_opt_ PDWORD RequiredSize
            ))pSetupDiGetDeviceInstanceIdW;
};