#pragma once
#include <phnt_windows.h>
#include <phnt.h>
#include <objbase.h>

#define WBEM_FLAG_CREATE_OR_UPDATE 0

GUID CSLSID_WbemLocator = { 0x4590f811, 0x1d3a, 0x11d0, 0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24 };
GUID SIID_IClassFactory = { 0x00000001, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 };
GUID SIID_IUnknown = { 0x00000000, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 };
GUID SIID_IWbemLocator = { 0xdc12a687, 0x737f, 0x11cf, 0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24 };

typedef long CIMTYPE;
typedef interface IEnumWbemClassObject IEnumWbemClassObject;
typedef interface IWbemCallResult IWbemCallResult;
typedef interface IWbemServices IWbemServices;
typedef interface IWbemObjectSink IWbemObjectSink;
typedef interface IWbemContext IWbemContext;
typedef interface IWbemClassObject IWbemClassObject;
typedef interface IWbemQualifierSet IWbemQualifierSet;

typedef /* [v1_enum] */
enum tag_WBEM_CONDITION_FLAG_TYPE
{
    WBEM_FLAG_ALWAYS = 0,
    WBEM_FLAG_ONLY_IF_TRUE = 0x1,
    WBEM_FLAG_ONLY_IF_FALSE = 0x2,
    WBEM_FLAG_ONLY_IF_IDENTICAL = 0x3,
    WBEM_MASK_PRIMARY_CONDITION = 0x3,
    WBEM_FLAG_KEYS_ONLY = 0x4,
    WBEM_FLAG_REFS_ONLY = 0x8,
    WBEM_FLAG_LOCAL_ONLY = 0x10,
    WBEM_FLAG_PROPAGATED_ONLY = 0x20,
    WBEM_FLAG_SYSTEM_ONLY = 0x30,
    WBEM_FLAG_NONSYSTEM_ONLY = 0x40,
    WBEM_MASK_CONDITION_ORIGIN = 0x70,
    WBEM_FLAG_CLASS_OVERRIDES_ONLY = 0x100,
    WBEM_FLAG_CLASS_LOCAL_AND_OVERRIDES = 0x200,
    WBEM_MASK_CLASS_CONDITION = 0x300
} 	WBEM_CONDITION_FLAG_TYPE;

enum tag_CIMTYPE_ENUMERATION
{
    CIM_ILLEGAL = 0xfff,
    CIM_EMPTY = 0,
    CIM_SINT8 = 16,
    CIM_UINT8 = 17,
    CIM_SINT16 = 2,
    CIM_UINT16 = 18,
    CIM_SINT32 = 3,
    CIM_UINT32 = 19,
    CIM_SINT64 = 20,
    CIM_UINT64 = 21,
    CIM_REAL32 = 4,
    CIM_REAL64 = 5,
    CIM_BOOLEAN = 11,
    CIM_STRING = 8,
    CIM_DATETIME = 101,
    CIM_REFERENCE = 102,
    CIM_CHAR16 = 103,
    CIM_OBJECT = 13,
    CIM_FLAG_ARRAY = 0x2000
} 	CIMTYPE_ENUMERATION;

typedef /* [v1_enum] */
enum tag_WBEM_GENERIC_FLAG_TYPE
{
    WBEM_FLAG_RETURN_IMMEDIATELY = 0x10,
    WBEM_FLAG_RETURN_WBEM_COMPLETE = 0,
    WBEM_FLAG_BIDIRECTIONAL = 0,
    WBEM_FLAG_FORWARD_ONLY = 0x20,
    WBEM_FLAG_NO_ERROR_OBJECT = 0x40,
    WBEM_FLAG_RETURN_ERROR_OBJECT = 0,
    WBEM_FLAG_SEND_STATUS = 0x80,
    WBEM_FLAG_DONT_SEND_STATUS = 0,
    WBEM_FLAG_ENSURE_LOCATABLE = 0x100,
    WBEM_FLAG_DIRECT_READ = 0x200,
    WBEM_FLAG_SEND_ONLY_SELECTED = 0,
    WBEM_RETURN_WHEN_COMPLETE = 0,
    WBEM_RETURN_IMMEDIATELY = 0x10,
    WBEM_MASK_RESERVED_FLAGS = 0x1f000,
    WBEM_FLAG_USE_AMENDED_QUALIFIERS = 0x20000,
    WBEM_FLAG_STRONG_VALIDATION = 0x100000
} 	WBEM_GENERIC_FLAG_TYPE;


