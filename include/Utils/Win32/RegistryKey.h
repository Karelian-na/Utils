#pragma once
#include "Win32Exceptions.h"

#pragma warning(push)
#pragma warning(disable : 4251)

_WIN32_BEGIN

enum RegistryKeyAccess {
	Read  = 0x01,
	Write = 0x10,
	All   = 0x11,
};

constexpr int GetNativeRegistryKeyAccess(const RegistryKeyAccess pc_right) {
	switch (pc_right) {
		case RegistryKeyAccess::Read:
			return KEY_READ;
		case RegistryKeyAccess::Write:
			return KEY_WRITE;
		case RegistryKeyAccess::All:
			return KEY_ALL_ACCESS;
		default:
			return KEY_READ;
	}
}

class UTILS_API RegistryKey {
public:
private:
	/// <summary>
	/// 指明析构函数被调用时, 当前对象是否应该进行析构操作
	/// </summary>
	bool m_bNeedDestruct_ = true;

	/// <summary>
	/// 注册表键的句柄
	/// </summary>
	HKEY m_hKey_;

	/// <summary>
	/// 当前注册表项名
	/// </summary>
	String m_strName_;

	/// <summary>
	/// 当前注册表项路径
	/// </summary>
	String m_strPath_;

	/// <summary>
	/// 打开注册表时的权限
	/// </summary>
	RegistryKeyAccess m_right_ = RegistryKeyAccess::Read;

private:
	/// <summary>
	/// 传入参数类型为系统注册表键句柄类型的构造函数, 用于内部构造系统根键
	/// </summary>
	/// <param name="hKey">系统根键句柄</param>
	explicit RegistryKey(HKEY p_hKey);

public:
	/// <summary>
	/// 无参构造函数
	/// </summary>
	RegistryKey() = default;

	/// <summary>
	/// 拷贝构造函数, 拷贝的对象将不会进行析构操作
	/// </summary>
	RegistryKey(const RegistryKey& pc_key);

	/// <summary>
	/// 移动构造函数, 转移系统句柄, 使其不进行关闭句柄操作
	/// </summary>
	RegistryKey(RegistryKey&& p_right) noexcept;

	/// <summary>
	/// 移动复制函数,作用同移动构造函数
	/// </summary>
	RegistryKey& operator=(RegistryKey&& p_right) noexcept;

	/// <summary>
	/// 析构函数
	/// </summary>
	~RegistryKey() noexcept;

	DECLARE_READONLY_PROPERTY(HKEY, HKey);
	DECLARE_READONLY_PROPERTY(String, Name);
	DECLARE_READONLY_PROPERTY(String, Path);

	/// <summary>
	/// 获取当前注册表对象对应的系统句柄
	/// </summary>
	/// <returns>系统句柄</returns>
	const HKEY& GetHkey() const {
		return m_hKey_;
	}

	/// <summary>
	/// judge if current key has no value
	/// </summary>
	const bool Empty() const noexcept {
		return m_hKey_ == NULL;
	}

	/// <summary>
	/// get current key's name.
	/// <para>For Example: if path is 'SOFTWARE\\Microsoft', then return 'Microsoft'</para>
	/// </summary>
	/// <returns>the name of current key</returns>
	const String& GetName() const noexcept {
		return m_strName_;
	}

	/// <summary>
	/// get current key's path
	/// </summary>
	/// <returns>current key's full path</returns>
	const String& GetPath() const noexcept {
		return m_strPath_;
	}

	/// <summary>
	/// open a sub key with its name
	/// </summary>
	/// <param name="pc_strSubKeyName">-- the sub key name will be opened</param>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="FileNotFoundException"></exception>
	/// <exception cref="InvalidArgumentException"></exception>
	/// <exception cref="NullPointerReferenceException"></exception>
	/// <returns>if true, the opened sub key, otherwise the RegistryKey::Empty</returns>
	RegistryKey OpenSubKey(const String& pc_strSubKeyName, const RegistryKeyAccess pc_right = RegistryKeyAccess::Read) const;

	/// <summary>
	/// create a sub key or open if exists
	/// </summary>
	/// <param name="pc_strSubKeyName">-- the sub key name will be created</param>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="AccessDeniedException"></exception>
	/// <exception cref="InvalidArgumentException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>if exists, then open it; otherwise the created sub key, return RegistryKey::Empty when failed</returns>
	RegistryKey CreateSubKey(const String& pc_strSubKeyName, const RegistryKeyAccess pc_right = RegistryKeyAccess::Read) const;

