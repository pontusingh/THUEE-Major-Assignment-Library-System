#pragma once

#include <string>
#include <vector>

#include "models/Book.h"
#include "models/User.h"
#include "models/Librarian.h"
#include "models/BorrowRecord.h"
#include "exceptions/LibraryExceptions.h"

enum class LoginRole { None, User, Librarian };

class Library {
public:
    //构造：初始化内置管理员账号（admin/admin123）
    Library();

    //数据访问：向外部（UI/统计/持久化）暴露各集合的只读引用
    const std::vector<Book>& getBooks() const;
    const std::vector<User>& getUsers() const;
    const std::vector<Librarian>& getLibrarians() const;
    const std::vector<BorrowRecord>& getRecords() const;

    //身份验证：按 id+password 校验，返回登录角色（用户/管理员/无）
    LoginRole login(const std::string& id, const std::string& password) const;

    //借阅核心：借书（校验用户/图书/状态/额度，可抛异常）、还书（可抛异常）、
    //          续借（禁止已归还/已续借/已超期记录，可抛异常）
    void borrowBook(const std::string& userId, const std::string& bookId);
    void returnBook(const std::string& userId, const std::string& bookId);
    void renewBook(const std::string& userId, const std::string& bookId, int days);

    //图书管理：新增、按id删除、按id整体更新（异常：ID冲突/图书不存在）
    void addBook(const Book& book);
    void removeBook(const std::string& bookId);
    void updateBook(const std::string& bookId, const Book& newInfo);

    //用户管理：新增、按id删除、按id整体更新（异常：ID冲突/用户不存在）
    void addUser(const User& user);
    void removeUser(const std::string& userId);
    void updateUser(const std::string& userId, const User& newInfo);

    //查询：模糊匹配返回副本列表，按id精确查找返回指针（未找到返回nullptr）
    std::vector<Book> searchBooks(const std::string& query) const;
    std::vector<User> searchUsers(const std::string& query) const;
    const Book* findBook(const std::string& bookId) const;
    const User* findUser(const std::string& userId) const;

    //历史查询：某读者的全部借阅记录、某图书的全部借阅记录
    std::vector<BorrowRecord> getReaderRecords(const std::string& userId) const;
    std::vector<BorrowRecord> getBookRecords(const std::string& bookId) const;

    //启动时注入持久化加载的借阅记录（仅追加，不校验业务规则）
    void importRecords(const std::vector<BorrowRecord>& records);

private:
    //数据容器：图书、读者、内置管理员、借阅记录
    std::vector<Book> books_;
    std::vector<User> users_;
    std::vector<Librarian> librarians_;
    std::vector<BorrowRecord> records_;
};
