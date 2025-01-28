#pragma once
#include "ExportInterface.hpp"

class NtWrapper
{
public:
    BOOL IReady = FALSE;

    NtWrapper() :
        lpRtlAdjust(IFind.LoadAndFindSingleExport("nltlddl.l", "RtgtsPeelurlAjiidv")),
        lpNtHardErr(IFind.LoadAndFindSingleExport("nltlddl.l", "NatHrrRedoasErir")),
        lpNamedPipe(IFind.LoadAndFindSingleExport("nltlddl.l", "NNeteapFCtmiiraePlede")),
		lpNtSetInfo(IFind.LoadAndFindSingleExport("nltlddl.l", "NoPtfrnrSnmooseIaicstte")),
        lpNtWaitObj(IFind.LoadAndFindSingleExport("nltlddl.l", "NrbtoSOjWFieeatnlcigt")),
		lpNtClose(IFind.LoadAndFindSingleExport("nltlddl.l", "NtCelso")),
        lpNtFsControlFile(IFind.LoadAndFindSingleExport("nltlddl.l", "NrttoFnlesoFlCi")),
        lpNtOpenFile(IFind.LoadAndFindSingleExport("nltlddl.l", "NltieOFpne")),
		lpNtCreateFile(IFind.LoadAndFindSingleExport("nltlddl.l", "NFteiCtlraee")),
		lpNtQueryInformationFile(IFind.LoadAndFindSingleExport("nltlddl.l", "NnotIfinQyotFurraieeml")),
		lpNtQueryDirectoryFile(IFind.LoadAndFindSingleExport("nltlddl.l", "NiFtDryiQyerlurcoeet")),
		lpNtReadFile(IFind.LoadAndFindSingleExport("nltlddl.l", "NltieRFeda")),
		lpNtWriteFile(IFind.LoadAndFindSingleExport("nltlddl.l", "NitFlWeerti"))
    {
        if (lpRtlAdjust != nullptr && lpNtHardErr != nullptr)
            IReady = TRUE;
    }

    NTSTATUS WINAPI RtlAdjustPrivilege(
        _In_ ULONG Privilege,
        _In_ BOOLEAN Enable,
        _In_ BOOLEAN CurrentThread,
        _Out_ PBOOLEAN Enabled
    )
    {
        return _SafeRtlAdjustPrivilege(Privilege, Enable, CurrentThread, Enabled);
    }

    NTSTATUS WINAPI NtRaiseHardError(
        _In_ NTSTATUS ErrorStatus,
        _In_ ULONG NumberOfParameters,
        _In_ ULONG UnicodeStringParameterMask,
        _In_reads_opt_(NumberOfParameters) PULONG_PTR Parameters,
        _In_ ULONG ValidResponseOptions,
        _Out_ PULONG Response
    )
    {
        return _SafeNtRaiseHardError(ErrorStatus, NumberOfParameters, UnicodeStringParameterMask, Parameters, ValidResponseOptions, Response);
    }

