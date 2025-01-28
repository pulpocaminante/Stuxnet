#pragma once
#include "WMIConnection.hpp"
#include "ExportInterface.hpp"
#include <any>

class QueryInterface
{
public:
	BOOL IReady;
	
	QueryInterface(COMWrapper& com_wrapper, WMIConnection& wmi_connetion) :
		IReady(FALSE), COM(com_wrapper), WMI(wmi_connetion)
	{
		if (COM.IReady && WMI.IReady)
			IReady = TRUE;
	}
	

	HRESULT Query(
		_In_ std::wstring sQuery, 
		_Out_ std::vector< std::map<std::wstring, std::any> >& vOut)
	{
        if (!WMI.bConnected)
            WMI.ConnectToNamespace("RVOM2OITC\\", 0);

        if (!WMI.bConnected)
        {
            ILog("Failed to connect to namespace\n");
            return E_FAIL; // Something is very wrong
        }

		// Declarations 
		LPCWSTR lpQuery = sQuery.c_str();
		std::vector<std::wstring> vResult;
		IWbemClassObject* pclsObj = NULL;
		IEnumWbemClassObject* pEnumerator = NULL;
		
		std::map<std::wstring, std::any> properties;

		// Convert query to LPWSTR
		//IFind.railfence_encipher(5, sQuery.c_str(), lpQuery);
		ILog("Querying: %ls\n", sQuery.c_str());

		// Execute query
		HRESULT hres = WMI.pSvc->ExecQuery(_bstr_t(L"WQL"), _bstr_t(lpQuery),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator);
		
		// Check for errors
		if (SUCCEEDED(hres))
		{
			// Get the data from the query enumerator
			ULONG uReturn = 0;
			while (pEnumerator)
			{
				// Next
				hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (0 == uReturn)
				{
					break;
				}

				// Check for errors
				if (FAILED(hres))
				{
					pclsObj->Release();
					pEnumerator->Release();
					return hres;
				}
                
				// Get the names of the properties
				SAFEARRAY* pNames = NULL;
				hres = pclsObj->GetNames(NULL, WBEM_FLAG_LOCAL_ONLY, NULL, &pNames);
				if (SUCCEEDED(hres))
				{
					// Get the data from the properties
					LONG lLBound = 0;
					LONG lUBound = 0;
					hres = SafeArrayGetLBound(pNames, 1, &lLBound);
					hres = SafeArrayGetUBound(pNames, 1, &lUBound);
                    
                    // Temporary variables for arrays
                    uintptr_t eUINT16 = 0;
                    uintptr_t eINT16 = 0;
                    std::vector<unsigned short> vUINT16;
                    std::vector<short> vINT16;
                    LONG lLArrayBound = 0;
                    LONG lUArrayBound = 0;
                    
                    std::wstring ws;
                    
					for (LONG i = lLBound; i <= lUBound; i++)
					{
						BSTR bstr;
						hres = SafeArrayGetElement(pNames, &i, &bstr);
						if (SUCCEEDED(hres))
						{
							// Get the value of the property
							VARIANT vtProp;
							VariantInit(&vtProp);
							hres = pclsObj->Get(bstr, 0, &vtProp, 0, 0);
							if (SUCCEEDED(hres))
							{
                                
								switch (vtProp.vt)
								{
									case VT_I4:
										properties[bstr] = vtProp.lVal;
										break;
									case VT_BSTR:
                                        ws = vtProp.bstrVal;
                                        properties[bstr] = ws;
										break;
									case VT_UI4:
										properties[bstr] = vtProp.ulVal;
										break;
									case VT_UINT:
										properties[bstr] = vtProp.uintVal;
										break;
									case VT_I2:
										properties[bstr] = vtProp.iVal;
										break;
									case VT_DATE:
										properties[bstr] = vtProp.date;
										break;
                                    case VT_BOOL:
                                        properties[bstr] = vtProp.boolVal;
                                        break;
                                    case VT_I2 | VT_ARRAY:
										properties[bstr] = vtProp.parray;
										break;
                                    case VT_I4 | VT_ARRAY:
                                        vINT16.clear();
                                        // Get the bounds of the array
                                        SafeArrayGetLBound(vtProp.parray, 1, &lLArrayBound);
                                        SafeArrayGetUBound(vtProp.parray, 1, &lUArrayBound);
                                        for (LONG i = lLArrayBound; i <= lUArrayBound || i <= 40; i++) {
                                            eINT16 = 0;
                                            // Get the element at the current index
                                            SafeArrayGetElement(vtProp.parray, &i, &eINT16);
											if(eINT16)
                                                vINT16.push_back(eINT16);
                                        }
                                        properties[bstr] = vINT16;
										break;
                                    case VT_UINT | VT_ARRAY:
										break;
									case VT_UI4 | VT_ARRAY:
                                        vUINT16.clear();
                                        // Get the bounds of the array
                                        SafeArrayGetLBound(vtProp.parray, 1, &lLArrayBound);
                                        SafeArrayGetUBound(vtProp.parray, 1, &lUArrayBound);
                                        for (LONG i = lLArrayBound; i <= lUArrayBound; i++) {
                                            eUINT16 = 0;
                                            // Get the element at the current index
                                            SafeArrayGetElement(vtProp.parray, &i, &eUINT16);
                                            if(eUINT16)
                                                vUINT16.push_back(eUINT16);
                                        }
                                        properties[bstr] = vUINT16;
                                        break;
									case VT_UI2 | VT_ARRAY:
									default:
										break;
								}
							}
							VariantClear(&vtProp);
						}
					}
					vOut.push_back(properties);
				}
				// Destroy safe array
				SafeArrayDestroy(pNames);
				
			}
			// Cleanup
            if(pEnumerator)
			    pEnumerator->Release();
            if(pclsObj)
			    pclsObj->Release();
			
		}
		return hres;
	}
    
