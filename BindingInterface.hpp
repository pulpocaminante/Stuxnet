#pragma once
#include "WMIConnection.hpp"

class BindingInterface
{
public:
    BOOL IReady;

    BindingInterface(COMWrapper& com_wrapper, WMIConnection& wmi_connection) :
        COM(com_wrapper), WMI(wmi_connection), IReady(FALSE),
        pFilterClass(NULL), pFilter(NULL), pBinderClass(NULL),
        pBinder(NULL), pConsumerClass(NULL), pConsumer(NULL),
        sFilterName_Bind(L""), sConsumerName_Bind(L"")
    {
        if (WMI.IReady && COM.IReady)
            IReady = TRUE;
    }

    ~BindingInterface()
    {
        if (pFilter)
            pFilter->Release();
        if (pFilterClass)
            pFilterClass->Release();
        if (pBinder)
            pBinder->Release();
        if (pBinderClass)
            pBinderClass->Release();
        if (pConsumer)
            pConsumer->Release();
        if (pConsumerClass)
            pConsumerClass->Release();
    }

    HRESULT CreateFilter(
        _In_ std::wstring sFilterName,
        _In_ std::wstring sFilterQuery,
        _In_ std::wstring sFilterNamespace)
    {
        if (wcscmp(this->WMI.currentNamespace, L"RsnObcoOuriTSit\\p") != 0)
            WMI.ConnectToNamespace("RsnObcoOuriTSit\\p", 0);
        
        if (!WMI.bConnected)
            return E_FAIL; // Something is very wrong
        
        if (pFilterClass != NULL)
        {
            ILog("Error: Can only create one filter per class instance\n");
            return E_FAIL;
        }

        HRESULT hres;

        // Variant initialization
        VARIANT vtProp;
        VARIANT vtProp2;
        VARIANT vtProp7;
        VARIANT vtProp11;
        VariantInit(&vtProp);
        VariantInit(&vtProp2);
        VariantInit(&vtProp7);
        VariantInit(&vtProp11);

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
        vtProp.vt = VT_BSTR;
        vtProp.bstrVal = SysAllocString(sFilterName.c_str());
        hres = pFilter->Put(_bstr_t(L"Name"), 0, &vtProp, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set Name property on Filter. Error code = 0x%lx\n", hres);
            goto cleanup;
            return hres;
        }

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
        vtProp7.vt = VT_BSTR;
        vtProp7.bstrVal = SysAllocString(L"WQL");
        hres = pFilter->Put(_bstr_t(L"QueryLanguage"), 0, &vtProp7, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set QueryLanguage property. Error code = 0x%lx\n", hres);
            goto cleanup;
        }

        // Set namespace = ROOT\DEFAULT
        vtProp11.vt = VT_BSTR;
        vtProp11.bstrVal = SysAllocString(sFilterNamespace.c_str());
        hres = pFilter->Put(_bstr_t(L"EventNamespace"), 0, &vtProp11, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set Namespace property. Error code = 0x%lx\n", hres);
            goto cleanup;
        }

        ILog("Generated filter successfully\n");

        this->sFilterName_Bind = sFilterName;

        // Cleanup
    cleanup:
        VariantClear(&vtProp);
        VariantClear(&vtProp2);
        VariantClear(&vtProp7);
        VariantClear(&vtProp11);

        return hres;
    }

    HRESULT CreateCommandLineConsumer(
        _In_     std::wstring sConsumerName,
        _In_     std::wstring sCommandLine,
        _In_opt_ std::wstring sExecutablePath)
    {
        if (wcscmp(this->WMI.currentNamespace, L"RsnObcoOuriTSit\\p") != 0)
            WMI.ConnectToNamespace("RsnObcoOuriTSit\\p", 0);
        
        if (!WMI.bConnected)
            return E_FAIL; // Something is very wrong
        
        if (pConsumerClass != NULL)
        {
            ILog("Error: Can only create one consumer per class instance\n");
            return E_FAIL;
        }

        HRESULT hres;

        // Initialize variants
        VARIANT vtProp3;
        VARIANT vtProp4;
        VARIANT vtProp8;
        VARIANT vtProp9;
        VariantInit(&vtProp3);
        VariantInit(&vtProp4);
        VariantInit(&vtProp8);
        VariantInit(&vtProp9);


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
        vtProp3.vt = VT_BSTR;
        vtProp3.bstrVal = SysAllocString(sConsumerName.c_str());
        hres = pConsumer->Put(_bstr_t(L"Name"), 0, &vtProp3, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set Name property on Consumer. Error code = 0x%lx\n", hres);
            goto cleanup;
        }

        vtProp4.vt = VT_BSTR;
        vtProp4.bstrVal = SysAllocString(sCommandLine.c_str());
        hres = pConsumer->Put(_bstr_t(L"CommandLineTemplate"), 0, &vtProp4, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set CommandLineTemplate property on Consumer. Error code = 0x%lx\n", hres);
            goto cleanup;
        }

        // Set executable path on the consumer
        if (!sExecutablePath.empty())
        {
            vtProp8.vt = VT_BSTR;
            vtProp8.bstrVal = NULL;
            hres = pConsumer->Put(_bstr_t(L"ExecutablePath"), 0, &vtProp8, 0);
            if (FAILED(hres))
            {
                ILog("Failed to set ExecutablePath property on Consumer. Error code = 0x%lx\n", hres);
                goto cleanup;
            }
        }
        
        ILog("Generated consumer class successfully\n");

        sConsumerName_Bind = sConsumerName;

        // Cleanup
    cleanup:
        VariantClear(&vtProp3);
        VariantClear(&vtProp4);
        VariantClear(&vtProp8);
        return hres;
    }

