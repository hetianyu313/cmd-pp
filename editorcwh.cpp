#include <windows.h>
#include <bits/stdc++.h>
#include <string>
#include <conio.h>
#include <algorithm>
using namespace std;
namespace cwh{
	HANDLE hConsole;
	void init() {
	    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	
	// 对话框类型枚举
	enum class DialogType { OK, YESNO, SURCAN, INPUT, CUSTOM };
	
	// 颜色定义
	enum ConsoleColor {
	    BLACK = 0,
	    BLUE = 1,
	    GREEN = 2,
	    CYAN = 3,
	    RED = 4,
	    MAGENTA = 5,
	    YELLOW = 6,
	    WHITE = 7,
	    GRAY = 8,
	    BRIGHT_BLUE = 9,
	    BRIGHT_GREEN = 10,
	    BRIGHT_CYAN = 11,
	    BRIGHT_RED = 12,
	    BRIGHT_MAGENTA = 13,
	    BRIGHT_YELLOW = 14,
	    BRIGHT_WHITE = 15 
	};
	
	// 保存原始控制台属性
	WORD originalAttributes;
	bool attributesSaved = false;
	
	void SaveOriginalAttributes() {
	    CONSOLE_SCREEN_BUFFER_INFO csbi;
	    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
	        originalAttributes = csbi.wAttributes;
	        attributesSaved = true;
	    }
	}
	
	void RestoreOriginalAttributes() {
	    if (attributesSaved) {
	        SetConsoleTextAttribute(hConsole, originalAttributes);
	    }
	}
	
	// 设置控制台颜色
	void SetColor(ConsoleColor text, ConsoleColor background = BLACK) {
	    SetConsoleTextAttribute(hConsole, (WORD)((background << 4) | text));
	}
	
	// 获取控制台窗口大小
	bool GetConsoleSize(int& width, int& height) {
	    CONSOLE_SCREEN_BUFFER_INFO csbi;
	    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return false;
	    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	    return true;
	}
	
	// 保存控制台区域内容
	struct ConsoleRegion {
	    vector<CHAR_INFO> buffer;
	    SMALL_RECT rect;
	};
	
	ConsoleRegion SaveConsoleRegion(int left, int top, int width, int height) {
	    ConsoleRegion region;
	    region.rect = { (SHORT)left, (SHORT)top, (SHORT)(left + width - 1), (SHORT)(top + height - 1) };
	    region.buffer.resize(width * height);
	    
	    COORD bufSize = { (SHORT)width, (SHORT)height };
	    COORD bufCoord = { 0, 0 };
	    ReadConsoleOutput(hConsole, region.buffer.data(), bufSize, bufCoord, &region.rect);
	    
	    return region;
	}
	
	void RestoreConsoleRegion(const ConsoleRegion& region) {
	    COORD bufSize = { 
	        (SHORT)(region.rect.Right - region.rect.Left + 1), 
	        (SHORT)(region.rect.Bottom - region.rect.Top + 1) 
	    };
	    COORD bufCoord = { 0, 0 };
	    SMALL_RECT writeRect = region.rect;
	    WriteConsoleOutput(hConsole, region.buffer.data(), bufSize, bufCoord, &writeRect);
	}
	
	void DrawText(int x, int y, const string& text, 
	             ConsoleColor textColor = BRIGHT_WHITE, 
	             ConsoleColor bgColor = BLACK,
	             bool ensureContrast = false) {
	    COORD pos = { (SHORT)x, (SHORT)y };
	    SetConsoleCursorPosition(hConsole, pos);
	    
	    // 确保足够的对比度
	    if (ensureContrast) {
	        if ((textColor <= WHITE && bgColor <= WHITE) || 
	            (textColor == bgColor)) {
	            textColor = (bgColor <= WHITE) ? BRIGHT_WHITE : BLACK;
	        }
	    }
	    
	    SetColor(textColor, bgColor);
	    DWORD written;
	    WriteConsoleA(hConsole, text.c_str(), static_cast<DWORD>(text.length()), &written, NULL);
	}
	
