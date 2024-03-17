#pragma once
#include <comdef.h>

#include "../StringUtils.h"

#define __WIN32 _UTILS Win32::
#define _WIN32_BEGIN namespace _UTILS Win32 {
#define _WIN32_END }

#ifdef _UNICODE
#define GetErrorString GetErrorStringW
#else
#define GetErrorString GetErrorStringA
#endif // _UNICODE

_WIN32_BEGIN

/// <summary>
/// 获取Com错误的信息
/// </summary>
/// <param name="hRes">错误代码</param>
/// <param name="pErrInfo">错误信息</param>
UTILS_API const TCHAR* GetComError(HRESULT hRes, IErrorInfo* pErrInfo = nullptr);

/// <summary>
/// 获取给定的Windows错误代码的错误信息
/// </summary>
/// <param name="p_lRes"></param>
/// <returns>宽字节编码的信息</returns>
UTILS_API std::wstring GetErrorStringW(LSTATUS p_lRes);

/// <summary>
/// 获取给定的Windows错误代码的错误信息
/// </summary>
/// <param name="p_lRes"></param>
/// <returns>多字节编码的信息</returns>
UTILS_API std::string GetErrorStringA(LSTATUS p_lRes);

/// <summary>
/// 判断某个进程是否存在
/// </summary>
/// <param name="strProcessName"></param>
/// <returns></returns>
UTILS_API bool ExistsProcess(const String& strProcessName);

/// <summary>
/// 检查是否连接到互联网
/// </summary>
/// <returns></returns>
UTILS_API bool IsConnectedToInternet();

/// <summary>
/// 获取系统字体列表
/// </summary>
/// <returns></returns>
UTILS_API StringList GetSystemFonts();

_WIN32_END