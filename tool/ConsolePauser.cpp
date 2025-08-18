#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;
int runProcess(const std::string &commandLine) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char *cmd = _strdup(commandLine.c_str());
    BOOL success = CreateProcessA(
        NULL,
        cmd,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );
    free(cmd);

    if (!success) {
        std::cerr << "CreateProcess failed. Error code: " << GetLastError() << std::endl;
        return -2147483648;
    }

    // 等待进程结束
    WaitForSingleObject(pi.hProcess, INFINITE);

    // 获取退出码
    DWORD exitCode;
    if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
        std::cerr << "GetExitCodeProcess failed. Error code: "
                  << GetLastError() << std::endl;
        exitCode = -2147483647;
    }

    // 关闭句柄
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (int)exitCode;
}
struct ConsoleColorMap { int r,g,b; WORD color; };
ConsoleColorMap colors[16] = {
    {0,0,0, 0}, {0,0,128, 1}, {0,128,0, 2}, {0,128,128, 3},
    {128,0,0, 4}, {128,0,128, 5}, {128,128,0, 6}, {192,192,192, 7},
    {128,128,128, 8}, {0,0,255, 9}, {0,255,0, 10}, {0,255,255, 11},
    {255,0,0, 12}, {255,0,255, 13}, {255,255,0, 14}, {255,255,255, 15}
};
void SetRGBmap(int a,int b = 0) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (b << 4) | a);
}
int main(int argv,char **argc){
	SetRGBmap(135);
	if(argv!=2){
		cout<<"usage:ConsolePauser.exe <filepath>\nby hty 20250818\n";
		SetRGBmap(15);
		system("pause");
		return 0;
	}
	unsigned int t1 = GetTickCount();
	cout<<"Starting process\n";
	SetRGBmap(15);
	int r = runProcess(argc[1]);
	SetRGBmap(135);
	unsigned int t2 = GetTickCount();
	cout<<"--------------------------------\n";
	unsigned int t = t2-t1;
	if(r<30000)cout<<"Process exited after "<<t<<"ms with return value "<<r<<"\n";
	else cout<<"Process exited after "<<t/1000<<"s with return value "<<r<<"\n";
	SetRGBmap(15);
	system("pause");
	return 0;
}