	/// <summary>
	/// delete the specified name of sub key
	/// </summary>
	/// <param name="pc_strSubKeyName">-- the sub key name will be deleted</param>
	/// <exception cref="FileNotFoundException"></exception>
	/// <exception cref="AccessDeniedException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>the deleted result, if not exists, return false</returns>
	bool DeleteSubKey(const String& pc_strSubKeyName) const;

	/// <summary>
	/// get the raw value which name is the passed value
	/// </summary>
	/// <param name="pc_strValueName">-- the sub key name will be deleted</param>
	/// <param name="lpdwType">-- the value type will be passed out.</param>
	/// <param name="p_dwReserved">reserved must be zero</param>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>a pointer that is vector of byte to the data, nullptr if not exists or failed</returns>
	std::vector<byte>* GetValue(const String& pc_strValueName, LPDWORD lpdwType, DWORD p_dwReserved) const;

	/// <summary>
	/// <para> get the specified type value which name is the passed value，</para>
	/// <para> this function is strict with the reg type，</para>
	/// <para> if the store reg type is not equal to the given type，the function will throw RegistryException </para>
	/// </summary>
	/// <typeparam name="_Type">the value type</typeparam>
	/// <param name="pc_strValueName">-- the value name which will be get</param>
	/// <param name="pc_default">-- the default value when doesn't exist</param>
	/// <exception cref="RegistryException"></exception>
	template <typename _Type>
	_Type GetValue(const String& pc_strValueName, const _Type& pc_default = {}) const {
		DWORD              dwType  = REG_NONE;
		std::vector<byte>* pvbData = GetValue(pc_strValueName, &dwType, 0);
		if (!pvbData) {
			return pc_default;
		}

		bool bMatched = false;
		if constexpr (std::is_same_v<_Type, String>) {
			bMatched = dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_LINK;
		} else if constexpr (std::is_same_v<_Type, StringList>) {
			bMatched
			    = dwType == REG_MULTI_SZ || dwType == REG_RESOURCE_LIST || dwType == REG_RESOURCE_REQUIREMENTS_LIST || dwType == REG_FULL_RESOURCE_DESCRIPTOR;
		} else if constexpr ((sizeof(DWORD) == sizeof(_Type) && std::is_arithmetic_v<_Type>) || std::is_same_v<_Type, bool>) {
			bMatched = dwType == REG_DWORD || dwType == REG_DWORD_BIG_ENDIAN || dwType == REG_DWORD_BIG_ENDIAN || dwType == REG_BINARY;
		} else if constexpr (sizeof(int64_t) == sizeof(_Type) && std::is_arithmetic_v<_Type>) {
			bMatched = dwType == REG_QWORD || dwType == REG_QWORD_LITTLE_ENDIAN || dwType == REG_BINARY;
		} else if constexpr (std::is_same_v<_Type, std::vector<byte>>) {
			bMatched = dwType == REG_BINARY;
		} else {
			// static_assert(false, "reg type only support String, StringList, arithmetic type, std::vector<byte>");
		}

		if (!bMatched) {
			throw RegistryException(TEXT("result type is not matched of the given type!"));
		}

		_Type res {};
		if constexpr (std::is_same_v<_Type, String>) {
			res = reinterpret_cast<TCHAR*>(pvbData->data());

		} else if constexpr (std::is_same_v<_Type, StringList>) {
			size_t idx = 0, count = 0;
			while (pvbData->at(idx) != TEXT('\0')) {
				String curStrValue = reinterpret_cast<TCHAR*>(&pvbData->at(idx));
				res.push_back(std::move(curStrValue));
				idx += (1 + curStrValue.size()) * sizeof(TCHAR);
				++count;
			}

		} else if constexpr (std::is_same_v<_Type, std::vector<byte>>) {
			return *pvbData;

		} else {
			res = *reinterpret_cast<_Type*>(pvbData->data());
		}

		return res;
	}

	/// <summary>
	/// create or replace a new REG_BINARY value
	/// </summary>
	/// <param name="pc_strValueName">-- the value name</param>
	/// <param name="vbValue">-- the value</param>
	/// <exception cref="AccessDeniedException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>true if operating was succeeded, otherwise false</returns>
	bool SetValue(const String& pc_strValueName, const std::vector<byte>& vbValue) const;

