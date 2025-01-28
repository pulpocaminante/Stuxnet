#pragma once
#include "ClassFactory.hpp"
#include "WMIConnection.hpp"
#include <fstream>
#include <mutex>
#include <thread>

static std::mutex fileMutex;

static HRESULT WINAPI FileWriteProc(WMIConnection& WMI, COMWrapper& ICom, std::wstring& sDriveName, std::wstring sFileName, std::wstring sFileContents)
{
	
}

static HRESULT WINAPI PutThreadProc(COMWrapper& ICom, std::wstring& sDriveName, std::wstring& sSection, ULONGLONG& index)
{
	// Lock the mutex
	std::lock_guard<std::mutex> lock(fileMutex);
	
	BOOL complete = FALSE;
	if(sSection.empty())
		return E_FAIL;
	
	// The loop ensures the thread will hang and wait for memory to become
	// available if `std::bad_alloc` is thrown
	while(!complete)
	{
		try
		{
			// Connect to namespace
			WMIConnection WMI(ICom);
			WMI.ConnectToNamespace("RVOM2OITC\\", TRUE);

			if (!WMI.bConnected)
			{
				ILog("Failed to connect to namespace\n");
				//std::this_thread::sleep_for(std::chrono::milliseconds(500));
				return E_FAIL; // Something is very wrong
			}
			
			// Definitions
			HRESULT hres = NULL;
			IWbemClassObject* pDrive = NULL;
			IWbemQualifierSet* pQualSet = NULL;

			// Key
			wchar_t key[32] = { 0 };
			wchar_t* pKey = key;
			swprintf_s(key, L"key%llu", index);

			// Value
			VARIANT v;
			VariantInit(&v);
			v.vt = VT_BSTR;

			if (WMI.IReady)
			{
				// Retrieve class 'sDriveName'
				hres = WMI.pSvc->GetObject(_bstr_t(sDriveName.c_str()), 0, NULL, &pDrive, NULL);
				
				if (FAILED(hres))
				{
					goto cleanup;
				}

				// Get the qualifier set for 'Filestore'
				hres = pDrive->GetPropertyQualifierSet(_bstr_t(L"Filestore"), (LPVOID**)&pQualSet);
				if (FAILED(hres))
				{
					goto cleanup;
				}

				// Create a new property
				v.bstrVal = SysAllocString(sSection.c_str());
				//hres = pDrive->Put(_bstr_t(sKey.c_str()), 0, &v, 0);
				pQualSet->Put(_bstr_t(pKey), &v, CIM_SINT32);
				
				if (FAILED(hres))
				{
					goto cleanup;
				}
				

				// Commit the changes
				hres = WMI.pSvc->PutClass(pDrive, WBEM_FLAG_CREATE_OR_UPDATE, NULL, NULL);

				if (FAILED(hres))
				{
					goto cleanup;
				}
				
				// Cleanup
			cleanup:
				complete = TRUE;
				if(pDrive)
					pDrive->Release();
				if(pQualSet)
					pQualSet->Release();
				VariantClear(&v);
				
				// Unlock the mutex
				//std::lock_guard<std::mutex> unlock(fileMutex);
				
				return hres;
			}
		}
		// Catch std::bad_alloc
		catch (std::bad_alloc& e)
		{
			ILog("Out of memory, thread is waiting...\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			continue;
		}
		// Catch anything else
		catch (...)
		{
			ILog("Unknown exception caught\n");
			return E_FAIL;
		}
	}
}

class WMIFSInterface
{
public:
	BOOL IReady;
	
	

	WMIFSInterface(COMWrapper& com_wrapper, WMIConnection& wmi_connection, ClassFactory& class_factory) :
		ICom(com_wrapper),
		WMI(wmi_connection),
		IFactory(class_factory),
		IReady(FALSE)
	{
		if (ICom.IReady && WMI.IReady)
			IReady = TRUE;
	}

	HRESULT CreateDrive(
		_In_	 std::wstring  sDriveName,
		_In_opt_ std::wstring* sFileContent)
	{
		HRESULT hres = IFactory.CreateWMIFSClass(sDriveName);
		return hres;
	}

	HRESULT DeleteDrive(
		_In_ std::wstring sDriveName)
	{
		HRESULT hres = IFactory.DeleteWMIFSClass(sDriveName);
		return hres;
	}

	HRESULT InsertSection(
		_In_ INT32& index,
		_In_ std::wstring& sSectionData,
		_In_ IWbemClassObject*& pDrive)
	{
		VARIANT vSectionData;
		VariantInit(&vSectionData);
		vSectionData.vt = VT_BSTR;
		vSectionData.bstrVal = SysAllocString((OLECHAR*)sSectionData.c_str());
		HRESULT hres = NULL;
		
		std::wstring sIndex = L"sec" + std::to_wstring(index);
		
		pDrive->Put(_bstr_t(sIndex.c_str()), 0, &vSectionData, CIM_STRING);

		if (FAILED(hres))
		{
			ILog("Failed to insert section. Error code = 0x%X \n", hres);
		}
		
		SysFreeString(vSectionData.bstrVal);
		vSectionData.bstrVal = NULL;
		VariantClear(&vSectionData);
		return hres;
	}

	HRESULT CreateDriveInstance(
		_In_ std::wstring sDriveName
	)
	{
		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);

		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}

		// Retrieve the class definition
		IWbemClassObject* pDrive = NULL;
		IWbemClassObject* pInstance = NULL;
		IWbemQualifierSet* pQualSet = NULL;
		VARIANT vFileSize;
		VariantInit(&vFileSize);
		vFileSize.vt = VT_I4;
		
		HRESULT hres = WMI.pSvc->GetObject(_bstr_t(sDriveName.c_str()), 0, NULL, &pDrive, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to retrieve class definition. Error code = 0x%X \n", hres);
			goto cleanup;
		}
		
		// Get the qualifier set for 'Filestore'
		hres = pDrive->GetPropertyQualifierSet(_bstr_t(L"Filestore"), (LPVOID**)&pQualSet);
		if (FAILED(hres))
		{
			ILog("Failed to retrieve qualifier set. Error code = 0x%X \n", hres);
			goto cleanup;
		}

		// Check the filesize
		hres = pQualSet->Get(_bstr_t(L"FileSize"), 0, &vFileSize, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to retrieve filesize. Error code = 0x%X \n", hres);
			goto cleanup;
		}

		if (vFileSize.lVal != 0)
		{
			// Create a class instance
			hres = pDrive->SpawnInstance(0, &pInstance);
			if (FAILED(hres))
			{
				ILog("Failed to retrieve class instance. Error code = 0x%X \n", hres);
				goto cleanup;
			}

			// Save the class instance
			hres = WMI.pSvc->PutInstance(pInstance, WBEM_FLAG_CREATE_OR_UPDATE, NULL, NULL);

			if (FAILED(hres))
			{
				ILog("Failed to save class instance. Error code = 0x%X \n", hres);
				goto cleanup;
			}
		}
		
	cleanup:
		if (pDrive)
			pDrive->Release();
		if (pInstance)
			pInstance->Release();
		if (pQualSet)
			pQualSet->Release();
		VariantClear(&vFileSize);
		return hres;
	}
	
	HRESULT WriteFile(
		_In_ std::wstring  sDriveName,
		_In_ std::wstring  sFileName,
		_In_ std::wstring& sFileContents)
	{

		HRESULT hres = NULL;

		// ------------------------------------------------------------------
		// Step 1. Connect to namespace -------------------------------------
		
		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);

		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}

		if (sFileContents.empty() || sFileContents.size() < 260)
		{
			ILog("Received an invalid file\n");
			return E_FAIL;
		}

		// ------------------------------------------------------------------
		// Step 2. Check if the specified drive exists ----------------------
		
		hres = IFactory.CheckClassExists(sDriveName);
		
		if (FAILED(hres))
		{
			ILog("Specified drive doesn't exist. Error code = 0x%X \n", hres);
			return hres;
		}

		IWbemClassObject* pDrive = NULL;
		VARIANT vVariant;
		VariantInit(&vVariant);

		// -----------------------------------------------------------------------------
		// Step 3. Retrieve the drive's class definition and Filestore qualifier set ---
		
		// Open the class for writing by calling the pSvc->GetObject method with the strObjectPath parameter set to the name of the class.
		vVariant.vt = VT_BSTR;
		vVariant.bstrVal = SysAllocString(sDriveName.c_str());
		hres = WMI.pSvc->GetObject(
			vVariant.bstrVal,
			0,
			0,
			&pDrive,
			NULL
		);

		if (FAILED(hres))
		{
			ILog("Failed to retrieve class definition. Error code = 0x%X \n", hres);
			return hres;
		}

		VariantClear(&vVariant);
		
		// Get the qualifier set for the class.
		BSTR KeyProp = SysAllocString(L"Filestore");

		IWbemQualifierSet* pQual = NULL;
		hres = pDrive->GetPropertyQualifierSet(KeyProp, (LPVOID**)&pQual);

		if (FAILED(hres))
		{
			ILog("Failed to get qualifier set. Error code = 0x%X \n", hres);
			return hres;
		}
		
		SysFreeString(KeyProp);


		// ------------------------------------------------------------------
		// Step 4. Prepare for writing --------------------------------------
		
		// Determine if file is too large
		size_t maxSections = 500;
		size_t maxSectionSize = 18000;
		size_t dataSize = sFileContents.length(); // string size in characters
		size_t minimumSections = dataSize / maxSectionSize;

		if(minimumSections > maxSections*maxSectionSize)
		{
			ILog("File too large.\n");
			return hres;
		}

		// Determine size of each section
		size_t sectionSize = (sFileContents.length() / maxSections); // characters per key:value pair
		size_t sectionsNeeded = maxSections;

		// Determine size of the last section
		int lastSectionSize = dataSize % sectionSize;
		printf("Need %ld sections. Last section size is %ld \n", sectionsNeeded, lastSectionSize);
		if (lastSectionSize == 0) lastSectionSize = sectionSize;



		// ------------------------------------------------------------------
		// Step 5. Write the file -------------------------------------------
		
		// Definitions
		
		// Key
		std::wstring key;

		// Value
		VARIANT v;
		VariantInit(&v);
		v.vt = VT_BSTR;

		// Subsection key
		std::wstring subkey;

		// Subsection value
		VARIANT q;
		VariantInit(&q);
		q.vt = VT_BSTR;

		// Section definitions
		size_t sectionIndex = 0;
		std::wstring sSection;

		// Qualifier set
		IWbemQualifierSet* pSubQual;

		// Save the bare class definition by calling the pSvc->PutClass method
		WMI.pSvc->PutClass(pDrive, WBEM_FLAG_CREATE_OR_UPDATE, NULL, NULL);

		// Release the class and qualifier set
		pQual->Release();
		pDrive->Release();
		pDrive = NULL;
		pQual = NULL;
		
		// Write the file (multithreaded)
		for (int i = 0; i <= sectionsNeeded; i++)
		{
			// Key name is formatted "key0", "key1", "key2", ...
			key = L"key" + std::to_wstring(i);
			
			// If its the last section we need to write, we need to use the last section size
			if (i == sectionsNeeded)
				sSection = sFileContents.substr(i * sectionSize, lastSectionSize);
			else
				sSection = sFileContents.substr(i * sectionSize, sectionSize);

			//ILog("Writing %ls section %ld \n", sSection.c_str(), sectionIndex);
			v.bstrVal = SysAllocString(sSection.c_str());
			
			// Write a section
			while (true)
			{
				try
				{
					std::thread t1(&PutThreadProc, std::ref(ICom), std::ref(sDriveName), std::ref(sSection), std::ref(sectionIndex));
					// Wait for the section to write
					t1.join();
				}
				// catch bad_alloc
				catch (std::bad_alloc& ba)
				{
					ILog("bad_alloc caught: %s\n", ba.what());
					continue;
				}
				break;
			}
			
			// Cleanup
			SysFreeString(q.bstrVal);
			SysFreeString(v.bstrVal);
			
			sectionIndex++;
		}

		ILog("Wrote %d sections\n ", sectionIndex);
		

		// ------------------------------------------------------------------
		// Step 6. Update the class with file information -------------------
		
		if (WMI.pSvc == NULL)
		{
			ILog("Stack corrupted\n");
			return E_FAIL;
		}

		// Retrieve the updated class
		hres = WMI.pSvc->GetObject(
			vVariant.bstrVal,
			0,
			0,
			&pDrive,
			NULL
		);

		// Retrieve the Filestore qualifier set for the class

		KeyProp = SysAllocString(L"Filestore");
		hres = pDrive->GetPropertyQualifierSet(KeyProp, (LPVOID**)&pQual);
		SysFreeString(KeyProp);
		
		if (FAILED(hres))
		{
			ILog("Failed to get qualifier set. Error code = 0x%X \n", hres);
			return hres;
		}
		
		// Write the file data as key:value pairs under the qualifier "Filestore"

		// Write the number of sections the file contains
		VARIANT vFileSize;
		VariantInit(&vFileSize);
		vFileSize.vt = VT_I4;
		vFileSize.lVal = sectionIndex;
		hres = pQual->Put(_bstr_t(L"Filesize"), &vFileSize, CIM_SINT32); // Note: flag 3 (CIM_SINT32) causes the qualifier to be
																		 // propogated directly to child instances without a reference
																		 // We need to do this every time
		if (FAILED(hres))
		{
			ILog("Failed to write filesize. Error code = 0x%X \n", hres);
			return hres;
		}
		VariantClear(&vFileSize);

		// Write the file creation time
		VARIANT vCreationTime;
		VariantInit(&vCreationTime);
		vCreationTime.vt = VT_BSTR;
		vCreationTime.bstrVal = SysAllocString(L"2020-01-01 00:00:00");

		hres = pQual->Put(_bstr_t(L"Created"), &vCreationTime, CIM_SINT32);
		VariantClear(&vCreationTime);

		// Write the filename
		VARIANT vFileName;
		VariantInit(&vFileName);
		vFileName.vt = VT_BSTR;
		vFileName.bstrVal = SysAllocString(sFileName.c_str());
		
		hres = pQual->Put(_bstr_t(L"Filename"), &vFileName, CIM_SINT32);
		VariantClear(&vFileName);
		
		// Save the class changes
		hres = WMI.pSvc->PutClass(pDrive, WBEM_FLAG_CREATE_OR_UPDATE, NULL, NULL);
		if (FAILED(hres))
		{
			ILog("Failed to save class changes. Error code = 0x%X \n", hres);
			return hres;
		}

		// Release the class and qualifier set
		pDrive->Release();
		pQual->Release();
		pDrive = NULL;
		pQual = NULL;
		
		// Create a new instance of the completed class
		CreateDriveInstance(sDriveName); // Needs to be instanced or powershell can't find the data

		// Cleanup
		if (pDrive)
			pDrive->Release();
		if (pQual)
			pQual->Release();
		return hres;
	}

	HRESULT ReadFile(
		_In_  std::wstring sDriveName,
		_In_  std::wstring sFileName,
		_Out_ std::wstring& sFileContents)
	{

		// ------------------------------------------------------------------
		// Step 1. Connect to the root\cimv2 namespace ----------------------
		
		if (wcscmp(this->WMI.currentNamespace, L"RVOM2OITC\\") != 0)
			WMI.ConnectToNamespace("RVOM2OITC\\", 0);

		if (!WMI.bConnected)
		{
			ILog("Failed to connect to namespace\n");
			return E_FAIL; // Something is very wrong
		}
		
		HRESULT hres = NULL;
		IWbemClassObject* pDrive = NULL;
		IWbemContext* pCtx = NULL;
		VARIANT vVariant;
		VariantInit(&vVariant);

		// ------------------------------------------------------------------
		// Step 2. Get the only instance of the class -----------------------
		
		// Note: this is not strictly necessary, as the class definition also
		// contains the file information. However, when we extract the file
		// using powershell we need to be using the instance anyway, so this
		// helps us to ensure that both the class and the instance have
		// been created correctly.
		
		// Retrieve all instances of the class using IWbemEnumerator
		IEnumWbemClassObject* pEnumerator = NULL;
		hres = WMI.pSvc->CreateInstanceEnum(
			_bstr_t(sDriveName.c_str()),
			0,
			0,
			&pEnumerator);
		
		if (FAILED(hres))
		{
			ILog("Failed to create instance enumerator. Error code = 0x%X \n", hres);
			return hres;
		}
		
		// Get the first instance
		ULONG uReturn = 0;
		hres = pEnumerator->Next(0, 1, &pDrive, &uReturn);
		if (FAILED(hres))
		{
			ILog("Failed to get first instance. Error code = 0x%X \n", hres);
			pEnumerator->Release();
			return hres;
		}
		pEnumerator->Release();

		if (pDrive == NULL)
		{
			ILog("Failed to populate pDrive. Error code = 0x%X\n", hres);
			ILog("Extended error information: %d\n", GetLastError());
			return E_FAIL;
		}
		// ------------------------------------------------------------------
		// Step 3. Retrieve the file information ----------------------------
		
		// Get the "Filestore" qualifier set for the class
		BSTR KeyProp = SysAllocString(L"Filestore");
		IWbemQualifierSet* pQual = NULL;
		hres = pDrive->GetPropertyQualifierSet(KeyProp, (LPVOID**)&pQual);
		
		if (FAILED(hres))
		{
			ILog("Failed to get qualifier set. Error code = 0x%X \n", hres);
			pDrive->Release();
			return hres;
		}

		// Retrieve the number of sections from the "Filesize" key:value pair
		VARIANT vFileSize;
		VariantInit(&vFileSize);
		vFileSize.vt = VT_I4;
		hres = pQual->Get(_bstr_t(L"Filesize"), NULL, &vFileSize, NULL);
		
		CIMTYPE ct = CIM_STRING;

		if (vFileSize.lVal == 0)
		{
			ILog("Filesize is 0. Error code = 0x%X \n", hres);
			pDrive->Release();
			return hres;
		}
		

		// ------------------------------------------------------------------
		// Step 4. Read the file --------------------------------------------
		
		
		// Definitions
		INT32 index = 0;
		INT32 sectionsNeeded = vFileSize.lVal;
		wchar_t key[32] = { 0 };
		wchar_t* pKey = key;

		// Retrieve each section
		while (index < sectionsNeeded)
		{
			// Get section
			swprintf_s(key, L"key%llu", index);
			BSTR bstrIndex = SysAllocString(_bstr_t(pKey));
			hres = pQual->Get(bstrIndex, 0, &vVariant, 0);
			
			// If getting the next section failed, we've encountered
			// some kind of error or the section count is incorrect
			if (FAILED(hres))
				break;
			
			// Append section to file contents
			INT32 sectionSize = wcslen(vVariant.bstrVal);
			sFileContents.append(vVariant.bstrVal, sectionSize);
			
			SysFreeString(bstrIndex);
			
			// Increment index
			index++;
		}

		ILog("Read %d sections\n", index);
		
		// Cleanup
		VariantClear(&vVariant);
		VariantClear(&vFileSize);
		SysFreeString(KeyProp);
		if(pDrive)
			pDrive->Release();
		if (pQual)
			pQual->Release();
		
		if(sFileContents.empty())
			return E_FAIL;
		
		return hres;
		
	}

private:
	COMWrapper& ICom;
	WMIConnection& WMI;
	ClassFactory& IFactory;
};