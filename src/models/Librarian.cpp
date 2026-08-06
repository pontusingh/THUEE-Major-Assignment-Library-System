#include "models/Librarian.h"

#include <utility>

Librarian::Librarian(std::string id, std::string name, std::string password, std::string email,
                     int privilegeLevel)
    : Person(std::move(id), std::move(name), std::move(password), std::move(email)),
      privilegeLevel_(privilegeLevel) {}

int Librarian::getPrivilegeLevel() const { return privilegeLevel_; }

bool Librarian::canManageUsers() const { return privilegeLevel_ > 1; }

void Librarian::display(std::ostream& os) const {
    os << "管理员[ID=" << getId()
       << ", 姓名=" << getName()
       << ", 权限等级=" << getPrivilegeLevel()
       << (canManageUsers() ? "(可管理用户)" : "(不可管理用户)")
       << "]\n";
}