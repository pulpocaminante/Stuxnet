#pragma once
#include <map>
#include "ComWrapper.hpp"

typedef struct AVProduct {
    std::wstring displayName;
    std::wstring instanceGuid;
    std::wstring pathSignedExe;
    std::wstring pathReportingExe;
    std::wstring timestamp;
    UINT productState = 0;
} AVProduct, * PAVProduct;

typedef struct Process {
	std::wstring name;
    std::wstring path;
    UINT32 ProcessId;
    
} Process, * PProcess;

class WMIConnection {
public:

    BOOL IReady;
    BOOL bInitialized;
    
    WMIConnection(COMWrapper& com_wrapper) : COM(com_wrapper),
        pEnumerator(NULL), pSvc(NULL), pLoc(NULL), hres(NULL),
        currentNamespace(namespaceBuf), bInitialized(FALSE),
        IReady(FALSE), bConnected(FALSE)
    {
        this->IReady = COM.IReady && this->InitializeWMI(FALSE);
    }

    ~WMIConnection() = default;

    
    BOOL __stdcall ConnectToNamespace(LPCSTR lpNamespaceIn, BOOL bCalledFromThread)
    {
        char* cNamespace = new CHAR[256];
        wchar_t* lpwNamespace = new WCHAR[256];
        wchar_t* lpwEncodedNamespace = new WCHAR[256];
        
        if (pLoc == nullptr)
            return FALSE;
        
        if (MultiByteToWideChar(CP_ACP, 0, lpNamespaceIn, -1, lpwEncodedNamespace, (strlen(lpNamespaceIn ) + 1) * 2) == 0)
        {
            ILog("Failed to convert namespace to wide char\n");
            goto cleanup;
        }
        
		IFind.railfence_decipher(5, lpNamespaceIn, cNamespace);

        if (MultiByteToWideChar(CP_ACP, 0, cNamespace, -1, lpwNamespace, (strlen(cNamespace) + 1) * 2) == 0)
        {
            ILog("Failed to convert namespace to wide char\n");
            goto cleanup;
        }

        
        // Connect to WMI through the IWbemLocator::ConnectServer method
        hres = pLoc->ConnectServer(
            _bstr_t(lpwNamespace), // Object path of WMI namespace
            NULL,                    // User name. NULL = current user
            NULL,                    // User password. NULL = current
            0,                       // Locale. NULL indicates current
            NULL,                    // Security flags.
            0,                       // Authority (for example, Kerberos)
            0,                       // Context object 
            &pSvc                    // pointer to IWbemServices proxy
        );
        if (FAILED(hres))
        {
            ILog("Could not connect. Error code = 0x%llx", (unsigned long long)hres);
            goto cleanup;
        }

        // Set security levels on the proxy 
        hres = CoSetProxyBlanket(
            pSvc,                        // Indicates the proxy to set
            RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
            RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
            NULL,                        // Server principal name 
            RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
            RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
            NULL,                        // client identity
            EOAC_NONE                    // proxy capabilities 
        );

        if (FAILED(hres) && !bCalledFromThread)
        {
            ILog("Could not set proxy blanket. Error code = 0x%lx\n", hres);
            goto cleanup;
        }

        wcsncpy(this->currentNamespace, lpwEncodedNamespace, wcslen(lpwEncodedNamespace)+1);


        if (!bCalledFromThread)
        {
            ILog("Connected to %ls WMI namespace\n", lpwNamespace);
        }
        
    cleanup:
        delete[] cNamespace;
        delete[] lpwNamespace;
        delete[] lpwEncodedNamespace;

        // If we jumped to the cleanup label we've failed
        if (pSvc == nullptr || !pSvc)
            return FALSE;
        
        bConnected = TRUE;
        return TRUE;
    }

