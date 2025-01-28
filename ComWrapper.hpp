#pragma once
#include <phnt_windows.h>
#include <phnt.h>
#include <stdio.h>
#include <time.h>
#include <SetupAPI.h>
#include <strsafe.h>
#include <iostream>
#include <vector>
#include <sddl.h>
#include <shellapi.h>
#include "config.h"
#include "util.h"
#include <comdef.h>
#include "wmi_defs.h"
#include "ExportInterface.hpp"

class COMWrapper
{
public:
	BOOL IReady;
	BOOL bInitialized;
	WCHAR namespaceBuf[256];
	LPWSTR currentNamespace;

	COMWrapper() :
		lpCoInitializeEx(IFind.LoadAndFindSingleExport("cdo.lmelbsa", "CloaiIizntexiE")),
		lpCoCreateInstance(IFind.LoadAndFindSingleExport("cdo.lmelbsa", "CIoeneCtscratnea")),
		lpCoUninitialize(IFind.LoadAndFindSingleExport("cdo.lmelbsa", "CiotaUilnnieiz")),
		lpCoSetProxyBlanket(IFind.LoadAndFindSingleExport("cdo.lmelbsa", "CxtooyeSrBkePlnta")),
		lpCoInitializeSecurity(IFind.LoadAndFindSingleExport("cdo.lmelbsa", "ClroaiuiIizctnteeyiS")),
		lpCoGetClassObject(IFind.LoadAndFindSingleExport("cdo.lmelbsa", "CsoastGlOceCbetj")),
		pEnumerator(NULL),
		pSvc(NULL),
		pLoc(NULL),
		hres(NULL),
		bInitialized(FALSE),
		IReady(FALSE),
		currentNamespace(namespaceBuf)
	{
		namespaceBuf[0] = '\0';
		if (lpCoInitializeEx != nullptr && lpCoCreateInstance != nullptr && lpCoUninitialize != nullptr && lpCoSetProxyBlanket != nullptr && lpCoInitializeSecurity != nullptr && lpCoGetClassObject != nullptr)
			this->IReady = TRUE;
	}

	~COMWrapper()
	{
		if (pSvc != NULL)
			pSvc->Release();
		if (pLoc != NULL)
			pLoc->Release();
	}

	HRESULT __stdcall CoInitializeEx(
		_In_opt_ LPVOID pvReserved,
		_In_ DWORD dwCoInit
	)
	{
		this->bInitialized = TRUE;
		return _SafeCoInitializeEx(pvReserved, dwCoInit);
	}

	HRESULT __stdcall CoCreateInstance(
		_In_ REFCLSID rclsid,
		_In_opt_ LPUNKNOWN pUnkOuter,
		_In_ DWORD dwClsContext,
		_In_ REFIID riid,
		_COM_Outptr_ LPVOID* ppv
	)
	{
		return _SafeCoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
	}

	void __stdcall CoUninitialize()
	{
		_SafeCoUninitialize();
	}

	HRESULT __stdcall CoSetProxyBlanket(
		_In_ IUnknown* pProxy,
		_In_ DWORD dwAuthnSvc,
		_In_ DWORD dwAuthzSvc,
		_In_opt_ OLECHAR* pServerPrincName,
		_In_ DWORD dwAuthnLevel,
		_In_ DWORD dwImpLevel,
		_In_opt_ RPC_AUTH_IDENTITY_HANDLE pAuthInfo,
		_In_ DWORD dwCapabilities
	)
	{
		return _SafeCoSetProxyBlanket(pProxy, dwAuthnSvc, dwAuthzSvc, pServerPrincName, dwAuthnLevel, dwImpLevel, pAuthInfo, dwCapabilities);
	}

	HRESULT __stdcall CoInitializeSecurity(
		_In_opt_ PSECURITY_DESCRIPTOR pSecDesc,
		_In_ LONG cAuthSvc,
		_In_reads_opt_(cAuthSvc) SOLE_AUTHENTICATION_SERVICE* asAuthSvc,
		_In_opt_ void* pReserved1,
		_In_ DWORD dwAuthnLevel,
		_In_ DWORD dwImpLevel,
		_In_opt_ void* pAuthList,
		_In_ DWORD dwCapabilities,
		_In_opt_ void* pReserved3
	)
	{
		return _SafeCoInitializeSecurity(pSecDesc, cAuthSvc, asAuthSvc, pReserved1, dwAuthnLevel, dwImpLevel, pAuthList, dwCapabilities, pReserved3);
	}

	HRESULT __stdcall CoGetClassObject(
		_In_ REFCLSID rclsid,
		_In_ DWORD dwClsContext,
		_In_opt_ LPVOID pvReserved,
		_In_ REFIID riid,
		_Outptr_ LPVOID  FAR* ppv
	)
	{
		return _SafeCoGetClassObject(rclsid, dwClsContext, pvReserved, riid, ppv);
	}


private:
	IExport IFind;
	LPVOID lpCoInitializeEx = nullptr;
	LPVOID lpCoCreateInstance = nullptr;
	LPVOID lpCoUninitialize = nullptr;
	LPVOID lpCoSetProxyBlanket = nullptr;
	LPVOID lpCoInitializeSecurity = nullptr;
	LPVOID lpCoGetClassObject = nullptr;

