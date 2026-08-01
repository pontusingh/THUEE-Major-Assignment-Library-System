#pragma once
#include <string>

class Searchable {
public:
    virtual bool matches(const std::string& query) const = 0;
    // 纯虚函数，参数为字符串类std::string常引用
    virtual ~Searchable() = default;
};
