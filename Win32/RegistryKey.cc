#include "Win32/RegistryKey.h"

_WIN32_BEGIN

RegistryKey::RegistryKey(HKEY p_key) {
	m_hKey_ = p_key;

	if (HKEY_USERS == m_hKey_) {
		m_strName_ = TEXT("HKU");
	} else if (HKEY_CURRENT_USER == m_hKey_) {
		m_strName_ = TEXT("HKCU");
	} else if (HKEY_CLASSES_ROOT == m_hKey_) {
		m_strName_ = TEXT("HKCR");
	} else if (HKEY_LOCAL_MACHINE == m_hKey_) {
		m_strName_ = TEXT("HKLM");
	} else if (HKEY_CURRENT_CONFIG == m_hKey_) {
		m_strName_ = TEXT("HKCC");
	} else if (HKEY_PERFORMANCE_DATA == m_hKey_) {
		m_strName_ = TEXT("HKPD");
	} else {
		throw InvalidArgumentException(TEXT("p_key"));
	}

	m_strPath_       = m_strName_;
	m_bNeedDestruct_ = false;
	m_right_         = RegistryKeyAccess::All;
}

RegistryKey::RegistryKey(const RegistryKey& p_key)
    : m_strName_(p_key.m_strName_)
    , m_strPath_(p_key.m_strPath_)
    , m_right_(p_key.m_right_) {
	m_hKey_          = p_key.m_hKey_;
	m_bNeedDestruct_ = false;
}

RegistryKey::RegistryKey(RegistryKey&& p_right) noexcept
    : m_strName_(std::move(p_right.m_strName_))
    , m_strPath_(std::move(p_right.m_strPath_))
    , m_right_(p_right.m_right_) {
	m_hKey_         = p_right.m_hKey_;
	p_right.m_hKey_ = NULL;
}

RegistryKey& RegistryKey::operator=(RegistryKey&& p_right) noexcept {
	this->~RegistryKey();
	m_strName_ = std::move(p_right.m_strName_);
	m_strPath_ = std::move(p_right.m_strPath_);
	m_hKey_    = p_right.m_hKey_;
	m_right_   = p_right.m_right_;

	p_right.m_hKey_ = NULL;
	return *this;
}

RegistryKey::~RegistryKey() noexcept {
	if (!m_bNeedDestruct_) {
		return;
	}

	if (m_hKey_) {
		::RegCloseKey(m_hKey_);
		m_hKey_ = nullptr;
	}
}

RegistryKey RegistryKey::OpenSubKey(const String& p_strSubKeyName, const RegistryKeyAccess pc_right) const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	if (p_strSubKeyName.empty()) {
		throw InvalidArgumentException(TEXT("subKeyName"));
	}

	RegistryKey key;
	LSTATUS     lRes = RegOpenKeyEx(m_hKey_, p_strSubKeyName.c_str(), 0, GetNativeRegistryKeyAccess(pc_right), &key.m_hKey_);
	key.m_right_     = pc_right;

	switch (lRes) {
		case ERROR_SUCCESS:
			break;
		case ERROR_FILE_NOT_FOUND:
			throw FileNotFoundException();
		case ERROR_ACCESS_DENIED:
			throw AccessDeniedException();
		default:
			throw RegistryException(lRes);
	}

	size_t nidx    = p_strSubKeyName.find_last_of(TEXT("\\"));
	key.m_strName_ = nidx == String::npos ? p_strSubKeyName : p_strSubKeyName.substr(nidx + 1);
	key.m_strPath_ = FORMAT("{}\\{}", m_strPath_, p_strSubKeyName);
	return key;
}

RegistryKey RegistryKey::CreateSubKey(const String& p_strSubKeyName, const RegistryKeyAccess pc_right) const {
	try {
		return OpenSubKey(p_strSubKeyName, pc_right);
	} catch (const FileNotFoundException&) { }

	if (!(m_right_ & RegistryKeyAccess::Write)) {
		throw AccessDeniedException();
	}

	RegistryKey key;
	LSTATUS     lRes = RegCreateKeyEx(m_hKey_, p_strSubKeyName.c_str(), 0, NULL, 0, GetNativeRegistryKeyAccess(pc_right), NULL, &key.m_hKey_, NULL);
	switch (lRes) {
		case ERROR_SUCCESS:
			break;
		case ERROR_ACCESS_DENIED:
			throw AccessDeniedException();
		default:
			throw RegistryException(lRes);
	}

	const size_t c_nIdx = p_strSubKeyName.find_last_of(TEXT('\\'), 0);
	key.m_strName_      = c_nIdx == String::npos ? p_strSubKeyName : p_strSubKeyName.substr(c_nIdx + 1);
	key.m_strPath_      = FORMAT("{}\\{}", m_strPath_, p_strSubKeyName);
	return key;
}

