#include "models/User.h"

#include <algorithm>
#include <utility>

User::User(std::string id, std::string name, std::string password, std::string email,
           int maxBorrowLimit)
    : Person(std::move(id), std::move(name), std::move(password), std::move(email)),
      maxBorrowLimit_(maxBorrowLimit) {}

int User::getMaxBorrowLimit() const { return maxBorrowLimit_; }

const std::vector<std::string>& User::getBorrowedBookIds() const { return borrowedBookIds_; }

bool User::canBorrow() const { return getCurrentBorrowCount() < maxBorrowLimit_; }

void User::addBorrowedBook(const std::string& bookId) { borrowedBookIds_.push_back(bookId); }

void User::removeBorrowedBook(const std::string& bookId) {
    auto it = std::find(borrowedBookIds_.begin(), borrowedBookIds_.end(), bookId);
    //自动推导类型，.find()来自<algorithm>，寻找begin到end的书
    if (it != borrowedBookIds_.end()) {
        //it = borrowedBookIds_.end()说明没找到
        borrowedBookIds_.erase(it);
    }
}

void User::display(std::ostream& os) const {
    os << "读者[ID=" << getId()
       << ", 姓名=" << getName()
       << ", 邮箱=" << getEmail()
       << ", 已借数=" << getCurrentBorrowCount()
       << "/" << maxBorrowLimit_ << "]\n";
    if (!borrowedBookIds_.empty()) {
        os << "  已借图书ID: ";
        for (size_t i = 0; i < borrowedBookIds_.size(); ++i) {
            if (i > 0) os << ", ";
            os << borrowedBookIds_[i];
        }
        os << "\n";
    }
}

bool User::matches(const std::string& query) const {
    return getId().find(query) != std::string::npos ||
           getName().find(query) != std::string::npos;
}