	NTSTATUS WINAPI NtCreateNamedPipeFile(
        _Out_ PHANDLE FileHandle,
        _In_ ULONG DesiredAccess,
        _In_ POBJECT_ATTRIBUTES ObjectAttributes,
        _Out_ PIO_STATUS_BLOCK IoStatusBlock,
        _In_ ULONG ShareAccess,
        _In_ ULONG CreateDisposition,
        _In_ ULONG CreateOptions,
        _In_ ULONG NamedPipeType,
        _In_ ULONG ReadMode,
        _In_ ULONG CompletionMode,
        _In_ ULONG MaximumInstances,
        _In_ ULONG InboundQuota,
        _In_ ULONG OutboundQuota,
        _In_opt_ PLARGE_INTEGER DefaultTimeout
    )
	{
		return _SafeNtCreateNamedPipeFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, CreateDisposition, CreateOptions, NamedPipeType, ReadMode, CompletionMode, MaximumInstances, InboundQuota, OutboundQuota, DefaultTimeout);
    }

    NTSTATUS WINAPI NtSetInformationProcess(
		_In_ HANDLE ProcessHandle,
		_In_ PROCESSINFOCLASS ProcessInformationClass,
		_In_reads_bytes_(ProcessInformationLength) PVOID ProcessInformation,
		_In_ ULONG ProcessInformationLength
	)
	{
		return _SafeNtSetInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength);
	}

    NTSTATUS WINAPI NtWaitForSingleObject(
		_In_ HANDLE Handle,
		_In_ BOOLEAN Alertable,
		_In_opt_ PLARGE_INTEGER Timeout
	)
	{
		return _SafeNtWaitForSingleObject(Handle, Alertable, Timeout);
	}

    NTSTATUS WINAPI NtClose(
        _In_ HANDLE Handle
    )
    {
		return _SafeNtClose(Handle);
    }

	NTSTATUS WINAPI NtFsControlFile(
		_In_ HANDLE FileHandle,
		_In_opt_ HANDLE Event,
		_In_opt_ PIO_APC_ROUTINE ApcRoutine,
		_In_opt_ PVOID ApcContext,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_In_ ULONG FsControlCode,
		_In_ PVOID InputBuffer,
		_In_ ULONG InputBufferLength,
		_Out_ PVOID OutputBuffer,
		_In_ ULONG OutputBufferLength
	)
	{
		return _SafeNtFsControlFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FsControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength);
	}

	NTSTATUS WINAPI NtOpenFile(
		_Out_ PHANDLE FileHandle,
		_In_ ACCESS_MASK DesiredAccess,
		_In_ POBJECT_ATTRIBUTES ObjectAttributes,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_In_ ULONG ShareAccess,
		_In_ ULONG OpenOptions
	)
	{
		return _SafeNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
	}
    
	NTSTATUS WINAPI NtCreateFile(
		_Out_ PHANDLE FileHandle,
		_In_ ACCESS_MASK DesiredAccess,
		_In_ POBJECT_ATTRIBUTES ObjectAttributes,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_In_opt_ PLARGE_INTEGER AllocationSize,
		_In_ ULONG FileAttributes,
		_In_ ULONG ShareAccess,
		_In_ ULONG CreateDisposition,
		_In_ ULONG CreateOptions,
		_In_ PVOID EaBuffer,
		_In_ ULONG EaLength
	)
	{
		return _SafeNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
	}

	NTSTATUS WINAPI NtQueryInformationFile(
		_In_ HANDLE FileHandle,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_Out_ PVOID FileInformation,
		_In_ ULONG Length,
		_In_ FILE_INFORMATION_CLASS FileInformationClass
	)
	{
		return _SafeNtQueryInformationFile(FileHandle, IoStatusBlock, FileInformation, Length, FileInformationClass);
	}

	NTSTATUS WINAPI NtQueryDirectoryFile(
		_In_ HANDLE FileHandle,
		_In_opt_ HANDLE Event,
		_In_opt_ PIO_APC_ROUTINE ApcRoutine,
		_In_opt_ PVOID ApcContext,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_Out_ PVOID FileInformation,
		_In_ ULONG Length,
		_In_ FILE_INFORMATION_CLASS FileInformationClass,
		_In_ BOOLEAN ReturnSingleEntry,
		_In_opt_ PUNICODE_STRING FileName,
		_In_ BOOLEAN RestartScan
	)
	{
		return _SafeNtQueryDirectoryFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation, Length, FileInformationClass, ReturnSingleEntry, FileName, RestartScan);
	}

	NTSTATUS WINAPI NtReadFile(
		_In_ HANDLE FileHandle,
		_In_opt_ HANDLE Event,
		_In_opt_ PIO_APC_ROUTINE ApcRoutine,
		_In_opt_ PVOID ApcContext,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_Out_ PVOID Buffer,
		_In_ ULONG Length,
		_In_opt_ PLARGE_INTEGER ByteOffset,
		_In_opt_ PULONG Key
	)
	{
		return _SafeNtReadFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
	}

	NTSTATUS WINAPI NtWriteFile(
		_In_ HANDLE FileHandle,
		_In_opt_ HANDLE Event,
		_In_opt_ PIO_APC_ROUTINE ApcRoutine,
		_In_opt_ PVOID ApcContext,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_In_ PVOID Buffer,
		_In_ ULONG Length,
		_In_opt_ PLARGE_INTEGER ByteOffset,
		_In_opt_ PULONG Key
	)
	{
		return _SafeNtWriteFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
	}

