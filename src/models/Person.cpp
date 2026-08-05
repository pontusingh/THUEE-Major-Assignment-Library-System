#include <utility>

#include "models/Person.h"

Person::Person(std::string id, std::string name, std::string password, std::string email)
    : id_(std::move(id)),//移动语义，不用拷贝，不用重新申请内存
      name_(std::move(name)),
      password_(std::move(password)),
      email_(std::move(email)) {}

const std::string& Person::getId() const { return id_; }

const std::string& Person::getName() const { return name_; }

const std::string& Person::getEmail() const { return email_; }

const std::string& Person::getPassword() const { return password_; }

void Person::setPassword(const std::string& password) { password_ = password; }

void Person::setEmail(const std::string& email) { email_ = email; }

bool Person::validatePassword(const std::string& pwd) const { return pwd == password_; }