bool RegistryKey::DeleteSubKey(const String& p_strSubKeyName) const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	if (!(m_right_ & RegistryKeyAccess::Write)) {
		throw AccessDeniedException();
	}

	LSTATUS lRes = RegDeleteKeyEx(m_hKey_, p_strSubKeyName.c_str(), 0, 0);

	switch (lRes) {
		case ERROR_FILE_NOT_FOUND:
			return false;
		case ERROR_SUCCESS:
			return true;
		default:
			throw RegistryException(lRes);
	}
}

std::vector<byte>* RegistryKey::GetValue(const String& p_strValueName, LPDWORD p_pdwType, DWORD p_dwReserved) const {
	static std::vector<byte> s_Result;
	s_Result.clear();

	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	DWORD   dwlen;
	LSTATUS lRes = RegQueryValueEx(m_hKey_, p_strValueName.c_str(), NULL, p_pdwType, NULL, &dwlen);
	if (lRes == ERROR_FILE_NOT_FOUND) {
		return nullptr;
	}

	if (lRes != ERROR_SUCCESS) {
		throw RegistryException(lRes);
	}

	s_Result.resize(dwlen);
	lRes = RegQueryValueEx(m_hKey_, p_strValueName.c_str(), NULL, NULL, &s_Result[0], &dwlen);
	return &s_Result;
}

bool RegistryKey::SetValue(const String& p_strValueName, const std::vector<byte>& p_vbValue) const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	if (!(m_right_ & RegistryKeyAccess::Write)) {
		throw AccessDeniedException();
	}

	LSTATUS lRes = RegSetValueEx(m_hKey_, p_strValueName.c_str(), 0, REG_BINARY, p_vbValue.data(), p_vbValue.size());
	return ERROR_SUCCESS == lRes;
}

bool RegistryKey::SetValue(const String& p_strValueName, const String& p_strValue) const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	if (!(m_right_ & RegistryKeyAccess::Write)) {
		throw AccessDeniedException();
	}

	LSTATUS lRes
	    = RegSetValueEx(m_hKey_, p_strValueName.c_str(), 0, REG_SZ, reinterpret_cast<const byte*>(p_strValue.c_str()), sizeof(TCHAR) * p_strValue.size());

	return ERROR_SUCCESS == lRes;
}

bool RegistryKey::SetValue(const String& p_strValueName, const StringList& p_vstrValues) const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	if (!(m_right_ & RegistryKeyAccess::Write)) {
		throw AccessDeniedException();
	}

	std::vector<byte> vbValue;
	for (const String& strValue : p_vstrValues) {
		std::vector<byte> vbItem;
		vbItem.resize(strValue.size() * sizeof(TCHAR));
		memcpy_s(vbItem.data(), vbItem.size(), strValue.c_str(), vbItem.size());
		vbValue.insert(vbValue.end(), vbItem.begin(), vbItem.end());
		vbValue.insert(vbValue.end(), 2, '\0');
	}
	vbValue.insert(vbValue.end(), 2, '\0');

	LSTATUS lRes = RegSetValueEx(m_hKey_, p_strValueName.c_str(), 0, REG_MULTI_SZ, vbValue.data(), vbValue.size());

	return ERROR_SUCCESS == lRes;
}

bool RegistryKey::SetValue(const String& p_strValueName, const int32_t& p_i32value, bool p_bisBigEndian) const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	if (!(m_right_ & RegistryKeyAccess::Write)) {
		throw AccessDeniedException();
	}

	LSTATUS lRes = RegSetValueEx(
	    m_hKey_,
	    p_strValueName.c_str(),
	    0,
	    p_bisBigEndian ? REG_DWORD_BIG_ENDIAN : REG_DWORD,
	    reinterpret_cast<const byte*>(&p_i32value),
	    sizeof(int32_t)
	);
	return ERROR_SUCCESS == lRes;
}

bool RegistryKey::SetValue(const String& p_strValueName, const int64_t& p_i64value) const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	if (!(m_right_ & RegistryKeyAccess::Write)) {
		throw AccessDeniedException();
	}

	LSTATUS lRes = RegSetValueEx(m_hKey_, p_strValueName.c_str(), 0, REG_QWORD, reinterpret_cast<const byte*>(&p_i64value), sizeof(int64_t));
	return ERROR_SUCCESS == lRes;
}