typedef /* [v1_enum] */
enum tag_WBEM_TIMEOUT_TYPE
{
    WBEM_NO_WAIT = 0,
    WBEM_INFINITE = 0xffffffff
} 	WBEM_TIMEOUT_TYPE;


class Win32_ProcessStartup
{
    UINT32 CreateFlags;
    BSTR EnvironmentVariables;
    UINT16 ErrorMode = 1;
    UINT32 FillAttribute;
    UINT32 PriorityClass;
    UINT16 ShowWindow;
    BSTR Title;
    BSTR WinstationDesktop;
    UINT32 X;
    UINT32 XCountChars;
    UINT32 XSize;
    UINT32 Y;
    UINT32 YCountChars;
    UINT32 YSize;
};

MIDL_INTERFACE("dc12a687-737f-11cf-884d-00aa004b2e24")
IWbemLocator : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE ConnectServer(
        /* [in] */ const BSTR strNetworkResource,
        /* [in] */ const BSTR strUser,
        /* [in] */ const BSTR strPassword,
        /* [in] */ const BSTR strLocale,
        /* [in] */ long lSecurityFlags,
        /* [in] */ const BSTR strAuthority,
        /* [in] */ LPVOID * pCtx, // Would be IWbemContext 
        /* [out] */ IWbemServices * *ppNamespace) = 0;

};