    HRESULT InvokeMethod(
        /* [in] */ __RPC__in const BSTR ClassName,
        /* [in] */ __RPC__in const BSTR MethodName,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemClassObject* pParamsInstance,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemClassObject** ppOutParams,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemCallResult** ppCallResult)
    {

        // Execute the method
		hres = pSvc->ExecMethod(ClassName, MethodName, 0, NULL, pParamsInstance, ppOutParams, ppCallResult);
        if (FAILED(hres))
        {
            ILog("Failed to execute method. Error code = 0x%lx\n", hres);
            return hres;
        }
        
        return hres;
    }

private:
    COMWrapper& COM;
    HRESULT hres;
    WCHAR namespaceBuf[256];
    IExport IFind;
    IEnumWbemClassObject* pEnumerator;
    
public:
    LPWSTR currentNamespace;
    BOOL bConnected;
    IWbemLocator* pLoc;
    IWbemServices* pSvc;

    BOOL __stdcall InitializeWMI(BOOL bCalledFromThread)
    {
        // Initialize COM.
        hres = COM.CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres))
        {
            ILog("Failed to initialize COM library. Error code = 0x%llx", (unsigned long long)hres);
            return FALSE;                  // Program has failed.
        }
        else
        {
            this->bInitialized = TRUE;
        }

        // Set general COM security levels 
        if (!bCalledFromThread)
        {
            hres = COM.CoInitializeSecurity(
                NULL,
                -1,                          // COM authentication
                NULL,                        // Authentication services
                NULL,                        // Reserved
                RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
                RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
                NULL,                        // Authentication info
                EOAC_NONE,                   // Additional capabilities 
                NULL                         // Reserved
            );
        }

        if (FAILED(hres))
        {
            //ILog("Failed to initialize security. Error code = 0x%llx\n", (unsigned long long)hres);
            //COM.CoUninitialize();
            //return FALSE;                    // Program has failed.
        }
        // Get the class factory for the WbemLocator object
        IClassFactory* pClassFactory = NULL;

        hres = COM.CoGetClassObject(CSLSID_WbemLocator, CLSCTX_INPROC_SERVER, NULL, SIID_IClassFactory, (void**)&pClassFactory);

        if (FAILED(hres)) {
            std::cout << "Failed to get class factory for WbemLocator object. Error code = 0x"
                << std::hex << hres << std::endl;
            COM.CoUninitialize();
            return FALSE;               // Program has failed.
        }

        // Create an instance of the WbemLocator object
        IUnknown* pUnk = NULL;
        hres = pClassFactory->CreateInstance(NULL, SIID_IUnknown, (void**)&pUnk);
        if (FAILED(hres)) {
            std::cout << "Failed to create instance of WbemLocator object. Error code = 0x"
                << std::hex << hres << std::endl;
            pClassFactory->Release();
            COM.CoUninitialize();
            return FALSE;                 // Program has failed.
        }

        hres = pUnk->QueryInterface(SIID_IWbemLocator, (void**)&pLoc);
        if (FAILED(hres)) {
            std::cout << "Failed to query for IWbemLocator interface. Error code = 0x"
                << std::hex << hres << std::endl;
            pUnk->Release();
            pClassFactory->Release();
            COM.CoUninitialize();
            return FALSE;    // Program has failed.
        }

        if (pLoc == nullptr)
        {
            std::cout << "Failed to get IWbemLocator interface. Error code = 0x"
                << std::hex << hres << std::endl;
            pUnk->Release();
            pClassFactory->Release();
            COM.CoUninitialize();
            return FALSE;     // Program has failed.
        }

        pUnk->Release();
        pClassFactory->Release();

        return TRUE;
    }

};


class RegistryBindingFast
{
public:
    BOOL IReady;
    
    RegistryBindingFast(COMWrapper& COM, WMIConnection& WMI) : COM(COM), WMI(WMI)
    {
        if(WMI.IReady && COM.IReady)
            if (WMI.ConnectToNamespace("RsnObcoOuriTSit\\p", 0))
                if(WMI.bConnected)
                    if (InitializeFilterBinder())
			            IReady = TRUE;
    }

    ~RegistryBindingFast()
    {
    }

