#pragma once

#include <stdexcept>
#include <string>

class LibraryException : public std::runtime_error {
public:
    LibraryException() : std::runtime_error("系统异常") {}
    explicit LibraryException(const std::string& msg) : std::runtime_error(msg) {}
    ~LibraryException() override = default;
};

class UserNotFoundException : public LibraryException {
public:
    UserNotFoundException() : LibraryException("用户不存在") {}
    explicit UserNotFoundException(const std::string& msg) : LibraryException(msg) {}
    ~UserNotFoundException() override = default;
};

class BookNotFoundException : public LibraryException {
public:
    BookNotFoundException() : LibraryException("图书不存在") {}
    explicit BookNotFoundException(const std::string& msg) : LibraryException(msg) {}
    ~BookNotFoundException() override = default;
};

class BorrowDeniedException : public LibraryException {
public:
    BorrowDeniedException() : LibraryException("图书不可借") {}
    explicit BorrowDeniedException(const std::string& msg) : LibraryException(msg) {}
    ~BorrowDeniedException() override = default;
};

class DuplicateIdException : public LibraryException {
public:
    DuplicateIdException() : LibraryException("ID 已存在") {}
    explicit DuplicateIdException(const std::string& msg) : LibraryException(msg) {}
    ~DuplicateIdException() override = default;
};

class RecordNotFoundException : public LibraryException {
public:
    RecordNotFoundException() : LibraryException("记录不存在") {}
    explicit RecordNotFoundException(const std::string& msg) : LibraryException(msg) {}
    ~RecordNotFoundException() override = default;
};

class InvalidInputException : public LibraryException {
public:
    InvalidInputException() : LibraryException("输入非法") {}
    explicit InvalidInputException(const std::string& msg) : LibraryException(msg) {}
    ~InvalidInputException() override = default;
};

class FileStorageException : public LibraryException {
public:
    FileStorageException() : LibraryException("文件读写失败") {}
    explicit FileStorageException(const std::string& msg) : LibraryException(msg) {}
    ~FileStorageException() override = default;
};
