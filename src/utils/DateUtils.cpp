#include "utils/DateUtils.h"

#include <cmath>
#include <cstdio>
#include <ctime>

namespace {

// "YYYY-MM-DD" -> time_t（当日正午解析，规避夏令时边界误差）
std::time_t parseToTime(const std::string& date) {
    int y = 0, m = 0, d = 0;
    std::sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d);
    std::tm t = {};
    t.tm_year = y - 1900;
    t.tm_mon = m - 1;
    t.tm_mday = d;
    t.tm_hour = 12;
    return std::mktime(&t);
}

}  // namespace

namespace Date {

std::string todayStr() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", now);
    return buf;
}

std::string addDays(const std::string& date, int days) {
    std::time_t tt = parseToTime(date);
    tt += static_cast<std::time_t>(days) * 86400;
    std::tm* r = std::localtime(&tt);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", r);
    return buf;
}

int daysBetween(const std::string& from, const std::string& to) {
    const std::time_t diff = parseToTime(to) - parseToTime(from);
    return static_cast<int>(std::llround(static_cast<double>(diff) / 86400.0));
}

}  // namespace Date