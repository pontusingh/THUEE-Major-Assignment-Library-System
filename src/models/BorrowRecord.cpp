#include "models/BorrowRecord.h"

#include <utility>

#include "utils/DateUtils.h"

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

void BorrowRecord::setRenewed(bool renewed) { isRenewed_ = renewed; }

bool BorrowRecord::isReturned() const { return !returnDate_.empty(); }

bool BorrowRecord::isOverdue(const std::string& currentDate) const {
    return !isReturned() && currentDate > dueDate_;
}

int BorrowRecord::calculateOverdueDays(const std::string& currentDate) const {
    if (!isOverdue(currentDate)) return 0;
    return Date::daysBetween(dueDate_, currentDate);
}

void BorrowRecord::renew(int days) {
    dueDate_ = Date::addDays(dueDate_, days);
    isRenewed_ = true;
}

void BorrowRecord::display(std::ostream& os) const {
    os << "借阅记录[记录ID=" << recordId_
       << ", 图书ID=" << bookId_
       << ", 用户ID=" << userId_
       << ", 借书日期=" << borrowDate_
       << ", 应还日期=" << dueDate_
       << ", 归还日期=" << (returnDate_.empty() ? "未归还" : returnDate_)
       << (isRenewed_ ? ", 已续借" : "");
    const std::string today = Date::todayStr();
    if (isOverdue(today))
        os << ", 已超期 " << calculateOverdueDays(today) << " 天";
    os << "]\n";
}

bool BorrowRecord::operator<(const BorrowRecord& other) const {
    if (borrowDate_ != other.borrowDate_) return borrowDate_ < other.borrowDate_;
    return recordId_ < other.recordId_;
}