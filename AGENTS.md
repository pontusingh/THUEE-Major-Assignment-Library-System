# AGENTS.md

## 项目
图书管理系统

## 基本要求
- 类数目不少于 4 个，每个类属性/方法均多于 3 个
- 定义一个抽象类，用继承派生这些类，并设计一个多重继承的派生类
- 引入虚函数的多态性、运算符重载等机制

## 功能要求
1. 用户信息管理：存储用户信息（姓名、借阅历史等），支持查询、增加、删除、修改
2. 图书信息管理：存储图书信息（书名、作者、类别、关键字、简介、借阅状态等），支持增删改查；查询需具备模糊匹配（如输入一字可查到包含该字的图书）
3. 借还书记录管理：记录借还书时间，查看用户或图书的借阅历史
4. 统计分析：如最受欢迎书籍、借阅最多用户、借阅量随时间趋势（参考年度报告）
5. 特殊情况处理：对非法输入等做合适处理

## 类层次结构

### 1. 接口类（纯虚基类）
*   **`Displayable`**
    *   **属性**：无
    *   **方法**：
        1.  `virtual void display(std::ostream& os) const = 0;`（纯虚函数，用于多态打印）

*   **`Searchable`**
    *   **属性**：无
    *   **方法**：
        1.  `virtual bool matches(const std::string& query) const = 0;`（纯虚函数，用于模糊匹配）

### 2. 实体类（Domain Models）

*   **`Person`（抽象基类）** —— 继承自 `Displayable`
    *   **属性**：`id`（唯一标识）、`name`（姓名）、`password`（登录密码）、`email`（邮箱）。
    *   **方法**：
        1.  Getters/Setters（ `getId()`, `getName()` ）
        2.  `validatePassword(string pwd) const`（验证密码）
        3.  `virtual void display(std::ostream& os) const = 0`（重写接口，仍为纯虚）

*   **`User`（普通读者）** —— **多重继承自 `Person` 和 `Searchable`**
    *   **属性**：`maxBorrowLimit`（最大借书量）、`currentBorrowCount`（当前已借数）、`borrowedBookIds`（已借图书 ID 列表）。
    *   **方法**：
        1.  `canBorrow() const`（是否可借）
        2.  `addBorrowedBook(string bookId)`（名下添加图书）
        3.  `removeBorrowedBook(string bookId)`（名下移除图书）
        4.  `display(std::ostream& os) const`（具体实现：打印读者及已借图书信息）
        5.  `matches(const std::string& query) const`（具体实现：匹配读者 ID 或姓名）

*   **`Librarian`（管理员）** —— 继承自 `Person`
    *   **属性**：`privilegeLevel`（权限等级）。
    *   **方法**：
        1.  `getPrivilegeLevel() const`（获取权限）
        2.  `canManageUsers() const`（是否可管理用户）
        3.  `display(std::ostream& os) const`（具体实现：打印管理员工作信息）

*   **`Book`（图书）** —— **多重继承自 `Displayable` 和 `Searchable`**
    *   **属性**：`id`（唯一图书ID）、`title`（书名）、`author`（作者）、`keywords`（关键字）、`category`（类别）、`publisherInfo`（出版信息）、`location`（馆藏地）、`isBorrowed`（借阅状态）。
    *   **方法**：
        1.  `borrowBook()` / `returnBook()`（修改借书状态）
        2.  `updateLocation(string newLoc)`（修改馆藏地）
        3.  `display(std::ostream& os) const`（具体实现：打印图书详细信息）
        4.  `matches(const std::string& query) const`（具体实现：不区分大小写匹配书名/作者/关键字）
        5.  `operator==`（运算符重载：根据 ID 判断是否为同一本书）

*   **`BorrowRecord`（借阅记录）** —— 继承自 `Displayable`
    *   **属性**：`recordId`（记录ID）、`bookId`（图书ID）、`userId`（用户ID）、`borrowDate`（借书日期）、`dueDate`（应还日期）、`returnDate`（归还日期）、`isRenewed`（是否续借）。
    *   **方法**：
        1.  `isOverdue(string currentDate) const`（判断是否超期）
        2.  `calculateOverdueDays(string currentDate) const`（计算超期天数）
        3.  `renew(int days)`（续借操作，更新应还日期）
        4.  `display(std::ostream& os) const`（具体实现：打印此条借阅记录）
        5.  `operator<`（运算符重载：按借阅日期排序，用于统计）
   
### 3. 控制类与异常机制

*   **`Library`（系统控制大脑）**
    *   **属性**：`books`（图书容器）、`users`（读者容器）、`librarians`（管理员容器）、`records`（记录容器）。
    *   **方法**（业务核心，异常抛出源头）：
        1.  `login(id, password)`（验证登录）
        2.  `borrowBook(userId, bookId)`（**可抛出异常**：未找到用户、未找到图书、图书已被借出、用户借阅超限）; `returnBook(userId, bookId)`（**可抛出异常**：未找到记录）
        3.  CRUD 方法，管理图书和用户：`addBook`, `removeBook`, `addUser`, `removeUser`,`updateBook`
        4.  查询方法：`searchBooks`, `getReaderRecords`, `getBookRecords`

*   **自定义异常体系**（继承自 `std::runtime_error`）
    *   `UserNotFoundException`（用户不存在异常）
    *   `BookNotFoundException`（图书不存在异常）
    *   `BorrowDeniedException`（借阅被拒绝异常）
    *   `DuplicateIdException`（ID 冲突异常）
    *   `RecordNotFoundException`
    *   `InvalidInputException`

## 运算符重载
为 `Displayable` 定义 `operator<<` 一次：
```cpp
std::ostream& operator<<(std::ostream& os, const Displayable& d) {
    d.display(os);   // 虚分发——多态与运算符重载一次完成
    return os;
}
```
每个 `Person`、`Book`、`BorrowRecord` 可直接 `std::cout << someObject`；配合 `Book::operator==`（按 ISBN）、`BorrowRecord::operator<`（按日期）。

## 目录结构

```
library_system/
├── include/
│   ├── interfaces/     Displayable.h, Searchable.h
│   ├── models/         Person.h, User.h, Librarian.h, Book.h, BorrowRecord.h
│   ├── core/           Library.h            (CRUD、借阅/归还、协调)
│   ├── stats/          Statistics.h         (热门书籍、借阅最多的用户、趋势)
│   ├── storage/        FileStorage.h        (加载/保存到磁盘)
│   ├── exceptions/     LibraryExceptions.h
│   └── ui/             ConsoleUI.h          (菜单循环)
├── src/                (与 include/ 镜像，每个 .h 对应一个 .cpp)
├── data/               (运行时持久化文件)
└── main.cpp            薄入口点，将 ConsoleUI 和 Library 连接在一起
```