	void DrawBox(int left, int top, int width, int height, ConsoleColor color = BRIGHT_WHITE) {
	    // 绘制上边框
	    string horizLine(width - 2, '-');
	    DrawText(left + 1, top, horizLine, color);
	    DrawText(left, top, "+", color);
	    DrawText(left + width - 1, top, "+", color);
	    
	    // 绘制左右边框
	    for (int y = 1; y < height - 1; y++) {
	        DrawText(left, top + y, "|", color);
	        DrawText(left + width - 1, top + y, "|", color);
	    }
	    
	    // 绘制下边框
	    DrawText(left + 1, top + height - 1, horizLine, color);
	    DrawText(left, top + height - 1, "+", color);
	    DrawText(left + width - 1, top + height - 1, "+", color);
	}
	
	// 自动换行函数
	vector<string> WordWrap(const string& text, int maxWidth) {
	    vector<string> lines;
	    string currentLine;
	    string currentWord;
	    
	    for (char ch : text) {
	        if (ch == ' ' || ch == '\n') {
	            // 检查添加这个词是否会超出长度
	            if (!currentWord.empty()) {
	                if (currentLine.empty()) {
	                    currentLine = currentWord;
	                } else if (currentLine.length() + 1 + currentWord.length() <= maxWidth) {
	                    currentLine += " " + currentWord;
	                } else {
	                    lines.push_back(currentLine);
	                    currentLine = currentWord;
	                }
	                currentWord.clear();
	            }
	            
	            if (ch == '\n') {
	                lines.push_back(currentLine);
	                currentLine.clear();
	            }
	        } else {
	            currentWord += ch;
	        }
	    }
	    
	    // 处理最后一个词
	    if (!currentWord.empty()) {
	        if (currentLine.empty()) {
	            currentLine = currentWord;
	        } else if (currentLine.length() + 1 + currentWord.length() <= maxWidth) {
	            currentLine += " " + currentWord;
	        } else {
	            lines.push_back(currentLine);
	            currentLine = currentWord;
	        }
	    }
	    
	    if (!currentLine.empty()) {
	        lines.push_back(currentLine);
	    }
	    
	    return lines;
	}
	
	// 单行文本输入函数（支持多行编辑但合并为一行）
	string GetSingleLineInput(int x, int y, int width, 
	                             ConsoleColor textColor = BRIGHT_WHITE,
	                             ConsoleColor bgColor = BLACK) {
	    // 使用3行编辑区域
	    vector<string> lines(4, "");
	    int currentLine = 0;
	    int cursorPos = 0;
	    
	    // 保存原始光标位置
	    CONSOLE_SCREEN_BUFFER_INFO csbi;
	    GetConsoleScreenBufferInfo(hConsole, &csbi);
	    COORD originalCursorPos = csbi.dwCursorPosition;
	    
	    // 绘制输入区域（3行）
	    SetColor(textColor, bgColor);
	    for (int i = 0; i < 3; i++) {
	        COORD pos = { (SHORT)x, (SHORT)(y + i) };
	        SetConsoleCursorPosition(hConsole, pos);
	        string clearStr(width, '_');
	        cout << clearStr;
	    }
	    
	    // 设置初始光标位置
	    COORD cursor = { (SHORT)(x + cursorPos), (SHORT)(y + currentLine) };
	    SetConsoleCursorPosition(hConsole, cursor);
	    
	    bool inputComplete = false;
	    bool cancelled = false;
	    
	    while (!inputComplete) {
	        int ch = _getch();
	        
	        // 处理方向键
	        if (ch == 0xE0 || ch == 0) {
	            int dir = _getch();
	            switch (dir) {
	                case 72: // 上箭头
	                    if (currentLine > 0) {
	                        currentLine--;
	                        cursorPos = min(cursorPos, static_cast<int>(lines[currentLine].length()));
	                    }
	                    break;
	                case 80: // 下箭头
	                    if (currentLine < 2) {
	                        currentLine++;
	                        cursorPos = min(cursorPos, static_cast<int>(lines[currentLine].length()));
	                    }
	                    break;
	                case 75: // 左箭头
	                    if (cursorPos > 0) {
	                        cursorPos--;
	                    }
	                    break;
	                case 77: // 右箭头
	                    if (cursorPos < static_cast<int>(lines[currentLine].length())) {
	                        cursorPos++;
	                    }
	                    break;
	            }
	        }
	        // 回车键完成输入
	        else if (ch == 13) {
	            inputComplete = true;
	        }
	        // ESC键取消输入
	        else if (ch == 27) {
	            inputComplete = true;
	            cancelled = true;
	        }
	        // 退格键
	        else if (ch == 8) {
	            if (cursorPos > 0) {
	                // 在当前行删除字符
	                lines[currentLine].erase(cursorPos - 1, 1);
	                cursorPos--;
	            }
	            else if (currentLine > 0) {
	                // 合并到上一行
	                cursorPos = static_cast<int>(lines[currentLine - 1].length());
	                lines[currentLine - 1] += lines[currentLine];
	                lines[currentLine].clear();
	                currentLine--;
	            }
	        }
	        // 可打印字符
	        else if (ch >= 32 && ch <= 126) {
	            if (static_cast<int>(lines[currentLine].length()) < width) {
	                lines[currentLine].insert(cursorPos, 1, static_cast<char>(ch));
	                cursorPos++;
	            }
	        }
	        
	        // 重绘所有行
	        for (int i = 0; i < 3; i++) {
	            COORD pos = { (SHORT)x, (SHORT)(y + i) };
	            SetConsoleCursorPosition(hConsole, pos);
	            string displayText = lines[i];
	            string padding(width - displayText.length(), '_');
	            cout << displayText << padding;
	        }
	        
	        // 更新光标位置
	        cursor.X = static_cast<SHORT>(x + cursorPos);
	        cursor.Y = static_cast<SHORT>(y + currentLine);
	        SetConsoleCursorPosition(hConsole, cursor);
	    }
	    
	    // 恢复原始光标位置
	    SetConsoleCursorPosition(hConsole, originalCursorPos);
	    
	    if (cancelled) {
	        return "\x1B";
	    }
	    
	    // 合并所有行为单行
	    string result;
	    for (const auto& line : lines) {
	        result += line;
	    }
	    return result;
	}
	
