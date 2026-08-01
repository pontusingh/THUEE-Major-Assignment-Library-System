#pragma once

#include <string>

#include "interfaces/Displayable.h"

class Person : public Displayable {
    public:
        Person() = default;

        Person(std::string id, std::string name, std::string password, std::string email);

        virtual ~Person() = default;

        //getter访问器
        const std::string& getId() const;
        const std::string& getName() const;
        const std::string& getEmail() const;

        //setter修改器
        void setPassword(const std::string& password);
        void setEmail(const std::string& email);

        //验证密码
        bool validatePassword(const std::string& pwd) const;

        void display(std::ostream& os) const override = 0;

    private:
        std::string id_;
        std::string name_;
        std::string password_;
        std::string email_;
};
