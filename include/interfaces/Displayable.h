#pragma once

#include <ostream>

class Displayable {
public:
    virtual void display(std::ostream& os) const = 0;
    //纯虚函数，参数为输出流基类std::ostream的引用
    virtual ~Displayable() = default;
};

//内联重载<<
inline std::ostream& operator<<(std::ostream& os, const Displayable& d) {
    d.display(os);
    return os;
}