	// 修改后的函数，支持自动换行和自定义按钮
	int createConsoleInsideWindow(const string& message, 
	                            DialogType type = DialogType::OK, 
	                            string* inputResult = nullptr,
	                            const vector<pair<string, int>>* customButtons = nullptr) {
	    SaveOriginalAttributes();
	    
	    // 获取控制台大小
	    int consoleWidth, consoleHeight;
	    if (!GetConsoleSize(consoleWidth, consoleHeight)) {
	        return -1;
	    }
	    
	    // 计算对话框宽度 (限制在控制台宽度范围内)
	    int maxBoxWidth = min(consoleWidth - 4, 80); // 最大80字符宽
	    int minBoxWidth = 30; // 最小30字符宽
	    
	    // 自动换行处理消息
	    int textWidth = maxBoxWidth - 8; // 减去边框和边距
	    vector<string> wrappedLines = WordWrap(message, textWidth);
	    
	    // 计算对话框大小
	    int boxWidth = max(minBoxWidth, min(maxBoxWidth, 
	        static_cast<int>(wrappedLines.empty() ? 0 : wrappedLines[0].length()) + 8));
	    int boxHeight = (type == DialogType::INPUT) ? 8 : (6 + static_cast<int>(wrappedLines.size()) - 1);
	    
	    int left = (consoleWidth - boxWidth) / 2;
	    int top = (consoleHeight - boxHeight) / 2;
	    
	    // 确保对话框不会超出控制台高度
	    if (top + boxHeight >= consoleHeight) {
	        boxHeight = consoleHeight - top - 2;
	    }
	    
	    // 保存原始控制台内容
	    ConsoleRegion savedRegion = SaveConsoleRegion(left, top, boxWidth, boxHeight);
	    
	    // 绘制对话框背景
	    SetColor(BRIGHT_WHITE, BLUE);
	    for (int y = 0; y < boxHeight; y++) {
	        for (int x = 0; x < boxWidth; x++) {
	            COORD pos = { static_cast<SHORT>(left + x), static_cast<SHORT>(top + y) };
	            SetConsoleCursorPosition(hConsole, pos);
	            cout << ' ';
	        }
	    }
	    
	    // 绘制边框
	    DrawBox(left, top, boxWidth, boxHeight, BRIGHT_YELLOW);
	    
	    // 显示消息 (多行)
	    for (size_t i = 0; i < wrappedLines.size(); i++) {
	        int msgX = left + (boxWidth - static_cast<int>(wrappedLines[i].length())) / 2;
	        int msgY = top + 1 + static_cast<int>(i);
	        DrawText(msgX, msgY, wrappedLines[i], BRIGHT_WHITE, BLUE, true);
	    }
	    
	    // 处理输入对话框
	    if (type == DialogType::INPUT) {
	        int inputWidth = boxWidth - 6;
	        int inputX = left + 3;
	        int inputY = top + static_cast<int>(wrappedLines.size()) + 1;
	        
	        string input = GetSingleLineInput(inputX, inputY, inputWidth, BRIGHT_WHITE, BLUE);
	        
	        if (inputResult != nullptr) {
	            *inputResult = input;
	        }
	        
	        RestoreConsoleRegion(savedRegion);
	        RestoreOriginalAttributes();
	        
	        // 返回1表示成功输入（包括空输入），0表示取消
	        return input == "\x1B" ? -1 : 0;
	    }
	    
	    // 绘制按钮
	    int buttonY = top + boxHeight - 3;
	    int result = 0;
	    
	    // 按钮绘制函数
	    auto drawButtons = [&](const vector<pair<string, int>>& buttons, int selected) {
	        int totalWidth = 0;
	        for (const auto& btn : buttons) {
	            totalWidth += static_cast<int>(btn.first.length()) + 4;
	        }
	        
	        int startX = left + (boxWidth - totalWidth) / 2;
	        for (size_t i = 0; i < buttons.size(); i++) {
	            // 绘制按钮文本
	            string btnText = " " + buttons[i].first + " ";
	            
	            // 设置按钮颜色
	            ConsoleColor textColor = BRIGHT_WHITE;
	            ConsoleColor bgColor = GRAY;
	            
	            if (i == selected) {
	                textColor = BLACK;
	                bgColor = BRIGHT_YELLOW;
	            }
	            
	            DrawText(startX, buttonY, btnText, textColor, bgColor, true);
	            startX += static_cast<int>(btnText.length()) + 2;
	        }
	    };
	    
	    // 按钮输入处理函数
	    auto handleInput = [&](const vector<pair<string, int>>& buttons) {
	        int selected = 0;
	        size_t buttonCount = buttons.size();
	        
	        while (true) {
	            drawButtons(buttons, selected);
	            
	            int ch = _getch();
	            if (ch == 0 || ch == 224) {
	                ch = _getch(); // 处理方向键
	            }
	            
	            switch (ch) {
	                case 75: // 左箭头
	                    selected = static_cast<int>((selected - 1 + buttonCount) % buttonCount);
	                    break;
	                case 77: // 右箭头
	                    selected = static_cast<int>((selected + 1) % buttonCount);
	                    break;
	                case 13: // 回车
	                    return buttons[selected].second;
	                case 27: // ESC
	                    return -1;
	                default:
	                    break;
	            }
	        }
	    };
	    
	    // 根据类型确定按钮列表
	    vector<pair<string, int>> buttonList;
	    
	    // 如果有自定义按钮，使用自定义按钮
	    if (customButtons != nullptr) {
	        buttonList = *customButtons;
	    }
	    // 否则使用预定义按钮
	    else {
	        switch (type) {
	            case DialogType::OK:
	                buttonList = { {"确定", 1} };
	                break;
	            case DialogType::YESNO:
	                buttonList = { {"是", 1}, {"否", 0} };
	                break;
	            case DialogType::SURCAN:
	                buttonList = { {"确定", 1}, {"消除", 0} };
	                break;
	            case DialogType::CUSTOM:
	                // 如果没有提供自定义按钮，使用默认确定按钮
	                buttonList = { {"确定", 1} };
	                break;
	            default:
	                buttonList = { {"确定", 1} };
	        }
	    }
	    
	    // 处理按钮输入
	    result = handleInput(buttonList);
	    
	    // 恢复原始控制台内容
	    RestoreConsoleRegion(savedRegion);
	    RestoreOriginalAttributes();
	    
	    return result;
	}
}

