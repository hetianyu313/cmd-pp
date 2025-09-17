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
    // ȥ�� VT ģʽ��ʶ
    dwMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if(!SetConsoleMode(hOut, dwMode)) {
        cerr << "SetConsoleMode failed\n";
    }
}

int main() {
    DisableVirtualTerminal();

    // ��ͳ API ��ʽ������������
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    cout << "Hello in red (WinAPI mode)\n";
    SetConsoleTextAttribute(hConsole, 7);
    system("cmd");
}