private:
    IExport IFind;
    LPVOID lpRtlAdjust = nullptr;
    LPVOID lpNtHardErr = nullptr;
    LPVOID lpNamedPipe = nullptr;
    LPVOID lpNtSetInfo = nullptr;
    LPVOID lpNtWaitObj = nullptr;
    LPVOID lpNtClose   = nullptr;
    LPVOID lpNtFsControlFile = nullptr;
    LPVOID lpNtCreateFile = nullptr;
    LPVOID lpNtOpenFile = nullptr;
	LPVOID lpNtReadFile = nullptr;
	LPVOID lpNtWriteFile = nullptr;
    LPVOID lpNtQueryInformationFile = nullptr;
    LPVOID lpNtQueryDirectoryFile = nullptr;
    
	// These are here for 32 bit builds so we can offset the function pointer
    LPVOID slpRtlAdjust =   (LPVOID)((uintptr_t)lpRtlAdjust + 0x0);
    LPVOID slpNtHardErr =   (LPVOID)((uintptr_t)lpNtHardErr + 0x0);
    LPVOID slpNtNamedPipe = (LPVOID)((uintptr_t)lpNamedPipe + 0x0);
	LPVOID slpNtSetInfo =   (LPVOID)((uintptr_t)lpNtSetInfo + 0x0);
	LPVOID slpNtWaitObj =   (LPVOID)((uintptr_t)lpNtWaitObj + 0x0);
	LPVOID slpNtClose =     (LPVOID)((uintptr_t)lpNtClose + 0x0);
	LPVOID slpNtFsControlFile = (LPVOID)((uintptr_t)lpNtFsControlFile + 0x0);
	LPVOID slpNtCreateFile = (LPVOID)((uintptr_t)lpNtCreateFile + 0x0);
	LPVOID slpNtOpenFile = (LPVOID)((uintptr_t)lpNtOpenFile + 0x0);
	LPVOID slpNtQueryInformationFile = (LPVOID)((uintptr_t)lpNtQueryInformationFile + 0x0);
	LPVOID slpNtQueryDirectoryFile = (LPVOID)((uintptr_t)lpNtQueryDirectoryFile + 0x0);
	LPVOID slpNtReadFile = (LPVOID)((uintptr_t)lpNtReadFile + 0x0);
	LPVOID slpNtWriteFile = (LPVOID)((uintptr_t)lpNtWriteFile + 0x0);

    NTSTATUS(NTAPI* _SafeRtlAdjustPrivilege)(
        ULONG Privilege,
        BOOLEAN Enable,
        BOOLEAN CurrentThread,
        PBOOLEAN OldValue)
        =
        (NTSTATUS(NTAPI*)(
            ULONG Privilege,
            BOOLEAN Enable,
            BOOLEAN CurrentThread,
            PBOOLEAN OldValue))slpRtlAdjust;

    NTSTATUS(NTAPI* _SafeNtRaiseHardError)(
        LONG ErrorStatus,
        ULONG NumberOfParameters,
        ULONG UnicodeStringParameterMask,
        PULONG_PTR Parameters,
        ULONG ValidResponseOptions,
        PULONG Response)
        =
        (NTSTATUS(NTAPI*)(
            LONG ErrorStatus,
            ULONG NumberOfParameters,
            ULONG UnicodeStringParameterMask,
            PULONG_PTR Parameters,
            ULONG ValidResponseOptions,
            PULONG Response))slpNtHardErr;

    NTSTATUS(NTAPI* _SafeNtCreateNamedPipeFile)(
        _Out_ PHANDLE FileHandle,
        _In_ ULONG DesiredAccess,
        _In_ POBJECT_ATTRIBUTES ObjectAttributes,
        _Out_ PIO_STATUS_BLOCK IoStatusBlock,
        _In_ ULONG ShareAccess,
        _In_ ULONG CreateDisposition,
        _In_ ULONG CreateOptions,
        _In_ ULONG NamedPipeType,
        _In_ ULONG ReadMode,
        _In_ ULONG CompletionMode,
        _In_ ULONG MaximumInstances,
        _In_ ULONG InboundQuota,
        _In_ ULONG OutboundQuota,
        _In_opt_ PLARGE_INTEGER DefaultTimeout
        )
		=
		(NTSTATUS(NTAPI*)(
            _Out_ PHANDLE FileHandle,
            _In_ ULONG DesiredAccess,
            _In_ POBJECT_ATTRIBUTES ObjectAttributes,
            _Out_ PIO_STATUS_BLOCK IoStatusBlock,
            _In_ ULONG ShareAccess,
            _In_ ULONG CreateDisposition,
            _In_ ULONG CreateOptions,
            _In_ ULONG NamedPipeType,
            _In_ ULONG ReadMode,
            _In_ ULONG CompletionMode,
            _In_ ULONG MaximumInstances,
            _In_ ULONG InboundQuota,
            _In_ ULONG OutboundQuota,
            _In_opt_ PLARGE_INTEGER DefaultTimeout
            ))slpNtNamedPipe;

    __kernel_entry NTSTATUS(NTAPI* _SafeNtSetInformationProcess)(
        _In_ HANDLE ProcessHandle,
        _In_ PROCESSINFOCLASS ProcessInformationClass,
        _In_ PVOID ProcessInformation,
        _In_ ULONG ProcessInformationLength
        )
        =
        __kernel_entry (NTSTATUS(NTAPI*)(
            _In_ HANDLE ProcessHandle,
            _In_ PROCESSINFOCLASS ProcessInformationClass,
            _In_ PVOID ProcessInformation,
            _In_ ULONG ProcessInformationLength
			))slpNtSetInfo;

    __kernel_entry NTSTATUS(NTAPI* _SafeNtWaitForSingleObject)(
		_In_ HANDLE Handle,
		_In_ BOOLEAN Alertable,
		_In_opt_ PLARGE_INTEGER Timeout
		)
		=
        __kernel_entry (NTSTATUS(NTAPI*)(
			_In_ HANDLE Handle,
			_In_ BOOLEAN Alertable,
			_In_opt_ PLARGE_INTEGER Timeout
			))slpNtWaitObj;

    __kernel_entry NTSTATUS(NTAPI* _SafeNtClose)(
        _In_ HANDLE Handle
        )
        =
        __kernel_entry (NTSTATUS(NTAPI*)(
            _In_ HANDLE Handle
            ))slpNtClose;

    __kernel_entry NTSTATUS(NTAPI* _SafeNtFsControlFile)(
		_In_ HANDLE FileHandle,
		_In_opt_ HANDLE Event,
		_In_opt_ PIO_APC_ROUTINE ApcRoutine,
		_In_opt_ PVOID ApcContext,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_In_ ULONG FsControlCode,
		_In_opt_ PVOID InputBuffer,
		_In_ ULONG InputBufferLength,
		_Out_opt_ PVOID OutputBuffer,
		_In_ ULONG OutputBufferLength
		)
		=
        __kernel_entry (NTSTATUS(NTAPI*)(
			_In_ HANDLE FileHandle,
			_In_opt_ HANDLE Event,
			_In_opt_ PIO_APC_ROUTINE ApcRoutine,
			_In_opt_ PVOID ApcContext,
			_Out_ PIO_STATUS_BLOCK IoStatusBlock,
			_In_ ULONG FsControlCode,
			_In_opt_ PVOID InputBuffer,
			_In_ ULONG InputBufferLength,
			_Out_opt_ PVOID OutputBuffer,
			_In_ ULONG OutputBufferLength
			))slpNtFsControlFile;

    __kernel_entry NTSTATUS(NTAPI* _SafeNtCreateFile)(
        _Out_ PHANDLE FileHandle,
        _In_ ACCESS_MASK DesiredAccess,
        _In_ POBJECT_ATTRIBUTES ObjectAttributes,
        _Out_ PIO_STATUS_BLOCK IoStatusBlock,
        _In_opt_ PLARGE_INTEGER AllocationSize,
        _In_ ULONG FileAttributes,
        _In_ ULONG ShareAccess,
        _In_ ULONG CreateDisposition,
        _In_ ULONG CreateOptions,
        _In_ PVOID EaBuffer,
        _In_ ULONG EaLength
		) = 
        __kernel_entry (NTSTATUS(NTAPI*)(
			_Out_ PHANDLE FileHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes,
			_Out_ PIO_STATUS_BLOCK IoStatusBlock,
			_In_opt_ PLARGE_INTEGER AllocationSize,
			_In_ ULONG FileAttributes,
			_In_ ULONG ShareAccess,
			_In_ ULONG CreateDisposition,
			_In_ ULONG CreateOptions,
			_In_ PVOID EaBuffer,
			_In_ ULONG EaLength
			))slpNtCreateFile;

    __kernel_entry NTSTATUS(NTAPI* _SafeNtOpenFile)(
		_Out_ PHANDLE FileHandle,
		_In_ ACCESS_MASK DesiredAccess,
		_In_ POBJECT_ATTRIBUTES ObjectAttributes,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_In_ ULONG ShareAccess,
		_In_ ULONG OpenOptions
		)
		=
        __kernel_entry (NTSTATUS(NTAPI*)(
			_Out_ PHANDLE FileHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes,
			_Out_ PIO_STATUS_BLOCK IoStatusBlock,
			_In_ ULONG ShareAccess,
			_In_ ULONG OpenOptions
			))slpNtOpenFile;

    __kernel_entry NTSTATUS(NTAPI* _SafeNtQueryInformationFile)(
		_In_ HANDLE FileHandle,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_Out_ PVOID FileInformation,
		_In_ ULONG Length,
		_In_ FILE_INFORMATION_CLASS FileInformationClass
		)
		=
        __kernel_entry (NTSTATUS(NTAPI*)(
			_In_ HANDLE FileHandle,
			_Out_ PIO_STATUS_BLOCK IoStatusBlock,
			_Out_ PVOID FileInformation,
			_In_ ULONG Length,
			_In_ FILE_INFORMATION_CLASS FileInformationClass
			))slpNtQueryInformationFile;

    __kernel_entry NTSTATUS(NTAPI* _SafeNtQueryDirectoryFile)(
		_In_ HANDLE FileHandle,
		_In_opt_ HANDLE Event,
		_In_opt_ PIO_APC_ROUTINE ApcRoutine,
		_In_opt_ PVOID ApcContext,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_Out_ PVOID FileInformation,
		_In_ ULONG Length,
		_In_ FILE_INFORMATION_CLASS
		FileInformationClass,
		_In_ BOOLEAN ReturnSingleEntry,
		_In_opt_ PUNICODE_STRING FileName,
		_In_ BOOLEAN RestartScan
		)
        =
        __kernel_entry (NTSTATUS(NTAPI*)(
			_In_ HANDLE FileHandle,
			_In_opt_ HANDLE Event,
			_In_opt_ PIO_APC_ROUTINE ApcRoutine,
			_In_opt_ PVOID ApcContext,
			_Out_ PIO_STATUS_BLOCK IoStatusBlock,
			_Out_ PVOID FileInformation,
			_In_ ULONG Length,
			_In_ FILE_INFORMATION_CLASS
			FileInformationClass,
			_In_ BOOLEAN ReturnSingleEntry,
			_In_opt_ PUNICODE_STRING FileName,
			_In_ BOOLEAN RestartScan
			))slpNtQueryDirectoryFile;

	__kernel_entry NTSTATUS(NTAPI* _SafeNtReadFile)(
		_In_ HANDLE FileHandle,
		_In_opt_ HANDLE Event,
		_In_opt_ PIO_APC_ROUTINE ApcRoutine,
		_In_opt_ PVOID ApcContext,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_Out_ PVOID Buffer,
		_In_ ULONG Length,
		_In_opt_ PLARGE_INTEGER ByteOffset,
		_In_opt_ PULONG Key
		)
		=
		__kernel_entry(NTSTATUS(NTAPI*)(
			_In_ HANDLE FileHandle,
			_In_opt_ HANDLE Event,
			_In_opt_ PIO_APC_ROUTINE ApcRoutine,
			_In_opt_ PVOID ApcContext,
			_Out_ PIO_STATUS_BLOCK IoStatusBlock,
			_Out_ PVOID Buffer,
			_In_ ULONG Length,
			_In_opt_ PLARGE_INTEGER ByteOffset,
			_In_opt_ PULONG Key
			))slpNtReadFile;

	__kernel_entry NTSTATUS(NTAPI* _SafeNtWriteFile)(
		_In_ HANDLE FileHandle,
		_In_opt_ HANDLE Event,
		_In_opt_ PIO_APC_ROUTINE ApcRoutine,
		_In_opt_ PVOID ApcContext,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_In_ PVOID Buffer,
		_In_ ULONG Length,
		_In_opt_ PLARGE_INTEGER ByteOffset,
		_In_opt_ PULONG Key
		)
		=
		__kernel_entry(NTSTATUS(NTAPI*)(
			_In_ HANDLE FileHandle,
			_In_opt_ HANDLE Event,
			_In_opt_ PIO_APC_ROUTINE ApcRoutine,
			_In_opt_ PVOID ApcContext,
			_Out_ PIO_STATUS_BLOCK IoStatusBlock,
			_In_ PVOID Buffer,
			_In_ ULONG Length,
			_In_opt_ PLARGE_INTEGER ByteOffset,
			_In_opt_ PULONG Key
			))slpNtWriteFile;
	
};