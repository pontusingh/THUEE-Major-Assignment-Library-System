#pragma once
#include <vector>
#include <string>
#include "models/Book.h"
#include "models/User.h"
#include "models/BorrowRecord.h"

class FileStorage {
public:
    // 阻止实例化，因为它是纯工具类
    FileStorage() = delete;

    // 1. 图书数据读写接口
    static std::vector<Book> loadBooks(const std::string& filePath);
    static void saveBooks(const std::string& filePath, const std::vector<Book>& books);

    // 2. 用户数据读写接口
    static std::vector<User> loadUsers(const std::string& filePath);
    static void saveUsers(const std::string& filePath, const std::vector<User>& users);

    // 3. 借阅记录数据读写接口
    static std::vector<BorrowRecord> loadRecords(const std::string& filePath);
    static void saveRecords(const std::string& filePath, const std::vector<BorrowRecord>& records);

private:
    // 辅助方法：将一行文本按指定分隔符拆分成字段
    static std::vector<std::string> split(const std::string& line, char delimiter);
};