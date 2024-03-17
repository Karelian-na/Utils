#pragma once
#include "../Exception.h"
#include "Win32Utils.h"

_WIN32_BEGIN

class RegistryException : public Exception {
private:
	const LSTATUS m_nInternalCode_;

public:
	RegistryException(LSTATUS p_lRes)
	    : Exception(ERR_SYSTEM, GetErrorString(p_lRes).c_str())
	    , m_nInternalCode_(p_lRes) {
	}

	RegistryException(const String& pc_strMsg)
	    : Exception(ERR_SYSTEM, pc_strMsg.c_str())
	    , m_nInternalCode_(ERROR_SUCCESS) {
	}

public:
	OVERRIDE_READONLY_PROPERTY(LSTATUS, InternalCode, m_nInternalCode_);
};

class ComInteropException
    : public Exception
    , public _com_error {
public:
	ComInteropException(HRESULT hRes, IErrorInfo* pErrInfo = nullptr)
	    : _com_error(hRes, pErrInfo, true)
	    , Exception(ERR_COM_INTEROP, GetComError(hRes, pErrInfo)) {
	}
};

inline void _stdcall HandleComError(HRESULT hr, IErrorInfo* perrinfo) {
	throw ComInteropException(hr, perrinfo);
}

_WIN32_END