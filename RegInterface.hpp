#pragma once
#include "WMIConnection.hpp"

class RegInterface
{
public:
	BOOL IReady;

	RegInterface(_In_ COMWrapper& COM, _In_ WMIConnection& WMI) : WMI(WMI), COM(COM), IReady(FALSE), pClass(NULL),
		pClassInstance(NULL)
	{
		if (WMI.IReady && COM.IReady)
			if (this->InitializeStdRegProv())
				IReady = TRUE;
	}

	HRESULT CreateKey(_In_ UINT32 RootKey, _In_ std::wstring SubKey)
	{
		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);

		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}
		
		BSTR ClassName = SysAllocString(L"StdRegProv");
		BSTR MethodName = SysAllocString(L"CreateKey");
		HRESULT hres;
		IWbemClassObject* pMethod = NULL;
		IWbemClassObject* pInParams = NULL;
		IWbemClassObject* pOutParams = NULL;
		IWbemCallResult* pCallResult = NULL;

		if (!IReady)
			return 0;

		// Get the method
		hres = pClass->GetMethod(MethodName, 0, &pMethod, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to get CreateKey method. Error code = 0x%lx\n", hres);
			goto cleanup;
		}

		// Get the in parameters
		hres = pMethod->SpawnInstance(0, &pInParams);
		if (FAILED(hres))
		{
			ILog("Failed to get CreateKey in parameters. Error code = 0x%lx\n", hres);
			goto cleanup;
		}

		// Set the in parameters
		VARIANT vtProp;
		VariantInit(&vtProp);
		vtProp.vt = VT_I4;
		vtProp.intVal = RootKey;
		hres = pInParams->Put(_bstr_t(L"hDefKey"), 0, &vtProp, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set hDefKey property. Error code = 0x%lx\n", hres);
			goto cleanup;
		}

		// Set the in parameters
		VARIANT vtProp2;
		VariantInit(&vtProp2);
		vtProp2.vt = VT_BSTR;
		vtProp2.bstrVal = SysAllocString(SubKey.c_str());

		hres = pInParams->Put(_bstr_t(L"sSubKeyName"), 0, &vtProp2, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set sSubKeyName property. Error code = 0x%lx\n", hres);
			goto cleanup;
		}

		hres = WMI.InvokeMethod(ClassName, MethodName, 0, NULL, pInParams, &pOutParams, &pCallResult);

		if (FAILED(hres))
		{
			ILog("Failed to invoke StdRegProv method. Error code = 0x%lx\n", hres);
			goto cleanup;
		}

		// Cleanup
	cleanup:
		if (pMethod)
			pMethod->Release();
		if (pInParams)
			pInParams->Release();
		if (pOutParams)
			pOutParams->Release();
		VariantClear(&vtProp);
		VariantClear(&vtProp2);
		SysFreeString(ClassName);
		SysFreeString(MethodName);

		if (SUCCEEDED(hres))
			ILog("Made new key %ls\n", SubKey.c_str());

		return hres;
	}

	UINT32 DELETEKEY(_In_ UINT32 RootKey, _In_ std::wstring SubKey)
	{
		return 0;
	}

	UINT32 DELETEVALUE(_In_ UINT32 RootKey, _In_ std::wstring SubKey, _In_ std::wstring ValueName)
	{
		return 0;
	}

	HRESULT EnumKey(_In_ UINT32 RootKey, _In_ std::wstring SubKey, _Out_ std::vector<std::wstring>& Names)
	{
		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);

		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}
		
		BSTR ClassName = SysAllocString(L"StdRegProv");
		BSTR MethodName = SysAllocString(L"EnumKey");

		HRESULT hres = NULL;
		IWbemClassObject* pMethod = NULL;
		IWbemClassObject* pInParams = NULL;
		IWbemClassObject* pOutParams = NULL;
		IWbemCallResult* pCallResult = NULL;

		if (!IReady)
			return hres;

		// Get the method
		hres = pClass->GetMethod(MethodName, 0, &pMethod, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to get EnumKey method. Error code = 0x%lx", hres);
			return hres;
		}

		// Get the in parameters
		hres = pMethod->SpawnInstance(0, &pInParams);
		if (FAILED(hres))
		{
			ILog("Failed to get EnumKey in parameters. Error code = 0x%lx", hres);
			return hres;
		}

		// Set the in parameters
		VARIANT vtProp;
		VariantInit(&vtProp);
		vtProp.vt = VT_I4;
		vtProp.intVal = RootKey;
		hres = pInParams->Put(_bstr_t(L"hDefKey"), 0, &vtProp, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set hDefKey property. Error code = 0x%lx", hres);
			return hres;
		}

		// Set the in parameters
		VARIANT vtProp2;
		VariantInit(&vtProp2);
		vtProp2.vt = VT_BSTR;
		vtProp2.bstrVal = SysAllocString(SubKey.c_str());
		hres = pInParams->Put(_bstr_t(L"sSubKeyName"), 0, &vtProp2, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set sSubKeyName property. Error code = 0x%lx", hres);
			goto cleanup;
		}

		hres = WMI.InvokeMethod(ClassName, MethodName, 0, NULL, pInParams, &pOutParams, &pCallResult);
		if (FAILED(hres))
		{
			ILog("Failed to invoke StdRegProv method. Error code = 0x%lx", hres);
			goto cleanup;
		}

		// Get the BSTR array of names
		VARIANT vtProp3;
		vtProp3.vt = VT_BSTR | VT_ARRAY;
		VariantInit(&vtProp3);
		hres = pOutParams->Get(_bstr_t(L"sNames"), 0, &vtProp3, 0, 0);
		if (FAILED(hres))
		{
			ILog("Failed to get sNames property. Error code = 0x%lx", hres);
			goto cleanup;
		}

		// Check if the array has been populated
		if (vtProp3.parray)
		{
			if (vtProp3.parray->rgsabound[0].cElements == 0)
			{
				ILog("No keys found");
				goto cleanup;
			}
			else
			{
				// Push the names into the vector
				for (LONG i = 0; i < vtProp3.parray->rgsabound[0].cElements; i++)
				{
					BSTR bstr;
					SafeArrayGetElement(vtProp3.parray, &i, &bstr);
					Names.push_back(bstr);
				}
			}

		}

		// Cleanup
	cleanup:
		if (pMethod)
			pMethod->Release();
		if (pInParams)
			pInParams->Release();
		if (pOutParams)
			pOutParams->Release();
		VariantClear(&vtProp3);
		VariantClear(&vtProp2);
		VariantClear(&vtProp);
		SysFreeString(ClassName);
		SysFreeString(MethodName);

		return hres;
	}

	HRESULT EnumValue(
		_In_ UINT32 RootKey,
		_In_ std::wstring SubKey,
		_Out_ std::map<std::wstring, INT32>& TypeMap)
	{
		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);

		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}
		
		BSTR ClassName = SysAllocString(L"StdRegProv");
		BSTR MethodName = SysAllocString(L"EnumValues");
		HRESULT hres;
		IWbemClassObject* pMethod = NULL;
		IWbemClassObject* pInParams = NULL;
		IWbemClassObject* pOutParams = NULL;
		IWbemCallResult* pCallResult = NULL;

		if (!IReady)
			return 0;

		// Get the method
		hres = pClass->GetMethod(MethodName, 0, &pMethod, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to get EnumValues method. Error code = 0x%lx", hres);
			return hres;
		}

		// Get the in parameters
		hres = pMethod->SpawnInstance(0, &pInParams);
		if (FAILED(hres))
		{
			ILog("Failed to get EnumValues in parameters. Error code = 0x%lx", hres);
			return hres;
		}

		// Set the in parameters
		VARIANT vtProp;
		VariantInit(&vtProp);
		vtProp.vt = VT_I4;
		vtProp.intVal = RootKey;
		hres = pInParams->Put(_bstr_t(L"hDefKey"), 0, &vtProp, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set hDefKey property. Error code = 0x%lx", hres);
			goto cleanup;
		}

		// Set the in parameters
		VARIANT vtProp2;
		VariantInit(&vtProp2);
		vtProp2.vt = VT_BSTR;
		vtProp2.bstrVal = SysAllocString(SubKey.c_str());
		hres = pInParams->Put(_bstr_t(L"sSubKeyName"), 0, &vtProp2, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set sSubKeyName property. Error code = 0x%lx", hres);
			goto cleanup;
		}

		hres = WMI.InvokeMethod(ClassName, MethodName, 0, NULL, pInParams, &pOutParams, &pCallResult);
		if (FAILED(hres))
		{
			ILog("Failed to invoke StdRegProv method. Error code = 0x%lx", hres);
			goto cleanup;
		}

		// Get the BSTR array of names
		VARIANT vtProp3;
		vtProp3.vt = VT_BSTR | VT_ARRAY;
		VariantInit(&vtProp3);
		hres = pOutParams->Get(_bstr_t(L"sNames"), 0, &vtProp3, 0, 0);
		if (FAILED(hres))
		{
			ILog("Failed to get sNames property. Error code = 0x%lx", hres);
			goto cleanup;
		}

		// Get the INT32 array of types
		VARIANT vtProp4;
		vtProp4.vt = VT_I4 | VT_ARRAY;
		VariantInit(&vtProp4);
		hres = pOutParams->Get(_bstr_t(L"Types"), 0, &vtProp4, 0, 0);
		if (FAILED(hres))
		{
			ILog("Failed to get Types property. Error code = 0x%lx", hres);
			goto cleanup;
		}

		// Check if the array has been populated
		if (vtProp3.parray)
		{
			if (vtProp3.parray->rgsabound[0].cElements == 0)
			{
				ILog("No values found");
				goto cleanup;
			}
			else
			{

				// Check if vtProp4 INT array has been populated

				if (vtProp4.parray)
				{
					if (vtProp4.parray->rgsabound[0].cElements == 0)
					{
						ILog("No types found");
						goto cleanup;
					}
					else
					{
						// Push the names into the vector
						for (LONG i = 0; i < vtProp3.parray->rgsabound[0].cElements; i++)
						{
							BSTR bstr;
							SafeArrayGetElement(vtProp3.parray, &i, &bstr);
							INT32 type;
							SafeArrayGetElement(vtProp4.parray, &i, &type);
							TypeMap.insert(std::pair<std::wstring, INT32>(bstr, type));
						}
					}
				}
			}

		}

		// Cleanup
	cleanup:
		if (pMethod)
			pMethod->Release();
		if (pInParams)
			pInParams->Release();
		if (pOutParams)
			pOutParams->Release();
		VariantClear(&vtProp);
		VariantClear(&vtProp2);
		VariantClear(&vtProp3);
		VariantClear(&vtProp4);
		SysFreeString(ClassName);
		SysFreeString(MethodName);

		return hres;
	}

	UINT32 GETKEYNAME(_In_ UINT32 RootKey, _In_ std::wstring SubKey, _Out_ std::wstring& Name)
	{
		return 0;
	}

	HRESULT GetStringValue(_In_ UINT32 RootKey, _In_ std::wstring SubKey, _In_ std::wstring ValueName, _Out_ std::wstring& Data)
	{
		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);

		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}
		
		BSTR ClassName = SysAllocString(L"StdRegProv");
		BSTR MethodName = SysAllocString(L"GetStringValue");

		IWbemClassObject* pMethod = NULL;
		IWbemClassObject* pInParams = NULL;
		IWbemClassObject* pOutParams = NULL;
		IWbemCallResult* pCallResult = NULL;
		HRESULT hres = 0;

		// Get the method
		hres = pClass->GetMethod(MethodName, 0, &pMethod, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to get StdRegProv method. Error code = 0x%lx", hres);
			return hres;
		}

		// Get the in parameters
		hres = pMethod->SpawnInstance(0, &pInParams);
		if (FAILED(hres))
		{
			ILog("Failed to spawn StdRegProv instance. Error code = 0x%lx", hres);
			return hres;
		}

		// Set the in parameters
		VARIANT vtProp;
		VariantInit(&vtProp);
		vtProp.vt = VT_I4;
		vtProp.lVal = RootKey;
		hres = pInParams->Put(_bstr_t(L"hDefKey"), 0, &vtProp, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set hDefKey property. Error code = 0x%lx", hres);
			return hres;
		}

		VARIANT vtProp2;
		VariantInit(&vtProp2);
		vtProp2.vt = VT_BSTR;
		vtProp2.bstrVal = SysAllocString(SubKey.c_str());
		hres = pInParams->Put(_bstr_t(L"sSubKeyName"), 0, &vtProp2, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set sSubKeyName property. Error code = 0x%lx", hres);
			return hres;
		}

		VARIANT vtProp3;
		VariantInit(&vtProp3);
		vtProp3.vt = VT_BSTR;
		vtProp3.bstrVal = SysAllocString(ValueName.c_str());
		hres = pInParams->Put(_bstr_t(L"sValueName"), 0, &vtProp3, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set sValueName property. Error code = 0x%lx", hres);
			return hres;
		}

		// Execute the method
		hres = WMI.InvokeMethod(ClassName, MethodName, 0, NULL, pInParams, &pOutParams, &pCallResult);
		if (FAILED(hres))
		{
			ILog("Failed to execute StdRegProv method. Error code = 0x%lx", hres);
			return hres;
		}

		// Get the out parameters
		VARIANT vtProp4;
		VariantInit(&vtProp4);
		hres = pOutParams->Get(_bstr_t(L"sValue"), 0, &vtProp4, NULL, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to get sValue property. Error code = 0x%lx", hres);
			return hres;
		}

		// Check if vtProp4 BSTR has been populated
		if (vtProp4.bstrVal)
		{
			Data = vtProp4.bstrVal;
		}

		// Cleanup
		if (pMethod)
			pMethod->Release();
		if (pInParams)
			pInParams->Release();
		if (pOutParams)
			pOutParams->Release();
		VariantClear(&vtProp);
		VariantClear(&vtProp2);
		VariantClear(&vtProp3);
		VariantClear(&vtProp4);
		SysFreeString(ClassName);
		SysFreeString(MethodName);


		return hres;
	}

	HRESULT SetStringValue(
		_In_ UINT32 RootKey,
		_In_ std::wstring sSubKeyName,
		_In_ std::wstring sValueName,
		_In_ std::wstring sValue
	)
	{
		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);

		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}
		
		BSTR ClassName = SysAllocString(L"StdRegProv");
		BSTR MethodName = SysAllocString(L"SetStringValue");
		HRESULT hres;
		IWbemClassObject* pMethod = NULL;
		IWbemClassObject* pInParams = NULL;
		IWbemClassObject* pParamsInstance = NULL;
		IWbemClassObject* pOutParams = NULL;
		IWbemCallResult* pCallResult = NULL;

		// Get the method
		hres = pClass->GetMethod(MethodName, 0, &pMethod, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to get SetStringValue method. Error code = 0x%lx\n", hres);
			return hres;
		}

		// Get the in parameters
		hres = pMethod->SpawnInstance(0, &pInParams);
		if (FAILED(hres))
		{
			ILog("Failed to get SetStringValue in parameters. Error code = 0x%lx\n", hres);
			return hres;
		}

		// Set the in parameters
		VARIANT vtProp;
		VariantInit(&vtProp);
		vtProp.vt = VT_I4;
		vtProp.intVal = RootKey;
		hres = pInParams->Put(_bstr_t(L"hDefKey"), 0, &vtProp, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set hDefKey property. Error code = 0x%lx\n", hres);
			return hres;
		}

		VARIANT vtProp2;
		VariantInit(&vtProp2);
		vtProp2.vt = VT_BSTR;
		vtProp2.bstrVal = SysAllocString(sSubKeyName.c_str());
		hres = pInParams->Put(_bstr_t(L"sSubKeyName"), 0, &vtProp2, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set sSubKeyName property. Error code = 0x%lx", hres);
			return hres;
		}

		VARIANT vtProp3;
		VariantInit(&vtProp3);
		vtProp3.vt = VT_BSTR;
		vtProp3.bstrVal = SysAllocString(sValueName.c_str());
		hres = pInParams->Put(_bstr_t(L"sValueName"), 0, &vtProp3, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set sValueName property. Error code = 0x%lx", hres);
			return hres;
		}

		VARIANT vtProp4;
		VariantInit(&vtProp4);
		vtProp4.vt = VT_BSTR;
		vtProp4.bstrVal = SysAllocString(sValue.c_str());
		hres = pInParams->Put(_bstr_t(L"sValue"), 0, &vtProp4, 0);
		if (FAILED(hres))
		{
			ILog("Failed to set sValue property. Error code = 0x%lx", hres);
			return hres;
		}

		// Execute the method
		hres = WMI.InvokeMethod(ClassName, MethodName, 0, NULL, pInParams, &pOutParams, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to execute CreateKey method. Error code = 0x%lx", hres);
			return hres;
		}

		if (SUCCEEDED(hres))
		{
			ILog("Successfully set string value %ls to %ls\nAt location: %ls\n", vtProp3.bstrVal, vtProp4.bstrVal, vtProp2.bstrVal);
		}

		VariantClear(&vtProp);
		VariantClear(&vtProp2);
		SysFreeString(ClassName);
		SysFreeString(MethodName);

		return hres;
	}

	UINT32 GETKEYSECURITY(_In_ UINT32 RootKey, _In_ std::wstring SubKey, _In_ UINT32 SecurityInformation, _Out_ std::wstring& Descriptor)
	{
		return 0;
	}

	BOOL InitializeStdRegProv()
	{

		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);

		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}
		
		BSTR ClassName = SysAllocString(L"StdRegProv");
		BSTR MethodName = SysAllocString(L"SetStringValue");
		HRESULT hres;

		hres = WMI.pSvc->GetObject(ClassName, 0, NULL, &pClass, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to get StdRegProv class object. Error code = 0x%lx\n", hres);
			return FALSE;
		}

		// Create instance of StdRegProv

		hres = pClass->SpawnInstance(0, &pClassInstance);
		if (FAILED(hres))
		{
			ILog("Failed to spawn StdRegProv instance. Error code = 0x%lx\n", hres);
			return FALSE;
		}

		return TRUE;
	}

private:
	WMIConnection& WMI;
	COMWrapper& COM;
	IWbemClassObject* pClass;
	IWbemClassObject* pClassInstance;
};
