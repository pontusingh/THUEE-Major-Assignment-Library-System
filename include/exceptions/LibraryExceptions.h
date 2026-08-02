#pragma once

#include <stdexcept>
#include <string>

class UserNotFoundException : public std::runtime_error {
public:
    UserNotFoundException() : std::runtime_error("用户不存在") {}
    explicit UserNotFoundException(const std::string& msg) : std::runtime_error(msg) {}
    ~UserNotFoundException() override = default;
};

class BookNotFoundException : public std::runtime_error {
public:
    BookNotFoundException() : std::runtime_error("图书不存在") {}
    explicit BookNotFoundException(const std::string& msg) : std::runtime_error(msg) {}
    ~BookNotFoundException() override = default;
};

class BookUnavailableException : public std::runtime_error {
public:
    BookUnavailableException() : std::runtime_error("图书不可借") {}
    explicit BookUnavailableException(const std::string& msg) : std::runtime_error(msg) {}
    ~BookUnavailableException() override = default;
};

class DuplicateIdException : public std::runtime_error {
public:
    DuplicateIdException() : std::runtime_error("ID 已存在") {}
    explicit DuplicateIdException(const std::string& msg) : std::runtime_error(msg) {}
    ~DuplicateIdException() override = default;
};

class RecordNotFoundException : public std::runtime_error {
public:
    RecordNotFoundException() : std::runtime_error("记录不存在") {}
    explicit RecordNotFoundException(const std::string& msg) : std::runtime_error(msg) {}
    ~RecordNotFoundException() override = default;
};

class InvalidInputException : public std::runtime_error {
public:
    InvalidInputException() : std::runtime_error("输入非法") {}
    explicit InvalidInputException(const std::string& msg) : std::runtime_error(msg) {}
    ~InvalidInputException() override = default;
};
