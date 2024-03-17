#include "Win32/Registry.h"

_WIN32_BEGIN

const RegistryKey Registry::Users { HKEY_USERS };
const RegistryKey Registry::CurrentUser { HKEY_CURRENT_USER };
const RegistryKey Registry::ClassesRoot { HKEY_CLASSES_ROOT };
const RegistryKey Registry::LocalMachine { HKEY_LOCAL_MACHINE };
const RegistryKey Registry::CurrentConfig { HKEY_CURRENT_CONFIG };
const RegistryKey Registry::PerformanceData { HKEY_PERFORMANCE_DATA };

const RegistryKey& Registry::ProcessKeyName_(String& pc_strKeyName) {
	const RegistryKey* pKeyParent;
	size_t offset = 5;
	if (pc_strKeyName.starts_with(L"HKU")) {
		pKeyParent = &Registry::Users;
		offset     = 4;
	} else if (pc_strKeyName.starts_with(L"HKCU")) {
		pKeyParent = &Registry::CurrentUser;
	} else if (pc_strKeyName.starts_with(L"HKCR")) {
		pKeyParent = &Registry::ClassesRoot;
	} else if (pc_strKeyName.starts_with(L"HKDD")) {
		pKeyParent = &Registry::PerformanceData;
	} else if (pc_strKeyName.starts_with(L"HKLM")) {
		pKeyParent = &Registry::LocalMachine;
	} else if (pc_strKeyName.starts_with(L"HKCC")) {
		pKeyParent = &Registry::CurrentConfig;
	} else if (pc_strKeyName.starts_with(L"HKPD")) {
		pKeyParent = &Registry::PerformanceData;
	} else {
		throw InvalidArgumentException(TEXT("keyName must be a string starts with sytem keyName such as HKCU which means HKEY_CURRENT_USER!"));
	}
	pc_strKeyName = pc_strKeyName.substr(offset);
	return *pKeyParent;
}

std::vector<byte>* Registry::GetValue(const String& pc_strKeyName, const String& pc_strValueName, LPDWORD p_pdwType) {
	String keyName(pc_strKeyName);
	const RegistryKey& parent = ProcessKeyName_(keyName);

	const RegistryKey&& key = parent.OpenSubKey(keyName);
	return key.GetValue(pc_strValueName, p_pdwType, 0);
}

bool Registry::SetValue(const String& pc_strKeyName, const String& pc_strValueName, const std::vector<byte>& pc_vbValue) {
	String keyName(pc_strKeyName);
	const RegistryKey& parent = ProcessKeyName_(keyName);
	const RegistryKey&& key   = parent.OpenSubKey(keyName);
	return key.SetValue(pc_strValueName, pc_vbValue);
}

bool Registry::SetValue(const String& pc_strKeyName, const String& pc_strValueName, const String& pc_strValue) {
	String keyName(pc_strKeyName);
	const RegistryKey& parent = ProcessKeyName_(keyName);
	const RegistryKey&& key   = parent.OpenSubKey(keyName);
	return key.SetValue(pc_strValueName, pc_strValue);
}

bool Registry::SetValue(const String& pc_strKeyName, const String& pc_strValueName, const StringList& pc_vstrValues) {
	String keyName(pc_strKeyName);
	const RegistryKey& parent = ProcessKeyName_(keyName);
	const RegistryKey&& key   = parent.OpenSubKey(keyName);
	return key.SetValue(pc_strValueName, pc_vstrValues);
}

bool Registry::SetValue(const String& pc_strKeyName, const String& pc_strValueName, const INT32 pc_dwValue, const bool pc_bIsBigEndian) {
	String keyName(pc_strKeyName);
	const RegistryKey& parent = ProcessKeyName_(keyName);
	const RegistryKey&& key   = parent.OpenSubKey(keyName);
	return key.SetValue(pc_strValueName, pc_dwValue, pc_bIsBigEndian);
}

bool Registry::SetValue(const String& pc_strKeyName, const String& pc_strValueName, const INT64 pc_n64Value) {
	String keyName(pc_strKeyName);
	const RegistryKey& parent = ProcessKeyName_(keyName);
	const RegistryKey&& key   = parent.OpenSubKey(keyName);
	return key.SetValue(pc_strValueName, pc_n64Value);
}

_WIN32_END