#pragma once

#include <string>
#include <vector>

#include "models/Person.h"
#include "interfaces/Searchable.h"

class User : public Person, public Searchable {
public:
    User() = default;

    User(std::string id, std::string name, std::string password, std::string email,
         int maxBorrowLimit);

    virtual ~User() = default;

    //getter
    int getMaxBorrowLimit() const;
    int getCurrentBorrowCount() const { return static_cast<int>(borrowedBookIds_.size()); }
    const std::vector<std::string>& getBorrowedBookIds() const;

    //是否能借
    bool canBorrow() const;

    //setter借还
    void addBorrowedBook(const std::string& bookId);
    void removeBorrowedBook(const std::string& bookId);

    //打印和搜索
    void display(std::ostream& os) const override;
    bool matches(const std::string& query) const override;

private:
    int maxBorrowLimit_ = 5;
    std::vector<std::string> borrowedBookIds_;
    //一个可以动态改变大小的字符串数组（列表）
};
