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
	
	// �Ի�������ö��
	enum class DialogType { OK, YESNO, SURCAN, INPUT, CUSTOM };
	
	// ��ɫ����
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
	
	// ����ԭʼ����̨����
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
	
	// ���ÿ���̨��ɫ
	void SetColor(ConsoleColor text, ConsoleColor background = BLACK) {
	    SetConsoleTextAttribute(hConsole, (WORD)((background << 4) | text));
	}
	
	// ��ȡ����̨���ڴ�С
	bool GetConsoleSize(int& width, int& height) {
	    CONSOLE_SCREEN_BUFFER_INFO csbi;
	    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return false;
	    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	    return true;
	}
	
	// �������̨��������
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
	    
	    // ȷ���㹻�ĶԱȶ�
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
	    // �����ϱ߿�
	    string horizLine(width - 2, '-');
	    DrawText(left + 1, top, horizLine, color);
	    DrawText(left, top, "+", color);
	    DrawText(left + width - 1, top, "+", color);
	    
	    // �������ұ߿�
	    for (int y = 1; y < height - 1; y++) {
	        DrawText(left, top + y, "|", color);
	        DrawText(left + width - 1, top + y, "|", color);
	    }
	    
	    // �����±߿�
	    DrawText(left + 1, top + height - 1, horizLine, color);
	    DrawText(left, top + height - 1, "+", color);
	    DrawText(left + width - 1, top + height - 1, "+", color);
	}
	
	// �Զ����к���
	vector<string> WordWrap(const string& text, int maxWidth) {
	    vector<string> lines;
	    string currentLine;
	    string currentWord;
	    
	    for (char ch : text) {
	        if (ch == ' ' || ch == '\n') {
	            // ������������Ƿ�ᳬ������
	            if (!currentWord.empty()) {
	                if (currentLine.empty()) {
	                    currentLine = currentWord;
	                } else if (currentLine.length() + 1 + currentWord.length() <= (unsigned int)maxWidth) {
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
	    
	    // �������һ����
	    if (!currentWord.empty()) {
	        if (currentLine.empty()) {
	            currentLine = currentWord;
	        } else if (currentLine.length() + 1 + currentWord.length() <= (unsigned int)maxWidth) {
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
	
	// �����ı����뺯����֧�ֶ��б༭���ϲ�Ϊһ�У�
	string GetSingleLineInput(int x, int y, int width, 
	                             ConsoleColor textColor = BRIGHT_WHITE,
	                             ConsoleColor bgColor = BLACK) {
	    // ʹ��3�б༭����
	    vector<string> lines(4, "");
	    int currentLine = 0;
	    int cursorPos = 0;
	    
	    // ����ԭʼ���λ��
	    CONSOLE_SCREEN_BUFFER_INFO csbi;
	    GetConsoleScreenBufferInfo(hConsole, &csbi);
	    COORD originalCursorPos = csbi.dwCursorPosition;
	    
	    // ������������3�У�
	    SetColor(textColor, bgColor);
	    for (int i = 0; i < 3; i++) {
	        COORD pos = { (SHORT)x, (SHORT)(y + i) };
	        SetConsoleCursorPosition(hConsole, pos);
	        string clearStr(width, '_');
	        cout << clearStr;
	    }
	    
	    // ���ó�ʼ���λ��
	    COORD cursor = { (SHORT)(x + cursorPos), (SHORT)(y + currentLine) };
	    SetConsoleCursorPosition(hConsole, cursor);
	    
	    bool inputComplete = false;
	    bool cancelled = false;
	    
	    while (!inputComplete) {
	        int ch = _getch();
	        
	        // �������
	        if (ch == 0xE0 || ch == 0) {
	            int dir = _getch();
	            switch (dir) {
	                case 72: // �ϼ�ͷ
	                    if (currentLine > 0) {
	                        currentLine--;
	                        cursorPos = min(cursorPos, static_cast<int>(lines[currentLine].length()));
	                    }
	                    break;
	                case 80: // �¼�ͷ
	                    if (currentLine < 2) {
	                        currentLine++;
	                        cursorPos = min(cursorPos, static_cast<int>(lines[currentLine].length()));
	                    }
	                    break;
	                case 75: // ���ͷ
	                    if (cursorPos > 0) {
	                        cursorPos--;
	                    }
	                    break;
	                case 77: // �Ҽ�ͷ
	                    if (cursorPos < static_cast<int>(lines[currentLine].length())) {
	                        cursorPos++;
	                    }
	                    break;
	            }
	        }
	        // �س����������
	        else if (ch == 13) {
	            inputComplete = true;
	        }
	        // ESC��ȡ������
	        else if (ch == 27) {
	            inputComplete = true;
	            cancelled = true;
	        }
	        // �˸��
	        else if (ch == 8) {
	            if (cursorPos > 0) {
	                // �ڵ�ǰ��ɾ���ַ�
	                lines[currentLine].erase(cursorPos - 1, 1);
	                cursorPos--;
	            }
	            else if (currentLine > 0) {
	                // �ϲ�����һ��
	                cursorPos = static_cast<int>(lines[currentLine - 1].length());
	                lines[currentLine - 1] += lines[currentLine];
	                lines[currentLine].clear();
	                currentLine--;
	            }
	        }
	        // �ɴ�ӡ�ַ�
	        else if (ch >= 32 && ch <= 126) {
	            if (static_cast<int>(lines[currentLine].length()) < width) {
	                lines[currentLine].insert(cursorPos, 1, static_cast<char>(ch));
	                cursorPos++;
	            }
	        }
	        
	        // �ػ�������
	        for (int i = 0; i < 3; i++) {
	            COORD pos = { (SHORT)x, (SHORT)(y + i) };
	            SetConsoleCursorPosition(hConsole, pos);
	            string displayText = lines[i];
	            string padding(width - displayText.length(), '_');
	            cout << displayText << padding;
	        }
	        
	        // ���¹��λ��
	        cursor.X = static_cast<SHORT>(x + cursorPos);
	        cursor.Y = static_cast<SHORT>(y + currentLine);
	        SetConsoleCursorPosition(hConsole, cursor);
	    }
	    
	    // �ָ�ԭʼ���λ��
	    SetConsoleCursorPosition(hConsole, originalCursorPos);
	    
	    if (cancelled) {
	        return "\x1B";
	    }
	    
	    // �ϲ�������Ϊ����
	    string result;
	    for (const auto& line : lines) {
	        result += line;
	    }
	    return result;
	}
	
	// �޸ĺ�ĺ�����֧���Զ����к��Զ��尴ť
	int createConsoleInsideWindow(const string& message, 
	                            DialogType type = DialogType::OK, 
	                            string* inputResult = nullptr,
	                            const vector<pair<string, int>>* customButtons = nullptr) {
	    SaveOriginalAttributes();
	    
	    // ��ȡ����̨��С
	    int consoleWidth, consoleHeight;
	    if (!GetConsoleSize(consoleWidth, consoleHeight)) {
	        return -1;
	    }
	    
	    // ����Ի����� (�����ڿ���̨��ȷ�Χ��)
	    int maxBoxWidth = min(consoleWidth - 4, 80); // ���80�ַ���
	    int minBoxWidth = 30; // ��С30�ַ���
	    
	    // �Զ����д�����Ϣ
	    int textWidth = maxBoxWidth - 8; // ��ȥ�߿�ͱ߾�
	    vector<string> wrappedLines = WordWrap(message, textWidth);
	    
	    // ����Ի����С
	    int boxWidth = max(minBoxWidth, min(maxBoxWidth, 
	        static_cast<int>(wrappedLines.empty() ? 0 : wrappedLines[0].length()) + 8));
	    int boxHeight = (type == DialogType::INPUT) ? 8 : (6 + static_cast<int>(wrappedLines.size()) - 1);
	    
	    int left = (consoleWidth - boxWidth) / 2;
	    int top = (consoleHeight - boxHeight) / 2;
	    
	    // ȷ���Ի��򲻻ᳬ������̨�߶�
	    if (top + boxHeight >= consoleHeight) {
	        boxHeight = consoleHeight - top - 2;
	    }
	    
	    // ����ԭʼ����̨����
	    ConsoleRegion savedRegion = SaveConsoleRegion(left, top, boxWidth, boxHeight);
	    
	    // ���ƶԻ��򱳾�
	    SetColor(BRIGHT_WHITE, BLUE);
	    for (int y = 0; y < boxHeight; y++) {
	        for (int x = 0; x < boxWidth; x++) {
	            COORD pos = { static_cast<SHORT>(left + x), static_cast<SHORT>(top + y) };
	            SetConsoleCursorPosition(hConsole, pos);
	            cout << ' ';
	        }
	    }
	    
	    // ���Ʊ߿�
	    DrawBox(left, top, boxWidth, boxHeight, BRIGHT_YELLOW);
	    
	    // ��ʾ��Ϣ (����)
	    for (size_t i = 0; i < wrappedLines.size(); i++) {
	        int msgX = left + (boxWidth - static_cast<int>(wrappedLines[i].length())) / 2;
	        int msgY = top + 1 + static_cast<int>(i);
	        DrawText(msgX, msgY, wrappedLines[i], BRIGHT_WHITE, BLUE, true);
	    }
	    
	    // ��������Ի���
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
	        
	        // ����1��ʾ�ɹ����루���������룩��0��ʾȡ��
	        return input == "\x1B" ? -1 : 0;
	    }
	    
	    // ���ư�ť
	    int buttonY = top + boxHeight - 3;
	    int result = 0;
	    
	    // ��ť���ƺ���
	    auto drawButtons = [&](const vector<pair<string, int>>& buttons, int selected) {
	        int totalWidth = 0;
	        for (const auto& btn : buttons) {
	            totalWidth += static_cast<int>(btn.first.length()) + 4;
	        }
	        
	        int startX = left + (boxWidth - totalWidth) / 2;
	        for (size_t i = 0; i < buttons.size(); i++) {
	            // ���ư�ť�ı�
	            string btnText = " " + buttons[i].first + " ";
	            
	            // ���ð�ť��ɫ
	            ConsoleColor textColor = BRIGHT_WHITE;
	            ConsoleColor bgColor = GRAY;
	            
	            if ((int)i == (int)selected) {
	                textColor = BLACK;
	                bgColor = BRIGHT_YELLOW;
	            }
	            
	            DrawText(startX, buttonY, btnText, textColor, bgColor, true);
	            startX += static_cast<int>(btnText.length()) + 2;
	        }
	    };
	    
	    // ��ť���봦����
	    auto handleInput = [&](const vector<pair<string, int>>& buttons) {
	        int selected = 0;
	        size_t buttonCount = buttons.size();
	        
	        while (true) {
	            drawButtons(buttons, selected);
	            
	            int ch = _getch();
	            if (ch == 0 || ch == 224) {
	                ch = _getch(); // �������
	            }
	            
	            switch (ch) {
	                case 75: // ���ͷ
	                    selected = static_cast<int>((selected - 1 + buttonCount) % buttonCount);
	                    break;
	                case 77: // �Ҽ�ͷ
	                    selected = static_cast<int>((selected + 1) % buttonCount);
	                    break;
	                case 13: // �س�
	                    return buttons[selected].second;
	                case 27: // ESC
	                    return -1;
	                default:
	                    break;
	            }
	        }
	    };
	    
	    // ��������ȷ����ť�б�
	    vector<pair<string, int>> buttonList;
	    
	    // ������Զ��尴ť��ʹ���Զ��尴ť
	    if (customButtons != nullptr) {
	        buttonList = *customButtons;
	    }
	    // ����ʹ��Ԥ���尴ť
	    else {
	        switch (type) {
	            case DialogType::OK:
	                buttonList = { {"ȷ��", 1} };
	                break;
	            case DialogType::YESNO:
	                buttonList = { {"��", 1}, {"��", 0} };
	                break;
	            case DialogType::SURCAN:
	                buttonList = { {"ȷ��", 1}, {"����", 0} };
	                break;
	            case DialogType::CUSTOM:
	                // ���û���ṩ�Զ��尴ť��ʹ��Ĭ��ȷ����ť
	                buttonList = { {"ȷ��", 1} };
	                break;
	            default:
	                buttonList = { {"ȷ��", 1} };
	        }
	    }
	    
	    // ����ť����
	    result = handleInput(buttonList);
	    
	    // �ָ�ԭʼ����̨����
	    RestoreConsoleRegion(savedRegion);
	    RestoreOriginalAttributes();
	    
	    return result;
	}
}

