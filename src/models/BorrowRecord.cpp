#include "models/BorrowRecord.h"

#include <cmath>
#include <cstdio>
#include <ctime>
#include <utility>

namespace {
// "YYYY-MM-DD" -> time_t（以当日正午解析，规避夏令时边界误差）
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

// 在 "YYYY-MM-DD" 上增减 days 天，返回新日期字符串
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
}  // namespace

BorrowRecord::BorrowRecord(std::string recordId, std::string bookId, std::string userId,
                           std::string borrowDate, std::string dueDate)
    : recordId_(std::move(recordId)),
      bookId_(std::move(bookId)),
      userId_(std::move(userId)),
      borrowDate_(std::move(borrowDate)),
      dueDate_(std::move(dueDate)) {}

const std::string& BorrowRecord::getRecordId() const { return recordId_; }
const std::string& BorrowRecord::getBookId() const { return bookId_; }
const std::string& BorrowRecord::getUserId() const { return userId_; }
const std::string& BorrowRecord::getBorrowDate() const { return borrowDate_; }
const std::string& BorrowRecord::getDueDate() const { return dueDate_; }
const std::string& BorrowRecord::getReturnDate() const { return returnDate_; }
bool BorrowRecord::isRenewed() const { return isRenewed_; }

void BorrowRecord::setReturnDate(const std::string& returnDate) { returnDate_ = returnDate; }

bool BorrowRecord::isReturned() const { return !returnDate_.empty(); }

bool BorrowRecord::isOverdue(const std::string& currentDate) const {
    return !isReturned() && currentDate > dueDate_;
}

int BorrowRecord::calculateOverdueDays(const std::string& currentDate) const {
    if (!isOverdue(currentDate)) return 0;
    return daysBetween(dueDate_, currentDate);
}

void BorrowRecord::renew(int days) {
    dueDate_ = addDays(dueDate_, days);
    isRenewed_ = true;
}

void BorrowRecord::display(std::ostream& os) const {
    os << "借阅记录[记录ID=" << recordId_
       << ", 图书ID=" << bookId_
       << ", 用户ID=" << userId_
       << ", 借书日期=" << borrowDate_
       << ", 应还日期=" << dueDate_
       << ", 归还日期=" << (returnDate_.empty() ? "未归还" : returnDate_)
       << (isRenewed_ ? ", 已续借" : "") << "]\n";
}

bool BorrowRecord::operator<(const BorrowRecord& other) const {
    if (borrowDate_ != other.borrowDate_) return borrowDate_ < other.borrowDate_;
    return recordId_ < other.recordId_;
}