bool RegistryKey::SetValue(const String& p_strValueName, const double& p_dvalue) const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	if (!(m_right_ & RegistryKeyAccess::Write)) {
		throw AccessDeniedException();
	}

	LSTATUS lRes = RegSetValueEx(m_hKey_, p_strValueName.c_str(), 0, REG_BINARY, reinterpret_cast<const byte*>(&p_dvalue), sizeof(double));
	return ERROR_SUCCESS == lRes;
}

bool RegistryKey::SetValue(const String& p_strValueName, const bool& p_bvalue) const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	if (!(m_right_ & RegistryKeyAccess::Write)) {
		throw AccessDeniedException();
	}

	LSTATUS lRes = RegSetValueEx(m_hKey_, p_strValueName.c_str(), 0, REG_BINARY, reinterpret_cast<const byte*>(&p_bvalue), sizeof(bool));
	return ERROR_SUCCESS == lRes;
}

bool RegistryKey::DeleteValue(const String& p_strValueName) const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	if (!(m_right_ & RegistryKeyAccess::Write)) {
		throw AccessDeniedException();
	}

	LSTATUS lRes = RegDeleteValue(m_hKey_, p_strValueName.c_str());
	if (lRes == ERROR_FILE_NOT_FOUND) {
		throw FileNotFoundException();
	}

	return ERROR_SUCCESS == lRes;
}

DWORD RegistryKey::GetValueType(const String& p_strValueName) const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	DWORD   dwType = REG_NONE;
	LSTATUS lRes   = RegQueryValueEx(m_hKey_, p_strValueName.c_str(), NULL, &dwType, NULL, NULL);
	if (lRes == ERROR_SUCCESS) {
		throw RegistryException(lRes);
	}

	return dwType;
}

StringList RegistryKey::GetValueNames() const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	StringList vsNames;
	DWORD      dwMaxLen;
	LSTATUS    lRes = RegQueryInfoKey(m_hKey_, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dwMaxLen, NULL, NULL, NULL);
	if (ERROR_SUCCESS != lRes) {
		throw RegistryException(lRes);
	}

	DWORD dwIdx = 0, dwCopiedLen = ++dwMaxLen;
	auto  vwcValueName = std::vector<TCHAR>(dwMaxLen);

	while (true) {
		lRes = RegEnumValue(m_hKey_, dwIdx, &vwcValueName[0], &dwCopiedLen, NULL, NULL, NULL, NULL);
		if (ERROR_SUCCESS == lRes) {
			vsNames.emplace_back(vwcValueName.data());
			++dwIdx;
			dwCopiedLen = dwMaxLen;
		} else if (ERROR_NO_MORE_ITEMS == lRes) {
			break;
		} else {
			throw RegistryException(lRes);
		}
	}
	return vsNames;
}

StringList RegistryKey::GetSubKeyNames() const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	DWORD dwMaxLen = 255;

	size_t nIdx          = 0;
	DWORD  dwCopiedLen   = dwMaxLen;
	auto   vwcSubKeyName = std::vector<TCHAR>(255);

	StringList vsNames;
	LSTATUS    lRes;
	while (true) {
		lRes = RegEnumKeyEx(m_hKey_, nIdx, &vwcSubKeyName[0], &dwCopiedLen, NULL, NULL, NULL, NULL);
		if (ERROR_SUCCESS == lRes) {
			vsNames.emplace_back(vwcSubKeyName.data());
			++nIdx;
			dwCopiedLen = dwMaxLen;
		} else if (ERROR_NO_MORE_ITEMS == lRes) {
			break;
		} else {
			throw RegistryException(lRes);
		}
	}

	return vsNames;
}

size_t RegistryKey::GetSubKeyCount() const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	DWORD   dwSubKeyCount = 0;
	LSTATUS lRes          = RegQueryInfoKey(m_hKey_, NULL, NULL, NULL, &dwSubKeyCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if (lRes != ERROR_SUCCESS) {
		throw RegistryException(lRes);
	}
	return dwSubKeyCount;
}

size_t RegistryKey::GetValueCount() const {
	if (nullptr == m_hKey_) {
		throw NullPointerReferenceException();
	}

	DWORD   dwValueCount = 0;
	LSTATUS lRes         = RegQueryInfoKey(m_hKey_, NULL, NULL, NULL, NULL, NULL, NULL, &dwValueCount, NULL, NULL, NULL, NULL);
	if (lRes != ERROR_SUCCESS) {
		throw RegistryException(lRes);
	}

	return dwValueCount;
}

void RegistryKey::Close() {
	if (m_hKey_) {
		RegCloseKey(m_hKey_);
		m_hKey_ = nullptr;
	}
}

_WIN32_END