MIDL_INTERFACE("9556dc99-828c-11cf-a37e-00aa003240c7")
IWbemServices : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE OpenNamespace(
        /* [in] */ __RPC__in const BSTR strNamespace,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt LPVOID * pCtx,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemServices * *ppWorkingNamespace,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemCallResult * *ppResult) = 0;

    virtual HRESULT STDMETHODCALLTYPE CancelAsyncCall(
        /* [in] */ __RPC__in_opt IWbemObjectSink* pSink) = 0;

    virtual HRESULT STDMETHODCALLTYPE QueryObjectSink(
        /* [in] */ long lFlags,
        /* [out] */ __RPC__deref_out_opt IWbemObjectSink** ppResponseHandler) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetObject(
        /* [in] */ __RPC__in const BSTR strObjectPath,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemClassObject** ppObject,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemCallResult** ppCallResult) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetObjectAsync(
        /* [in] */ __RPC__in const BSTR strObjectPath,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemObjectSink* pResponseHandler) = 0;

    virtual HRESULT STDMETHODCALLTYPE PutClass(
        /* [in] */ __RPC__in_opt IWbemClassObject* pObject,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemCallResult** ppCallResult) = 0;

    virtual HRESULT STDMETHODCALLTYPE PutClassAsync(
        /* [in] */ __RPC__in_opt IWbemClassObject* pObject,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemObjectSink* pResponseHandler) = 0;

    virtual HRESULT STDMETHODCALLTYPE DeleteClass(
        /* [in] */ __RPC__in const BSTR strClass,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemCallResult** ppCallResult) = 0;

    virtual HRESULT STDMETHODCALLTYPE DeleteClassAsync(
        /* [in] */ __RPC__in const BSTR strClass,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemObjectSink* pResponseHandler) = 0;

    virtual HRESULT STDMETHODCALLTYPE CreateClassEnum(
        /* [in] */ __RPC__in const BSTR strSuperclass,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [out] */ __RPC__deref_out_opt IEnumWbemClassObject** ppEnum) = 0;

    virtual HRESULT STDMETHODCALLTYPE CreateClassEnumAsync(
        /* [in] */ __RPC__in const BSTR strSuperclass,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemObjectSink* pResponseHandler) = 0;

    virtual HRESULT STDMETHODCALLTYPE PutInstance(
        /* [in] */ __RPC__in_opt IWbemClassObject* pInst,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemCallResult** ppCallResult) = 0;

    virtual HRESULT STDMETHODCALLTYPE PutInstanceAsync(
        /* [in] */ __RPC__in_opt IWbemClassObject* pInst,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemObjectSink* pResponseHandler) = 0;

    virtual HRESULT STDMETHODCALLTYPE DeleteInstance(
        /* [in] */ __RPC__in const BSTR strObjectPath,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemCallResult** ppCallResult) = 0;

    virtual HRESULT STDMETHODCALLTYPE DeleteInstanceAsync(
        /* [in] */ __RPC__in const BSTR strObjectPath,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemObjectSink* pResponseHandler) = 0;

    virtual HRESULT STDMETHODCALLTYPE CreateInstanceEnum(
        /* [in] */ __RPC__in const BSTR strFilter,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [out] */ __RPC__deref_out_opt IEnumWbemClassObject** ppEnum) = 0;

    virtual HRESULT STDMETHODCALLTYPE CreateInstanceEnumAsync(
        /* [in] */ __RPC__in const BSTR strFilter,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemObjectSink* pResponseHandler) = 0;

    virtual HRESULT STDMETHODCALLTYPE ExecQuery(
        /* [in] */ __RPC__in const BSTR strQueryLanguage,
        /* [in] */ __RPC__in const BSTR strQuery,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [out] */ __RPC__deref_out_opt IEnumWbemClassObject** ppEnum) = 0;

    virtual HRESULT STDMETHODCALLTYPE ExecQueryAsync(
        /* [in] */ __RPC__in const BSTR strQueryLanguage,
        /* [in] */ __RPC__in const BSTR strQuery,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemObjectSink* pResponseHandler) = 0;

    virtual HRESULT STDMETHODCALLTYPE ExecNotificationQuery(
        /* [in] */ __RPC__in const BSTR strQueryLanguage,
        /* [in] */ __RPC__in const BSTR strQuery,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [out] */ __RPC__deref_out_opt IEnumWbemClassObject** ppEnum) = 0;

    virtual HRESULT STDMETHODCALLTYPE ExecNotificationQueryAsync(
        /* [in] */ __RPC__in const BSTR strQueryLanguage,
        /* [in] */ __RPC__in const BSTR strQuery,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemObjectSink* pResponseHandler) = 0;

    virtual HRESULT STDMETHODCALLTYPE ExecMethod(
        /* [in] */ __RPC__in const BSTR strObjectPath,
        /* [in] */ __RPC__in const BSTR strMethodName,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemClassObject* pInParams,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemClassObject** ppOutParams,
        /* [unique][in][out] */ __RPC__deref_opt_inout_opt IWbemCallResult** ppCallResult) = 0;

    virtual HRESULT STDMETHODCALLTYPE ExecMethodAsync(
        /* [in] */ __RPC__in const BSTR strObjectPath,
        /* [in] */ __RPC__in const BSTR strMethodName,
        /* [in] */ long lFlags,
        /* [in] */ __RPC__in_opt IWbemContext* pCtx,
        /* [in] */ __RPC__in_opt IWbemClassObject* pInParams,
        /* [in] */ __RPC__in_opt IWbemObjectSink* pResponseHandler) = 0;

};

