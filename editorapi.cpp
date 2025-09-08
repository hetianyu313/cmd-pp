#include<bits/stdc++.h>
#include<windows.h>
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
//string currentLang = "zh-cn"; //默认语言
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
        langMap[id] = text;
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
EDITOR void clearOutputBuffer(HWND hConsole) {
    if (!hConsole) return;
    
    // 获取当前控制台信息
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    
    // 计算需要清除的字符数
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    
    // 用空格填充整个缓冲区
    FillConsoleOutputCharacter(
        GetStdHandle(STD_OUTPUT_HANDLE),
        ' ',
        dwConSize,
        coordScreen,
        &cCharsWritten
    );
    
    // 重置光标位置
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordScreen);
    
    // 可选：清除滚动缓冲区
    if (hConsole) {
        SendMessage(hConsole, EM_SETSEL, 0, -1);
        SendMessage(hConsole, EM_REPLACESEL, 0, (LPARAM)"");
    }
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
//输出网址
EDITOR void EdWriteUrl(const std::string& displayName, const std::string& url) {
    if (g_vt_support) {
        cout << "\033]8;;" << url << "\a" << displayName << "\033]8;;\a";
    } else {
        // 如果控制台不支持，就退化为常规输出
        cout << displayName << " (" << url << ")";
    }
}
#endif //EDITORAPI_INC
