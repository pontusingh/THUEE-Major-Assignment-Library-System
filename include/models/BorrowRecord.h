#pragma once

#include <string>

#include "interfaces/Displayable.h"

class BorrowRecord : public Displayable {
public:
    BorrowRecord() = default;

    BorrowRecord(std::string recordId, std::string bookId, std::string userId,
                 std::string borrowDate, std::string dueDate);

    virtual ~BorrowRecord() = default;

    const std::string& getRecordId() const;
    const std::string& getBookId() const;
    const std::string& getUserId() const;
    const std::string& getBorrowDate() const;
    const std::string& getDueDate() const;
    const std::string& getReturnDate() const;
    bool isRenewed() const;

    void setReturnDate(const std::string& returnDate);
    void setRenewed(bool renewed);
    bool isReturned() const;

    bool isOverdue(const std::string& currentDate) const;
    int calculateOverdueDays(const std::string& currentDate) const;
    
    void renew(int days);

    void display(std::ostream& os) const override;

    bool operator<(const BorrowRecord& other) const;

private:
    std::string recordId_;
    std::string bookId_;
    std::string userId_;
    std::string borrowDate_;
    std::string dueDate_;
    std::string returnDate_;
    bool isRenewed_ = false;
};
