#pragma once

#include <string>

#include "interfaces/Displayable.h"
#include "interfaces/Searchable.h"

class Book : public Displayable, public Searchable {
public:
    Book() = default;

    Book(std::string id, std::string title, std::string author, std::string keywords,
         std::string category, std::string publisherInfo, std::string location);

    virtual ~Book() = default;

    const std::string& getId() const;
    const std::string& getTitle() const;
    const std::string& getAuthor() const;
    const std::string& getKeywords() const;
    const std::string& getCategory() const;
    const std::string& getPublisherInfo() const;
    const std::string& getLocation() const;
    bool isBorrowed() const;

    void setTitle(const std::string& title);
    void setAuthor(const std::string& author);
    void setKeywords(const std::string& keywords);
    void setCategory(const std::string& category);
    void setPublisherInfo(const std::string& publisherInfo);

    void borrowBook();
    void returnBook();
    void updateLocation(const std::string& newLoc);

    void display(std::ostream& os) const override;
    bool matches(const std::string& query) const override;

    bool operator==(const Book& other) const;

private:
    std::string id_;
    std::string title_;
    std::string author_;
    std::string keywords_;
    std::string category_;
    std::string publisherInfo_;
    std::string location_;
    bool isBorrowed_ = false;
};
