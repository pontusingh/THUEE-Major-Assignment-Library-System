#pragma once

#include <ostream>

class Displayable {
public:
    virtual void display(std::ostream& os) const = 0;
    virtual ~Displayable() = default;
};

inline std::ostream& operator<<(std::ostream& os, const Displayable& d) {
    d.display(os);
    return os;
}
