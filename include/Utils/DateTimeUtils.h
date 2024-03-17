#pragma once
#include <chrono>
#include <sstream>
#include <string>

#include "utils_def.h"

#define DEFAULT_DATETIME_PARSER "%Y/%m/%d %H:%M:%OS"
#define DEFAULT_DATETIME_FORMAT "{:" DEFAULT_DATETIME_PARSER "}"
#define DEFAULT_DATETIME_REGEX TEXT("^\\d{4}(/\\d{2}){2}( (\\d{2}:){2}\\d{2})?$")

#define DEFAULT_DATE_FORMAT "{:%Y/%m/%d}"

_UTILS_BEGIN

using Clock        = std::chrono::system_clock;
using DateTime     = std::chrono::time_point<std::chrono::local_t, Clock::duration>;
using Date         = std::chrono::year_month_day;
using Time         = std::chrono::hh_mm_ss<Clock::duration>;
using Years        = std::chrono::years;
using Months       = std::chrono::months;
using Days         = std::chrono::days;
using Hours        = std::chrono::hours;
using Minutes      = std::chrono::minutes;
using Seconds      = std::chrono::seconds;
using MilliSeconds = std::chrono::milliseconds;
using MicroSeconds = std::chrono::microseconds;

class UTILS_API DateTimeUtils {
public:
	/// <summary>
	/// 将给定字符串按给定的日期格式转换为日期
	/// </summary>
	/// <typeparam name="_CharT">字符串的字符类型</typeparam>
	/// <param name="p_cszFmt">日期格式</param>
	/// <param name="p_cszValue">将要转换的字符串</param>
	/// <returns>转换后的日期</returns>
	template <class _CharT>
	static inline DateTime Parse(const _CharT* p_cszFmt, const _CharT* p_cszValue) {
		DateTime tp;
		std::basic_istringstream<_CharT>(p_cszValue) >> std::chrono::parse(p_cszFmt, tp);
		return tp;
	}

	/// <summary>
	/// 将给定字符串按给定的日期格式转换为日期
	/// </summary>
	/// <typeparam name="_CharT">字符串的字符类型</typeparam>
	/// <param name="p_cszFmt">日期格式</param>
	/// <param name="pc_strValue">将要转换的字符串</param>
	/// <returns>转换后的日期</returns>
	template <class _CharT>
	static inline DateTime Parse(const _CharT* p_cszFmt, const std::basic_string<_CharT>& pc_strValue) {
		return Parse(p_cszFmt, pc_strValue.c_str());
	}

	/// <summary>
	/// 获取给定时间间隔所包含的年数
	/// </summary>
	/// <param name="p_dur">将要获取的时间间隔</param>
	static inline int GetYear(Clock::duration p_dur) {
		return std::chrono::duration_cast<Years>(p_dur).count();
	}

	/// <summary>
	/// 获取给定时间间隔所包含的月数
	/// </summary>
	/// <param name="p_dur">将要获取的时间间隔</param>
	static inline int GetMonths(Clock::duration p_dur) {
		return std::chrono::duration_cast<Months>(p_dur).count();
	}

	/// <summary>
	/// 获取给定时间间隔所包含的天数
	/// </summary>
	/// <param name="p_dur">将要获取的时间间隔</param>
	static inline int GetDays(Clock::duration p_dur) {
		return std::chrono::duration_cast<Days>(p_dur).count();
	}

	/// <summary>
	/// 获取给定时间间隔所包含的小时数
	/// </summary>
	/// <param name="p_dur">将要获取的时间间隔</param>
	static inline int GetHours(Clock::duration p_dur) {
		return std::chrono::duration_cast<Hours>(p_dur).count();
	}

	/// <summary>
	/// 获取给定时间间隔所包含的分钟数
	/// </summary>
	/// <param name="p_dur">将要获取的时间间隔</param>
	static inline int GetMinutes(Clock::duration p_dur) {
		return std::chrono::duration_cast<Minutes>(p_dur).count();
	}

	/// <summary>
	/// 获取给定时间间隔所包含的秒数
	/// </summary>
	/// <param name="p_dur">将要获取的时间间隔</param>
	static inline long long GetSeconds(Clock::duration p_dur) {
		return std::chrono::duration_cast<Seconds>(p_dur).count();
	}

	/// <summary>
	/// 将给定时间点增加给定的日长，不改变原因数据
	/// </summary>
	/// <param name="p_tp">将要增加的时间点</param>
	/// <param name="p_nDays">增加的天数</param>
	/// <returns>增加后的时间点</returns>
	static inline DateTime AddDays(DateTime p_tp, int p_nDays) {
		return p_tp + std::chrono::days(p_nDays);
	}

	/// <summary>
	/// 获取系统当前时间
	/// </summary>
	static inline DateTime Now() {
		return DateTime { Clock::now().time_since_epoch() + Hours(8) };
	}

	/// <summary>
	/// 将给定双精度浮点数转为时间点
	/// </summary>
	/// <param name="p_dDate">将要转换的值</param>
	static inline DateTime FromDouble(double p_dDate) {
		return DateTime(Clock::from_time_t(static_cast<time_t>(p_dDate * 24 * 3600 - 2209190400)).time_since_epoch());
	}
};

_UTILS_END