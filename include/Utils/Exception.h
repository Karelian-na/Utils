#pragma once
#include <exception>
#include <map>

#include "StringUtils.h"

#pragma warning(push)
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

_UTILS_BEGIN

enum UtilErrors {
	/// <summary>
	/// 未知错误
	/// </summary>
	ERR_UNKNOWN = 0x1000'001,

	/// <summary>
	/// 系统调用错误
	/// </summary>
	ERR_SYSTEM,

	/// <summary>
	/// COM互操作错误
	/// </summary>
	ERR_COM_INTEROP,

	/// <summary>
	/// 指定文件不存在
	/// </summary>
	ERR_FILE_NOT_FOUND,

	/// <summary>
	/// 空指针引用错误
	/// </summary>
	ERR_NULLPTR,

	/// <summary>
	/// 无效的参数
	/// </summary>
	ERR_INVALID_ARGUMENT,

	/// <summary>
	/// 访问被拒绝
	/// </summary>
	ERR_ACCESS_DENIED,
};

/// <summary>
/// 向异常反馈信息映射中注册错误信息
/// </summary>
/// <param name="pc_mpErrMsgs">将要注册的错误信息</param>
UTILS_API void RegisterErrorMessages(const std::map<long, const TCHAR*>& pc_mpErrMsgs);

/// <summary>
/// 根据给定的错误码获取基本错误信息
/// </summary>
/// <param name="p_lCode">错误代码</param>
UTILS_API const TCHAR* GetErrorMessage(long p_lCode);

/// <summary>
/// 连接 给定的错误码对应的基本错误信息 与 指定的附加信息
/// </summary>
/// <param name="p_lBaseErrCode">错误码</param>
/// <param name="p_cwszMsg">附加信息</param>
/// <returns>连接后的错误信息</returns>
UTILS_API const TCHAR* ConcatBaseErrorMessage(long p_lBaseErrCode, const TCHAR* p_cwszMsg);

class UTILS_API Exception : public std::exception {
protected:
	const long m_lCode;

public:
	Exception() = delete;

	inline Exception(long p_lCode)
	    : std::exception(StringUtils::WideCharToMultiByte(GetErrorMessage(p_lCode)).c_str())
	    , m_lCode(p_lCode) {
	}

	inline Exception(long p_lCode, const wchar_t* p_cwszMsg)
	    : std::exception(StringUtils::WideCharToMultiByte(ConcatBaseErrorMessage(p_lCode, p_cwszMsg)).c_str())
	    , m_lCode(p_lCode) {
	}

	inline Exception(long p_lCode, const char* p_cszMsg)
	    : Exception(p_lCode, StringUtils::MultiByteToWideChar(p_cszMsg).c_str()) {
	}

	inline Exception(const std::exception& pc_ex) noexcept
	    : Exception(ERR_UNKNOWN, pc_ex.what()) {
	}

	inline Exception(const char* p_cszMsg) noexcept
	    : Exception(ERR_UNKNOWN, p_cszMsg) {
	}

	inline Exception(const wchar_t* p_cwszMsg) noexcept
	    : Exception(ERR_UNKNOWN, p_cwszMsg) {
	}

	/// <summary>
	/// 此错误的错误代码
	/// </summary>
	DECLARE_READONLY_PROPERTY_WITH_BODY(long, Code, m_lCode);
	DECLARE_VIRTUAL_READONLY_PROPERTY_WITH_BODY(long, InternalCode, m_lCode);

	/// <summary>
	/// 此错误的完整描述
	/// </summary>
	DECLARE_READONLY_PROPERTY(String, Msg);

	inline const String GetMsg() const noexcept {
		return StringUtils::MultiByteToWideChar(what());
	}
};

class UTILS_API NullPointerReferenceException : public Exception {
public:
	NullPointerReferenceException(const wchar_t* p_cwszName = TEXT(""))
	    : Exception(ERR_NULLPTR, p_cwszName) {
	}
};

class UTILS_API InvalidArgumentException : public Exception {
public:
	InvalidArgumentException(const wchar_t* p_cwszName = TEXT(""))
	    : Exception(ERR_INVALID_ARGUMENT, p_cwszName) {
	}
};

class UTILS_API FileNotFoundException : public Exception {
public:
	FileNotFoundException(const wchar_t* p_cwszPath = TEXT(""))
	    : Exception(ERR_FILE_NOT_FOUND, p_cwszPath) {
	}
};

class UTILS_API AccessDeniedException : public Exception {
public:
	AccessDeniedException()
	    : Exception(UtilErrors::ERR_ACCESS_DENIED) {
	}
};

_UTILS_END

#pragma warning(pop)