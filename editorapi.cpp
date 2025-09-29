#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
#define EDITOR
using namespace std;
//editorapi.cpp
//-std=c++14 -Os -s
#ifndef EDITORAPI_INC
#define EDITORAPI_INC
HANDLE hConsole;
string exedir;
struct Hook {
    string event;    // onSave, onRun
    string command;  //"python myplugin.py"
};
vector<Hook> hooks;
unordered_map<int, string> langMap;
//string currentLang = "zh-cn"; //Ĭ������
string lan_str(int num){
    if(langMap.find(num)!=langMap.end()){
        return langMap[num];
    }
    return "[missing:"+to_string(num)+"]";
}
void edt_pause(){
	cout<<lan_str(300);
	char c = _getch();
}
string lan_cstr(int num){
    return lan_cstr(num).c_str();
}
EDITOR string replaceall(string s, const string& s1, const string& s2) {
    if (s1.empty()) return s;  // ��������ѭ��
    size_t pos = 0;
    while ((pos = s.find(s1, pos)) != string::npos) {
        s.replace(pos, s1.length(), s2);
        pos += s2.length();  // �������滻���֣������ظ�����
    }
    return s;
}
void load_language(const string& langCode){
    langMap.clear();
    //currentLang = langCode;
    string fn = exedir+"setting\\"+langCode+".lang";
    ifstream ifs(fn);
    if(!ifs.is_open()){
        cerr<<"[lang] cannot open "<<fn<<endl;
        return;
    }
    string line;
    while(getline(ifs, line)){
        if(line.empty() || line[0]=='#') continue;
        size_t pos = line.find('=');
        if(pos == string::npos) continue;

        int id = stoi(line.substr(0, pos));
        string text = line.substr(pos+1);
        string text1 = replaceall(text,"\\033","\033");
        langMap[id] = text1;
    }
    cout<<"[lang] loaded "<<langMap.size()<<" entries from "<<fn<<endl;
    cout<<"[lang] name="<<lan_str(100)<<" / "<<lan_str(101)<<endl;
}
void load_hooks() {
    hooks.clear();
    string fn = exedir + "setting\\plugins.ini";
    ifstream ifs(fn);
    if(!ifs.is_open()){
        cout << "no plugins.ini, skip" << endl;
        return;
    }
    string line;
    while(getline(ifs,line)){
        if(line.empty() || line[0]=='#') continue;// ignore comment
        // ??: event=command
        auto pos = line.find('=');
        if(pos==string::npos) continue;
        Hook h;
        h.event = line.substr(0,pos);
        h.command = line.substr(pos+1);
        hooks.push_back(h);
    }
    cout<<"Loaded "<<hooks.size()<<" hooks"<<endl;
}
void runHooks(const string& event, const string& fname){
    for(auto &h : hooks){
        if(h.event == event){
        	if(h.command==""){
        		cout<<"[Hook] No hook at "<<event<<endl;
			}
			else{
	            string cmd = h.command + " \"" + fname + "\"";
	            cout << "[Hook] "<<event<<" -> "<<cmd<<endl;
	            system(cmd.c_str());				
			}
        }
    }
}
bool g_vt_support = 0;//support visual terminal
//font mode
EDITOR void TogBold(bool b){
    if(g_vt_support) cout << (b ? "\033[1m" : "\033[22m");
}
EDITOR void TogItalic(bool b){
    if(g_vt_support) cout << (b ? "\033[3m" : "\033[23m");
}
EDITOR void TogUnderline(bool b){
    if(g_vt_support) cout << (b ? "\033[4m" : "\033[24m");
}
EDITOR void ResetFormat(){
    if(g_vt_support) cout << "\033[0m";
}
//init
EDITOR void Edinit(){
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if(GetConsoleMode(hConsole, &mode)){
        DWORD newMode = mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if(SetConsoleMode(hConsole, newMode)){
            g_vt_support = true;
        } else {
            g_vt_support = false;
        }
    }
}
// �ƶ���굽ָ��λ��
EDITOR void EdmoveTo(int x, int y) {
    COORD pos = {static_cast<SHORT>(y), static_cast<SHORT>(x)};
    SetConsoleCursorPosition(hConsole, pos);
}
// ��ȡ��ǰ���λ��
EDITOR COORD EdgetPosition() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return csbi.dwCursorPosition;
}
EDITOR void clearInputBuffer() {
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
EDITOR void clearOutputBuffer(HWND hConsole) {
    if (!hConsole) return;
    
    // ��ȡ��ǰ����̨��Ϣ
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    
    // ������Ҫ������ַ���
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    
    // �ÿո��������������
    FillConsoleOutputCharacter(
        GetStdHandle(STD_OUTPUT_HANDLE),
        ' ',
        dwConSize,
        coordScreen,
        &cCharsWritten
    );
    
    // ���ù��λ��
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordScreen);
    
    // ��ѡ���������������
    if (hConsole) {
        SendMessage(hConsole, EM_SETSEL, 0, -1);
        SendMessage(hConsole, EM_REPLACESEL, 0, (LPARAM)"");
    }
}
// ����ƶ����
EDITOR void EdmoveRelative(int dx, int dy) {
    COORD current = EdgetPosition();
    EdmoveTo(current.X + dx, current.Y + dy);
}
// ������ֵ������� 
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
EDITOR void EdShowConsoleCursor(bool show) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = show;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}
EDITOR void EdSetCursorSize(DWORD size) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.dwSize = size;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}
// �������n������ƶ���
EDITOR void EdmoveLeft(int n) {
    COORD current = EdgetPosition();
    int newY = current.Y - n;
    
    // �߽��飬ȷ�������Ƴ�����̨��߽�
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int minY = csbi.srWindow.Left;
    
    newY = max(newY, minY);
    EdmoveTo(newY,current.X );
}
// ��ȡ����̨�ߴ�
EDITOR void EdgetConsoleSize(int& width, int& height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
// ���ÿ���̨���ڴ�С
EDITOR void EdSetConsoleSize(int width, int height) {
    COORD coord = {static_cast<SHORT>(width), static_cast<SHORT>(height)};
    SMALL_RECT rect = {0, 0, static_cast<SHORT>(width-1), static_cast<SHORT>(height-1)};

    SetConsoleScreenBufferSize(hConsole, coord);
    SetConsoleWindowInfo(hConsole, TRUE, &rect);
}
// ��������(��ʾ*)
EDITOR string EdGetPassword(const string& prompt = "Password: ") {
    cout << prompt;
    string password;
    char ch;
    while((ch = _getch()) != '\r') {
        if(ch == '\b' && !password.empty()) {
            cout << "\b \b";
            password.pop_back();
        }
        else if(isprint(ch)) {
            cout << '*';
            password += ch;
        }
    }
    cout << endl;
    return password;
}
// ���ÿ���̨��������С
// width, height: �µĻ��������ߣ��ַ�����
// ���� true ��ʾ�ɹ���false ��ʾʧ�ܣ���ͨ�� GetLastError() ��һ���Ų�ԭ��
EDITOR bool EdSetBufferSize(int width, int height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return false;

    // �������ߴ�
    COORD bufSize;
    bufSize.X = static_cast<SHORT>(width);
    bufSize.Y = static_cast<SHORT>(height);

    // ���� WinAPI
    return SetConsoleScreenBufferSize(hConsole, bufSize);
}
// ��ѡ�����ÿ���̨���ڳߴ磨ע�⣺���ڴ�С���ܴ��ڻ�������С��
// left, top, right, bottom: �����ڻ������е�����0-based���±����Ϊ���������-1��
// ���� true ��ʾ�ɹ���false ��ʾʧ��
EDITOR bool EdSetWindowSize(int left, int top, int right, int bottom) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return false;

    SMALL_RECT windowRect;
    windowRect.Left   = static_cast<SHORT>(left);
    windowRect.Top    = static_cast<SHORT>(top);
    windowRect.Right  = static_cast<SHORT>(right);
    windowRect.Bottom = static_cast<SHORT>(bottom);

    // ���� WinAPI
    return SetConsoleWindowInfo(hConsole, TRUE, &windowRect);
}
//�����ַ
EDITOR void EdWriteUrl(const std::string& displayName, const std::string& url) {
    if (g_vt_support) {
        cout << "\033]8;;" << url << "\a" << displayName << "\033]8;;\a";
    } else {
        // �������̨��֧�֣����˻�Ϊ�������
        cout << displayName << " (" << url << ")";
    }
}
#include <thread>
class consolenoclose
{
public:
	void run(){
        hWnd = GetConsoleWindow();
        if (hWnd)
        {
            HMENU hMenu = GetSystemMenu(hWnd, FALSE);
            if (hMenu)
            {
                // remove the Close entry
                DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
                DrawMenuBar(hWnd);
            }
        }

        // swallow console control events
        SetConsoleCtrlHandler(HandlerRoutine, TRUE);

        // start background thread to keep it disabled
        worker = std::thread(&consolenoclose::KeepDisabled, this);
        worker.detach();
	}
    // ctor: disable close button & CTRL+C, start background thread
    consolenoclose(){}

    // dtor: uninstall handler
    ~consolenoclose()
    {
        SetConsoleCtrlHandler(HandlerRoutine, FALSE);
    }

private:
    HWND             hWnd;
    std::thread      worker;

    // console�\ctrl handler: return TRUE to say "handled" => prevents default action
    static BOOL WINAPI HandlerRoutine(DWORD ctrlType)
    {
        switch (ctrlType)
        {
            case CTRL_C_EVENT:
            case CTRL_BREAK_EVENT:
            case CTRL_CLOSE_EVENT:
            case CTRL_LOGOFF_EVENT:
            case CTRL_SHUTDOWN_EVENT:
                return TRUE;
            default:
                return FALSE;
        }
    }

    // periodically re�\gray out the Close menu item
    void KeepDisabled()
    {
        while (true)
        {
            if (hWnd)
            {
                HMENU hMenu = GetSystemMenu(hWnd, FALSE);
                if (hMenu)
                {
                    EnableMenuItem(hMenu,
                                   SC_CLOSE,
                                   MF_BYCOMMAND | MF_GRAYED);
                    DrawMenuBar(hWnd);
                }
            }
            Sleep(5000); // 5 second
        }
    }
};
#endif //EDITORAPI_INC
