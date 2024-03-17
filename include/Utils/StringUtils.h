#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <format>
#include <regex>
#include <string>
#include <vector>

#include "utils_def.h"

#define FORMAT(fmt, ...) std::format(TEXT(fmt), __VA_ARGS__)
#define FORMAT_SZ(fmt, ...) FORMAT(fmt, __VA_ARGS__).c_str()

#define PB_TO_S(pByte) reinterpret_cast<const TCHAR*>(pByte)
#define PU8STR_TO_PSTR(u8str) reinterpret_cast<const char*>(u8str)

_UTILS_BEGIN

using String       = std::basic_string<TCHAR>;
using StringList   = std::vector<String>;
using Regex        = std::basic_regex<String::value_type>;
using MatchResults = std::match_results<String::const_iterator>;

class UTILS_API StringUtils {
public:
	/// <summary>
	/// 使用给定字符串组，并在头和尾去掉满足给定数据的中的任何一个，不改变原因字符串
	/// </summary>
	/// <typeparam name="_CharT">字符串字符类型</typeparam>
	/// <param name="pc_str">将要处理的字符串</param>
	/// <param name="pc_lists">自定义去除的串</param>
	/// <returns>处理过后的字符串</returns>
	template <typename _CharT>
	static std::basic_string<_CharT> Trim(
	    const std::basic_string<_CharT>& pc_str, std::initializer_list<const std::basic_string<_CharT>> pc_lists = {}) {
		std::basic_string<_CharT> res { pc_str };
		if (res.empty())
			return res;

		size_t nLen;
		bool bFlag = true;
		while ((nLen = res.size()) > 0 && bFlag) {
			bFlag = false;
			switch (res[0]) {
				case _CharT(' '):
				case _CharT('\r'):
				case _CharT('\n'):
					bFlag = true;
					res.erase(0, 1);
					break;
				default: {
					for (auto& item : pc_lists) {
						if (res.starts_with(item)) {
							res.erase(0, item.size());
							bFlag = true;
							break; // for
						}
					}
					break; // switch
				}
			}

			if (nLen == 0) {
				break;
			}

			switch (res[nLen - 1]) {
				case _CharT(' '):
				case _CharT('\r'):
				case _CharT('\n'):
					bFlag = true;
					res.erase(nLen - 1, 1);
					break;
				default: {
					for (auto& item : pc_lists) {
						if (res.ends_with(item)) {
							res.erase(nLen - item.size(), item.size());
							bFlag = true;
							break; // for
						}
					}
					break; // switch
				}
			}
		}
		return res;
	}

	/// <summary>
	/// 使用给定字符串组，并在头和尾去掉满足给定数据的中的任何一个，不改变原有字符串
	/// </summary>
	/// <typeparam name="_CharT">字符串字符类型</typeparam>
	/// <param name="pc_str">将要处理的字符串</param>
	/// <returns>处理后的字符串</returns>
	template <typename _CharT>
	static std::basic_string<_CharT> Trim(const _CharT* pc_szTrimValue, std::initializer_list<const std::basic_string<_CharT>>&& pc_lists = {}) {
		/*if (nullptr == pc_szTrimValue) {
		    throw NullPointerReferenceException(TEXT("pc_szTrimValue"));
		}*/
		return Trim<_CharT>(
		    std::basic_string<_CharT> { pc_szTrimValue }, std::forward<std::initializer_list<const std::basic_string<_CharT>>>(pc_lists));
	}

	/// <summary>
	/// 使用指定分割符分割给定的字符串，不改变原有数据
	/// </summary>
	/// <typeparam name="_CharT">字符串的字符类型</typeparam>
	/// <param name="pc_strValue">将要分割的字符串</param>
	/// <param name="pc_strSep">分隔符</param>
	/// <returns>分割后的字符串数组</returns>
	template <class _CharT>
	static std::vector<std::basic_string<_CharT>> Split(const std::basic_string<_CharT>& pc_strValue, const std::basic_string<_CharT>& pc_strSep) {
		std::vector<std::basic_string<_CharT>> vstrResult;
		size_t nPosBegin     = 0;
		size_t nPosSeperator = pc_strValue.find(pc_strSep);

		while (nPosSeperator != pc_strValue.npos) {
			vstrResult.push_back(pc_strValue.substr(nPosBegin, nPosSeperator - nPosBegin)); //
			nPosBegin     = nPosSeperator + pc_strSep.size(); // 分隔符的下一个元素
			nPosSeperator = pc_strValue.find(pc_strSep, nPosBegin);
		}

		if (nPosBegin != pc_strValue.length()) // 指向最后一个元素，加进来
			vstrResult.push_back(pc_strValue.substr(nPosBegin));

		return vstrResult;
	}

	/// <summary>
	/// 使用指定分割符分割给定的字符串，不改变原有数据
	/// </summary>
	/// <typeparam name="_CharT">字符串的字符类型</typeparam>
	/// <param name="pc_strValue">将要分割的字符串</param>
	/// <param name="p_cszSeperator">分隔符</param>
	/// <returns>分割后的字符串数组</returns>
	template <class _CharT>
	static std::vector<std::basic_string<_CharT>> Split(const std::basic_string<_CharT>& pc_strValue, const _CharT* p_cszSeperator) {
		return Split(pc_strValue, std::basic_string<_CharT>(p_cszSeperator));
	}