MIDL_INTERFACE("dc12a681-737f-11cf-884d-00aa004b2e24")
IWbemClassObject : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetQualifierSet(
        /* [out] */ LPVOID * *ppQualSet) = 0;

    virtual HRESULT STDMETHODCALLTYPE Get(
        /* [string][in] */ LPCWSTR wszName,
        /* [in] */ long lFlags,
        /* [unique][in][out] */ VARIANT* pVal,
        /* [unique][in][out] */ CIMTYPE* pType,
        /* [unique][in][out] */ long* plFlavor) = 0;

    virtual HRESULT STDMETHODCALLTYPE Put(
        /* [string][in] */ LPCWSTR wszName,
        /* [in] */ long lFlags,
        /* [in] */ VARIANT* pVal,
        /* [in] */ CIMTYPE Type) = 0;

    virtual HRESULT STDMETHODCALLTYPE Delete(
        /* [string][in] */ LPCWSTR wszName) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetNames(
        /* [string][in] */ LPCWSTR wszQualifierName,
        /* [in] */ long lFlags,
        /* [in] */ VARIANT* pQualifierVal,
        /* [out] */ SAFEARRAY** pNames) = 0;

    virtual HRESULT STDMETHODCALLTYPE BeginEnumeration(
        /* [in] */ long lEnumFlags) = 0;

    virtual HRESULT STDMETHODCALLTYPE Next(
        /* [in] */ long lFlags,
        /* [unique][in][out] */ BSTR* strName,
        /* [unique][in][out] */ VARIANT* pVal,
        /* [unique][in][out] */ CIMTYPE* pType,
        /* [unique][in][out] */ long* plFlavor) = 0;

    virtual HRESULT STDMETHODCALLTYPE EndEnumeration(void) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPropertyQualifierSet(
        /* [string][in] */ LPCWSTR wszProperty,
        /* [out] */ LPVOID** ppQualSet) = 0;

    virtual HRESULT STDMETHODCALLTYPE Clone(
        /* [out] */ IWbemClassObject** ppCopy) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetObjectText(
        /* [in] */ long lFlags,
        /* [out] */ BSTR* pstrObjectText) = 0;

    virtual HRESULT STDMETHODCALLTYPE SpawnDerivedClass(
        /* [in] */ long lFlags,
        /* [out] */ IWbemClassObject** ppNewClass) = 0;

    virtual HRESULT STDMETHODCALLTYPE SpawnInstance(
        /* [in] */ long lFlags,
        /* [out] */ IWbemClassObject** ppNewInstance) = 0;

    virtual HRESULT STDMETHODCALLTYPE CompareTo(
        /* [in] */ long lFlags,
        /* [in] */ IWbemClassObject* pCompareTo) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPropertyOrigin(
        /* [string][in] */ LPCWSTR wszName,
        /* [out] */ BSTR* pstrClassName) = 0;

    virtual HRESULT STDMETHODCALLTYPE InheritsFrom(
        /* [in] */ LPCWSTR strAncestor) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetMethod(
        /* [string][in] */ LPCWSTR wszName,
        /* [in] */ long lFlags,
        /* [out] */ IWbemClassObject** ppInSignature,
        /* [out] */ IWbemClassObject** ppOutSignature) = 0;

    virtual HRESULT STDMETHODCALLTYPE PutMethod(
        /* [string][in] */ LPCWSTR wszName,
        /* [in] */ long lFlags,
        /* [in] */ IWbemClassObject* pInSignature,
        /* [in] */ IWbemClassObject* pOutSignature) = 0;

    virtual HRESULT STDMETHODCALLTYPE DeleteMethod(
        /* [string][in] */ LPCWSTR wszName) = 0;

    virtual HRESULT STDMETHODCALLTYPE BeginMethodEnumeration(
        /* [in] */ long lEnumFlags) = 0;

    virtual HRESULT STDMETHODCALLTYPE NextMethod(
        /* [in] */ long lFlags,
        /* [unique][in][out] */ BSTR* pstrName,
        /* [unique][in][out] */ IWbemClassObject** ppInSignature,
        /* [unique][in][out] */ IWbemClassObject** ppOutSignature) = 0;

    virtual HRESULT STDMETHODCALLTYPE EndMethodEnumeration(void) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetMethodQualifierSet(
        /* [string][in] */ LPCWSTR wszMethod,
        /* [out] */ LPVOID** ppQualSet) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetMethodOrigin(
        /* [string][in] */ LPCWSTR wszMethodName,
        /* [out] */ BSTR* pstrClassName) = 0;

};

