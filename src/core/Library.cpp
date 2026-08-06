#include "core/Library.h"

#include <exception>
#include <string>
#include <utility>
#include <vector>

#include "utils/DateUtils.h"

namespace {

constexpr int DEFAULT_BORROW_DAYS = 28;

// 校验 ID：不允许为空或含 | , ; 换行（保证文件无需转义）
void validateId(const std::string& id) {
    const std::string forbidden = "|,;\n";
    if (id.empty()) throw InvalidInputException("ID 不能为空");
    if (id.find_first_of(forbidden) != std::string::npos)
        throw InvalidInputException("ID 含非法字符");
}

// 生成下一条借阅记录 ID：R + (现有记录数字后缀最大值 + 1)
std::string nextRecordId(const std::vector<BorrowRecord>& records) {
    int maxNum = 0;
    for (const auto& r : records) {
        const std::string& id = r.getRecordId();
        size_t pos = id.find_last_not_of("0123456789");
        size_t start = (pos == std::string::npos) ? 0 : pos + 1;
        if (start < id.size()) {
            try {
                int n = std::stoi(id.substr(start));
                if (n > maxNum) maxNum = n;
            } catch (const std::exception&) {
            }
        }
    }
    return "R" + std::to_string(maxNum + 1);
}

}  // namespace

Library::Library() {
    librarians_.push_back(Librarian("admin", "admin", "admin123", "admin@lib.com", 3));
}

const std::vector<Book>& Library::getBooks() const { return books_; }
const std::vector<User>& Library::getUsers() const { return users_; }
const std::vector<Librarian>& Library::getLibrarians() const { return librarians_; }
const std::vector<BorrowRecord>& Library::getRecords() const { return records_; }

LoginRole Library::login(const std::string& id, const std::string& password) const {
    for (const auto& l : librarians_) {//for循环遍历librarian容器，取引用
        if (l.getId() == id && l.validatePassword(password)) return LoginRole::Librarian;
    }//ID对上，密码对上，登录
    for (const auto& u : users_) {
        if (u.getId() == id && u.validatePassword(password)) return LoginRole::User;
    }
    return LoginRole::None;
}

void Library::borrowBook(const std::string& userId, const std::string& bookId) {
    User* user = nullptr;
    for (size_t i = 0; i < users_.size(); ++i) {
        if (users_[i].getId() == userId) {
            user = &users_[i];
            break;
        }
    }//遍历users
    if (!user) throw UserNotFoundException();

    Book* book = nullptr;
    for (size_t i = 0; i < books_.size(); ++i) {
        if (books_[i].getId() == bookId) {
            book = &books_[i];
            break;
        }
    }
    if (!book) throw BookNotFoundException();
    if (book->isBorrowed()) throw BorrowDeniedException("图书已被借出");
    if (!user->canBorrow()) throw BorrowDeniedException("借阅数量已达上限");

    const std::string today = Date::todayStr();
    //图书侧、读者侧、记录侧
    book->borrowBook();//(*book).borrowBook()
    user->addBorrowedBook(bookId);
    records_.push_back(BorrowRecord(nextRecordId(records_), bookId, userId, today,
                                    Date::addDays(today, DEFAULT_BORROW_DAYS)));
}

void Library::returnBook(const std::string& userId, const std::string& bookId) {
    //记录侧
    BorrowRecord* rec = nullptr;
    for (size_t i = 0; i < records_.size(); ++i) {
        const BorrowRecord& r = records_[i];
        if (r.getUserId() == userId && r.getBookId() == bookId && !r.isReturned()) {
            rec = &records_[i];
            break;
        }
    }
    if (!rec) throw RecordNotFoundException();
    rec->setReturnDate(Date::todayStr());

    //图书侧
    Book* book = nullptr;
    for (size_t i = 0; i < books_.size(); ++i) {
        if (books_[i].getId() == bookId) {
            book = &books_[i];
            break;
        }
    }
    if (book) book->returnBook();

    //用户侧
    User* user = nullptr;
    for (size_t i = 0; i < users_.size(); ++i) {
        if (users_[i].getId() == userId) {
            user = &users_[i];
            break;
        }
    }
    if (user) user->removeBorrowedBook(bookId);
}

