#include "ui/ConsoleUI.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "models/Book.h"
#include "models/BorrowRecord.h"
#include "models/User.h"
#include "stats/Statistics.h"
#include "storage/FileStorage.h"

namespace {

const std::string BOOKS_FILE = "data/books.txt";
const std::string USERS_FILE = "data/users.txt";
const std::string RECORDS_FILE = "data/records.txt";
constexpr int RENEW_DAYS = 14;

std::string trim(const std::string& s) {
    size_t begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

}  // namespace

ConsoleUI::ConsoleUI(Library& library) : library_(library) {}

std::string ConsoleUI::readLine(const std::string& prompt) const {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    line = trim(line);
    if (line.find('|') != std::string::npos)
        throw InvalidInputException("输入不能包含字符 '|'");  // 文件分隔符契约
    return line;
}

int ConsoleUI::readInt(const std::string& prompt) const {
    while (true) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);
        line = trim(line);
        if (line.empty()) continue;
        try {
            return std::stoi(line);
        } catch (const std::exception&) {
            std::cout << "输入非法，请重新输入整数。\n";
        }
    }
}

void ConsoleUI::persist() const {
    FileStorage::saveBooks(BOOKS_FILE, library_.getBooks());
    FileStorage::saveUsers(USERS_FILE, library_.getUsers());
    FileStorage::saveRecords(RECORDS_FILE, library_.getRecords());
}

void ConsoleUI::run() {
    while (true) loginLoop();
}

void ConsoleUI::loginLoop() {
    std::cout << "\n===== 图书管理系统 =====\n";
    const std::string id = readLine("请输入 ID (输入 q 退出): ");
    if (id == "q" || id == "quit") {
        std::cout << "再见！\n";
        std::exit(0);
    }
    const std::string password = readLine("请输入密码: ");

    const LoginRole role = library_.login(id, password);
    if (role == LoginRole::User) {
        showUserMenu(id);
    } else if (role == LoginRole::Librarian) {
        showAdminMenu();
    } else {
        std::cout << "登录失败：ID 或密码错误。\n";
    }
}

void ConsoleUI::showUserMenu(const std::string& userId) {
    std::cout << "\n您好，" << userId << " (读者)！\n";
    while (true) {
        std::cout << "\n----- 读者菜单 -----\n"
                  << "1. 查询图书\n"
                  << "2. 借书\n"
                  << "3. 还书\n"
                  << "4. 续借\n"
                  << "5. 我的借阅记录\n"
                   << "6. 我的读书报告\n"
                   << "7. 我的信息\n"
                  << "0. 退出登录\n"
                  << "请选择: ";
        const int choice = readInt("");
        if (choice == 0) return;
        try {
            switch (choice) {
                case 1: handleSearchBooks(); break;
                case 2: handleBorrow(userId); break;
                case 3: handleReturn(userId); break;
                case 4: handleRenew(userId); break;
                case 5: handleMyRecords(userId); break;
                case 6: handleMyReport(userId); break;
                case 7: handleMyInfo(userId); break;
                default: std::cout << "无效选项。\n"; break;
            }
        } catch (const LibraryException& e) {
            std::cout << "操作失败：" << e.what() << "\n";
        }
    }
}

void ConsoleUI::showAdminMenu() {
    std::cout << "\n您好，管理员！\n";
    while (true) {
        std::cout << "\n----- 管理员菜单 -----\n"
                  << "1. 图书管理\n"
                  << "2. 用户管理\n"
                  << "3. 借阅记录\n"
                  << "4. 统计信息\n"
                  << "0. 退出登录\n"
                  << "请选择: ";
        const int choice = readInt("");
        if (choice == 0) return;
        try {
            switch (choice) {
                case 1: handleBookManagement(); break;
                case 2: handleUserManagement(); break;
                case 3: handleRecordsMenu(); break;
                case 4: handleStatistics(); break;
                default: std::cout << "无效选项。\n"; break;
            }
        } catch (const LibraryException& e) {
            std::cout << "操作失败：" << e.what() << "\n";
        }
    }
}

void ConsoleUI::handleSearchBooks() const {
    const std::string query = readLine("输入查询关键字 (输入 q 返回): ");
    if (query == "q" || query == "quit") return;
    const auto result = library_.searchBooks(query);
    if (result.empty()) {
        std::cout << "未找到匹配的图书。\n";
        return;
    }
    std::cout << "找到 " << result.size() << " 本：\n";
    for (const auto& b : result) std::cout << b;
}

void ConsoleUI::handleBorrow(const std::string& userId) {
    const std::string bookId = readLine("输入要借的图书 ID: ");
    library_.borrowBook(userId, bookId);
    persist();
    std::cout << "借书成功。\n";
}