	LPVOID slpCoInitializeEx = (LPVOID)((uintptr_t)lpCoInitializeEx + 0x0);
	LPVOID slpCoCreateInstance = (LPVOID)((uintptr_t)lpCoCreateInstance + 0x0);
	LPVOID slpCoUninitialize = (LPVOID)((uintptr_t)lpCoUninitialize + 0x0);
	LPVOID slpCoSetProxyBlanket = (LPVOID)((uintptr_t)lpCoSetProxyBlanket + 0x0);
	LPVOID slpCoInitializeSecurity = (LPVOID)((uintptr_t)lpCoInitializeSecurity + 0x0);
	LPVOID slpCoGetClassObject = (LPVOID)((uintptr_t)lpCoGetClassObject + 0x0);

	HRESULT(__stdcall* _SafeCoInitializeEx)(
		_In_opt_ LPVOID pvReserved,
		_In_ DWORD dwCoInit
		) =
		(HRESULT(__stdcall*)(_In_opt_ LPVOID pvReserved,
			_In_ DWORD dwCoInit
			))slpCoInitializeEx;

	HRESULT(__stdcall* _SafeCoCreateInstance)(
		_In_ REFCLSID rclsid,
		_In_opt_ LPVOID pUnkOuter,
		_In_ DWORD dwClsContext,
		_In_ REFIID riid,
		_COM_Outptr_ LPVOID* ppv
		) =
		(HRESULT(__stdcall*)(_In_ REFCLSID rclsid,
			_In_opt_ LPVOID pUnkOuter,
			_In_ DWORD dwClsContext,
			_In_ REFIID riid,
			_COM_Outptr_ LPVOID * ppv
			))slpCoCreateInstance;

	HRESULT(__stdcall* _SafeCoUninitialize)(
		VOID
		) =
		(HRESULT(__stdcall*)(VOID))slpCoUninitialize;

	HRESULT(__stdcall* _SafeCoSetProxyBlanket)(
		_In_ LPVOID pProxy,
		_In_ DWORD dwAuthnSvc,
		_In_ DWORD dwAuthzSvc,
		_In_opt_ LPWSTR pServerPrincName,
		_In_ DWORD dwAuthnLevel,
		_In_ DWORD dwImpLevel,
		_In_opt_ LPVOID pAuthInfo,
		_In_ DWORD dwCapabilities
		) =
		(HRESULT(__stdcall*)(_In_ LPVOID pProxy,
			_In_ DWORD dwAuthnSvc,
			_In_ DWORD dwAuthzSvc,
			_In_opt_ LPWSTR pServerPrincName,
			_In_ DWORD dwAuthnLevel,
			_In_ DWORD dwImpLevel,
			_In_opt_ LPVOID pAuthInfo,
			_In_ DWORD dwCapabilities
			))slpCoSetProxyBlanket;

	HRESULT(__stdcall* _SafeCoInitializeSecurity)(
		_In_opt_ PSECURITY_DESCRIPTOR pSecDesc,
		_In_ LONG cAuthSvc,
		_In_reads_opt_(cAuthSvc) SOLE_AUTHENTICATION_SERVICE* asAuthSvc,
		_In_opt_ void* pReserved1,
		_In_ DWORD dwAuthnLevel,
		_In_ DWORD dwImpLevel,
		_In_opt_ void* pAuthList,
		_In_ DWORD dwCapabilities,
		_In_opt_ void* pReserved3
		) =
		(HRESULT(__stdcall*)(
			_In_opt_ PSECURITY_DESCRIPTOR pSecDesc,
			_In_ LONG cAuthSvc,
			_In_reads_opt_(cAuthSvc) SOLE_AUTHENTICATION_SERVICE * asAuthSvc,
			_In_opt_ void* pReserved1,
			_In_ DWORD dwAuthnLevel,
			_In_ DWORD dwImpLevel,
			_In_opt_ void* pAuthList,
			_In_ DWORD dwCapabilities,
			_In_opt_ void* pReserved3
			))slpCoInitializeSecurity;

	HRESULT(__stdcall* _SafeCoGetClassObject)(
		_In_ REFCLSID rclsid,
		_In_ DWORD dwClsContext,
		_In_opt_ LPVOID pvReserved,
		_In_ REFIID riid,
		_Outptr_ LPVOID  FAR* ppv
		) =
		(HRESULT(__stdcall*)(
			_In_ REFCLSID rclsid,
			_In_ DWORD dwClsContext,
			_In_opt_ LPVOID pvReserved,
			_In_ REFIID riid,
			_Outptr_ LPVOID  FAR * ppv
			))slpCoGetClassObject;


	// WMI definitions

	IEnumWbemClassObject* pEnumerator;
	IWbemServices* pSvc;
	IWbemLocator* pLoc;
	HRESULT hres;
};