#pragma once
#include "RegistryKey.h"

_WIN32_BEGIN

class UTILS_API Registry {
public:
	/// <summary>
	/// 系统键: HKEY_USERS, 缩写HKU
	/// </summary>
	static const RegistryKey Users;

	/// <summary>
	/// 系统键: HKEY_CURRENT_USER, 缩写HKCU
	/// </summary>
	static const RegistryKey CurrentUser;

	/// <summary>
	/// 系统键: HKEY_CLASSES_ROOT, 缩写HKCR
	/// </summary>
	static const RegistryKey ClassesRoot;

	/// <summary>
	/// 系统键: HKEY_LOCAL_MACHINE, 缩写HKLM
	/// </summary>
	static const RegistryKey LocalMachine;

	/// <summary>
	/// 系统键: HKEY_CURRENT_CONFIG, 缩写HKCC
	/// </summary>
	static const RegistryKey CurrentConfig;

	/// <summary>
	/// 系统键: HKEY_PERFORMANCE_DATA, 缩写HKPD
	/// </summary>
	static const RegistryKey PerformanceData;

private:
	/// <summary>
	/// 处理用户传入的完整项的路径, 分割根键名和子项路径, 修改传入的参数为分割后的子项的路径
	/// </summary>
	/// <param name="p_strKeyName">将要处理的完整项的路径</param>
	/// <returns>返回完整路径中指定的根键所定义的对象
	/// <para>例如: HKLM\\SOFTWARE\\xxx, 则返回LocalMachine</para>
	/// </returns>
	static const RegistryKey& ProcessKeyName_(String& p_strKeyName);

public:
	/// <summary>
	/// 根据给定的项的路径和键名获取注册表中对应的值
	/// </summary>
	/// <param name="pc_strKeyName">将要获取的项的路径</param>
	/// <param name="pc_strValueName">将要获取的键名</param>
	/// <param name="p_pdwType">获取的值的类型</param>
	/// <exception cref="RegistryException"></exception>
	/// <returns></returns>
	static std::vector<byte>* GetValue(const String& pc_strKeyName, const String& pc_strValueName, LPDWORD p_pdwType);

	/// <summary>
	/// 根据给定的项的路径和键名写入REG_BINARY类型的值到注册表
	/// </summary>
	/// <param name="pc_strKeyName">注册表项路径, 以系统键缩写开始</param>
	/// <param name="pc_strValueName">将要设置的值的名称</param>
	/// <param name="vbValue">将要设置BINARY的值</param>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="NullPointerReferenceException"></exception>
	/// <returns></returns>
	static bool SetValue(const String& pc_strKeyName, const String& pc_strValueName, const std::vector<byte>& p_vbValue);

	/// <summary>
	/// 根据给定的项的路径和键名写入REG_SZ类型的值到注册表
	/// </summary>
	/// <param name="pc_strKeyName">注册表项路径, 以系统键缩写开始</param>
	/// <param name="pc_strValueName">将要设置的值的名称</param>
	/// <param name="pc_strValue">将要设置的SZ的值</param>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="NullPointerReferenceException"></exception>
	/// <returns></returns>
	static bool SetValue(const String& pc_strKeyName, const String& pc_strValueName, const String& pc_strValue);

	/// <summary>
	/// 根据给定的项的路径和键名写入REG_MULTI_SZ类型的值到注册表
	/// </summary>
	/// <param name="pc_strKeyName">注册表项路径, 以系统键缩写开始</param>
	/// <param name="pc_strValueName">将要设置的值的名称</param>
	/// <param name="pc_vstrValues">将要设置的MULTI_SZ的值</param>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="NullPointerReferenceException"></exception>
	/// <returns></returns>
	static bool SetValue(const String& pc_strKeyName, const String& pc_strValueName, const StringList& pc_vstrValues);

	/// <summary>
	/// 根据给定的项的路径和键名写入REG_DWORD类型的值到注册表
	/// </summary>
	/// <param name="pc_strKeyName">注册表项路径, 以系统键缩写开始</param>
	/// <param name="pc_strValueName">将要设置的值的名称</param>
	/// <param name="pc_dwValue">将要设置的DWORD的值</param>
	/// <param name="pc_bIsBigEndian">是否以大端存储</param>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="NullPointerReferenceException"></exception>
	/// <returns></returns>
	static bool SetValue(const String& pc_strKeyName, const String& pc_strValueName, const INT32 pc_dwValue, const bool pc_bIsBigEndian = false);

	/// <summary>
	/// 根据给定的项的路径和键名写入REG_QWORD类型的值到注册表
	/// </summary>
	/// <param name="pc_strKeyName">注册表项路径, 以系统键缩写开始</param>
	/// <param name="pc_strValueName">将要设置的值的名称</param>
	/// <param name="pc_n64Value">将要设置的QWORD的值</param>
	/// <exception cref="RegistryException"></exception>
	/// <exception cref="NullPointerReferenceException"></exception>
	/// <returns></returns>
	static bool SetValue(const String& pc_strKeyName, const String& pc_strValueName, const INT64 pc_n64Value);
};

_WIN32_END