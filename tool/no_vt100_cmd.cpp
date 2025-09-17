#include <windows.h>
#include <iostream>
using namespace std;

void DisableVirtualTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if(!GetConsoleMode(hOut, &dwMode)) {
        cerr << "GetConsoleMode failed\n";
        return;
    }
    // 去掉 VT 模式标识
    dwMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if(!SetConsoleMode(hOut, dwMode)) {
        cerr << "SetConsoleMode failed\n";
    }
}

int main() {
    DisableVirtualTerminal();

    // 传统 API 方式设置文字属性
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    cout << "Hello in red (WinAPI mode)\n";
    SetConsoleTextAttribute(hConsole, 7);
    system("cmd");
}
