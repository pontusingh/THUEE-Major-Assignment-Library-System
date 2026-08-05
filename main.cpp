// main.cpp — 核心层断言驱动（Step 2 阶段版本，Step 3/4 将改写）

#include <cmath>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include "core/Library.h"
#include "models/Book.h"
#include "models/BorrowRecord.h"
#include "models/User.h"
#include "stats/Statistics.h"

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

// 供断言复现 Library 内部日期计算的工具
std::string todayStr() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", now);
    return buf;
}

std::string addDays(const std::string& date, int days) {
    int y = 0, m = 0, d = 0;
    std::sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d);
    std::tm t = {};
    t.tm_year = y - 1900;
    t.tm_mon = m - 1;
    t.tm_mday = d;
    t.tm_hour = 12;
    std::time_t tt = std::mktime(&t);
    tt += static_cast<std::time_t>(days) * 86400;
    std::tm* r = std::localtime(&tt);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", r);
    return buf;
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
    Library lib;

    // ---- 登录 ----
    check(lib.login("admin", "admin123") == LoginRole::Librarian, "登录 内置管理员");
    check(lib.login("admin", "wrong") == LoginRole::None, "登录 密码错误");
    check(lib.login("nobody", "x") == LoginRole::None, "登录 用户不存在");

    // ---- 图书/用户 CRUD ----
    lib.addBook(Book("b001", "红楼梦", "曹雪芹", "古典,小说", "文学", "人民文学出版社", "A-1"));
    lib.addBook(Book("b002", "C++入门", "张三", "cpp,编程", "技术", "清华大学出版社", "A-2"));
    lib.addBook(Book("b003", "西游记", "吴承恩", "古典,神魔", "文学", "人民文学出版社", "A-3"));
    lib.addBook(Book("b004", "Java实战", "李四", "java,编程", "技术", "机械工业出版社", "B-1"));
    lib.addUser(User("u001", "王五", "pwd1", "w@example.com", 1));
    lib.addUser(User("u002", "赵六", "pwd2", "z@example.com", 5));

    check(throws([&] { lib.addBook(Book("b001", "重复", "", "", "", "", "")); }),
          "addBook 重复 ID 抛 DuplicateIdException");
    check(throws([&] { lib.addUser(User("u001", "重复", "", "", 5)); }),
          "addUser 重复 ID 抛 DuplicateIdException");
    check(throws([&] { lib.addBook(Book("bad,id", "x", "", "", "", "", "")); }),
          "addBook ID 含逗号抛 InvalidInputException");
    check(throws([&] { lib.addBook(Book("bad;id", "x", "", "", "", "", "")); }),
          "addBook ID 含分号抛 InvalidInputException");
    check(throws([&] { lib.addUser(User("bad|id", "x", "", "", 5)); }),
          "addUser ID 含竖线抛 InvalidInputException");
    check(throws([&] { lib.addBook(Book("", "x", "", "", "", "", "")); }),
          "addBook 空 ID 抛 InvalidInputException");
    check(throws([&] { lib.removeBook("nope"); }), "removeBook 不存在抛 BookNotFoundException");
    check(throws([&] { lib.removeUser("nope"); }), "removeUser 不存在抛 UserNotFoundException");
    check(throws([&] { lib.updateBook("nope", Book("nope", "x", "", "", "", "", "")); }),
          "updateBook 不存在抛 BookNotFoundException");
    check(throws([&] { lib.updateUser("nope", User("nope", "x", "", "", 5)); }),
          "updateUser 不存在抛 UserNotFoundException");

    lib.updateBook("b002", Book("b002", "C++入门与进阶", "张三", "cpp,编程", "技术",
                                "清华大学出版社", "A-2"));
    check(lib.findBook("b002") != nullptr && lib.findBook("b002")->getTitle() == "C++入门与进阶",
          "updateBook 整体更新生效");

    lib.updateBook("b002", Book("b002", "", "", "", "", "", "C区-9排"));
    check(lib.findBook("b002")->getTitle() == "C++入门与进阶",
          "updateBook 空字段保持原值");
    check(lib.findBook("b002")->getLocation() == "C区-9排",
          "updateBook 非空 location 替换原值");
    check(lib.findBook("b002")->getId() == "b002", "updateBook ID 保持不变");

    lib.updateUser("u002", User("u002", "改名者", "otherpwd", "newemail@example.com", 99));
    check(lib.findUser("u002")->getEmail() == "newemail@example.com", "updateUser 更新邮箱");
    check(lib.findUser("u002")->getName() == "赵六", "updateUser 姓名保持不变");
    check(lib.findUser("u002")->getMaxBorrowLimit() == 5, "updateUser 借阅上限保持不变");
    check(lib.findUser("u002")->getCurrentBorrowCount() == 0, "updateUser 已借列表保持不变");

    // ---- 借书 ----
    lib.borrowBook("u001", "b001");
    check(lib.getRecords().size() == 1 && lib.getRecords()[0].getRecordId() == "R1",
          "borrowBook 生成记录 R1");
    check(lib.findBook("b001")->isBorrowed(), "borrowBook 后图书已借出");
    check(lib.findUser("u001")->getCurrentBorrowCount() == 1, "borrowBook 后读者已借数 1");

    lib.updateBook("b001", Book("b001", "红楼梦修订版", "", "", "", "", ""));
    check(lib.findBook("b001")->isBorrowed(), "updateBook 不覆盖借阅状态");
    check(lib.findBook("b001")->getTitle() == "红楼梦修订版", "updateBook 非空 title 生效");

    check(throws([&] { lib.borrowBook("u001", "b001"); }),
          "重复借同一本书抛 BorrowDeniedException(已借出)");
    check(throws([&] { lib.borrowBook("u001", "b002"); }),
          "超限借阅抛 BorrowDeniedException(上限)");
    check(throws([&] { lib.borrowBook("ghost", "b001"); }),
          "借阅不存在用户抛 UserNotFoundException");
    check(throws([&] { lib.borrowBook("u001", "ghost"); }),
          "借阅不存在图书抛 BookNotFoundException");

    // ---- 续借 ----
    const std::string today = todayStr();
    const std::string due1 = addDays(today, 28);
    check(lib.getRecords()[0].getDueDate() == due1, "借阅默认应还日期 = 今日+28天");
    lib.renewBook("u001", "b001", 14);
    check(lib.getRecords()[0].getDueDate() == addDays(due1, 14), "renewBook 续借加 14 天");
    check(lib.getRecords()[0].isRenewed(), "renewBook 后记录置续借标记");
    check(throws([&] { lib.renewBook("u001", "b001", 14); }),
          "二次续借抛 BorrowDeniedException");

    // ---- 还书 ----
    lib.returnBook("u001", "b001");
    check(!lib.findBook("b001")->isBorrowed(), "returnBook 后图书在馆");
    check(lib.findUser("u001")->getCurrentBorrowCount() == 0, "returnBook 后读者已借数 0");
    check(lib.getRecords()[0].getReturnDate() == today, "returnBook 记录填写归还日期");
    check(throws([&] { lib.returnBook("u001", "b001"); }),
          "重复还书抛 RecordNotFoundException");
    check(throws([&] { lib.renewBook("u001", "b001", 14); }),
          "对已归还记录续借抛 RecordNotFoundException");

    // 再次借阅 recordId 自增
    lib.borrowBook("u001", "b001");
    check(lib.getRecords().size() == 2 && lib.getRecords()[1].getRecordId() == "R2",
          "二次借阅生成记录 R2");
    lib.returnBook("u001", "b001");

    // ---- 历史查询 ----
    lib.borrowBook("u002", "b002");
    lib.borrowBook("u002", "b003");
    check(lib.getReaderRecords("u002").size() == 2, "getReaderRecords 过滤读者记录");
    check(lib.getBookRecords("b003").size() == 1, "getBookRecords 过滤图书记录");
    check(lib.getReaderRecords("ghost").empty(), "getReaderRecords 未知读者为空");
    check(lib.getBookRecords("ghost").empty(), "getBookRecords 未知图书为空");

    // ---- 模糊查询 ----
    auto hitJava = lib.searchBooks("java");
    check(hitJava.size() == 1 && hitJava[0].getId() == "b004", "searchBooks 大小写不敏感");
    auto hitClassic = lib.searchBooks("古典");
    check(hitClassic.size() == 2, "searchBooks 关键字命中 2 本");
    check(lib.searchBooks("不存在的书").empty(), "searchBooks 无命中为空");
    check(lib.searchUsers("赵六").size() == 1, "searchUsers 按姓名命中");
    check(lib.searchUsers("u00").size() == 2, "searchUsers 按 ID 片段命中");
    check(lib.findBook("b001") != nullptr && lib.findBook("ghost") == nullptr,
          "findBook 精确查找");
    check(lib.findUser("u001") != nullptr && lib.findUser("ghost") == nullptr,
          "findUser 精确查找");

    // ---- 用户删除 ----
    lib.removeUser("u001");
    check(lib.findUser("u001") == nullptr, "removeUser 后用户消失");
    check(lib.login("u001", "pwd1") == LoginRole::None, "删除后登录失败");
    check(throws([&] { lib.borrowBook("u001", "b001"); }),
          "删除后借阅抛 UserNotFoundException");

    // ---- Statistics ----
    std::vector<BorrowRecord> recs;
    BorrowRecord rA("rA", "b001", "u1", "2026-01-01", "2026-01-31");
    BorrowRecord rB("rB", "b002", "u1", "2026-02-01", "2026-03-03");
    BorrowRecord rC("rC", "b001", "u2", "2026-03-01", "2026-03-31");
    BorrowRecord rD("rD", "b003", "u2", "2026-04-01", "2026-05-01");
    BorrowRecord rE("rE", "b003", "u1", "2026-05-01", "2026-05-31");
    rA.setReturnDate("2026-01-11");
    rB.setReturnDate("2026-02-16");
    rC.setReturnDate("2026-03-06");
    rE.setReturnDate("2026-05-12");
    recs = {rA, rB, rC, rD, rE};

    std::vector<Book> statBooks;
    statBooks.push_back(Book("b001", "红楼梦", "曹雪芹", "古典", "文学", "人民文学出版社", "A"));
    statBooks.push_back(Book("b002", "C++入门", "张三", "cpp", "技术", "清华大学出版社", "A"));
    statBooks.push_back(Book("b003", "西游记", "吴承恩", "古典", "文学", "人民文学出版社", "A"));

    std::vector<User> statUsers;
    statUsers.push_back(User("u1", "甲", "", "", 5));
    statUsers.push_back(User("u2", "乙", "", "", 5));

    auto topBooks = Statistics::getTopBooks(recs, statBooks, 3);
    check(topBooks.size() == 3 && topBooks[0].first.getId() == "b001" &&
              topBooks[0].second == 2 && topBooks[1].first.getId() == "b003" &&
              topBooks[1].second == 2 && topBooks[2].first.getId() == "b002" &&
              topBooks[2].second == 1,
          "getTopBooks 频次降序 + ID 决胜");

    auto topUsers = Statistics::getTopUsers(recs, statUsers, 5);
    check(topUsers.size() == 2 && topUsers[0].first.getId() == "u1" && topUsers[0].second == 3 &&
              topUsers[1].first.getId() == "u2" && topUsers[1].second == 2,
          "getTopUsers 借阅量排序");

    check(std::fabs(Statistics::getSystemAverageBorrowDuration(recs) - 10.25) < 1e-9,
          "getSystemAverageBorrowDuration 忽略未归还 = 10.25");

    auto rep1 = Statistics::generateUserReport({rA, rB, rE}, statBooks);
    check(rep1.totalBorrowedCount == 3, "报告 totalBorrowedCount = 3");
    check(rep1.totalBorrowDays == 36.0, "报告 totalBorrowDays = 36");
    check(std::fabs(rep1.averageBorrowDays - 12.0) < 1e-9, "报告 averageBorrowDays = 12");
    check(rep1.favoriteCategory == "文学", "报告 favoriteCategory = 文学");
    check(rep1.longestBorrowedBook == "C++入门", "报告 longestBorrowedBook = C++入门");

    auto rep2 = Statistics::generateUserReport({rC, rD}, statBooks);
    check(rep2.totalBorrowedCount == 1 && rep2.totalBorrowDays == 5.0 &&
              std::fabs(rep2.averageBorrowDays - 5.0) < 1e-9,
          "报告跳过未归还记录");
    check(rep2.longestBorrowedBook == "红楼梦", "报告 未归还记录不影响最久图书");

    auto repEmpty = Statistics::generateUserReport({}, statBooks);
    check(repEmpty.totalBorrowedCount == 0 && repEmpty.averageBorrowDays == 0.0 &&
              repEmpty.favoriteCategory.empty() && repEmpty.longestBorrowedBook.empty(),
          "报告 空记录全默认值");

    std::cout << "\n===== 结果: " << passCount << " PASS, " << failCount << " FAIL =====\n";
    return failCount == 0 ? 0 : 1;
}