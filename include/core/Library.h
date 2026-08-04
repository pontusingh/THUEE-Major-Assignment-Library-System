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
    Library();

    const std::vector<Book>& getBooks() const;
    const std::vector<User>& getUsers() const;
    const std::vector<Librarian>& getLibrarians() const;
    const std::vector<BorrowRecord>& getRecords() const;

    LoginRole login(const std::string& id, const std::string& password) const;

    void borrowBook(const std::string& userId, const std::string& bookId);
    void returnBook(const std::string& userId, const std::string& bookId);

    void addBook(const Book& book);
    void removeBook(const std::string& bookId);
    void updateBook(const std::string& bookId, const Book& newInfo);

    void addUser(const User& user);
    void removeUser(const std::string& userId);
    void updateUser(const std::string& userId, const User& newInfo);

    std::vector<Book> searchBooks(const std::string& query) const;
    std::vector<BorrowRecord> getReaderRecords(const std::string& userId) const;
    std::vector<BorrowRecord> getBookRecords(const std::string& bookId) const;

private:
    std::vector<Book> books_;
    std::vector<User> users_;
    std::vector<Librarian> librarians_;
    std::vector<BorrowRecord> records_;
};
