#pragma once
#include "WMIConnection.hpp"

class ClassFactory
{
public:
	BOOL IReady;
	
	ClassFactory(COMWrapper& com_wrapper, WMIConnection& wmi_connection) :
		ICom(com_wrapper),
		WMI(wmi_connection),
		IReady(FALSE)
	{
		if (ICom.IReady && WMI.IReady)
			IReady = TRUE;
	}

	HRESULT CreateWMIFSClass(
		_In_ std::wstring sClassName
	)
	{
		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);
		else
			ILog("We're connected\n");

		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}
		
		HRESULT hres = NULL;
		IWbemClassObject* pClass = NULL;
		IWbemContext* pCtx = NULL;
		VARIANT vVariant;
		VariantInit(&vVariant);

		// Check if class already exists by calling GetObject
		hres = CheckClassExists(sClassName);
		
		if (SUCCEEDED(hres))
		{
			ILog("Class already exists, deleting it\n");
			// Enumerate all instances of the class
			IEnumWbemClassObject* pEnumerator = NULL;
			hres = WMI.pSvc->CreateInstanceEnum(
				_bstr_t(sClassName.c_str()),
				WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
				NULL,
				&pEnumerator
			);
			
			// If instances exist, delete all of them
			if (SUCCEEDED(hres))
			{
				IWbemClassObject* pInstance = NULL;
				ULONG uReturn = 0;
				while (pEnumerator)
				{
					hres = pEnumerator->Next(WBEM_INFINITE, 1, &pInstance, &uReturn);
					if (0 == uReturn)
						break;

					// Get object path for the instance
					VARIANT vPath;
					VariantInit(&vPath);
					hres = pInstance->Get(L"__RELPATH", 0, &vPath, 0, 0);
					
					if (SUCCEEDED(hres))
					{
						// Delete the instance
						hres = WMI.pSvc->DeleteInstance(vPath.bstrVal, 0, NULL, NULL);
					}
					VariantClear(&vPath);
					
					pInstance->Release();
				}
				pEnumerator->Release();
			}

			// Delete the class
			hres = WMI.pSvc->DeleteClass(_bstr_t(sClassName.c_str()), 0, NULL, NULL);
			
			pClass = NULL;
		}
		else
		{
			pClass = NULL;
		}
		
		// Retrieve a definition for the new class by calling the pSvc->GetObject method with the strObjectPath parameter set to a null value.

		hres = WMI.pSvc->GetObject(
			0,
			0,
			pCtx,
			&pClass,
			NULL
		);
		
		if (FAILED(hres))
		{
			ILog("Failed to create new class definition. Error code = 0x%X \n", hres);
			return hres;
		}

		// Establish a name for the class by setting the __CLASS system property with a call to the pClass Put method.
		vVariant.vt = VT_BSTR;
		vVariant.bstrVal = SysAllocString(sClassName.c_str());
		hres = pClass->Put(
			_bstr_t("__CLASS"),
			0,
			&vVariant,
			0
		);
		
		if (FAILED(hres))
		{
			ILog("Failed to set class name. Error code = 0x%X \n", hres);
			return hres;
		}
		
		// The following code example describes how to create the Index property, which is labeled as a key property in Step 4.

		 BSTR KeyProp = SysAllocString(L"Index");
		 
		hres = pClass->Put(
			KeyProp,
			0,
			NULL,
			CIM_STRING);

		//Attach the Key standard qualifier to the key property by first calling the IWbemClassObject::GetPropertyQualifierSet method and then the IWbemQualifierSet::Put method.

		IWbemQualifierSet* pQual = NULL;
		IWbemQualifierSet* pQual2 = NULL;
		IWbemQualifierSet* pQual3 = NULL;
		pClass->GetPropertyQualifierSet(KeyProp, (LPVOID**) & pQual);
		SysFreeString(KeyProp);

		V_VT(&vVariant) = VT_BOOL;
		V_BOOL(&vVariant) = VARIANT_TRUE;
		BSTR Key = SysAllocString(L"Key");

		pQual->Put(Key, &vVariant, 0);   // Flavors not required for Key 
		SysFreeString(Key);
		VariantClear(&vVariant);

		// Add additional properties to the class
		// Filestore
		
		hres = pClass->Put(
			_bstr_t("Filestore"),
			0,
			NULL,
			CIM_STRING);

		if (FAILED(hres))
		{
			ILog("Failed to add Filestore property. Error code = 0x%X \n", hres);
			goto cleanup;
		}

		// Add key qualifier set to Filestore property
		hres = pClass->GetPropertyQualifierSet(_bstr_t("Filestore"), (LPVOID**)&pQual2);
		
		if (FAILED(hres))
		{
			ILog("Failed to get Filestore property qualifier set. Error code = 0x%X \n", hres);
			goto cleanup;
		}
		
		V_VT(&vVariant) = VT_BOOL;
		V_BOOL(&vVariant) = VARIANT_TRUE;
		Key = SysAllocString(L"Key");
		
		hres = pQual2->Put(Key, &vVariant, 0);   // Flavors not required for Key
		SysFreeString(Key);
		
		if (FAILED(hres))
		{
			ILog("Failed to add Key qualifier set to Filestore property. Error code = 0x%X \n", hres);
			goto cleanup;
		}
		
		// Add MaxLen qualifier to Filestore as CIM_SINT32
		V_VT(&vVariant) = VT_I4;
		V_I4(&vVariant) = 2147483647 - 1; // INT32 max value
		hres = pQual2->Put(_bstr_t(L"MaxLen"), &vVariant, CIM_SINT32);
		VariantClear(&vVariant);

		if (FAILED(hres))
		{
			ILog("Failed to add MaxLen qualifier to Filestore property. Error code = 0x%X \n", hres);
			goto cleanup;
		}
		
		// Register the new class using pSvc->PutClass
		
		hres = WMI.pSvc->PutClass(pClass, 0, pCtx, NULL);

		if (FAILED(hres))
		{
			ILog("Failed to register class. Error code = 0x%X \n", hres);
			goto cleanup;
		}

		// Cleanup
	cleanup:
		if(pQual)
			pQual->Release();
		if(pQual2)
			pQual2->Release();
		if (pQual3)
			pQual3->Release();
		if(pClass)
			pClass->Release();
		VariantClear(&vVariant);
		
		return hres;
	}

	HRESULT DeleteWMIFSClass(
		_In_ std::wstring sClassName)
	{
		HRESULT hres = NULL;
		VARIANT vVariant;
		VariantInit(&vVariant);
		vVariant.vt = VT_BSTR;
		vVariant.bstrVal = SysAllocString(sClassName.c_str());
		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);

		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}
		
		IWbemClassObject* pClass = NULL;

		hres = CheckClassExists(sClassName);

		if (FAILED(hres))
		{
			ILog("Class does not exist. Error code = 0x%X \n", hres);
			goto cleanup;
		}

		// Delete the class using pSvc->DeleteClass
		hres = WMI.pSvc->DeleteClass(vVariant.bstrVal, 0, NULL, NULL);
		
		if (FAILED(hres))
		{
			ILog("Failed to delete class. Error code = 0x%X \n", hres);
			goto cleanup;
		}
		
		// Cleanup
		cleanup:
		VariantClear(&vVariant);
		if(pClass)
			pClass->Release();

		return hres;
	}

	HRESULT CheckClassExists(
		_In_ std::wstring sClassName
	)
	{
		HRESULT hres = NULL;
		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);


		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}
		
		IWbemClassObject* pClass = NULL;

		// Retrieve class definition for sClassName
		VARIANT vVariant;
		VariantInit(&vVariant);
		V_VT(&vVariant) = VT_BSTR;
		V_BSTR(&vVariant) = SysAllocString(sClassName.c_str());

		hres = WMI.pSvc->GetObject(
			vVariant.bstrVal,
			0,
			NULL,
			&pClass,
			NULL);

		if (FAILED(hres))
		{
			// Class doesn't exist
			goto cleanup;
		}
		
		// Cleanup
	cleanup:
		if(pClass)
			pClass->Release();
		VariantClear(&vVariant);
		
		return hres;
	}

private:
	COMWrapper& ICom;
	WMIConnection& WMI;
};