#pragma once
#include "WMIConnection.hpp"

class Win32Interface
{
public:

    BOOL IReady;
    BOOL bInitialized;

    Win32Interface(COMWrapper& com_wrapper, WMIConnection& wmi_connection) :
        COM(com_wrapper), WMI(wmi_connection),
        IReady(FALSE), bInitialized(FALSE)
    {
        if (WMI.ConnectToNamespace("RVOM2OITC\\", 0) && WMI.IReady && COM.IReady)
            this->IReady = TRUE;
    }

    UINT32 Enumerate(

    )
    {

    }

    UINT32 Create(
        _In_      LPCWSTR CommandLine,
        _In_opt_  LPCWSTR CurrentDirectory,
        _In_opt_  LPVOID ProcessStartupInformation,
        _Out_ UINT32* ProcessId,
        _Out_ UINT32* ReturnValue)
    {
        if (!IReady || !WMI.IReady) return 0;

        if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
            WMI.ConnectToNamespace("RVOM2OITC\\", 0);
        
        if (!WMI.bConnected)
        {
            ILog("Failed to connect to namespace\n");
            return E_FAIL; // Something is very wrong
        }
        
        // Object declarations
        IWbemClassObject* pClass = NULL; // Win32_Process
        IWbemClassObject* pInParamsDefinition = NULL; // Win32_Process.Create
        IWbemClassObject* pOutParamsDefinition = NULL; // Win32_Process.Create
        IWbemClassObject* pInParams = NULL; // Win32_Process.Create
        IWbemClassObject* pOutParams = NULL; // Win32_Process.Create
        IWbemClassObject* pClassInstance = NULL; // Win32_Process
        IWbemCallResult* pCallResult = NULL;
        IWbemClassObject* pStartupObject = NULL; // Win32_ProcessStartup
        IWbemClassObject* pStartupInstance = NULL; // Win32_ProcessStartup
        IWbemClassObject* pParamsInstance = NULL; // Win32_ProcessStartup

        // Method declarations
        BOOL bInitialized = FALSE;
        HRESULT hres;
        WCHAR szSystemDir[MAX_PATH];

        // If no working directory is passed, set it to system32
        if (CurrentDirectory == NULL)
        {
            // Get system directory
            GetSystemDirectory(szSystemDir, MAX_PATH);
            CurrentDirectory = szSystemDir;
        }

        LPCWSTR lpwMethodName = L"Create";
        LPCWSTR lpwClassName = L"Win32_Process";

        // Initialize the COM
        hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres))
        {
            ILog("Failed to initialize COM library. Error code = 0x%lx\n", hres);
            return hres;                  // Program has failed.
        }
        else bInitialized = TRUE;

        //Setup the method call
        BSTR MethodName = SysAllocString(lpwMethodName);
        BSTR ClassName = SysAllocString(lpwClassName);

        hres = WMI.pSvc->GetObject(ClassName, 0, NULL, &pClass, NULL);
        if (FAILED(hres))
        {
            ILog("Failed to get class object. Error code = 0x%lx\n", hres);
            return hres;
        }

        // Get the class object
        hres = pClass->GetMethod(lpwMethodName, 0,
            &pInParamsDefinition, NULL);

        hres = pInParamsDefinition->SpawnInstance(0, &pClassInstance);
        if (FAILED(hres))
        {
            ILog("Failed to get class. Error code = 0x%lx\n", hres);
            return hres;
        }


        hres = WMI.pSvc->GetObject(_bstr_t(L"Win32_ProcessStartup"), 0, NULL, &pStartupObject, NULL);
        if (FAILED(hres))
        {
            ILog("Failed to get Win32_ProcessStartup object. Error code = 0x%lx\n", hres);
            return hres;
        }

        hres = pStartupObject->SpawnInstance(0, &pStartupInstance);
        if (FAILED(hres))
        {
            ILog("Failed to spawn Win32_ProcessStartup instance. Error code = 0x%lx\n", hres);
            return hres;
        }
        //Create the values for the in parameters
        VARIANT varParams;
        VariantInit(&varParams);
        varParams.vt = VT_I2;
        varParams.intVal = SW_SHOW;

        hres = pStartupInstance->Put(_bstr_t(L"ShowWindow"), 0, &varParams, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set ShowWindow property. Error code = 0x%lx\n", hres);
            return hres;
        }

        hres = pClass->GetMethod(_bstr_t(lpwMethodName), 0, &pInParamsDefinition, NULL);
        if (FAILED(hres))
        {
            ILog("Failed to get method definition. Error code = 0x%lx\n", hres);
            return hres;
        }

        hres = pInParamsDefinition->SpawnInstance(0, &pParamsInstance);
        if (FAILED(hres))
        {
            ILog("Failed to spawn instance. Error code = 0x%lx\n", hres);
            return hres;
        }

        // Construct the variant for CurrentDirectory
        VARIANT varCurrentDirectory;
        VariantInit(&varCurrentDirectory);
        varCurrentDirectory.vt = VT_BSTR;
        varCurrentDirectory.bstrVal = SysAllocString(CurrentDirectory);

        hres = pParamsInstance->Put(_bstr_t(L"CurrentDirectory"), 0, &varCurrentDirectory, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set CurrentDirectory property. Error code = 0x%lx\n", hres);
            return hres;
        }

        // Construct VARIANT for command line
        VARIANT varCommand;
        VariantInit(&varCommand);
        varCommand.vt = VT_BSTR;
        varCommand.bstrVal = SysAllocString(CommandLine);
        hres = pParamsInstance->Put(_bstr_t(L"CommandLine"), 0, &varCommand, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set CommandLine property. Error code = 0x%lx\n", hres);
            return hres;
        }

        // Pass Win32_ProcessStartup object to the method
        // But it has to be a VARIANT
        // 
        VARIANT varStartup;
        VariantInit(&varStartup);
        varStartup.vt = VT_UNKNOWN;
        varStartup.punkVal = pStartupInstance;
        hres = pParamsInstance->Put(_bstr_t(L"ProcessStartupInformation"), 0, &varStartup, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set ProcessStartupInformation property. Error code = 0x%lx\n", hres);
            return hres;
        }


        WMI.InvokeMethod(ClassName, MethodName, 0, NULL, pParamsInstance, &pOutParams, &pCallResult);

        // Get the return value
        VARIANT vtProp;
        VariantInit(&vtProp);
        hres = pOutParams->Get(_bstr_t(L"ReturnValue"), 0, &vtProp, 0, 0);
        if (FAILED(hres))
        {
            ILog("Failed to get return value. Error code = 0x%lx\n", hres);
            return hres;
        }

        // Get the process ID
        VARIANT vtProp2;
        VariantInit(&vtProp2);
        hres = pOutParams->Get(_bstr_t(L"ProcessId"), 0, &vtProp2, 0, 0);
        if (FAILED(hres))
        {
            ILog("Failed to get process ID. Error code = 0x%lx\n", hres);
            return hres;
        }

        // Print the return value and process id
        ILog("Return Value = %d Process ID = %d \n", vtProp.intVal, vtProp2.intVal);

        // Store the return values for process ID & return value
        *ProcessId = vtProp2.intVal;
        *ReturnValue = vtProp.intVal;

        // Cleanup
        if (pStartupInstance)
            pStartupInstance->Release();
        if (pParamsInstance)
            pParamsInstance->Release();
        if (pClassInstance)
            pClassInstance->Release();
        if (pInParamsDefinition)
            pInParamsDefinition->Release();
        if (pStartupObject)
            pStartupObject->Release();
        if (pClass)
            pClass->Release();
        if (pOutParams)
            pOutParams->Release();
        if (pInParams)
            pInParams->Release();
        varStartup.punkVal = NULL;
        hres = VariantClear(&varStartup);
        hres = VariantClear(&vtProp);
        hres = VariantClear(&vtProp2);
        hres = VariantClear(&varCommand);
        hres = VariantClear(&varCurrentDirectory);
        hres = VariantClear(&varParams);
        SysFreeString(ClassName);
        SysFreeString(MethodName);
        if (bInitialized)
            CoUninitialize();
    }

    UINT32 TERMINATE(_In_ UINT32 Reason)
    {

        return 0;
    }

    UINT32 SETPRIORITY(_In_ UINT32 Priority)
    {

        return 0;
    }

private:
    WMIConnection& WMI;
    COMWrapper& COM;
};