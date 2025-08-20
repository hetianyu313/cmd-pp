# CMD++ —— 控制台 C++ 轻量级 IDE

> 一个以 **Windows 控制台** 为界面的简易 C++ 编辑器/IDE，支持语法高亮、代码编辑、自动补全、中文输入和文件操作等功能。  
> “边敲边跑的轻量化玩具 IDE，写 C++ 像打游戏”。

---

## ✨ 功能特性

- **编辑器核心**
  - 光标移动（上下左右、Home、End、PgUp、PgDn）
  - 自动缩进、回车后智能补空格
  - 自动补全符号（`()`、`[]`、`""` 等成对符号）
  - 中文输入法支持（拼音 → 汉字）
  - 退格键支持中英文、跨行合并
  - 快捷菜单（ESC 呼出）

- **代码显示**
  - **语法高亮**
    - 类型关键字（int, char, string, …）
    - 常见函数和控制语句（for, while, if, return, …）
    - 符号/运算符
    - 宏定义、字符串、注释、数字
  - 行号显示
  - 控制台颜色渲染

- **文件操作**
  - 文件新建/插入/保存/加载
  - 代码复制到剪切板（支持 ANSI/UTF-8/UTF-16）
  - 文件路径输入与自动生成 `.cpp` 文件
  - 查找/替换（单次/全局）

- **扩展工具**
  - 代码混淆（调用外部工具 `CodeObfuscator.exe`）
  - 代码风格化（调用外部工具 `cpp_code_nai.exe`）
  - 简单“项目管理”菜单（保存、运行、视图设置）
  - STL容器帮助

- **防误操作**
  - 禁止控制台右键菜单、禁止快速编辑模式  
    → 防止代码区被误粘贴！
  - 禁止关闭键

---

## 📦 工程结构

主程序：  
```
editorcode.cpp   // 控制台编辑器入口
```

依赖的其它模块：  
```
editorapi.cpp
editorcpp.cpp
editorrgb.cpp
editorjqb.cpp
editorcwh.cpp
editorchar.cpp
editorstr.cpp
editorview.cpp
editorsrf.cpp
```

这些模块负责界面输出、语法高亮颜色方案（RGB 管理）、文件管理、中文拼音输入法数据等。

---

## 🎮 快捷键操作

- **输入模式**
  - 普通输入：键盘输入即可
  - Tab → 插入 4 个空格
  - Enter → 换行 + 自动缩进
  - Backspace → 向前删除字符/跨行合并

- **移动**
  - 上下左右 → 光标移动
  - Home / End → 行首/行尾
  - PgUp / PgDn → 翻页

- **特殊功能**
  - `Esc` → 菜单（保存、运行、关于、查找/替换、复制到剪切板）
  - `F1` → 插入代码模式（批量输入）
  - `F2` → 切换中文输入
  - `F3` → 刷新显示

---

## 🖥️ 运行说明

编译建议参数（g++ MinGW 环境）：  
```bash
g++ editorcode.cpp -std=c++14 -O2 -s -m32 -o CMDpp.exe
```

运行：  
```bash
CMDpp.exe
```

首次启动会清屏并显示编辑器界面。菜单与提示均在程序内部完成。  

---

## 📑 功能模块简述

- `_ed_flash` → 重绘屏幕、显示行号和代码高亮
- `_edf_addch` → 插入字符/自动补全括号
- `_edf_enter` → 回车并缩进
- `_edf_back` → 退格删除（含中文编码处理）
- `_edf_insert` → 插入输入模式
- `_edf_save_file` → 保存文件
- `_edf_load_file` → 打开文件
- `_edf_find / _edf_find_next / _edf_replace` → 查找与替换
- `_edf_jqb` → 拷贝内容到剪贴板
- `_edf_zhcn` → 中文输入模式
- `_edf_obfus / _edf_cstyle` → 调用外部工具进行代码混淆/格式化
- `_edf_escape` → ESC 菜单总控
- `main()` → 初始化所有模块，循环运行 `_ed_run` 捕获键盘事件

---

## 🙋 作者信息

- 作者：HeTianYu313  
- 语言：C++ (MinGW64, Windows 专用)  
- 主页：[GitHub.com/HeTianYu313](https://github.com/HeTianYu313)

---

## ⚠️ 注意事项

- 本编辑器仅在 **Windows 控制台** 下工作  
- 混淆/风格化功能依赖外部工具执行  
- 中文输入功能依赖自带拼音库  
- 不适合大型工程，仅适合学习/玩耍/炫技  
