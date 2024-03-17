#include "Logger.h"

#include <filesystem>

#include "DateTimeUtils.h"

_UTILS_BEGIN

Logger::Logger(const String& pc_strName, const String& pc_strFilePath_, _UTILS LogLevel p_level)
    : m_strName_(pc_strName)
    , m_logLevel_(p_level) {
	if (!pc_strFilePath_.empty()) {
		m_strLogFilePath_ = pc_strFilePath_;
		return;
	}

	std::filesystem::path strFilePath = std::filesystem::current_path();
	strFilePath /= TEXT("logs\\");
	m_strLogFilePath_ = strFilePath.native();
}

Logger::~Logger() {
}

void Logger::Log_(const TCHAR* p_cszFuncName, const String& pc_strType, const String& pc_strMsg) const {
	std::basic_ofstream<String::value_type> ofs;
	ofs.imbue(std::locale("", std::locale::all ^ std::locale::numeric));
	const DateTime dtNow = DateTimeUtils::Now();

	auto strFileName = std::filesystem::path(m_strLogFilePath_);
	if (!std::filesystem::exists(strFileName)) {
		try {
			std::filesystem::create_directories(strFileName);
		} catch (const std::exception&) {
			return;
		}
	}

	strFileName /= FORMAT("{}-{:%Y%m%d}.log", m_strName_, dtNow);
	bool bExists = std::filesystem::exists(strFileName);
	ofs.open(strFileName, std::ios::app);

	if (!ofs.is_open()) {
		return;
	}

	String strLine;
	if (m_bFirstLog_) {
		m_bFirstLog_ = false;
		if (bExists) {
			strLine = TEXT("\n");
			ofs.write(strLine.c_str(), strLine.size());
		}
	}

	strLine = FORMAT("[{:%Y-%m-%d %H:%M:%OS}.{:03d}] [{:5}]  {}: {}\n", dtNow,
	    std::chrono::duration_cast<MilliSeconds>(dtNow.time_since_epoch()).count() % 1000, pc_strType, p_cszFuncName, StringUtils::Trim(pc_strMsg));

#ifdef _DEBUG
	::OutputDebugString(strLine.c_str());
#endif // DEBUG

	ofs.write(strLine.c_str(), strLine.size());
	ofs.close();
}

const String Logger::GetFullFilePath() const {
	const DateTime dtNow = DateTimeUtils::Now();
	auto strFileName     = std::filesystem::path(m_strLogFilePath_);
	strFileName /= FORMAT("{}-{:%Y%m%d}.log", m_strName_, dtNow);

	return strFileName.native();
}

_UTILS_END