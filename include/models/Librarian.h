#pragma once

#include <string>

#include "models/Person.h"

class Librarian : public Person {
public:
    Librarian() = default;

    Librarian(std::string id, std::string name, std::string password, std::string email,
              int privilegeLevel);

    virtual ~Librarian() = default;

    int getPrivilegeLevel() const;
    bool canManageUsers() const;

    void display(std::ostream& os) const override;

private:
    int privilegeLevel_ = 1;
};
