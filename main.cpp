// main.cpp — 最终应用入口：加载持久化数据 → 启动交互控制台

#include <iostream>

#include "core/Library.h"
#include "exceptions/LibraryExceptions.h"
#include "storage/FileStorage.h"
#include "ui/ConsoleUI.h"

int main() {
    Library lib;

    // 每个集合独立加载：任一集合损坏仅警告并跳过该集合，其余正常加载
    try {
        auto books = FileStorage::loadBooks("data/books.txt");
        for (const auto& b : books) lib.addBook(b);
    } catch (const LibraryException& e) {
        std::cerr << "警告：图书数据加载失败，已跳过。原因: " << e.what() << "\n";
    }

    try {
        auto users = FileStorage::loadUsers("data/users.txt");
        for (const auto& u : users) lib.addUser(u);
    } catch (const LibraryException& e) {
        std::cerr << "警告：用户数据加载失败，已跳过。原因: " << e.what() << "\n";
    }

    try {
        lib.importRecords(FileStorage::loadRecords("data/records.txt"));
    } catch (const LibraryException& e) {
        std::cerr << "警告：借阅记录加载失败，已跳过。原因: " << e.what() << "\n";
    }

    ConsoleUI ui(lib);
    ui.run();
    return 0;
}