#pragma once
#include "Win32Exceptions.h"

_WIN32_BEGIN

class UTILS_API Shell {
public:
	/// <summary>
	/// 移动文件或目录
	/// </summary>
	/// <param name="pc_strSrc">源文件或目录</param>
	/// <param name="pc_strDest">目标文件或目录</param>
	/// <returns></returns>
	static HRESULT MoveItem(const String& pc_strSrc, const String& pc_strDest);
};

_WIN32_END