// main.cpp — 存储层 round-trip 断言驱动（Step 3 阶段版本，Step 4 将改写为最终应用入口）

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "models/Book.h"
#include "models/BorrowRecord.h"
#include "models/User.h"
#include "exceptions/LibraryExceptions.h"
#include "storage/FileStorage.h"

namespace {

int passCount = 0;
int failCount = 0;

void check(bool cond, const std::string& name) {
    if (cond) {
        ++passCount;
        std::cout << "PASS: " << name << "\n";
    } else {
        ++failCount;
        std::cout << "FAIL: " << name << "\n";
    }
}

template <typename F>
bool throws(F&& f) {
    try {
        f();
        return false;
    } catch (const LibraryException&) {
        return true;
    }
}

}  // namespace

int main() {
    const std::filesystem::path dir =
        std::filesystem::temp_directory_path() / "opencode_libsys_test";
    std::filesystem::create_directories(dir);
    const std::string bookPath = (dir / "books.txt").string();
    const std::string userPath = (dir / "users.txt").string();
    const std::string recPath = (dir / "records.txt").string();

    // ---- 图书 round-trip ----
    std::vector<Book> books;
    books.push_back(Book("b001", "红楼梦", "曹雪芹", "古典,小说", "文学",
                         "人民文学出版社", "A-1"));
    books.push_back(Book("b002", "C++ Primer", "Lippman", "cpp,入门", "技术",
                         "机械工业出版社", "B-2"));
    books[1].borrowBook();
    FileStorage::saveBooks(bookPath, books);
    auto loadedBooks = FileStorage::loadBooks(bookPath);
    check(loadedBooks.size() == 2, "图书 round-trip 数量一致");
    check(loadedBooks[0].getId() == "b001" && loadedBooks[0].getTitle() == "红楼梦" &&
              loadedBooks[0].getAuthor() == "曹雪芹" &&
              loadedBooks[0].getKeywords() == "古典,小说" &&
              loadedBooks[0].getCategory() == "文学" &&
              loadedBooks[0].getPublisherInfo() == "人民文学出版社" &&
              loadedBooks[0].getLocation() == "A-1" && !loadedBooks[0].isBorrowed(),
          "图书 round-trip 全字段(含中文/未借)");
    check(loadedBooks[1].isBorrowed(), "图书 round-trip 借出状态恢复");

    // ---- 用户 round-trip ----
    std::vector<User> users;
    users.push_back(User("u001", "张三", "pwd123", "zhangsan@example.com", 2));
    users[0].addBorrowedBook("b001");
    users[0].addBorrowedBook("b002");
    users.push_back(User("u002", "李四", "secret", "lisi@example.com", 5));
    FileStorage::saveUsers(userPath, users);
    auto loadedUsers = FileStorage::loadUsers(userPath);
    check(loadedUsers.size() == 2, "用户 round-trip 数量一致");
    check(loadedUsers[0].getId() == "u001" && loadedUsers[0].getName() == "张三" &&
              loadedUsers[0].getEmail() == "zhangsan@example.com" &&
              loadedUsers[0].getPassword() == "pwd123" &&
              loadedUsers[0].getMaxBorrowLimit() == 2,
          "用户 round-trip 全字段(含密码)");
    check(loadedUsers[0].getBorrowedBookIds() ==
                  std::vector<std::string>{"b001", "b002"},
          "用户 round-trip 已借列表恢复");
    check(loadedUsers[1].getBorrowedBookIds().empty(), "用户 round-trip 空已借列表");

    // ---- 借阅记录 round-trip ----
    std::vector<BorrowRecord> records;
    BorrowRecord r1("R1", "b001", "u001", "2026-07-20", "2026-08-17");
    r1.renew(14);
    BorrowRecord r2("R2", "b002", "u001", "2026-08-01", "2026-08-29");
    r2.setReturnDate("2026-08-10");
    records.push_back(r1);
    records.push_back(r2);
    FileStorage::saveRecords(recPath, records);
    auto loadedRecords = FileStorage::loadRecords(recPath);
    check(loadedRecords.size() == 2, "记录 round-trip 数量一致");
    check(loadedRecords[0].getRecordId() == "R1" && loadedRecords[0].getBookId() == "b001" &&
              loadedRecords[0].getUserId() == "u001" &&
              loadedRecords[0].getBorrowDate() == "2026-07-20" &&
              loadedRecords[0].getDueDate() == "2026-08-31" &&
              loadedRecords[0].getReturnDate().empty() && loadedRecords[0].isRenewed(),
          "记录 round-trip 续借标记+未归还");
    check(loadedRecords[1].getReturnDate() == "2026-08-10" && loadedRecords[1].isReturned() &&
              !loadedRecords[1].isRenewed(),
          "记录 round-trip 归还日期+未续借");

    // ---- 不存在文件 -> 空 ----
    check(FileStorage::loadBooks((dir / "missing_books.txt").string()).empty(),
          "loadBooks 不存在文件返回空");
    check(FileStorage::loadUsers((dir / "missing_users.txt").string()).empty(),
          "loadUsers 不存在文件返回空");
    check(FileStorage::loadRecords((dir / "missing_records.txt").string()).empty(),
          "loadRecords 不存在文件返回空");

    // ---- 损坏行 -> 异常 ----
    const std::string badPath = (dir / "bad_books.txt").string();
    {
        std::ofstream bad(badPath, std::ios::binary);
        bad << "只有两个字段|ab\n";
    }
    check(throws([&] { FileStorage::loadBooks(badPath); }),
          "loadBooks 字段数错误抛 FileStorageException");

    // ---- 保存到非法路径 -> 异常 ----
    check(throws([&] { FileStorage::saveBooks("Z:\\no_such_dir\\books.txt", books); }),
          "saveBooks 非法路径抛 FileStorageException");

    std::cout << "\n===== 结果: " << passCount << " PASS, " << failCount << " FAIL =====\n";
    return failCount == 0 ? 0 : 1;
}