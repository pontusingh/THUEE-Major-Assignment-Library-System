#include "models/BorrowRecord.h"

bool BorrowRecord::operator<(const BorrowRecord& other) const {
    if (borrowDate_ != other.borrowDate_) return borrowDate_ < other.borrowDate_;
    return recordId_ < other.recordId_;
}
