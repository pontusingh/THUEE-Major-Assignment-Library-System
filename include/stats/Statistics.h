#pragma once

#include <string>
#include <utility>
#include <vector>

#include "models/Book.h"
#include "models/User.h"
#include "models/BorrowRecord.h"

class Statistics {
public:
    struct UserReadingReport {
        int totalBorrowedCount = 0;
        double totalBorrowDays = 0.0;
        double averageBorrowDays = 0.0;
        std::string favoriteCategory;
        std::string longestBorrowedBook;
    };

    //静态成员函数，返回书目和借书次数键值对的列表
    static std::vector<std::pair<Book, int>> getTopBooks(
        const std::vector<BorrowRecord>& records,
        const std::vector<Book>& books,
        int topN = 5);

    static std::vector<std::pair<User, int>> getTopUsers(
        const std::vector<BorrowRecord>& records,
        const std::vector<User>& users,
        int topN = 5);

    static double getSystemAverageBorrowDuration(
        const std::vector<BorrowRecord>& returnedRecords);

    static UserReadingReport generateUserReport(
        const std::vector<BorrowRecord>& userRecords,
        const std::vector<Book>& allBooks);

private:
    static int dateToAbsoluteDays(const std::string& date);
    static int calculateDaysDifference(const std::string& from, const std::string& to);
};