    HRESULT QueryAntivirusProducts(
        _In_ std::vector<AVProduct>& pAVProducts)
    {

        if (wcscmp(WMI.currentNamespace, L"RutOcrneOeierTStC2\\y") != 0)
            WMI.ConnectToNamespace("RutOcrneOeierTStC2\\y", 0);

        if (!WMI.bConnected)
        {
            ILog("Failed to connect to namespace\n");
            return E_FAIL; // Something is very wrong
        }
        
        // Use the IWbemServices pointer to make requests of WMI
        HRESULT hres;
        
        LPWSTR lpQuery = new WCHAR[256];
        IEnumWbemClassObject* pEnumerator = NULL;
        IFind.railfence_wdecipher(5, L"S trE*FniPoL RAVsdETO iuutCMrc", lpQuery);
        hres = WMI.pSvc->ExecQuery(
            _bstr_t("WQL"),
            _bstr_t(lpQuery),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator);

        if (FAILED(hres))
        {
            ILog("Query for antivirus products failed. Error code = 0x%lx\n", hres);
            return hres;               // Program has failed.
        }
        delete[] lpQuery;

        // Get the data from the query in step 6
        IWbemClassObject* pclsObj = NULL;
        ULONG uReturn = 0;

        while (pEnumerator)
        {

            AVProduct avProduct;

            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            if (0 == uReturn)
            {
                break;
            }

            VARIANT vtProp = { 0 };
            // Get the value of the Name property

            LPWSTR displayName = new WCHAR[256];
            IFind.railfence_wdecipher(5, L"daiNmsyepal", displayName);
            hr = pclsObj->Get(displayName, 0, &vtProp, 0, 0);
            if (!FAILED(hr)) {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    ILog("Property displayName is not set\n");
                else
                {
                    // Copy to output structure
                    avProduct.displayName = vtProp.bstrVal;
                    VariantClear(&vtProp);
                }

            }
            delete[] displayName;

            // Get the value of the instanceGuid property
            LPWSTR instanceGuid = new WCHAR[256];
            IFind.railfence_wdecipher(5, L"iGneuscitnda", instanceGuid);
            hr = pclsObj->Get(instanceGuid, 0, &vtProp, 0, 0);
            if (!FAILED(hr)) {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    ILog("Property is not set\n");
                else
                {
                    // Copy to output structure
                    avProduct.instanceGuid = vtProp.bstrVal;
                    VariantClear(&vtProp);
                }
            }
            delete[] instanceGuid;

            // Get the value of the productState property
            LPWSTR productState = new WCHAR[256];
            IFind.railfence_wdecipher(5, L"ptrSaottdceu", productState);
            hr = pclsObj->Get(productState, 0, &vtProp, 0, 0);
            if (!FAILED(hr)) {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    ILog("Property is not set\n");
                else
                {
                    // Copy to output structure
                    avProduct.productState = vtProp.uintVal;
                    VariantClear(&vtProp);
                }
            }
            delete[] productState;

            // Get the value of the pathToSignedProductExe
            LPWSTR pathToSignedProductExe = new WCHAR[256];
            IFind.railfence_wdecipher(5, L"pguaindctSeothodrEeTPx", pathToSignedProductExe);
            hr = pclsObj->Get(pathToSignedProductExe, 0, &vtProp, 0, 0);
            if (!FAILED(hr)) {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    ILog("Property is not set\n");
                else
                {
                    // Copy to output structure
                    avProduct.pathSignedExe = vtProp.bstrVal;
                    VariantClear(&vtProp);
                }
            }
            delete[] pathToSignedProductExe;

            // Get the value of the pathToSignedReportingExe
            LPWSTR pathToSignedReportingExe = new WCHAR[256];
            IFind.railfence_wdecipher(5, L"pgrainotetSepixhodenETRg", pathToSignedReportingExe);
            hr = pclsObj->Get(pathToSignedReportingExe, 0, &vtProp, 0, 0);
            if (!FAILED(hr)) {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    ILog("Property is not set\n");
                else
                {
                    // Copy to output structure
                    avProduct.pathReportingExe = vtProp.bstrVal;
                    VariantClear(&vtProp);
                }
            }
            delete[] pathToSignedReportingExe;


            // Get the value of the onAccessScanningEnabled property
            LPWSTR timestamp = new WCHAR[256];
            IFind.railfence_wdecipher(5, L"tpimmaets", timestamp);
            hr = pclsObj->Get(timestamp, 0, &vtProp, 0, 0);
            if (!FAILED(hr)) {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    ILog("Property is not set\n");
                else
                {
                    // Copy to output structure
                    avProduct.timestamp = vtProp.bstrVal;
                    VariantClear(&vtProp);
                }
            }
            delete[] timestamp;
            // Push the AVProduct to the vector
            pAVProducts.push_back(avProduct);

        }

        // Cleanup
        if (pEnumerator)
            pEnumerator->Release();
        if (pclsObj)
            pclsObj->Release();
        return hres;
    }
    
private:
	WMIConnection& WMI;
	COMWrapper& COM;
	IExport IFind;
};