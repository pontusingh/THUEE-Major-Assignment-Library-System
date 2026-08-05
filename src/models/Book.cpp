#include "models/Book.h"

#include <algorithm>
#include <cctype>
#include <utility>

namespace {
std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return out;
}
}  // 匿名命名空间，把辅助函数限制在Book.cpp内部

Book::Book(std::string id, std::string title, std::string author, std::string keywords,
           std::string category, std::string publisherInfo, std::string location)
    : id_(std::move(id)),
      title_(std::move(title)),
      author_(std::move(author)),
      keywords_(std::move(keywords)),
      category_(std::move(category)),
      publisherInfo_(std::move(publisherInfo)),
      location_(std::move(location)) {}

const std::string& Book::getId() const { return id_; }
const std::string& Book::getTitle() const { return title_; }
const std::string& Book::getAuthor() const { return author_; }
const std::string& Book::getKeywords() const { return keywords_; }
const std::string& Book::getCategory() const { return category_; }
const std::string& Book::getPublisherInfo() const { return publisherInfo_; }
const std::string& Book::getLocation() const { return location_; }
bool Book::isBorrowed() const { return isBorrowed_; }

void Book::setTitle(const std::string& title) { title_ = title; }
void Book::setAuthor(const std::string& author) { author_ = author; }
void Book::setKeywords(const std::string& keywords) { keywords_ = keywords; }
void Book::setCategory(const std::string& category) { category_ = category; }
void Book::setPublisherInfo(const std::string& publisherInfo) { publisherInfo_ = publisherInfo; }

void Book::borrowBook() { isBorrowed_ = true; }
void Book::returnBook() { isBorrowed_ = false; }
void Book::updateLocation(const std::string& newLoc) { location_ = newLoc; }

void Book::display(std::ostream& os) const {
    os << "图书[ID=" << id_
       << ", 书名=" << title_
       << ", 作者=" << author_
       << ", 类别=" << category_
       << ", 关键字=" << keywords_
       << ", 出版信息=" << publisherInfo_
       << ", 馆藏地=" << location_
       << ", 状态=" << (isBorrowed_ ? "已借出" : "在馆") << "]\n";
}

bool Book::matches(const std::string& query) const {
    const std::string q = toLower(query);
    return toLower(title_).find(q) != std::string::npos ||
           toLower(author_).find(q) != std::string::npos ||
           toLower(keywords_).find(q) != std::string::npos;
}

bool Book::operator==(const Book& other) const { return id_ == other.id_; }