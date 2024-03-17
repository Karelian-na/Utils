#pragma once
#include "StringUtils.h"

_UTILS_BEGIN

std::wstring StringUtils::MultiByteToWideChar(const std::string& pc_str, int p_nCodePage) {
	std::vector<wchar_t> result;
	DWORD dwLen = ::MultiByteToWideChar(p_nCodePage, 0, pc_str.c_str(), -1, NULL, 0);
	result.resize(dwLen);
	::MultiByteToWideChar(p_nCodePage, 0, pc_str.c_str(), -1, &result[0], dwLen);
	return result.data();
}

std::string StringUtils::WideCharToMultiByte(const std::wstring& p_wstr, int p_nCodePage) {
	std::vector<char> result;
	DWORD dwLen = ::WideCharToMultiByte(p_nCodePage, 0, p_wstr.c_str(), -1, NULL, 0, NULL, NULL);
	result.resize(dwLen);
	::WideCharToMultiByte(p_nCodePage, 0, p_wstr.c_str(), -1, &result[0], dwLen, NULL, NULL);
	return result.data();
}

_UTILS_END