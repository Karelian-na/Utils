#pragma once
#include <iostream>
#include <vector>

#include "Win32Utils.h"

_WIN32_BEGIN

class UTILS_API DeviceUtils final {
private:
	static StringList GetWmiQueryResult_(String wmiQuery, String propNameOfResultObject, bool allowEmptyItems = false);

private:
	static bool s_bInited_;

public:
	/// <summary>
	/// 获取当前设备的所有硬盘ID
	/// </summary>
	/// <returns></returns>
	static inline StringList GetDiskIds() {
		return GetWmiQueryResult_(TEXT("SELECT SerialNumber FROM Win32_DiskDrive"), TEXT("SerialNumber"));
	}

	/// <summary>
	/// 获取所有处理器ID
	/// </summary>
	/// <returns></returns>
	static inline StringList GetProcessorIds() {
		return GetWmiQueryResult_(TEXT("SELECT ProcessorId FROM Win32_Processor"), TEXT("ProcessorId"));
	}

	/// <summary>
	/// 获取操作系统名称
	/// </summary>
	/// <returns></returns>
	static inline StringList GetOperatingSystem() {
		return GetWmiQueryResult_(TEXT("SELECT * FROM Win32_OperatingSystem"), TEXT("Name"));
	}
};

_WIN32_END