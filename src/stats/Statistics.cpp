#include "stats/Statistics.h"

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "utils/DateUtils.h"

std::vector<std::pair<Book, int>> Statistics::getTopBooks(
    const std::vector<BorrowRecord>& records, const std::vector<Book>& books, int topN) {
    std::map<std::string, int> freq;//有序键值对容器
    for (const auto& r : records) freq[r.getBookId()]++;

    //排序
    std::vector<std::pair<std::string, int>> ranking;
    ranking.reserve(freq.size());
    for (const auto& kv : freq) ranking.emplace_back(kv.first, kv.second);

    std::sort(ranking.begin(), ranking.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });

    if (topN <= 0) return {};
    if (static_cast<int>(ranking.size()) > topN) ranking.resize(topN);

    std::vector<std::pair<Book, int>> result;//检索topN本书名
    for (const auto& kv : ranking) {
        const Book* match = nullptr;
        for (size_t i = 0; i < books.size(); ++i) {
            if (books[i].getId() == kv.first) {
                match = &books[i];
                break;
            }
        }
        if (match) result.emplace_back(*match, kv.second);
    }
    return result;
}

std::vector<std::pair<User, int>> Statistics::getTopUsers(
    const std::vector<BorrowRecord>& records, const std::vector<User>& users, int topN) {
    std::map<std::string, int> freq;
    for (const auto& r : records) freq[r.getUserId()]++;

    std::vector<std::pair<std::string, int>> ranking;
    ranking.reserve(freq.size());
    for (const auto& kv : freq) ranking.emplace_back(kv.first, kv.second);

    std::sort(ranking.begin(), ranking.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });

    if (topN <= 0) return {};
    if (static_cast<int>(ranking.size()) > topN) ranking.resize(topN);

    std::vector<std::pair<User, int>> result;
    for (const auto& kv : ranking) {
        const User* match = nullptr;
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].getId() == kv.first) {
                match = &users[i];
                break;
            }
        }
        if (match) result.emplace_back(*match, kv.second);
    }
    return result;
}

double Statistics::getSystemAverageBorrowDuration(
    const std::vector<BorrowRecord>& returnedRecords) {
    double total = 0.0;
    int count = 0;
    for (const auto& r : returnedRecords) {
        if (!r.isReturned()) continue;
        total += Date::daysBetween(r.getBorrowDate(), r.getReturnDate());
        ++count;
    }
    return count == 0 ? 0.0 : total / count;
}

Statistics::UserReadingReport Statistics::generateUserReport(
    const std::vector<BorrowRecord>& userRecords, const std::vector<Book>& allBooks) {
    UserReadingReport rep;
    std::map<std::string, int> categoryFreq;
    int longestDays = -1;

    for (const auto& r : userRecords) {
        if (!r.isReturned()) continue;
        const int days = Date::daysBetween(r.getBorrowDate(), r.getReturnDate());

        ++rep.totalBorrowedCount;
        rep.totalBorrowDays += days;

        const Book* match = nullptr;
        for (size_t i = 0; i < allBooks.size(); ++i) {
            if (allBooks[i].getId() == r.getBookId()) {
                match = &allBooks[i];
                break;
            }
        }
        if (!match) continue;

        ++categoryFreq[match->getCategory()];

        if (days > longestDays) {
            longestDays = days;
            rep.longestBorrowedBook = match->getTitle();
        } else if (days == longestDays &&
                   (rep.longestBorrowedBook.empty() || match->getTitle() < rep.longestBorrowedBook)) {
            rep.longestBorrowedBook = match->getTitle();
        }
    }

    int bestFreq = -1;
    for (const auto& kv : categoryFreq) {
        if (kv.second > bestFreq ||
            (kv.second == bestFreq && (rep.favoriteCategory.empty() || kv.first < rep.favoriteCategory))) {
            bestFreq = kv.second;
            rep.favoriteCategory = kv.first;
        }
    }

    if (rep.totalBorrowedCount > 0)
        rep.averageBorrowDays = rep.totalBorrowDays / rep.totalBorrowedCount;
    return rep;
}