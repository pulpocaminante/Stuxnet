#include "ExportInterface.hpp"

class KernelbaseWrapper {

public:
	BOOL IReady = FALSE;

	KernelbaseWrapper() :
		lpOpenProcessToken(IFind.LoadAndFindSingleExport("kseaerb.nldlel", "OepcsneosenrTkPo")),
		lpGetTokenInformation(IFind.LoadAndFindSingleExport("kseaerb.nldlel", "GIienntotefanTkomor"))
	{
		if (lpOpenProcessToken != nullptr && lpGetTokenInformation != nullptr)
			IReady = TRUE;
	}

	BOOL WINAPI OpenProcessToken(
		_In_ HANDLE ProcessHandle,
		_In_ DWORD DesiredAccess,
		_Outptr_ PHANDLE TokenHandle
	)
	{
		return _SafeOpenProcessToken(ProcessHandle, DesiredAccess, TokenHandle);
	}
	
	BOOL WINAPI GetTokenInformation(
		_In_ HANDLE TokenHandle,
		_In_ TOKEN_INFORMATION_CLASS TokenInformationClass,
		_Out_writes_bytes_to_opt_(TokenInformationLength, *ReturnLength) LPVOID TokenInformation,
		_In_ DWORD TokenInformationLength,
		_Out_ PDWORD ReturnLength
	)
	{
		return _SafeGetTokenInformation(TokenHandle, TokenInformationClass, TokenInformation, TokenInformationLength, ReturnLength);
	}
	
private:
	IExport IFind;
	LPVOID lpOpenProcessToken = nullptr;
	LPVOID lpGetTokenInformation = nullptr;

	LPVOID slpOpenProcessToken = (LPVOID)((uintptr_t)lpOpenProcessToken + 0x0);
	LPVOID slpGetTokenInformation = (LPVOID)((uintptr_t)lpGetTokenInformation + 0x0);

	BOOL(WINAPI* _SafeOpenProcessToken)(
		_In_ HANDLE ProcessHandle,
		_In_ DWORD DesiredAccess,
		_Outptr_ PHANDLE TokenHandle
		)
		=
		(BOOL(WINAPI*)(
			_In_ HANDLE ProcessHandle,
			_In_ DWORD DesiredAccess,
			_Outptr_ PHANDLE TokenHandle
			))slpOpenProcessToken;
	
	BOOL(WINAPI* _SafeGetTokenInformation)(
		_In_ HANDLE TokenHandle,
		_In_ TOKEN_INFORMATION_CLASS TokenInformationClass,
		_Out_writes_bytes_to_opt_(TokenInformationLength, *ReturnLength) LPVOID TokenInformation,
		_In_ DWORD TokenInformationLength,
		_Out_ PDWORD ReturnLength
		)
		=
		(BOOL(WINAPI*)(
			_In_ HANDLE TokenHandle,
			_In_ TOKEN_INFORMATION_CLASS TokenInformationClass,
			_Out_writes_bytes_to_opt_(TokenInformationLength, *ReturnLength) LPVOID TokenInformation,
			_In_ DWORD TokenInformationLength,
			_Out_ PDWORD ReturnLength
			))slpGetTokenInformation;
	
};