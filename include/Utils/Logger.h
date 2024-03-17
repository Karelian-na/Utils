#pragma once
#include <fstream>

#include "StringUtils.h"

#pragma warning(push)
#pragma warning(disable : 4251)

_UTILS_BEGIN

enum class UTILS_API LogLevel {
	NONE,
	TRACE,
	DEBUG,
	INFO,
	WARN,
	ERR,
};

#ifdef _DEBUG
#define DEFAULT_LOG_LEVEL LogLevel::DEBUG
#else
#define DEFAULT_LOG_LEVEL LogLevel::INFO
#endif // DEBUG

class UTILS_API Logger {
private:
	mutable bool m_bFirstLog_ = true;

	String m_strName_;
	LogLevel m_logLevel_;
	String m_strLogFilePath_;

private:
	void Log_(const TCHAR* p_cszFuncName, const String& pc_strType, const String& pc_strMsg) const;

public:
	/// <summary>
	/// 日志级别
	/// </summary>
	DECLARE_PROPERTY_WITH_BODY(LogLevel, LogLevel, m_logLevel_);

	/// <summary>
	/// 创建日志记录器实例
	/// </summary>
	/// <param name="pc_strName">日志记录器的名称</param>
	/// <param name="pc_strFilePath_">日志文件的位置，默认为应用程序安装目录下的logs目录</param>
	/// <param name="p_level">记录级别，默认为DEFAULT_LOG_LEVEL</param>
	/// <returns>创建的日志记录器实例</returns>
	Logger(const String& pc_strName, const String& pc_strFilePath_ = TEXT(""), _UTILS LogLevel p_level = DEFAULT_LOG_LEVEL);
	Logger(const Logger&) = delete;
	Logger(Logger&&)      = delete;
	~Logger();

	Logger& operator=(const Logger&) = delete;
	Logger& operator=(Logger&&)      = delete;

	/// <summary>
	/// 记录跟踪日志
	/// </summary>
	/// <param name="p_cszFuncName">当前函数名</param>
	/// <param name="pc_strMsg">记录的信息</param>
	inline virtual void Trace(const TCHAR* p_cszFuncName, const String& pc_strMsg) const noexcept {
		if (m_logLevel_ == LogLevel::TRACE) {
			this->Log_(p_cszFuncName, TEXT("trace"), pc_strMsg);
		}
	}

	/// <summary>
	/// 记录调试日志
	/// </summary>
	/// <param name="p_cszFuncName">当前函数名</param>
	/// <param name="pc_strMsg">记录的信息</param>
	inline virtual void Debug(const TCHAR* p_cszFuncName, const String& pc_strMsg) const noexcept {
		if (m_logLevel_ <= LogLevel::DEBUG) {
			this->Log_(p_cszFuncName, TEXT("debug"), pc_strMsg);
		}
	}

	/// <summary>
	/// 记录消息日志
	/// </summary>
	/// <param name="p_cszFuncName">当前函数名</param>
	/// <param name="pc_strMsg">记录的信息</param>
	inline virtual void Info(const TCHAR* p_cszFuncName, const String& pc_strMsg) const noexcept {
		this->Log_(p_cszFuncName, TEXT("info"), pc_strMsg);
	}

	/// <summary>
	/// 记录警告日志
	/// </summary>
	/// <param name="p_cszFuncName">当前函数名</param>
	/// <param name="pc_strMsg">记录的信息</param>
	inline virtual void Warn(const TCHAR* p_cszFuncName, const String& pc_strMsg) const noexcept {
		this->Log_(p_cszFuncName, TEXT("warn"), pc_strMsg);
	}

	/// <summary>
	/// 记录错误日志
	/// </summary>
	/// <param name="p_cszFuncName">当前函数名</param>
	/// <param name="pc_strMsg">记录的信息</param>
	inline virtual void Error(const TCHAR* p_cszFuncName, const String& pc_strMsg) const noexcept {
		this->Log_(p_cszFuncName, TEXT("error"), pc_strMsg);
	}

	const String GetFullFilePath() const;
};

#define TraceM(msg) Trace(TEXT(__FUNCTION__), msg)
#define DebugM(msg) Debug(TEXT(__FUNCTION__), msg)
#define InfoM(msg) Info(TEXT(__FUNCTION__), msg)
#define WarnM(msg) Warn(TEXT(__FUNCTION__), msg)
#define ErrorM(msg) Error(TEXT(__FUNCTION__), msg)

_UTILS_END

#pragma warning(pop)