	/// <summary>
	/// 用给定字符串作为连接符，连接给定的字符串数组
	/// </summary>
	/// <typeparam name="_CharT">字符串的字符类型</typeparam>
	/// <param name="pc_vstrValues">将要连接的字符串</param>
	/// <param name="p_cszSperator">连接符</param>
	/// <returns>连接后的字符串</returns>
	template <class _CharT>
	static std::basic_string<_CharT> Join(const std::vector<std::basic_string<_CharT>>& pc_vstrValues, const _CharT* p_cszSperator) {
		if (0 == pc_vstrValues.size()) {
			return std::basic_string<_CharT>();
		}

		if (1 == pc_vstrValues.size()) {
			return pc_vstrValues[0];
		}

		std::basic_string<_CharT> strRes;
		std::for_each_n(pc_vstrValues.begin(), pc_vstrValues.size() - 1,
		    [&strRes, &p_cszSperator](const auto& strItem) { strRes.append(strItem + p_cszSperator); });
		strRes.append(pc_vstrValues[pc_vstrValues.size() - 1]);

		return strRes;
	}

	/// <summary>
	/// 尝试将字符串转为指定的数值类型
	/// </summary>
	/// <typeparam name="_NumT">将要转换的数值类型</typeparam>
	/// <typeparam name="_CharT">字符串的字符类型</typeparam>
	/// <param name="pc_strValue">将要转换的字符串</param>
	/// <param name="p_nBase">转换进制格式</param>
	/// <returns>转换后的数值</returns>
	template <typename _NumT,
	    class _CharT,
	    typename = std::enable_if_t<std::_Is_any_of_v<_CharT, char, wchar_t> && std::_Is_any_of_v<_NumT, int, double, long, size_t>>>
	static _NumT ValueOf(const std::basic_string<_CharT>& pc_strValue, int p_nBase = 10) {
		if constexpr (std::is_same_v<_NumT, int>) {
			return std::stoi(pc_strValue, nullptr, p_nBase);
		} else if constexpr (std::is_same_v<_NumT, double>) {
			return std::stod(pc_strValue, nullptr);
		} else if constexpr (std::is_same_v<_NumT, long>) {
			return std::stol(pc_strValue, nullptr, p_nBase);
		} else if constexpr (std::is_same_v<_NumT, size_t>) {
			return static_cast<size_t>(std::stoul(pc_strValue, nullptr, p_nBase));
		}
	}

	/// <summary>
	/// 尝试将字符串转为指定的数值类型
	/// </summary>
	/// <typeparam name="_NumT">将要转换的数值类型</typeparam>
	/// <typeparam name="_CharT">字符串的字符类型</typeparam>
	/// <param name="pc_strValue">将要转换的字符串</param>
	/// <param name="p_nBase">转换进制格式</param>
	/// <returns>转换后的数值</returns>
	template <typename _T,
	    class _CharT,
	    typename = std::enable_if_t<std::_Is_any_of_v<_CharT, char, wchar_t> && std::_Is_any_of_v<_T, int, double, long, size_t>>>
	static _T ValueOf(const _CharT* p_cszValue, int p_nBase = 10) {
		return ValueOf<_T>(std::basic_string<_CharT>(p_cszValue), p_nBase);
	}

	/// <summary>
	/// 将给定的数值类型转为字符串
	/// </summary>
	/// <typeparam name="_NumT">将要转换的数值的类型</typeparam>
	/// <param name="p_num">将要转换的数值</param>
	/// <returns>转换后的结果</returns>
	template <typename _NumT, typename = std::enable_if_t<std::is_arithmetic_v<_NumT>>>
	static String ToString(_NumT p_num) {
#ifdef _UNICODE
		return std::to_wstring(p_num);
#else
		return std::to_string(p_num);
#endif // _UNICODE
	}

	/// <summary>
	/// 将多字节字字符串转为宽字节字符串
	/// </summary>
	/// <param name="pc_str">将要转换的字符串</param>
	/// <param name="p_nCodePage">指定转换的代码页，默认为UTF8</param>
	/// <returns>转换后的字符串</returns>
	static std::wstring MultiByteToWideChar(const std::string& pc_str, int p_nCodePage = CP_UTF8);

	/// <summary>
	/// 将宽字节字符串转为多字节字字符串
	/// </summary>
	/// <param name="pc_str">将要转换的字符串</param>
	/// <param name="p_nCodePage">指定转换的代码页，默认为UTF8</param>
	/// <returns>转换后的字符串</returns>
	static std::string WideCharToMultiByte(const std::wstring& pc_wstr, int p_nCodePage = CP_UTF8);

	/// <summary>
	/// 将给定字符串转当前平台使用的编码方式的字符串
	/// </summary>
	/// <param name="pc_str">将要转换的字符串</param>
	/// <returns>转换后的字符串</returns>
	template <typename _CharT, typename = std::enable_if_t<std::_Is_any_of_v<_CharT, char, wchar_t>>>
	static String ToCurrentEncodingString(const std::basic_string<_CharT> pc_strValue) {
		if constexpr (std::is_same_v<_CharT, TCHAR>) {
			return pc_strValue;
		} else {
#ifndef _UNICODE
			return StringUtils::WideCharToMultiByte(pc_strValue);
#else
			return StringUtils::MultiByteToWideChar(pc_strValue);
#endif // _UNICODE
		}
	}
};

_UTILS_END