#include "storage/FileStorage.h"

#include <fstream>
#include <string>
#include <vector>

#include "exceptions/LibraryExceptions.h"

namespace {

// 将 "1" 视为 true，其余视为 false
bool parseBool(const std::string& s) { return s == "1"; }

}  // namespace

//辅助方法，分割字段
std::vector<std::string> FileStorage::split(const std::string& line, char delimiter) {
    std::vector<std::string> parts;
    size_t start = 0;
    while (true) {
        const size_t pos = line.find(delimiter, start);
        if (pos == std::string::npos) {
            parts.push_back(line.substr(start));
            break;
        }
        parts.push_back(line.substr(start, pos - start));
        start = pos + 1;
    }
    return parts;
}

// ========== 图书 ==========

std::vector<Book> FileStorage::loadBooks(const std::string& filePath) {
    std::ifstream in(filePath, std::ios::binary);
    if (!in) return {};

    std::vector<Book> books;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        const auto fields = split(line, '|');
        if (fields.size() != 8) throw FileStorageException("图书数据格式错误");
        Book b(fields[0], fields[1], fields[2], fields[3], fields[4], fields[5], fields[6]);
        if (parseBool(fields[7])) b.borrowBook();
        books.push_back(b);
    }
    return books;
}

void FileStorage::saveBooks(const std::string& filePath, const std::vector<Book>& books) {
    std::ofstream out(filePath, std::ios::binary | std::ios::trunc);
    if (!out) throw FileStorageException("无法打开图书数据文件");
    for (const auto& b : books) {
        out << b.getId() << '|' << b.getTitle() << '|' << b.getAuthor() << '|'
            << b.getKeywords() << '|' << b.getCategory() << '|' << b.getPublisherInfo() << '|'
            << b.getLocation() << '|' << (b.isBorrowed() ? "1" : "0") << '\n';
    }
}

// ========== 用户 ==========

std::vector<User> FileStorage::loadUsers(const std::string& filePath) {
    std::ifstream in(filePath, std::ios::binary);
    if (!in) return {};

    std::vector<User> users;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        const auto fields = split(line, '|');
        if (fields.size() != 6) throw FileStorageException("用户数据格式错误");
        int limit = 0;
        try {
            limit = std::stoi(fields[4]);
        } catch (const std::exception&) {
            throw FileStorageException("用户借阅上限格式错误");
        }
        User u(fields[0], fields[1], fields[2], fields[3], limit);
        for (const auto& id : split(fields[5], ',')) {
            if (!id.empty()) u.addBorrowedBook(id);
        }
        users.push_back(u);
    }
    return users;
}

void FileStorage::saveUsers(const std::string& filePath, const std::vector<User>& users) {
    std::ofstream out(filePath, std::ios::binary | std::ios::trunc);
    if (!out) throw FileStorageException("无法打开用户数据文件");
    for (const auto& u : users) {
        out << u.getId() << '|' << u.getName() << '|' << u.getPassword() << '|'
            << u.getEmail() << '|' << u.getMaxBorrowLimit() << '|';
        const auto& ids = u.getBorrowedBookIds();
        for (size_t i = 0; i < ids.size(); ++i) {
            if (i > 0) out << ',';
            out << ids[i];
        }
        out << '\n';
    }
}

// ========== 借阅记录 ==========

std::vector<BorrowRecord> FileStorage::loadRecords(const std::string& filePath) {
    std::ifstream in(filePath, std::ios::binary);
    if (!in) return {};

    std::vector<BorrowRecord> records;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        const auto fields = split(line, '|');
        if (fields.size() != 7) throw FileStorageException("借阅记录数据格式错误");
        BorrowRecord r(fields[0], fields[1], fields[2], fields[3], fields[4]);
        if (parseBool(fields[6])) r.setRenewed(true);
        if (!fields[5].empty()) r.setReturnDate(fields[5]);
        records.push_back(r);
    }
    return records;
}

void FileStorage::saveRecords(const std::string& filePath,
                              const std::vector<BorrowRecord>& records) {
    std::ofstream out(filePath, std::ios::binary | std::ios::trunc);
    if (!out) throw FileStorageException("无法打开借阅记录数据文件");
    for (const auto& r : records) {
        out << r.getRecordId() << '|' << r.getBookId() << '|' << r.getUserId() << '|'
            << r.getBorrowDate() << '|' << r.getDueDate() << '|' << r.getReturnDate() << '|'
            << (r.isRenewed() ? "1" : "0") << '\n';
    }
}