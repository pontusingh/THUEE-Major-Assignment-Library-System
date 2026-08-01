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
抽象类 `Person` 继承 `Displayable`；`User` 继承 `Person` 和 `Searchable`；`Librarian` 继承 `Person`；`Book` 继承 `Displayable` 和 `Searchable`。

## 其余成员
- `BorrowRecord`：普通类，持有 `userId`、`bookId`、`borrowDate`、`returnDate`；实现 `Displayable`，重载 `operator<`（按日期排序）
- `Library`：协调者，持有集合，暴露 CRUD/借阅/搜索/统计方法，供 `main.cpp`/CLI 交互
- 自定义异常层次结构（派生自 `std::runtime_error`）：`DuplicateIdException`、`RecordNotFoundException`、`InvalidInputException`、`BookUnavailableException`

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

## 开发阶段
1. 锁作用域：持久化用文本/CSV（`users.txt`、`books.txt`、`records.txt`）；模糊搜索用不区分大小写的子串匹配
2. 领域模型：草绘类图
3. 模块架构：按目录结构 + `Makefile`/`CMakeLists.txt`
4. 接口定义：先写各 `.h`（完整签名、无函数体）
5. 异常与验证：CLI 捕获格式错误，`Library` 抛领域错误
6. 持久化：每文件一对 `load()`/`save()`，启动及每次变更后调用
7. 统计：最热门=按 `bookId` 计数降序；借阅最多用户=按 `userId` 计数；趋势=按月分组到 `std::map<std::string,int>`
8. 模块实现：接口 → 模型 → 核心 → 存储 → UI，逐个测试
9. 边界测试：重复 ID、借不存在书、负数副本数、空搜索串、错误日期
10. 报告：架构 + 错误日志