void ConsoleUI::handleReturn(const std::string& userId) {
    const std::string bookId = readLine("输入要还的图书 ID: ");
    library_.returnBook(userId, bookId);
    persist();
    std::cout << "还书成功。\n";
}

void ConsoleUI::handleRenew(const std::string& userId) {
    const std::string bookId = readLine("输入要续借的图书 ID: ");
    library_.renewBook(userId, bookId, RENEW_DAYS);
    persist();
    std::cout << "续借成功，应还日期已延长 " << RENEW_DAYS << " 天。\n";
}

void ConsoleUI::handleMyRecords(const std::string& userId) const {
    const auto records = library_.getReaderRecords(userId);
    if (records.empty()) {
        std::cout << "您还没有借阅记录。\n";
        return;
    }
    for (const auto& r : records) std::cout << r;
}

void ConsoleUI::handleMyReport(const std::string& userId) const {
    const auto records = library_.getReaderRecords(userId);
    const auto rep = Statistics::generateUserReport(records, library_.getBooks());
    std::cout << "===== 个人读书报告 =====\n"
              << "累计借阅数: " << rep.totalBorrowedCount << "\n"
              << "累计借阅天数: " << rep.totalBorrowDays << "\n"
              << "平均借阅天数: " << rep.averageBorrowDays << "\n"
              << "最偏好类别: " << (rep.favoriteCategory.empty() ? "无" : rep.favoriteCategory)
              << "\n"
              << "借阅最久的书: "
              << (rep.longestBorrowedBook.empty() ? "无" : rep.longestBorrowedBook) << "\n";
}

void ConsoleUI::handleMyInfo(const std::string& userId) {
    while (true) {
        const User* user = library_.findUser(userId);
        if (!user) throw UserNotFoundException();
        std::cout << "\n===== 我的信息 =====\n";
        user->display(std::cout);
        std::cout << "1. 修改信息\n"
                  << "0. 返回\n"
                  << "请选择: ";
        const int choice = readInt("");
        if (choice == 0) return;
        if (choice != 1) {
            std::cout << "无效选项。\n";
            continue;
        }
        const std::string email = readLine("新邮箱 (空格不修改): ");
        const std::string password = readLine("新密码 (空格不修改): ");
        User newInfo(userId, user->getName(), password, email, user->getMaxBorrowLimit());
        library_.updateUser(userId, newInfo);
        persist();
        std::cout << "更新成功。\n";
    }
}

void ConsoleUI::handleBookManagement() {
    while (true) {
        std::cout << "\n----- 图书管理 -----\n"
                  << "1. 添加图书\n"
                  << "2. 删除图书\n"
                  << "3. 更新图书\n"
                  << "4. 查询图书\n"
                  << "0. 返回\n"
                  << "请选择: ";
        const int choice = readInt("");
        if (choice == 0) return;
        try {
            switch (choice) {
                case 1: {
                    const std::string id = readLine("图书 ID: ");
                    const std::string title = readLine("书名: ");
                    const std::string author = readLine("作者: ");
                    const std::string keywords = readLine("关键字: ");
                    const std::string category = readLine("类别: ");
                    const std::string publisher = readLine("出版信息: ");
                    const std::string location = readLine("馆藏地: ");
                    library_.addBook(Book(id, title, author, keywords, category, publisher,
                                          location));
                    persist();
                    std::cout << "添加成功。\n";
                    break;
                }
                case 2: {
                    const std::string id = readLine("输入要删除的图书 ID: ");
                    library_.removeBook(id);
                    persist();
                    std::cout << "删除成功。\n";
                    break;
                }
                case 3: {
                    const std::string id = readLine("图书 ID: ");
                    const Book* book = library_.findBook(id);
                    if (!book) {
                        std::cout << "图书不存在。\n";
                        break;
                    }
                    std::cout << "当前信息:\n" << *book;
                    std::cout << "（直接回车表示不修改该字段）\n";
                    const std::string title = readLine("新书名: ");
                    const std::string author = readLine("新作者: ");
                    const std::string keywords = readLine("新关键字: ");
                    const std::string category = readLine("新类别: ");
                    const std::string publisher = readLine("新出版信息: ");
                    const std::string location = readLine("新馆藏地: ");

                    Book newInfo(id, title, author, keywords, category, publisher, location);
                    library_.updateBook(id, newInfo);
                    persist();
                    std::cout << "更新成功。\n";
                    break;
                }
                case 4:
                    handleSearchBooks();
                    break;
                default:
                    std::cout << "无效选项。\n";
                    break;
            }
        } catch (const LibraryException& e) {
            std::cout << "操作失败：" << e.what() << "\n";
        }
    }
}

