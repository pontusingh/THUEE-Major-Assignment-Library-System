#pragma once

#include <string>

#include "core/Library.h"

class ConsoleUI {
public:
    explicit ConsoleUI(Library& library);//构造函数，explicit防止隐式类型转换

    void run();

private:
    //辅助方法，安全读取整行文本/整数
    std::string readLine(const std::string& prompt) const;
    int readInt(const std::string& prompt) const;

    //手动触发持久化
    void persist() const;

    //登录，区分用户和管理
    void loginLoop();
    void showUserMenu(const std::string& userId);
    void showAdminMenu();

    //用户：处理图书查询，借还，续借，记录，统计报告
    void handleSearchBooks() const;
    void handleBorrow(const std::string& userId);
    void handleReturn(const std::string& userId);
    void handleRenew(const std::string& userId);
    void handleMyRecords(const std::string& userId) const;
    void handleMyReport(const std::string& userId) const;

    //管理：管理书目，用户，记录，统计数据
    void handleBookManagement();
    void handleUserManagement();
    void handleAllRecords() const;
    void handleStatistics() const;

    Library& library_;
};