    HRESULT BindRegistryFilter(_In_ std::wstring sFilterName, 
        _In_ std::wstring sConsumerName, 
        _In_ std::wstring sFilterQuery,
        _In_ std::wstring sCommand)
    {

        if (wcscmp(this->WMI.currentNamespace, L"RsnObcoOuriTSit\\p") != 0)
            WMI.ConnectToNamespace("RsnObcoOuriTSit\\p", 0);

        if (!WMI.bConnected)
            return E_FAIL; // Something is very wrong
        
        HRESULT hres;
        IWbemClassObject* pFilterClass = NULL;
        IWbemClassObject* pConsumerClass = NULL;
        IWbemClassObject* pBindingClass = NULL;
		IWbemClassObject* pConsumer = NULL;
		IWbemClassObject* pFilter = NULL;
		IWbemClassObject* pBinding = NULL;
        std::wstring sFilterPath = L"__EventFilter.Name=\"" + sFilterName + L"\"";
        std::wstring sConsumerPath = L"CommandLineEventConsumer.Name=\"" + sConsumerName + L"\"";
        
		// Get the filter class
		hres = WMI.pSvc->GetObject(_bstr_t(L"__EventFilter"), 0, NULL, &pFilterClass, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to get __EventFilter class object. Error code = 0x%lx\n", hres);
			return hres;
		}

		// Create the filter instance
		hres = pFilterClass->SpawnInstance(0, &pFilter);
		if (FAILED(hres))
		{
			ILog("Failed to spawn __EventFilter instance. Error code = 0x%lx\n", hres);
			return hres;
		}
        
        // Put name, query and query language
        VARIANT vtProp;
        VariantInit(&vtProp);
        vtProp.vt = VT_BSTR;
        vtProp.bstrVal = SysAllocString(sFilterName.c_str());
        hres = pFilter->Put(_bstr_t(L"Name"), 0, &vtProp, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set Name property on Filter. Error code = 0x%lx\n", hres);
            goto cleanup;
            return hres;
        }
        VariantClear(&vtProp);

        VARIANT vtProp2;
        VariantInit(&vtProp2);
        vtProp2.vt = VT_BSTR;
		vtProp2.bstrVal = SysAllocString(sFilterQuery.c_str());
		hres = pFilter->Put(_bstr_t(L"Query"), 0, &vtProp2, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set Query property. Error code = 0x%lx\n", hres);
			goto cleanup;
			return hres;
		}
        
        // Set Language = WQL
		VARIANT vtProp7;
		VariantInit(&vtProp7);
        vtProp7.vt = VT_BSTR;
		vtProp7.bstrVal = SysAllocString(L"WQL");
		hres = pFilter->Put(_bstr_t(L"QueryLanguage"), 0, &vtProp7, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set QueryLanguage property. Error code = 0x%lx\n", hres);
            goto cleanup;
        }

        // Set namespace = ROOT\DEFAULT
		VARIANT vtProp11;
		VariantInit(&vtProp11);
		vtProp11.vt = VT_BSTR;
		vtProp11.bstrVal = SysAllocString(L"root/cimv2");
		hres = pFilter->Put(_bstr_t(L"EventNamespace"), 0, &vtProp11, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set Namespace property. Error code = 0x%lx\n", hres);
			goto cleanup;
		}
        
        // Get the consumer class
		hres = WMI.pSvc->GetObject(_bstr_t(L"CommandLineEventConsumer"), 0, NULL, &pConsumerClass, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to get CommandLineEventConsumer class object. Error code = 0x%lx\n", hres);
			goto cleanup;
		}

		hres = pConsumerClass->SpawnInstance(0, &pConsumer);
		if (FAILED(hres))
		{
			ILog("Failed to spawn CommandLineEventConsumer instance. Error code = 0x%lx\n", hres);
			goto cleanup;
		}
        
        // Put name and commandline template to consumer
		VARIANT vtProp3;
		VariantInit(&vtProp3);
		vtProp3.vt = VT_BSTR;
		vtProp3.bstrVal = SysAllocString(sConsumerName.c_str());
		hres = pConsumer->Put(_bstr_t(L"Name"), 0, &vtProp3, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set Name property on Consumer. Error code = 0x%lx\n", hres);
            goto cleanup;
		}
		VariantClear(&vtProp3);
        
		VARIANT vtProp4;
		VariantInit(&vtProp4);
		vtProp4.vt = VT_BSTR;
		vtProp4.bstrVal = SysAllocString(sCommand.c_str());
		hres = pConsumer->Put(_bstr_t(L"CommandLineTemplate"), 0, &vtProp4, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set CommandLineTemplate property on Consumer. Error code = 0x%lx\n", hres);
			goto cleanup;
		}
  //      
  //      // Set executable path on the consumer
		//VARIANT vtProp8;
		//VariantInit(&vtProp8);
		//vtProp8.vt = VT_BSTR;
  //      vtProp8.bstrVal = NULL;
		//hres = pConsumer->Put(_bstr_t(L"ExecutablePath"), 0, &vtProp8, 0);
		//if (FAILED(hres))
		//{
		//	ILog("Failed to set ExecutablePath property on Consumer. Error code = 0x%lx\n", hres);
		//	goto cleanup;
		//}   
        
        // Get the binding class
		hres = WMI.pSvc->GetObject(_bstr_t(L"__FilterToConsumerBinding"), 0, NULL, &pBindingClass, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to get __FilterToConsumerBinding class object. Error code = 0x%lx\n", hres);
			goto cleanup;
		}
        
		// Create the binding instance
		hres = pBindingClass->SpawnInstance(0, &pBinding);
		if (FAILED(hres))
		{
			ILog("Failed to spawn __FilterToConsumerBinding instance. Error code = 0x%lx\n", hres);
			goto cleanup;
		}

        // Set the filter and consumer on the binding
		hres = pBinding->SpawnInstance(0, &pBinding);
        if (FAILED(hres))
        {
            ILog("Failed to spawn __FilterToConsumerBinding instance. Error code = 0x%lx", hres);
            goto cleanup;
        }
        
		VARIANT vtProp5;
		VariantInit(&vtProp5);
		vtProp5.vt = VT_BSTR;
		vtProp5.bstrVal = SysAllocString(sFilterPath.c_str());
		hres = pBinding->Put(_bstr_t(L"Filter"), 0, &vtProp5, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set Filter property. Error code = 0x%lx\n", hres);
			goto cleanup;
		}
        
		VARIANT vtProp6;
		VariantInit(&vtProp6);
		vtProp6.vt = VT_BSTR;
		vtProp6.bstrVal = SysAllocString(sConsumerPath.c_str());
		hres = pBinding->Put(_bstr_t(L"Consumer"), 0, &vtProp6, 0);
		if (FAILED(hres))
        {
			ILog("Failed to set Consumer property. Error code = 0x%lx\n", hres);
			goto cleanup;
		}
        
        // Set the filter, consumer, and filter to consumer binding in WMI
		hres = WMI.pSvc->PutInstance(pFilter, WBEM_FLAG_CREATE_OR_UPDATE, NULL, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to put filter instance. Error code = 0x%lx\n", hres);
			goto cleanup;
		}
        
		hres = WMI.pSvc->PutInstance(pConsumer, WBEM_FLAG_CREATE_OR_UPDATE, NULL, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to put consumer instance. Error code = 0x%lx\n", hres);
			goto cleanup;
		}

		hres = WMI.pSvc->PutInstance(pBinding, WBEM_FLAG_CREATE_OR_UPDATE, NULL, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to put binding instance. Error code = 0x%lx\n", hres);
			goto cleanup;
		}
        

        // Cleanup
        cleanup:
		if (pFilter)
			pFilter->Release();
		if (pConsumer)
			pConsumer->Release();
		if (pBinding)
			pBinding->Release();
   
        if(SUCCEEDED(hres))
            ILog("Filter, consumer and binding created successfully\n");
        
		return hres;
        
    }

private:
    
    BOOL InitializeFilterBinder()
    {
		return TRUE;
    }
    
    WMIConnection& WMI;
    COMWrapper& COM;
};