MIDL_INTERFACE("44aca674-e8fc-11d0-a07c-00c04fb68820")
IWbemContext : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE Clone(
        /* [out] */ IWbemContext * *ppNewCopy) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetNames(
        /* [in] */ long lFlags,
        /* [out] */ SAFEARRAY** pNames) = 0;

    virtual HRESULT STDMETHODCALLTYPE BeginEnumeration(
        /* [in] */ long lFlags) = 0;

    virtual HRESULT STDMETHODCALLTYPE Next(
        /* [in] */ long lFlags,
        /* [out] */ BSTR* pstrName,
        /* [out] */ VARIANT* pValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE EndEnumeration(void) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetValue(
        /* [string][in] */ LPCWSTR wszName,
        /* [in] */ long lFlags,
        /* [in] */ VARIANT* pValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetValue(
        /* [string][in] */ LPCWSTR wszName,
        /* [in] */ long lFlags,
        /* [out] */ VARIANT* pValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE DeleteValue(
        /* [string][in] */ LPCWSTR wszName,
        /* [in] */ long lFlags) = 0;

    virtual HRESULT STDMETHODCALLTYPE DeleteAll(void) = 0;

};

MIDL_INTERFACE("7c857801-7381-11cf-884d-00aa004b2e24")
IWbemObjectSink : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE Indicate(
        /* [in] */ long lObjectCount,
        /* [size_is][in] */ __RPC__in_ecount_full(lObjectCount) IWbemClassObject * *apObjArray) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetStatus(
        /* [in] */ long lFlags,
        /* [in] */ HRESULT hResult,
        /* [unique][in] */ __RPC__in_opt BSTR strParam,
        /* [unique][in] */ __RPC__in_opt IWbemClassObject* pObjParam) = 0;

};

MIDL_INTERFACE("027947e1-d731-11ce-a357-000000000001")
IEnumWbemClassObject : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;

    virtual HRESULT STDMETHODCALLTYPE Next(
        /* [in] */ long lTimeout,
        /* [in] */ ULONG uCount,
        /* [length_is][size_is][out] */ __RPC__out_ecount_part(uCount, *puReturned) IWbemClassObject** apObjects,
        /* [out] */ __RPC__out ULONG* puReturned) = 0;

    virtual HRESULT STDMETHODCALLTYPE NextAsync(
        /* [in] */ ULONG uCount,
        /* [in] */ __RPC__in_opt IWbemObjectSink* pSink) = 0;

    virtual HRESULT STDMETHODCALLTYPE Clone(
        /* [out] */ __RPC__deref_out_opt IEnumWbemClassObject** ppEnum) = 0;

    virtual HRESULT STDMETHODCALLTYPE Skip(
        /* [in] */ long lTimeout,
        /* [in] */ ULONG nCount) = 0;

};

MIDL_INTERFACE("dc12a680-737f-11cf-884d-00aa004b2e24")
IWbemQualifierSet : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE Get(
        /* [string][in] */ LPCWSTR wszName,
        /* [in] */ long lFlags,
        /* [unique][in][out] */ VARIANT * pVal,
        /* [unique][in][out] */ long* plFlavor) = 0;

    virtual HRESULT STDMETHODCALLTYPE Put(
        /* [string][in] */ LPCWSTR wszName,
        /* [in] */ VARIANT* pVal,
        /* [in] */ long lFlavor) = 0;

    virtual HRESULT STDMETHODCALLTYPE Delete(
        /* [string][in] */ LPCWSTR wszName) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetNames(
        /* [in] */ long lFlags,
        /* [out] */ SAFEARRAY** pNames) = 0;

    virtual HRESULT STDMETHODCALLTYPE BeginEnumeration(
        /* [in] */ long lFlags) = 0;

    virtual HRESULT STDMETHODCALLTYPE Next(
        /* [in] */ long lFlags,
        /* [unique][in][out] */ BSTR* pstrName,
        /* [unique][in][out] */ VARIANT* pVal,
        /* [unique][in][out] */ long* plFlavor) = 0;

    virtual HRESULT STDMETHODCALLTYPE EndEnumeration(void) = 0;

};