    HRESULT BindFilterAndConsumer()
    {
        if (wcscmp(this->WMI.currentNamespace, L"RsnObcoOuriTSit\\p") != 0)
            WMI.ConnectToNamespace("RsnObcoOuriTSit\\p", 0);

        if (!WMI.bConnected)
            return E_FAIL; // Something is very wrong
        
        if (pFilter == NULL || pConsumer == NULL)
        {
            ILog("Error: Filter and Consumer must be created before binding\n");
            return E_FAIL;
        }

        if (sFilterName_Bind.empty() || sConsumerName_Bind.empty())
        {
            ILog("Error: Filter and Consumer must be created before binding\n");
            return E_FAIL;
        }

        std::wstring sFilterPath = L"__EventFilter.Name=\"" + sFilterName_Bind + L"\"";
        std::wstring sConsumerPath = L"CommandLineEventConsumer.Name=\"" + sConsumerName_Bind + L"\"";

        HRESULT hres;

        // Initialize variants
        VARIANT vtProp6;
        VARIANT vtProp5;
        VariantInit(&vtProp5);
        VariantInit(&vtProp6);

        // Get the binding class
        hres = WMI.pSvc->GetObject(_bstr_t(L"__FilterToConsumerBinding"), 0, NULL, &pBinderClass, NULL);
        if (FAILED(hres))
        {
            ILog("Failed to get __FilterToConsumerBinding class object. Error code = 0x%lx\n", hres);
            goto cleanup;
        }

        // Create the binding instance
        hres = pBinderClass->SpawnInstance(0, &pBinder);
        if (FAILED(hres))
        {
            ILog("Failed to spawn __FilterToConsumerBinding instance. Error code = 0x%lx\n", hres);
            goto cleanup;
        }

        // Set the filter and consumer on the binding
        hres = pBinder->SpawnInstance(0, &pBinder);
        if (FAILED(hres))
        {
            ILog("Failed to spawn __FilterToConsumerBinding instance. Error code = 0x%lx", hres);
            goto cleanup;
        }

        vtProp5.vt = VT_BSTR;
        vtProp5.bstrVal = SysAllocString(sFilterPath.c_str());
        hres = pBinder->Put(_bstr_t(L"Filter"), 0, &vtProp5, 0);
        if (FAILED(hres))
        {
            ILog("Failed to set Filter property. Error code = 0x%lx\n", hres);
            goto cleanup;
        }

        vtProp6.vt = VT_BSTR;
        vtProp6.bstrVal = SysAllocString(sConsumerPath.c_str());
        hres = pBinder->Put(_bstr_t(L"Consumer"), 0, &vtProp6, 0);
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

        hres = WMI.pSvc->PutInstance(pBinder, WBEM_FLAG_CREATE_OR_UPDATE, NULL, NULL);
        if (FAILED(hres))
        {
            ILog("Failed to put binding instance. Error code = 0x%lx\n", hres);
            goto cleanup;
        }

        // Cleanup
    cleanup:
        VariantClear(&vtProp5);
        VariantClear(&vtProp6);
        ILog("Successfully bound consumer & fitler\n");
        return hres;
    }

private:
    COMWrapper& COM;
    WMIConnection& WMI;
    IWbemClassObject* pFilterClass;
    IWbemClassObject* pFilter;
    IWbemClassObject* pBinderClass;
    IWbemClassObject* pBinder;
    IWbemClassObject* pConsumerClass;
    IWbemClassObject* pConsumer;
    std::wstring sFilterName_Bind;
    std::wstring sConsumerName_Bind;
};