	/// <summary>
	/// create or replace a new REG_SZ value
	/// </summary>
	/// <param name="pc_strValueName">-- the value name</param>
	/// <param name="pc_strValue">-- the value</param>
	/// <exception cref="AccessDeniedException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>true if operating was succeeded, otherwise false</returns>
	bool SetValue(const String& pc_strValueName, const String& pc_strValue) const;

	/// <summary>
	/// create or replace a new REG_NONE value
	/// </summary>
	/// <param name="pc_strValueName">-- the value name</param>
	/// <param name="pc_vstrValues">-- the value</param>
	/// <exception cref="AccessDeniedException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>true if operating was succeeded, otherwise false</returns>
	bool SetValue(const String& pc_strValueName, const StringList& pc_vstrValues) const;

	/// <summary>
	/// create or replace a new REG_DWORD value
	/// </summary>
	/// <param name="pc_strValueName">-- the value name</param>
	/// <param name="pc_dwValue">-- the value</param>
	/// <param name="pc_bIsBigEndian">-- specify if need to store with bigEndian</param>
	/// <exception cref="AccessDeniedException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>true if operating was succeeded, otherwise false</returns>
	bool SetValue(const String& pc_strValueName, const int32_t& pc_dwValue, const bool pc_bIsBigEndian = false) const;

	/// <summary>
	/// create or replace a new REG_BINARY value with double
	/// </summary>
	/// <param name="pc_strValueName">-- the value name</param>
	/// <param name="dValue">-- the value</param>
	/// <exception cref="AccessDeniedException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>true if operating was succeeded, otherwise false</returns>
	bool SetValue(const String& pc_strValueName, const double& pc_dValue) const;

	/// <summary>
	/// create or replace a new REG_QWORD value
	/// </summary>
	/// <param name="pc_strValueName">-- the value name</param>
	/// <param name="n64Value">-- the value</param>
	/// <exception cref="AccessDeniedException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>true if operating was succeeded, otherwise false</returns>
	bool SetValue(const String& pc_strValueName, const int64_t& pc_n64Value) const;

	/// <summary>
	/// create or replace a new REG_BINARY value
	/// </summary>
	/// <param name="pc_strValueName">-- the value name</param>
	/// <param name="bool">-- the value</param>
	/// <exception cref="AccessDeniedException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>true if operating was succeeded, otherwise false</returns>
	bool SetValue(const String& pc_strValueName, const bool& pc_bValue) const;

	/// <summary>
	/// delete a value if exists
	/// </summary>
	/// <param name="pc_strValueName">-- the value name</param>
	/// <exception cref="FileNotFoundException"></exception>
	/// <exception cref="AccessDeniedException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>true if operating was succeeded, otherwise false</returns>
	bool DeleteValue(const String& pc_strValueName) const;

	/// <summary>
	/// get the value's type
	/// </summary>
	/// <param name="pc_strValueName">-- the value name</param>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>the value type of specified key</returns>
	DWORD GetValueType(const String& pc_strValueName) const;

	/// <summary>
	/// enum the value names
	/// </summary>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>The names of all values for the current key</returns>
	StringList GetValueNames() const;

	/// <summary>
	/// enum its sub key names
	/// </summary>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>The names of all children of the current key</returns>
	StringList GetSubKeyNames() const;

	/// <summary>
	/// get the sub key count
	/// </summary>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>the number of subkeys for the current key</returns>
	size_t GetSubKeyCount() const;

	/// <summary>
	/// get the value count
	/// </summary>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="NullPointerReferenceException">thrown when current key was closed</exception>
	/// <returns>the number of values for the current key</returns>
	size_t GetValueCount() const;

	/// <summary>
	/// judge if passed key's hKey is equal to current key's hKey.
	/// </summary>
	/// <param name="other">another RegistryKey object</param>
	/// <returns></returns>
	inline bool operator==(const RegistryKey& p_other) const noexcept {
		return m_hKey_ == p_other.m_hKey_;
	}

	/// <summary>
	/// close current key
	/// </summary>
	void Close();

	friend class Registry;
};

_WIN32_END

#pragma warning(pop)