#pragma once

#include "StringUtils.h"

#pragma warning(push)
#pragma warning(disable : 4251)

_UTILS_BEGIN

class DeviceInfo {
private:
	/// <summary>
	/// 当前设备拥有的所有的硬盘的Id
	/// </summary>
	String m_vstrDiskIds_;

public:
	DECLARE_PROPERTY_WITH_BODY(String, DiskIds, m_vstrDiskIds_);
};

_UTILS_END

#pragma warning(pop)