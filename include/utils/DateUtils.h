#pragma once

#include <string>

// 统一日期工具：唯一的 YYYY-MM-DD 日期计算来源
// 替代原先分散在 Library/BorrowRecord/Statistics 中的三套实现
namespace Date {

// 今日日期，格式 YYYY-MM-DD
std::string todayStr();

// 在 "YYYY-MM-DD" 上增减 days 天，返回新日期字符串
std::string addDays(const std::string& date, int days);

// "YYYY-MM-DD" 之间相差的天数为 to - from
int daysBetween(const std::string& from, const std::string& to);

}  // namespace Date