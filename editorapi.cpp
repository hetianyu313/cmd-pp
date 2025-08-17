#include<bits/stdc++.h>
#include<windows.h>
#define EDITOR
using namespace std;
//editorapi.cpp
//-std=c++14 -Os -s
#ifndef EDITORAPI_INC
#define EDITORAPI_INC
HANDLE hConsole;
//init
EDITOR void Edinit(){
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}
// 移动光标到指定位置
EDITOR void EdmoveTo(int x, int y) {
    COORD pos = {static_cast<SHORT>(y), static_cast<SHORT>(x)};
    SetConsoleCursorPosition(hConsole, pos);
}
// 获取当前光标位置
EDITOR COORD EdgetPosition() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return csbi.dwCursorPosition;
}
EDITOR void clearInputBuffer() {
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
// 相对移动光标
EDITOR void EdmoveRelative(int dx, int dy) {
    COORD current = EdgetPosition();
    EdmoveTo(current.X + dx, current.Y + dy);
}
// 输出文字到缓冲区 
EDITOR void EdwriteToConsole(int x, int y, const std::string& text) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD charsWritten;
    COORD pos = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    
    WriteConsoleOutputCharacterA(
        hConsole,
        text.c_str(),
        text.length(),
        pos,
        &charsWritten
    );
}
// 光标左移n格（相对移动）
EDITOR void EdmoveLeft(int n) {
    COORD current = EdgetPosition();
    int newY = current.Y - n;
    
    // 边界检查，确保不会移出控制台左边界
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int minY = csbi.srWindow.Left;
    
    newY = max(newY, minY);
    EdmoveTo(newY,current.X );
}
// 获取控制台尺寸
EDITOR void EdgetConsoleSize(int& width, int& height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
#endif //EDITORAPI_INC
