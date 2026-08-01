**项目**：图书管理系统

**基本要求**：
系统中设计的类的数目不少于4个，每个类中要有各自的属性（多于3个）和方法（多于3个）；需要定义一个抽象类，采用继承方式派生这些类。并设计一个多重继承的派生类。在程序设计中，引入虚函数的多态性、运算符重载等机制。

**功能要求**：
①　用户信息管理：存储用户信息，如用户姓名、借阅历史等。支持对用户的信息进行查询、增加、删除、修改等操作；
②　图书信息管理：存储图书信息，如书名、作者、类别、关键字、简介、借阅状态等。支持对图书的信息进行查询、增加、删除、修改等操作，且要求图书查询功能具有一定模糊查询的能力，例如用户输入一个字，可查询到包含该字的图书；
③　借还书记录管理：记录用户借还书的时间等信息、查看用户或图书的借阅历史等；
④　统计分析功能：根据借还书记录进行简单的统计分析，比如最受欢迎的书籍、借阅最多的用户、用户借阅量随时间的变化趋势等。可参考某些app的年度报告；
⑤　特殊情况处理：对非法输入等特殊情况进行合适的处理；

**类层次结构**：
抽象类`Person`继承`Displayable`,`User`继承`Person`和`Searchable`，`Librarian`继承`Person`,`Book`继承`Displayable`和`Searchable`

**其余成员**：
- `BorrowRecord`——普通类，持有`userId`、`bookId`、`borrowDate`、`returnDate`。同时也实现了`Displayable`（因此`operator<<`可以统一作用于它，见下文），并重载了`operator<`，以便记录按日期排序。
- `Library`——协调者。拥有集合，暴露CRUD/借阅/搜索/统计方法。这是`main.cpp`（或你的CLI层）与之交互的地方。
- 一个派生自`std::runtime_error`的小型自定义异常层次结构：`DuplicateIdException`、`RecordNotFoundException`、`InvalidInputException`、`BookUnavailableException`——这能正确地处理要求5（非法输入），而不是到处用`if`检查。

**运算符重载**：为`Displayable`定义`operator<<` *一次*，而不是为每个类各定义一次：
```cpp
std::ostream& operator<<(std::ostream& os, const Displayable& d) {
    d.display(os);   // 虚分发——多态和运算符重载在一行中完成
    return os;
}
```
每个`Person`、`Book`和`BorrowRecord`都免费获得了可用的`std::cout << someObject`。再加上`Book::operator==`（按ISBN比较）和`BorrowRecord::operator<`（按日期比较），满足了运算符重载要求。

## 目录结构

```
library_system/
├── include/
│   ├── interfaces/     Displayable.h, Searchable.h
│   ├── models/          Person.h, User.h, Librarian.h, Book.h, BorrowRecord.h
│   ├── core/            Library.h              (CRUD、借阅/归还、协调)
│   ├── stats/           Statistics.h            (热门书籍、借阅最多的用户、趋势)
│   ├── storage/         FileStorage.h           (加载/保存到磁盘)
│   ├── exceptions/      LibraryExceptions.h
│   └── ui/              ConsoleUI.h             (菜单循环)
├── src/                 (与include/镜像，每个.h对应一个.cpp)
├── data/                (运行时持久化文件)
└── main.cpp             薄入口点，将ConsoleUI和Library连接在一起
```

映射到你的MCU项目：`interfaces/` + `models/` 扮演了你的`drivers/`所扮演的角色（低层、哑巴的构建块）；`core/` 是你的`behavior/`；`storage/` 名称相同，做同样的工作。这里没有与`power/`或`network/`真正对应的部分——这没关系，并非每个层都需要一个孪生，强行对应就相当于在软件中添加一个你不需要的舵机。

## 阶段计划

**阶段1——锁定作用域决策。** 有两件事会在静默中决定你的整体架构，所以在画任何东西之前先决定它们：(a) 持久化格式——我会默认每个实体一个纯文本/CSV文件（`users.txt`、`books.txt`、`records.txt`），简单且易于评分/调试；(b) 对于评分者来说，“模糊搜索”意味着什么——对名称/标题/作者进行不区分大小写的子串匹配是一个安全的基线，能明确满足要求；Levenshtein距离的拼写容错是一个锦上添花的功能，你可以在后续添加而不需要更改`Searchable`接口，因为它被隐藏在`matches()`后面。

**阶段2——领域模型**（就是你刚刚在图中所看到的）。先在纸上草绘，就像你夏天准备C++库时那样——先不要打开编辑器。

**阶段3——模块架构。** 上述目录树，加上决定你的构建设置（一个简单的`Makefile`或`CMakeLists.txt`对于一个课程项目来说就足够了——不需要更花哨的东西）。

**阶段4——接口定义。** 编写每个`.h`文件，包含完整的方法签名和成员列表，不包含任何函数体。这是实际交付给Claude Code的工件——与MCU项目中“仅框架级别”的规则相同。

**阶段5——异常和验证策略。** 明确界定边界：CLI输入解析捕获*格式*错误（非数字输入、空字符串），`Library`方法抛出*领域*错误（重复ID、书籍已被借出、归还从未借出的书）。

**阶段6——持久化设计。** 每个存储文件一对`load()`/`save()`，在启动时和每次变更后调用（最简单正确的行为——在此规模下不需要脏标记系统）。

**阶段7——统计模块。** 在编码前确定每个统计指标的实际算法：最热门 = 对`BorrowRecord.bookId`进行频率计数，降序排列；借阅最多的用户 = 对`userId`做同样处理；随时间趋势 = 按月将`borrowDate`分组到`std::map<std::string,int>`中。

**阶段8——逐个模块交付给Claude Code。** 接口 → 模型 → 核心 → 存储 → UI，在继续之前测试每一个。保持那个帮你完成MCU报告的规则：不接受任何你不能逐行解释的东西。

**阶段9——边界情况测试。** 特别针对要求5：重复ID、借阅不存在的书籍、负数副本数、空搜索字符串、格式错误的日期。

**阶段10——报告。** 架构部分 + 错误日志，与你的MCU报告格式相同。

**现在**，实际下一步是阶段1——在任何文件布局有意义之前，先决定持久化格式和模糊搜索的标准。你想直接采用上述默认设置并直接进入阶段4的完整方法签名草绘，还是想先重新考虑其中任何一个？