void Library::renewBook(const std::string& userId, const std::string& bookId, int days) {
    BorrowRecord* rec = nullptr;
    for (size_t i = 0; i < records_.size(); ++i) {
        const BorrowRecord& r = records_[i];
        if (r.getUserId() == userId && r.getBookId() == bookId && !r.isReturned()) {
            rec = &records_[i];
            break;
        }
    }
    if (!rec) throw RecordNotFoundException();
    const std::string today = Date::todayStr();
    if (rec->isReturned()) throw BorrowDeniedException("已归还，不可续借");
    if (rec->isRenewed()) throw BorrowDeniedException("已续借过一次，不可再续");
    if (rec->isOverdue(today)) throw BorrowDeniedException("已超期，不可续借");
    rec->renew(days);
}

void Library::addBook(const Book& book) {
    validateId(book.getId());
    if (findBook(book.getId())) throw DuplicateIdException();
    books_.push_back(book);
}

void Library::removeBook(const std::string& bookId) {
    for (size_t i = 0; i < books_.size(); ++i) {
        if (books_[i].getId() == bookId) {
            books_.erase(books_.begin() + i);
            return;
        }
    }
    throw BookNotFoundException();
}

void Library::updateBook(const std::string& bookId, const Book& newInfo) {
    Book* book = nullptr;
    for (size_t i = 0; i < books_.size(); ++i) {
        if (books_[i].getId() == bookId) {
            book = &books_[i];
            break;
        }
    }
    if (!book) throw BookNotFoundException();

    // 非空字段合并：空字段保持原值；id 与借阅状态一律不动
    if (!newInfo.getTitle().empty()) book->setTitle(newInfo.getTitle());
    if (!newInfo.getAuthor().empty()) book->setAuthor(newInfo.getAuthor());
    if (!newInfo.getKeywords().empty()) book->setKeywords(newInfo.getKeywords());
    if (!newInfo.getCategory().empty()) book->setCategory(newInfo.getCategory());
    if (!newInfo.getPublisherInfo().empty()) book->setPublisherInfo(newInfo.getPublisherInfo());
    if (!newInfo.getLocation().empty()) book->updateLocation(newInfo.getLocation());
}

void Library::addUser(const User& user) {
    validateId(user.getId());
    if (findUser(user.getId())) throw DuplicateIdException();
    users_.push_back(user);
}

void Library::removeUser(const std::string& userId) {
    for (size_t i = 0; i < users_.size(); ++i) {
        if (users_[i].getId() == userId) {
            users_.erase(users_.begin() + i);
            return;
        }
    }
    throw UserNotFoundException();
}

void Library::updateUser(const std::string& userId, const User& newInfo) {
    User* user = nullptr;
    for (size_t i = 0; i < users_.size(); ++i) {
        if (users_[i].getId() == userId) {
            user = &users_[i];
            break;
        }
    }
    if (!user) throw UserNotFoundException();

    // 支持更新邮箱和密码（User 无 setName/setMaxBorrowLimit setter）；
    // 姓名、借阅上限、已借列表、id 均保持不变；空白输入（空格/回车）表示不修改
    if (!newInfo.getEmail().empty() && newInfo.getEmail() != " ") user->setEmail(newInfo.getEmail());
    if (!newInfo.getPassword().empty() && newInfo.getPassword() != " ") user->setPassword(newInfo.getPassword());
}

std::vector<Book> Library::searchBooks(const std::string& query) const {
    std::vector<Book> result;
    for (const auto& b : books_) {
        if (b.matches(query)) result.push_back(b);
    }
    return result;
}

std::vector<User> Library::searchUsers(const std::string& query) const {
    std::vector<User> result;
    for (const auto& u : users_) {
        if (u.matches(query)) result.push_back(u);
    }
    return result;
}

const Book* Library::findBook(const std::string& bookId) const {
    for (size_t i = 0; i < books_.size(); ++i) {
        if (books_[i].getId() == bookId) return &books_[i];
    }
    return nullptr;
}

const User* Library::findUser(const std::string& userId) const {
    for (size_t i = 0; i < users_.size(); ++i) {
        if (users_[i].getId() == userId) return &users_[i];
    }
    return nullptr;
}

std::vector<BorrowRecord> Library::getReaderRecords(const std::string& userId) const {
    std::vector<BorrowRecord> result;
    for (const auto& r : records_) {
        if (r.getUserId() == userId) result.push_back(r);
    }
    return result;
}

std::vector<BorrowRecord> Library::getBookRecords(const std::string& bookId) const {
    std::vector<BorrowRecord> result;
    for (const auto& r : records_) {
        if (r.getBookId() == bookId) result.push_back(r);
    }
    return result;
}

void Library::importRecords(const std::vector<BorrowRecord>& records) {
    records_.insert(records_.end(), records.begin(), records.end());
}