void ConsoleUI::handleUserManagement() {
    while (true) {
        std::cout << "\n----- 用户管理 -----\n"
                  << "1. 添加用户\n"
                  << "2. 删除用户\n"
                  << "3. 修改用户信息\n"
                  << "4. 查询用户\n"
                  << "0. 返回\n"
                  << "请选择: ";
        const int n = readInt("");
        if (n == 0) return;
        try {
            switch (n) {
                case 1: {
                    const std::string id = readLine("用户 ID: ");
                    const std::string name = readLine("姓名: ");
                    const std::string password = readLine("密码: ");
                    const std::string email = readLine("邮箱: ");
                    const int limit = readInt("最大借阅量: ");
                    library_.addUser(User(id, name, password, email, limit));
                    persist();
                    std::cout << "添加成功。\n";
                    break;
                }
                case 2: {
                    const std::string id = readLine("输入要删除的用户 ID: ");
                    library_.removeUser(id);
                    persist();
                    std::cout << "删除成功。\n";
                    break;
                }
                case 3: {
                    const std::string id = readLine("用户 ID: ");
                    const User* user = library_.findUser(id);
                    if (!user) {
                        std::cout << "用户不存在。\n";
                        break;
                    }
                    const std::string email = readLine("新邮箱 (空格不修改): ");
                    const std::string password = readLine("新密码 (空格不修改): ");
                    User newInfo(id, user->getName(), password, email,
                                 user->getMaxBorrowLimit());
                    library_.updateUser(id, newInfo);
                    persist();
                    std::cout << "更新成功。\n";
                    break;
                }
                case 4: {
                    const std::string query = readLine("输入查询关键字 (输入 q 返回): ");
                    if (query == "q" || query == "quit") break;
                    const auto result = library_.searchUsers(query);
                    if (result.empty()) {
                        std::cout << "未找到匹配的用户。\n";
                        break;
                    }
                    for (const auto& u : result) std::cout << u;
                    break;
                }
                default:
                    std::cout << "无效选项。\n";
                    break;
            }
        } catch (const LibraryException& e) {
            std::cout << "操作失败：" << e.what() << "\n";
        }
    }
}

void ConsoleUI::handleRecordsMenu() const {
    while (true) {
        std::cout << "\n----- 借阅记录 -----\n"
                  << "1. 全部借阅记录\n"
                  << "2. 按用户查询借阅记录\n"
                  << "3. 按书目查询借阅记录\n"
                  << "0. 返回\n"
                  << "请选择: ";
        const int choice = readInt("");
        if (choice == 0) return;
        switch (choice) {
            case 1:
                handleAllRecords();
                break;
            case 2: {
                const std::string userId = readLine("输入用户 ID (输入 q 返回): ");
                if (userId == "q" || userId == "quit") break;
                const auto records = library_.getReaderRecords(userId);
                if (records.empty()) {
                    std::cout << "该用户暂无借阅记录。\n";
                    break;
                }
                std::cout << "用户 [" << userId << "] 的借阅记录：\n";
                for (const auto& r : records) std::cout << r;
                break;
            }
            case 3: {
                const std::string bookId = readLine("输入图书 ID (输入 q 返回): ");
                if (bookId == "q" || bookId == "quit") break;
                const auto records = library_.getBookRecords(bookId);
                if (records.empty()) {
                    std::cout << "该书暂无借阅记录。\n";
                    break;
                }
                std::cout << "图书 [" << bookId << "] 的借阅记录：\n";
                for (const auto& r : records) std::cout << r;
                break;
            }
            default:
                std::cout << "无效选项。\n";
                break;
        }
    }
}

void ConsoleUI::handleAllRecords() const {
    const auto& records = library_.getRecords();
    if (records.empty()) {
        std::cout << "暂无借阅记录。\n";
        return;
    }
    for (const auto& r : records) std::cout << r;
}

void ConsoleUI::handleStatistics() const {
    const auto& records = library_.getRecords();
    const auto& books = library_.getBooks();
    const auto& users = library_.getUsers();

    std::cout << "===== 最受欢迎图书 (Top 5) =====\n";
    const auto topBooks = Statistics::getTopBooks(records, books, 5);
    if (topBooks.empty()) {
        std::cout << "无数据。\n";
    } else {
        int rank = 1;
        for (const auto& kv : topBooks)
            std::cout << rank++ << ". " << kv.first.getTitle() << " (" << kv.second
                      << " 次)\n";
    }

    std::cout << "===== 最活跃用户 (Top 5) =====\n";
    const auto topUsers = Statistics::getTopUsers(records, users, 5);
    if (topUsers.empty()) {
        std::cout << "无数据。\n";
    } else {
        int rank = 1;
        for (const auto& kv : topUsers)
            std::cout << rank++ << ". " << kv.first.getName() << " (" << kv.second << " 本)\n";
    }

    std::cout << "===== 平均借阅时长 =====\n";
    std::cout << Statistics::getSystemAverageBorrowDuration(records) << " 天\n";
}