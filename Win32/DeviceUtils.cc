#pragma once
#include "Win32/DeviceUtils.h"

#define _WIN32_DCOM
#include <Wbemidl.h>

#include "Win32/Win32Exceptions.h"

_WIN32_BEGIN

_COM_SMARTPTR_TYPEDEF(IWbemLocator, __uuidof(IWbemLocator));
_COM_SMARTPTR_TYPEDEF(IWbemServices, __uuidof(IWbemServices));
_COM_SMARTPTR_TYPEDEF(IWbemClassObject, __uuidof(IWbemClassObject));
_COM_SMARTPTR_TYPEDEF(IEnumWbemClassObject, __uuidof(IEnumWbemClassObject));

bool DeviceUtils::s_bInited_ = false;

StringList DeviceUtils::GetWmiQueryResult_(String p_strQuery, String p_strPropName, bool allowEmptyItems) {
	StringList vstrRes;

	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	IWbemLocatorPtr pLoc;
	hRes = pLoc.CreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER);
	if (FAILED(hRes)) {
		throw ComInteropException(hRes);
	}

	IWbemServicesPtr pSvc;
	hRes = pLoc->ConnectServer(_bstr_t(TEXT("root\\cimv2")), NULL, NULL, NULL, WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &pSvc);
	if (FAILED(hRes)) {
		throw ComInteropException(hRes);
	}

	hRes = ::CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	IEnumWbemClassObjectPtr pEnumerator;
	hRes = pSvc->ExecQuery(
	    _bstr_t(TEXT("WQL")), _bstr_t(p_strQuery.c_str()), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	if (FAILED(hRes)) {
		throw ComInteropException(hRes);
	}

	ULONG uRes;
	IWbemClassObjectPtr pObj;

	hRes = WBEM_S_NO_ERROR;
	while (WBEM_S_NO_ERROR == hRes) {
		hRes = pEnumerator->Next(WBEM_INFINITE, 1, &pObj, &uRes);
		if (0 == uRes) {
			break;
		}
		VARIANT vtProp;
		::VariantInit(&vtProp);
		hRes = pObj->Get(p_strPropName.c_str(), 0, &vtProp, NULL, NULL);
		if (nullptr != vtProp.bstrVal) {
			vstrRes.push_back(StringUtils::Trim(vtProp.bstrVal));
		}
		::VariantClear(&vtProp);
	}

	return vstrRes;
}

_WIN32_END