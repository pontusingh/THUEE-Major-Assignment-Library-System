// main.cpp — 模型层断言驱动（Step 1 阶段版本，Step 4 将改写为最终应用入口）

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "models/Book.h"
#include "models/BorrowRecord.h"
#include "models/Librarian.h"
#include "models/Person.h"
#include "models/User.h"

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

}  // namespace

int main() {
    // ---- Person 基类 ----
    Person* p = nullptr;
    User u1("u001", "张三", "pwd123", "zhangsan@example.com", 2);
    p = &u1;
    check(p->getId() == "u001", "Person getter getId");
    check(p->getName() == "张三", "Person getter getName");
    check(u1.getEmail() == "zhangsan@example.com", "Person getter getEmail");
    check(u1.validatePassword("pwd123"), "validatePassword 正确密码");
    check(!u1.validatePassword("wrong"), "validatePassword 错误密码");
    u1.setPassword("newpwd");
    check(u1.validatePassword("newpwd"), "setPassword 后验证通过");
    u1.setEmail("new@example.com");
    check(u1.getEmail() == "new@example.com", "setEmail 生效");

    // ---- User ----
    check(u1.getMaxBorrowLimit() == 2, "User getMaxBorrowLimit");
    check(u1.getCurrentBorrowCount() == 0, "User 初始已借数 0");
    check(u1.canBorrow(), "User canBorrow 未借满");
    u1.addBorrowedBook("b001");
    u1.addBorrowedBook("b002");
    check(u1.getCurrentBorrowCount() == 2, "addBorrowedBook 后已借数 2");
    check(!u1.canBorrow(), "User canBorrow 借满后拒绝");
    check(u1.getBorrowedBookIds()[0] == "b001", "getBorrowedBookIds 顺序");
    u1.addBorrowedBook("b003");
    check(u1.getCurrentBorrowCount() == 3, "超限后仍可加入（借阅上限由 Library 层校验）");
    u1.removeBorrowedBook("b002");
    check(u1.getCurrentBorrowCount() == 2, "removeBorrowedBook 后已借数 2");
    u1.removeBorrowedBook("not-exist");
    check(u1.getCurrentBorrowCount() == 2, "removeBorrowedBook 不存在的 ID 静默忽略");
    check(u1.matches("u00"), "User matches 匹配 ID 片段");
    check(u1.matches("张三"), "User matches 匹配姓名");
    check(!u1.matches("李四"), "User matches 不匹配");

    // ---- Librarian ----
    Librarian lib1("l001", "管理员甲", "admin123", "admin@lib.com", 1);
    Librarian lib2("l002", "管理员乙", "pass", "lib2@lib.com", 3);
    check(lib1.getPrivilegeLevel() == 1, "Librarian getPrivilegeLevel");
    check(!lib1.canManageUsers(), "Librarian 等级1 不可管理用户");
    check(lib2.canManageUsers(), "Librarian 等级3 可管理用户");

    // ---- Book ----
    Book b1("b001", "C++ Primer", "Lippman", "cpp,入门", "技术", "人民邮电出版社", "A区-1排");
    Book b2("b002", "Java 编程思想", "Bruce Eckel", "java,oop", "技术", "机械工业出版社", "B区-2排");
    check(b1.getTitle() == "C++ Primer", "Book getTitle");
    check(b1.getAuthor() == "Lippman", "Book getAuthor");
    check(b1.getCategory() == "技术", "Book getCategory");
    check(!b1.isBorrowed(), "Book 初始在馆");
    b1.borrowBook();
    check(b1.isBorrowed(), "Book borrowBook 后已借出");
    b1.returnBook();
    check(!b1.isBorrowed(), "Book returnBook 后在馆");
    b1.updateLocation("C区-3排");
    check(b1.getLocation() == "C区-3排", "Book updateLocation 更新馆藏地");
    b1.setTitle("C++ Primer 5th");
    check(b1.getTitle() == "C++ Primer 5th", "Book setTitle");
    b1.setKeywords("cpp,primer");
    check(b1.getKeywords() == "cpp,primer", "Book setKeywords");

    // matches 不区分大小写
    check(b2.matches("java"), "Book matches 书名小写");
    check(b2.matches("JAVA"), "Book matches 书名大写");
    check(b2.matches("Bruce"), "Book matches 作者");
    check(b2.matches("OOP"), "Book matches 关键字大小写不敏感");
    check(!b2.matches("python"), "Book matches 不匹配");

    // operator==
    Book b3("b001", "另一本书", "某人", "", "", "", "");
    check(b1 == b3, "Book operator== 同 ID 相等");
    check(!(b1 == b2), "Book operator== 异 ID 不等");

    // ---- BorrowRecord ----
    BorrowRecord r1("r001", "b001", "u001", "2026-07-20", "2026-08-03");
    BorrowRecord r2("r002", "b002", "u001", "2026-07-01", "2026-07-15");
    check(r1.getRecordId() == "r001", "BorrowRecord getRecordId");
    check(r1.getBookId() == "b001", "BorrowRecord getBookId");
    check(r1.getUserId() == "u001", "BorrowRecord getUserId");
    check(r1.getBorrowDate() == "2026-07-20", "BorrowRecord getBorrowDate");
    check(r1.getDueDate() == "2026-08-03", "BorrowRecord getDueDate");
    check(!r1.isReturned(), "BorrowRecord 初始未归还");
    check(!r1.isRenewed(), "BorrowRecord 初始未续借");

    // isOverdue 边界：当天不算逾期
    check(!r1.isOverdue("2026-08-03"), "isOverdue 当天不算逾期");
    check(r1.isOverdue("2026-08-04"), "isOverdue 超一天算逾期");
    check(!r1.isOverdue("2026-08-02"), "isOverdue 未到期不算");
    check(r1.calculateOverdueDays("2026-08-15") == 12, "calculateOverdueDays 逾期 12 天");
    check(r1.calculateOverdueDays("2026-08-02") == 0, "calculateOverdueDays 未逾期为 0");
    check(!r2.isOverdue("2026-07-15"), "isOverdue 另一条当天边界");

    // renew 跨月
    r1.renew(30);
    check(r1.getDueDate() == "2026-09-02", "renew(30) 跨月加天 08-03 -> 09-02");
    check(r1.isRenewed(), "renew 后 isRenewed 置位");

    BorrowRecord r3("r003", "b001", "u001", "2026-01-30", "2026-02-10");
    r3.renew(10);
    check(r3.getDueDate() == "2026-02-20", "renew(10) 跨月 02-10 -> 02-20");

    r1.setReturnDate("2026-08-05");
    check(r1.isReturned(), "setReturnDate 后 isReturned");
    check(!r1.isOverdue("2026-08-06"), "已归还记录不判逾期");

    // operator<< 多态输出
    std::cout << "---- 多态输出演示 ----\n";
    const Displayable& dp = u1;
    const Displayable& dl = lib2;
    const Displayable& db = b1;
    const Displayable& dr = r2;
    std::cout << dp;
    std::cout << dl;
    std::cout << db;
    std::cout << dr;

    // operator< 排序
    std::vector<BorrowRecord> recs = {r1, r2, r3};
    std::sort(recs.begin(), recs.end());
    check(recs[0].getRecordId() == "r003" && recs[1].getRecordId() == "r002" &&
              recs[2].getRecordId() == "r001",
          "BorrowRecord operator< 按借书日期排序");

    std::cout << "\n===== 结果: " << passCount << " PASS, " << failCount << " FAIL =====\n";
    return failCount == 0 ? 0 : 1;
}