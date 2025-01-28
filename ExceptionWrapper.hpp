#pragma once
#include <phnt_windows.h>
#include "ExportInterface.hpp"

// Interface for kernel32.dll exception handling functions
class ExceptionWrapper {
public:
    BOOL IReady = FALSE;
    ExceptionWrapper() :
        pSetUnhandledExceptionHandler(IExport::LoadAndFindSingleExport("k.e2dr3lnlle", "SdptenletletaeciirUhdxoFnEn"))
    {
        if (pSetUnhandledExceptionHandler != nullptr)
            IReady = TRUE;
        else
            ILog("SetUnhandledExceptionFilter not found\n");
    }

    LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilter(_In_opt_ LPTOP_LEVEL_EXCEPTION_FILTER lpTop)
    {
        return _SafeSetUnhandledExceptionFilter(lpTop);
    }

private:
    LPVOID pSetUnhandledExceptionHandler = nullptr;
    LPVOID slpSetUnhandledExceptionHandler = (LPVOID)((uintptr_t)pSetUnhandledExceptionHandler + 0x0);
    LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI* _SafeSetUnhandledExceptionFilter)(_In_opt_ LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter) =
        (LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI*)(_In_opt_ LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter))slpSetUnhandledExceptionHandler;
};