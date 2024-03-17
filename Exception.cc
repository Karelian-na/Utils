#include "Exception.h"

#include <mutex>

_UTILS_BEGIN

String g_strFinalMessage;
std::mutex g_mtxFinalMessage;

std::map<long, const TCHAR*> g_mpErrMsgs {
	{ UtilErrors::ERR_UNKNOWN, TEXT("未知错误") },
	{ UtilErrors::ERR_SYSTEM, TEXT("系统调用错误") },
	{ UtilErrors::ERR_COM_INTEROP, TEXT("COM调用错误") },
	{ UtilErrors::ERR_FILE_NOT_FOUND, TEXT("指定文件不存在") },
	{ UtilErrors::ERR_NULLPTR, TEXT("空指针的引用") },
	{ UtilErrors::ERR_INVALID_ARGUMENT, TEXT("无效的参数") },
};

UTILS_API void RegisterErrorMessages(const std::map<long, const TCHAR*>& pc_mpErrMsgs) {
	g_mpErrMsgs.insert(pc_mpErrMsgs.cbegin(), pc_mpErrMsgs.cend());
}
 
UTILS_API const TCHAR* GetErrorMessage(long p_lCode) {
	const auto iter = g_mpErrMsgs.find(p_lCode);

	if (iter == g_mpErrMsgs.end()) {
		return TEXT("");
	}
	return iter->second;
}

UTILS_API const TCHAR* ConcatBaseErrorMessage(long p_lBaseErrCode, const TCHAR* p_cwszMsg) {
	std::lock_guard<std::mutex> guard(g_mtxFinalMessage);

	g_strFinalMessage = GetErrorMessage(p_lBaseErrCode);
	if (!g_strFinalMessage.empty() && p_cwszMsg) {
		g_strFinalMessage.append(TEXT("："));
	}

	if (p_cwszMsg) {
		g_strFinalMessage += StringUtils::Trim(p_cwszMsg, { String(TEXT("。")) }) + TEXT("！");
	}

	return g_strFinalMessage.c_str();
}

_UTILS_END