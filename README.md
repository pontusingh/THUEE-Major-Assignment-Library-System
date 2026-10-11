# 图书管理系统

> **thu「计算机程序设计基础」夏季小学期大作业**
>
> 本项目是一个基于 C++17 的控制台图书管理系统，覆盖用户/图书信息管理、借还书记录管理、统计分析、持久化存储等功能，综合运用面向对象的封装、继承、多态、运算符重载及异常处理等机制。

---

## AI 使用声明

本项目在开发过程中使用了 AI 编程助手辅助完成部分代码编写、调试、重构与文档撰写。

---

## 项目简介

本系统是一个纯命令行（Console）交互的图书管理程序，实现了课程大作业要求中的全部核心功能：

1. **用户信息管理**：存储读者姓名、邮箱、登录密码、借阅额度与已借图书，支持查询、增加、删除、修改。
2. **图书信息管理**：存储书名、作者、关键字、类别、出版信息、馆藏地与借阅状态，支持增删改查；查询支持**模糊匹配**（输入单个字即可检索到包含该字的图书）。
3. **借还书记录管理**：记录借书/应还/归还时间与续借状态，支持查看用户或图书的借阅历史、续借与超期计算。
4. **统计分析**：最受欢迎图书、借阅最多用户、系统平均借阅时长、个人读书报告等。
5. **特殊情况处理**：对非法输入、ID 冲突、未找到对象、借阅受限等场景通过自定义异常体系做合适处理。

---

## 面向对象设计

### 接口（纯虚基类）

| 接口 | 纯虚函数 | 说明 |
| --- | --- | --- |
| `Displayable` | `virtual void display(std::ostream&) const = 0` | 多态打印 |
| `Searchable` | `virtual bool matches(const std::string&) const = 0` | 模糊匹配 |

### 类层次结构

```
Displayable            Searchable
    │                        │
    ├── Person（抽象基类）────┼────→ User（多重继承 Person + Searchable）
    │        │               │
    │        └──── Librarian（继承 Person）      Book（多重继承 Displayable + Searchable）
    │
    ├── Book
    └── BorrowRecord
```

| 类 | 继承关系 | 说明 |
| --- | --- | --- |
| `Person` | 继承 `Displayable` | 抽象基类：`id`、`name`、`password`、`email`，`display` 仍为纯虚 |
| `User` | **多重继承** `Person`、`Searchable` | 普通读者：借阅额度、已借列表、借还操作 |
| `Librarian` | 继承 `Person` | 管理员：权限等级、可否管理用户 |
| `Book` | **多重继承** `Displayable`、`Searchable` | 图书实体：借阅状态、模糊匹配、`operator==` |
| `BorrowRecord` | 继承 `Displayable` | 借阅记录：借/还/应还日期、续借、超期计算 |
| `Library` | — | 控制大脑：登录、借还、CRUD、查询，业务异常抛出源头 |
| `Statistics` | — | 纯静态工具类：热门图书、活跃用户、个人报告 |
| `FileStorage` | — | 纯静态工具类：数据加载/保存到磁盘 |
| `ConsoleUI` | — | 菜单循环，连接 `Library` 与用户 |

### 面向对象机制体现

- **虚函数多态**：`Displayable::display`、`Searchable::matches` 为纯虚函数，通过基类指针/引用实现运行时多态；配合全局重载 `operator<<`（虚分发），使 `std::cout << obj` 一次完成多态打印与运算符重载。
- **运算符重载**：
  - `std::ostream& operator<<(std::ostream&, const Displayable&)`：统一打印入口；
  - `Book::operator==`：按图书 ID 判断是否为同一本书。
- **异常机制**：自定义异常体系，均继承自 `std::runtime_error`。

### 自定义异常体系

```
LibraryException（继承 std::runtime_error）
 ├── UserNotFoundException    用户不存在
 ├── BookNotFoundException    图书不存在
 ├── BorrowDeniedException    借阅被拒绝
 ├── DuplicateIdException     ID 冲突
 ├── RecordNotFoundException  记录不存在
 ├── InvalidInputException    输入非法
 └── FileStorageException     文件读写失败
```

---

## 目录结构

```
.
├── include/
│   ├── interfaces/     Displayable.h, Searchable.h
│   ├── models/         Person.h, User.h, Librarian.h, Book.h, BorrowRecord.h
│   ├── core/           Library.h
│   ├── stats/          Statistics.h
│   ├── storage/        FileStorage.h
│   ├── exceptions/     LibraryExceptions.h
│   ├── ui/             ConsoleUI.h
│   └── utils/          DateUtils.h
├── src/                与 include/ 镜像，每个 .h 对应一个 .cpp
├── build/data/         运行时持久化数据（books.txt / users.txt / records.txt / librarians.txt）
├── CMakeLists.txt
└── main.cpp            薄入口点：加载数据 → 启动控制台
```

---

## 构建与运行

### 环境要求

- CMake ≥ 3.15
- 支持 C++17 的编译器（MSVC / GCC / Clang）

### 构建

```bash
cmake -S . -B build
cmake --build build
```

### 运行

构建产物为 `build/LibrarySystem.exe`（Windows，由 `CMakeLists.txt` 的 `POST_BUILD` 自动在可执行文件旁创建 `data/` 目录）：

```bash
build\LibrarySystem.exe
```

> 说明：Windows 下程序启动时会将控制台代码页切换为 UTF-8（65001），以保证中文正常显示。

---

## 使用说明

### 内置管理员账号

系统在 `Library` 构造时注入固定管理员账号（**不落盘、不可增删**）：

| 账号 | 密码 |
| --- | --- |
| `admin` | `admin123` |

### 主要操作

- **登录**：输入读者 / 管理员 ID 与密码，系统区分角色进入对应菜单。
- **读者**：图书模糊查询、借书、还书、续借、查看个人借阅记录、生成个人读书报告、修改个人信息。
- **管理员**：图书管理（增删改查）、用户管理、馆员管理、查看全部借阅记录、统计分析（热门图书、活跃用户、平均借阅时长等）。

---

## 数据持久化

- 数据文件位于可执行文件旁的 `data/` 目录，每类数据一个文件、每行一条记录，字段以 `|` 分隔，文本统一 UTF-8。
- 文件列表：`books.txt`、`users.txt`、`records.txt`、`librarians.txt`。
- 加载时文件不存在返回空集合；解析出错抛 `FileStorageException`。
- UI 录入字段若包含 `|` 或换行符，会抛出 `InvalidInputException`，从而保证文件无需转义。

---

## 日期与统计规则

- 日期格式统一为 `YYYY-MM-DD`，由 `utils/DateUtils.h` 中的 `Date` 命名空间提供唯一实现（今日日期、加减天数、天数差）。
- `Statistics::generateUserReport` 仅统计**已归还**记录（`returnDate` 为空则跳过），借阅时长一律取 `returnDate - borrowDate`。
