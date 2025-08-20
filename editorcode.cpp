#include<bits/stdc++.h>
#include<conio.h>
#include<windows.h>
#include"editorapi.cpp"
#include"editorcpp.cpp"
#include"editorrgb.cpp"
#include"editorjqb.cpp"
#include"editorcwh.cpp"
#include"editorchar.cpp"
#include"editorstr.cpp"
#include"editorview.cpp"
#include"editorsrf.cpp"
using namespace std;
//-std=c++14 -O2 -s -m32
namespace _ed_code{
	int _ed_width,_ed_height;
	int _ed_top = 0;//顶端的行数 
	int _ed_line = 0;//界面行数 
	int zz_x = 0,zz_y = 0;//指针行\列 
	vector<string> v = {""};
	string g_ktip = "";//按键提示 
	int init(){
		system("cls");
		Edinit();
		EdgetConsoleSize(_ed_width,_ed_height);
		_ed_line = _ed_height-10;
		_ed_top = 0;  // 明确初始化
		zz_x = 0; zz_y = 0;  // 确保从(0,0)开始
		v = vector<string>(1, "");  // 明确初始化
		//printf("_ed_code.init:console size %dx%d\n",_ed_width,_ed_height);
		return 0;
	}
	unordered_map<string,bool> mp_c_hs1 = {
	{"int",1},{"long",1},{"double",1},{"char",1},{"short",1},
	{"float",1},{"string",1},{"list",1},{"queue",1},{"bool",1},
	{"unordered_map",1},{"map",1},{"pair",1},{"unsigned",1},{"signed",1},
	{"stack",1},{"vector",1},{"set",1},{"multiset",1},{"tuple",1},
	{"array",1},{"deque",1},{"unordered_set",1},{"bitset",1},{"struct",1},
	{"void",1},{"const",1},{"constexpr",1},{"inline",1},{"register",1},
	{"namespace",1},{"static",1},{"priority_queue",1},{"auto",1},
	{"class",1},{"public",1},{"volatile",1},{"protected",1},{"template",1},
	{"wchar_t",1},{"wstring",1},{"__int128",1},
	};
	unordered_map<string,bool> mp_c_hs2 = {
	{"sort",1},{"unique",1},{"next_permutation",1},{"__builtin_popcount",1},{"__builtin_ctz",1},
	{"__builtin_clz",1},{"min_element",1},{"max_element",1},{"nth_element",1},{"for",1},
	{"while",1},{"do",1},{"try",1},{"goto",1},{"default",1},
	{"switch",1},{"case",1},{"break",1},{"continue",1},{"if",1},
	{"else",1},{"return",1},{"exit",1},{"catch",1},{"throw",1},
	{"virtual",1},{"operator",1},{"typedef",1},{"friend",1},
	{"new",1},{"extern",1},{"enum",1},{"sizeof",1},{"private",1},
	{"asm",1},{"delete",1},{"union",1},{"static_cast",1},{"reinterpret_cast",1},
	{"NULL",1},{"nullptr",1}, 
	};
	unordered_map<char,bool> mp_c_qh = {
	{'+',1},{'-',1},{'*',1},{'/',1},{'%',1},{'^',1},{'|',1},{'{',1},{'}',1},{'[',1},
	{']',1},{'(',1},{')',1},{'&',1},{'!',1},{'~',1},{'<',1},{'>',1},{',',1},{'.',1},
	{'=',1},{';',1},{':',1},{'?',1},
	};
	bool fc_zdbq = 1;//当前状态是否允许自动补全,比如 " 中不补全 
	void _ed_outcol(string s,bool &f_dhzs,int col = -1){
		bool f_define = 0;
		bool f_str = 0;
		bool f_zs = 0;
		bool f_num = 0;
		char lc = 0;
		string gjz = "";
		int cnt = 0;
		if(f_dhzs) g_conc.SetRGBmap(g_view.c_zs);
		for(char c : s){
			bool no_putchar_this = 0;
			if(f_dhzs){
				if(lc=='*'&&c=='/'){
					f_dhzs = 0;
					g_conc.SetRGBmap(g_view.c_code);
				}
				else{
					if(cnt==zz_y&&col==zz_x)fc_zdbq = 0;
					no_putchar_this = 1;
					putchar(c);
				}
			}
			else if(!f_dhzs && !f_str && !f_zs && lc=='/' && c=='*'){
				f_dhzs = (bool)1;
				if(cnt==zz_y&&col==zz_x)fc_zdbq = 0;
				g_conc.SetRGBmap(g_view.c_zs);
				no_putchar_this = 1;
				putchar(c);
			}
			else if(!f_define && !f_str && !f_zs && !f_dhzs && c=='#' && lc!='\''){
				f_define = 1;
				if(col==zz_x)fc_zdbq = 0;
				g_conc.SetRGBmap(g_view.c_def);
			}
			else if(!f_define && !f_zs && !f_dhzs && lc!='\\' && c=='\"'){
				f_str = !f_str;
				if(!f_str){
					no_putchar_this = 1;
					putchar(c);	
					if(cnt>zz_y&&col==zz_x)fc_zdbq = 1;
				}
				else{
					if(cnt<=zz_y&&col==zz_x)fc_zdbq = 0;
				}
				g_conc.SetRGBmap(f_str ? g_view.c_str : g_view.c_code);
			}
			else if(!f_define && !f_str && !f_zs && !f_dhzs && lc=='/' && c=='/'){
				no_putchar_this = 1;
				g_conc.SetRGBmap(g_view.c_zs);
				COORD pos = EdgetPosition();
				EdmoveTo(pos.Y,pos.X-1);
				putchar('/');
				putchar('/');
				f_zs = 1;
				if(cnt<=zz_y&&col==zz_x)fc_zdbq = 0;
			}
			else if(!f_num && (isdigit(c)) && (mp_c_qh[lc] || lc==0 || lc==' ') && !f_define && !f_str && !f_zs && !f_dhzs){
				f_num = 1;
				g_conc.SetRGBmap(g_view.c_num);
				//if(cnt==zz_y&&col==zz_x)fc_zdbq = 0;
			}
			else if(f_num && !(isdigit(c)||c=='.'||c=='L'||c=='F'||c=='U'||c=='x'||c=='X'||c=='b'||c=='B'
			||c=='A'||c=='B'||c=='C'||c=='D'||c=='E'||c=='a'||c=='b'||c=='c'||c=='d'||c=='e'||c=='f')){
				f_num = 0;
				g_conc.SetRGBmap(g_view.c_code);
				//if(cnt==zz_y&&col==zz_x)fc_zdbq = 0;
			}
			if(!f_define && !f_str && !f_zs && !f_dhzs && !f_num){
				if(mp_c_qh[c]==1||c==' '||cnt==s.size()-1){
					if(mp_c_hs1[gjz]&&gjz.size()>0){
						//EdmoveLeft(gjz.size());
						COORD pos = EdgetPosition();
						//cerr<<"pos x="<<pos.X<<" y="<<pos.Y<<" sz="<<gjz.size();
						EdmoveTo(pos.Y,pos.X-gjz.size());
						g_conc.SetRGBmap(g_view.c_type);
						for(char c : gjz){
							putchar(c);
						}
						g_conc.SetRGBmap(g_view.c_code);
					}
					else if(mp_c_hs2[gjz]&&gjz.size()>0){
						//EdmoveLeft(gjz.size());
						COORD pos = EdgetPosition();
						//cerr<<"pos x="<<pos.X<<" y="<<pos.Y<<" sz="<<gjz.size();
						EdmoveTo(pos.Y,pos.X-gjz.size());
						g_conc.SetRGBmap(g_view.c_func);
						for(char c : gjz){
							putchar(c);
						}
						g_conc.SetRGBmap(g_view.c_code);
					}
					if(mp_c_qh[c]){
						g_conc.SetRGBmap(g_view.c_sign);
					}
					gjz = "";
				}
				else{
					g_conc.SetRGBmap(g_view.c_code);
					gjz+=c;
				}
			} 
			lc = c;//last char
			cnt++;
			if(!no_putchar_this)putchar(c);
		}
	}
	const string fl = "																														";
	void _ed_flash(){
		//system("cls");
		EdmoveTo(0,0);
		bool f_dhzs = 0;
		fc_zdbq = 1;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		for(int i = 0;i<=_ed_line;i++){
			int cl = i+_ed_top;
			EdmoveTo(i,0);
			COORD coord = {0, (SHORT)i}; // 第 i 行
			DWORD written;
			FillConsoleOutputCharacterA(hConsole, ' ', _ed_width, coord, &written);
			FillConsoleOutputAttribute(hConsole,g_view.c_code, _ed_width, coord, &written);
			g_conc.SetRGBmap(g_view.c_line);
			//g_conc.SetRGBmap((g_view.c_code%16+1)%16,g_view.c_code/16);
			EdmoveTo(i,0);
			cout<<fl;
			if(v.size()-1<cl)continue;
			EdmoveTo(i,0);
			//g_conc.SetRGBmap(7);
			printf("%-3d",cl);
			g_conc.SetRGBmap(g_view.c_code);
			//cout<<v[cl]<<endl;
			_ed_outcol(v[cl],f_dhzs,i==zz_x?i:-1);
		}
	}
	void _edf_up(){
		if(zz_x > 0){  // 确保不是第一行
			zz_x--;  // 先移动到上一行
			// 调整列位置不超过上一行的长度
			if(zz_y > v[zz_x].size()){
				zz_y = v[zz_x].size();
			}
			// 视口滚动逻辑
			if(zz_x < _ed_top){
				_ed_top = zz_x;
			}
		}
		//printf("_edf_up zz:%d,%d top:%d line:%d\n",zz_x,zz_y,_ed_top,_ed_line);
	}
	void _edf_down(){
		if(zz_x<v.size()-1){
			if(v[zz_x+1].size()>=zz_y){
				zz_x++;
			}
			else{
				zz_y = v[zz_x+1].size();
				zz_x++;
			}
		}
		if(zz_x>=_ed_top+_ed_line) _ed_top = zz_x-_ed_line;
	}
	void _edf_left(){
		if(zz_y>0){
			zz_y--;
		}
	}
	void _edf_right(){
		if(zz_y<v[zz_x].size()){
			zz_y++;
		}
	}
	/*void _edf_enter(){
		string s = v[zz_x].substr(zz_y);
		v[zz_x].erase(v[zz_x].begin()+zz_y,v[zz_x].end());
		v.insert(v.begin()+zz_x+1,(string)s);
		zz_x++;
		zz_y = 0; 
		if(zz_x>_ed_top+_ed_line) _ed_top = zz_x-_ed_line;
	}*/
	bool gf_atsj = 1;
	int get_indent_level(int line) {
	    int res = 0;
	    /*for (int i = 0; i < line; i++) {
	        string s = v[i];
	        // ??????/Tab
	        if(!s.empty())
	            s.erase(s.find_last_not_of(" \t") + 1);
	
	        for (char c : s) {
	            if (c == '{') level++;
	            if (c == '}') level = max(0, level - 1);
	        }
	    }*/
	    if(line==0) return 0;
	    string s = v[line-1];
	    for(char c : s){
	    	if(c==' ')res++;
	    	else if(c=='\t')res+=4;
	    	else break;
		}
	    return res>>2;
	}
	void _edf_enter() {
	    string rest = v[zz_x].substr(zz_y);
	    v[zz_x].erase(v[zz_x].begin() + zz_y, v[zz_x].end());
	    v.insert(v.begin() + zz_x + 1, rest);
	    zz_x++;
	    zz_y = 0;
	    if (gf_atsj) {
	        int level = get_indent_level(zz_x);
	        string prev = v[zz_x - 1];
	        if(!prev.empty())
	            prev.erase(prev.find_last_not_of(" \t") + 1);
	        if (!prev.empty() && prev.back() == '{') {
	            level++;
	        }
	        string indent(level * 4, ' ');
	        v[zz_x].insert(0, indent);
	        zz_y = indent.size();
	    }
	    if (zz_x > _ed_top + _ed_line) _ed_top = zz_x - _ed_line;
	}
	unordered_map<char,char> zdbq = {
	{'(',')'}/*,{'{','}'}*/,{'[',']'},{'<','>'},{'\"','\"'},
	{'\'','\''},//自动补全 
	};
	void _edf_addch(char c, bool sd = 0) {
	    if(c == '\t'){
	        for(int i=0;i<4;i++){
	            v[zz_x].insert(v[zz_x].begin()+zz_y, 32);
	            zz_y++;
	        }
	    }
	    else if(c == '}' && sd==1 && gf_atsj) {
	        string &line = v[zz_x];
	        int spaces = 0;
	        for(char ch : line){
	            if(ch==' ') spaces++;
	            else if(ch=='\t') spaces += 4;
	            else break;
	        }
	        if(zz_y <= spaces && spaces >= 4){
	            line.erase(0,4);
	            zz_y = max(0, zz_y-4);
	        }
	        line.insert(line.begin()+zz_y, '}');
	        zz_y++;
	    }
	    else if(zdbq[c]!=0 && sd==1 && fc_zdbq==1){
	        v[zz_x].insert(v[zz_x].begin()+zz_y,c); zz_y++;
	        v[zz_x].insert(v[zz_x].begin()+zz_y,zdbq[c]);
	    }
	    else{
	        v[zz_x].insert(v[zz_x].begin()+zz_y,c); zz_y++;
	    }
	}
	/*void _edf_back(bool ntp = 0) {
		// 安全校验
		if(v.empty() || zz_x >= v.size()) return;
		// 情况1：当前行有字符可删除（普通退格）
		if(zz_y > 0) {
			char c = v[zz_x][zz_y-1];
			v[zz_x].erase(zz_y-1, 1);  // 删除前一个字符
			zz_y--;
			if(!(c>=32&&c<=127) && ntp==0) _edf_back(1);
		}
		// 情况2：行首且不是第一行（跨行退格）
		else if(zz_x > 0) {
			int prev_len = v[zz_x-1].size();  // 保存上一行长度
			v[zz_x-1] += v[zz_x];	  // 合并两行内容
			v.erase(v.begin()+zz_x);	// 删除当前行
			// 更新光标位置
			zz_x--;
			zz_y = prev_len;
			// 视口调整逻辑
			if(zz_x < _ed_top) {
				_ed_top = max(0, zz_x - _ed_line/2);  // 保持光标在视口中央
			}
		}
		// 情况3：第一行行首（无操作）
	}*/
	// 窗口过程钩子
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (msg == WM_CONTEXTMENU) {
			return 0; // 完全阻止右键菜单
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}	
	void _edf_nopaste() {
		HWND hConsole = GetConsoleWindow();
		// 修改窗口样式
		LONG_PTR style = GetWindowLongPtr(hConsole, GWL_STYLE);
		SetWindowLongPtr(hConsole, GWL_STYLE, style & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
		// 设置新的窗口过程
		SetWindowLongPtr(hConsole, GWLP_WNDPROC, (LONG_PTR)WndProc);
		// 禁用快速编辑模式
		HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
		DWORD mode;
		GetConsoleMode(hInput, &mode);
		SetConsoleMode(hInput, mode & ~ENABLE_QUICK_EDIT_MODE);
		// 禁用系统菜单
		HMENU hMenu = GetSystemMenu(hConsole, FALSE);
		if (hMenu) {
			DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
			DeleteMenu(hMenu, SC_MINIMIZE, MF_BYCOMMAND);
			DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
			DrawMenuBar(hConsole);
		}
	}
	void _edf_back() {
		if (v.empty() || zz_x >= v.size()) return;
		// 情况1：当前行有字符可删
		if (zz_y > 0) {
			unsigned char c = static_cast<unsigned char>(v[zz_x][zz_y - 1]);
			if (zz_y >= 2) {
				unsigned char c1 = static_cast<unsigned char>(v[zz_x][zz_y - 2]);
				unsigned char c2 = c;
				// c1 在 0x81~0xFE 且 c2 在 0x40~0xFE 且 不等于 0x7F
				if (c1 >= 0x81 && c1 <= 0xFE &&
					c2 >= 0x40 && c2 <= 0xFE && c2 != 0x7F) {
					v[zz_x].erase(zz_y - 2, 2);
					zz_y -= 2;
					return;
				}
			}
			v[zz_x].erase(zz_y - 1, 1);
			zz_y--;
		}
		// 情况2：行首且不是第一行（跨行退格）
		else if (zz_x > 0) {
			int prev_len = v[zz_x - 1].size();
			v[zz_x - 1] += v[zz_x];
			v.erase(v.begin() + zz_x);
			zz_x--;
			zz_y = prev_len;
			if (zz_x < _ed_top) {
				_ed_top = std::max(0, zz_x - _ed_line / 2);
			}
		}
	}
	void _edf_home(){
		zz_y = 0;
	}
	void _edf_end(){
		zz_y = v[zz_x].size();
	}
	void _edf_pgup() {
		if(_ed_top > 0) {
			_ed_top--;
			zz_x--;
			// 光标位置校正（更安全的写法）
		}
	}
	void _edf_pgdn() {
		if(_ed_top + _ed_line < v.size()) {
			_ed_top++;
			zz_x++;
			// 光标位置校正（包含边界检查）
		}
	}
	void _edf_insert(){
		system("cls");
		clearInputBuffer();
		char c;
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		cout<<"Input your code now,end with ESCAPE\n";
		string s = "";
		int cnt = 0;
		while(1){
			c = _getch();
			if(c==13){
				v.insert(v.begin()+zz_x,s);
				zz_x++;
				s = "";
			}
			else if(c==27||c==0 ||c==0xE0){
				if(s!="") v.insert(v.begin()+zz_x,s);
				zz_x++;
				s = "";
				break;
			}
			else if(c=='\t'){
				s+="    ";
				cnt+=3;
			}
			else{
				s+=c;
			}
			if(cnt%100==0)printf("input %d bytes\n",cnt);
			cnt++;
		}
		zz_x = 0;
		zz_y = 0;
		_ed_top = 0;
	}
	void _edf_save_file(){
		system("cls");
		char c;
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		string fn = _ed_cpp::_ed_in_f;
		if(fn==""){
			cout<<"Input your file path\n";
			clearInputBuffer();
			getline(cin,fn);
		}
		else{
			fn = fn.substr(0,fn.find_last_of("."))+".cpp";
		}
		cout<<"file path:"<<fn<<endl;
		ofstream ofs(fn.c_str());
		_ed_cpp::_ed_in_f = fn;//自动设置 
		for(int i = 0;i<(int)v.size();i++){
			ofs<<v[i];
			if(i!=v.size()-1) ofs<<"\n";
		}
		ofs.close();
		cout<<"file saved successfully\n";
		system("pause");
	}
	// 将文件每行内容读取到vector<string>
	vector<string> readFileToVector(const string& filename) {
		vector<string> lines;
		ifstream file(filename);
		if (!file.is_open()) {
			cerr << "Error opening file: " << filename << endl;
			return lines;
		}
		string line;
		while (getline(file, line)) {
			lines.push_back(line);
		}
		file.close();
		return lines;
	}
	void _edf_load_file(){
		system("cls");
		clearInputBuffer();
		char c;
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		cout<<"Input your file path\n";
		string fn;
		getline(cin,fn);
		_ed_cpp::_ed_in_f = fn;//自动设置 
		v.clear();
		v = readFileToVector(fn);
		zz_x = 0;
		zz_y = 0;
		_ed_top = 0;
		system("pause");
	}
	void _edf_about(){
		system("cls");
		clearInputBuffer();
		cout<<"CMD++\nby HeTianYu313 (Github.com)\nLanguage:C++(MinGW64)\n";
		system("pause");
	}
	void _edf_obfus(){
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0);
		g_conc.SetRGBmap(135);
		cout<<"Input obfuscated code path:\n";
		string fn;
		getline(cin,fn);
		g_conc.SetRGBmap(78);
		cout<<"警告:代码混淆功能不推荐使用,产生后果自负!\n";
		cout<<"	 这是最后一次提醒,你可以关闭窗口	 \n";
		g_conc.SetRGBmap(135);
		system("pause");
		string cmd = "\""+_ed_cpp::exedir+"tool\\CodeObfuscator.exe\" \""+_ed_cpp::_ed_in_f+"\" \""+fn+"\"";
		cout<<"命令:"<<cmd<<endl;
		int r = _ed_cpp::runProcess(cmd.c_str());
		cout<<"执行结果:"<<r<<endl;
		system("pause");
	}
	void _edf_cstyle(){
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0);
		g_conc.SetRGBmap(135);
		cout<<"Input styled code path:\n";
		string fn;
		getline(cin,fn);
		g_conc.SetRGBmap(78);
		cout<<"警告:代码整理功能不推荐使用,产生后果自负!\n";
		cout<<"	 这是最后一次提醒,你可以关闭窗口	 \n";
		g_conc.SetRGBmap(135);
		system("pause");
		string cmd = "\""+_ed_cpp::exedir+"tool\\cpp_code_nai.exe\" \""+_ed_cpp::_ed_in_f+"\" \""+fn+"\"";
		cout<<"命令:"<<cmd<<endl;
		int r = _ed_cpp::runProcess(cmd.c_str());
		cout<<"执行结果:"<<r<<endl;
		system("pause");
	}
	void _edf_curto(){
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0);
		g_conc.SetRGBmap(135);
		cout<<"cursor line number:\n";
		int ln;
		cin>>ln;
		if(ln<0||ln>=v.size()){
			ln = 0;
		}
		_ed_top = ln;
		zz_x = ln;
		zz_y = 0;
	}
	void _edf_jqb(){
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0);
		g_conc.SetRGBmap(135);
		cout<<"将文件复制到剪切板\n";
		cout<<"0.ansi 1.utf8 2.utf16\n";
		int ec = 0;
		cin>>ec;
		string s = "";
		for(int i = 0;i<(int)v.size();i++){
			s+=v[i];
			if(i!=v.size()-1)s+="\n";
		}
		_ed_jqb jq;
		if(ec==0){
			jq.setTextA(s.c_str());
		}
		if(ec==1){
			string u = AnsiToUtf8(s);
			jq.setTextUTF8(u.c_str());
		}
		if(ec==2){
			wstring u = AnsiToUtf16(s);
			jq.setTextW(u.c_str());
		}
		cout<<"已复制到剪切板\n";
		system("pause");
	}
	// 记录上一次查找内容和光标位置
	string _last_find = "";
	int _last_x = -1;
	int _last_y = -1;
	// 查找第一个匹配
	void _edf_find() {
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0); g_conc.SetRGBmap(135);
	
		cout << "Find text内容:" << endl;
		string keyword;
		getline(cin, keyword);
	
		if(keyword.empty()) {
			cout << "Empty input, cancelled.\n";
			system("pause");
			return;
		}
	
		bool found = false;
		int fx = -1, fy = -1;
	
		for(size_t i = 0; i < v.size(); i++) {
			size_t pos = v[i].find(keyword);
			if(pos != string::npos) {
				fx = (int)i;
				fy = (int)pos;
				found = true;
				break;
			}
		}
	
		if(found) {
			zz_x = fx; zz_y = fy;
			if(zz_x < _ed_top) {
				_ed_top = zz_x;
			} else if(zz_x >= _ed_top + _ed_line) {
				_ed_top = max(0, zz_x - _ed_line/2);
			}
			_last_find = keyword;
			_last_x = zz_x;
			_last_y = zz_y;
			cout << "Found at line " << fx << " col " << fy << endl;
			Sleep(200);
		} else {
			cout << "Not found: " << keyword << endl;
			system("pause");
		}
		//system("pause");
	}
	// 查找下一个（基于上一次关键字/位置）
	void _edf_find_next() {
		if(_last_find.empty()) {
			system("cls");
			cout << "No previous search. Use find first.\n";
			system("pause");
			//cout<<"_last_find empty,user input\n";
			//_edf_find();
			return;
		}
	
		bool found = false;
		int fx = -1, fy = -1;
	
		// 从当前位置继续往后查找
		for(size_t i = _last_x; i < v.size(); i++) {
			size_t startPos = (i == _last_x ? _last_y + 1 : 0);
			size_t pos = v[i].find(_last_find, startPos);
			if(pos != string::npos) {
				fx = (int)i;
				fy = (int)pos;
				found = true;
				break;
			}
		}
	
		if(found) {
			zz_x = fx; zz_y = fy;
			if(zz_x < _ed_top) {
				_ed_top = zz_x;
			} else if(zz_x >= _ed_top + _ed_line) {
				_ed_top = max(0, zz_x - _ed_line/2);
			}
			_last_x = zz_x;
			_last_y = zz_y;
			system("cls");
			cout << "Next found at line " << fx << " col " << fy << endl;
		} else {
			system("cls");
			cout << "No more matches of \"" << _last_find << "\"\n";
			system("pause");
		}
		//system("pause");
		Sleep(200);
	}
	// 替换（一个或全部）
	void _edf_replace() {
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0); g_conc.SetRGBmap(135);
	
		string from, to;
		cout << "Replace what从? ";
		getline(cin, from);
		if(from.empty()) {
			cout << "Empty search, cancelled.\n";
			system("pause");
			return;
		}
		cout << "Replace with替换到? ";
		getline(cin, to);
	
		cout << "Replace mode: \n1. Replace first occurrence替换第一个\n2. Replace all替换全部\n";
		char c = _getch();
	
		int cnt = 0;
	
		if(c == '1') {
			bool done = false;
			for(size_t i = 0; i < v.size() && !done; i++) {
				size_t pos = v[i].find(from);
				if(pos != string::npos) {
					v[i].replace(pos, from.size(), to);
					zz_x = (int)i;
					zz_y = (int)(pos + to.size());
					if(zz_x < _ed_top) {
						_ed_top = zz_x;
					} else if(zz_x >= _ed_top + _ed_line) {
						_ed_top = max(0, zz_x - _ed_line/2);
					}
					cnt = 1;
					done = true;
				}
			}
		} else if(c == '2') {
			for(size_t i = 0; i < v.size(); i++) {
				size_t pos = 0;
				while((pos = v[i].find(from, pos)) != string::npos) {
					v[i].replace(pos, from.size(), to);
					pos += to.size();
					cnt++;
				}
			}
		}
	
		cout << "Replaced " << cnt << " occurrences.\n";
		system("pause");
	}
	void _edf_escape(){
		system("cls");
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		cout<<"选择完成后如果没有反应按一次回车,部分功能需要保存后使用\n";
		cout<<"0.back\n1.insert\n2.exit cmd++\n3.save file\n4.load file\n5.complete\n6.complete and run\n7.about\n";
		cout<<"8.obfuscate code\n9.move cursor\na.copy to clipboard\nb.set view of IDE\n";
		cout<<"c.code style\nd.find first\ne.find\nf.replace\n";
		int c = _getch();
		switch (c){
			case '0':return;break;
			case '1':_edf_insert();break;
			case '2':exit(0);break;
			case '3':_edf_save_file();break;
			case '4':_edf_load_file();break;
			case '5':_ed_cpp::_ed_complete_file(0);break;
			case '6':_ed_cpp::_ed_complete_file(1);break;
			case '7':_edf_about();break;
			case '8':_edf_obfus();break;
			case '9':_edf_curto();break;
			case 'a':_edf_jqb();break;
			case 'b':eview::_ev_main();break;
			case 'c':_edf_cstyle();break;
			case 'd':_edf_find();break;
			case 'e':_edf_find_next();break;
			case 'f':_edf_replace();break;
		}
		g_conc.SetRGBmap(15);
		system("cls");
	}
	void _edf_zhcn_display(vector<_sr_py> v,int p,string ci){
		g_conc.SetRGBmap(8);
		g_ktip = "home:上一页 end:下一页 escape:退出中文输入 0~9:选择 a-z:拼音输入 A-Z:直接输入";
		EdmoveTo(_ed_line+1,0);
		cout<<fl<<endl<<fl<<endl;
		EdmoveTo(_ed_line+1,0);
		printf("_ed_run zz:%d,%d top:%d line:%d vs:%d\n",zz_x,zz_y,_ed_top,_ed_line,v.size());
		   cout<<g_ktip<<"\n"; 
		g_conc.SetRGBmap(112);
		EdmoveTo(_ed_line+3,0);
		cout<<fl<<"\n"<<fl<<"\n"<<fl;
		EdmoveTo(_ed_line+3,0);
		string s = "中文:";
		for(int i = 0;i<10;i++){
			int l = p*10+i;
			if(l<v.size())s+=to_string(((i+1)%10))+"."+v[l].hz+" ";
		}
		cout<<s<<"\n"<<"页码:"<<p<<"/"<<v.size()/10<<"(候选:"<<v.size()<<")\n";
		cout<<"输入:"<<ci;
		EdmoveTo(zz_x-_ed_top,zz_y+3);
	}
	void _edf_zhcn(){
		system("cls");
		_ed_flash();
		int c;
		int page = 0;
		string ci = "";
		//string str = "";
		vector<_sr_py> vp;
		_edf_zhcn_display(vp,page,ci);
		while(1){//get char add transfer to Chinese
			c = _getch();
			if(c==8){
				if(ci.size()>0) ci.erase(ci.size()-1);//删除中文区
				else{
					_edf_back();//删除代码区 
					_ed_flash();
				}
				vp = srf::py_near(ci);
			}
			else if(c == 0 || c == 0xE0){
				c = _getch();
				if((int)c==71&&(page*10+1<vp.size())){//home
					page++;
				}
				else if((int)c==79&&page>0){//end
					page--;
				}
				else{
					switch(c){
						case 72: _edf_up();_ed_flash(); break;	// VK_UP
						case 80: _edf_down();_ed_flash(); break;  // VK_DOWN
						case 75: _edf_left();_ed_flash(); break;  // VK_LEFT
						case 77: _edf_right();_ed_flash(); break; // VK_RIGHT
						default: c = 32;
					}
				}
				//if(page>(vp.size()-1)/10) page = ((int)vp.size()-1)/10;
				//if(page<=0) page=0;	
			}
			else if(c==13){
				if(vp.size()>0){ 
					for(char i : vp[0].hz){//add string to code
						_edf_addch(i);
					}
					//str+=vp[0].hz;
					vp.clear();
					ci= "";
					_ed_flash();
				}
				else{
					_edf_enter();
					_ed_flash();
				} 
			}
			else if(c==27){
				break;
			}
			else if(c>='a'&&c<='z'){
				ci.push_back(c);
				vp.clear();
				vp = srf::py_near(ci);
			}
			else if(c>='A'&&c<='Z'){
				//str+=c;
				_edf_addch(c);
				_ed_flash();
			}
			else if(c>='0'&&c<='9'){
				int n = ((c-'0'-1+10)%10)+page*10;
				if(vp.size()>n){
					for(char i : vp[n].hz){//add string to code
						_edf_addch(i);
					}
					//str+=vp[n].hz;
					vp.clear();
					ci= "";
					_ed_flash();
				}
			}
			if(page>vp.size()/10) page = ((int)vp.size())/10;
			if(page<=0) page=0;
			_edf_zhcn_display(vp,page,ci);
		}
		system("cls");
		_ed_flash();
		/*for(char i : str){//add string to code
			_edf_addch(i);
		}*/
	}
	void _edf_flash1(){
		system("cls");
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD mode;
		GetConsoleMode(hConsole, &mode);
		SetConsoleMode(hConsole, ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hConsole, &csbi);
		clearInputBuffer();
		clearOutputBuffer(NULL);
		EdmoveTo(0,0);
		_ed_flash();
		for(int i = 0;i<(int)v.size();i++){
			string s = v[i];
			string t = "";
			for(char c : s){
				if(c=='\t') t+="    ";
				else t+=c;
			}
			v[i] = t;
		}
	}
	int _ed_run(){ 
		int c = _getch(); 
		//cout<<c<<endl;
		g_ktip = "esc:菜单 f1:插入 f2:输入中文 f3:刷新(可修复bug)";
		if(zz_x<0||zz_x>v.size()){
			zz_x = _ed_top;
		}
		else{
			if(c == 0 || c == 0xE0){
				c = _getch();
				//cout<<"="<<c<<endl;
				switch(c){
					case 72: _edf_up(); break;	// VK_UP
					case 80: _edf_down(); break;  // VK_DOWN
					case 75: _edf_left(); break;  // VK_LEFT
					case 77: _edf_right(); break; // VK_RIGHT
					case 71: _edf_home(); break; // home
					case 79: _edf_end(); break;  // end
					case 0x49: _edf_pgup(); break; // pgup
					case 0x51: _edf_pgdn(); break;  // pgdn
					case 0x3b: _edf_insert(); break;  // f1
					case 0x3c: _edf_zhcn(); break;  // f2
					case 0x3d: _edf_flash1(); break;  // f3
				}
			}
			else {
				switch(c){
					case 8: _edf_back(); break;  //backspace
					case 13: _edf_enter(); break;//enter
					case 27: _edf_escape(); break;	 //escape
					default: _edf_addch(c,1); break;
				}
			}
		}
		_ed_flash();
		g_conc.SetRGBmap(8);
		EdmoveTo(_ed_line+1,0);
		cout<<fl<<endl<<fl<<endl;
		EdmoveTo(_ed_line+1,0);
		printf("_ed_run zz:%d,%d top:%d line:%d vs:%d\n",zz_x,zz_y,_ed_top,_ed_line,v.size());
		cout<<g_ktip<<"\n"; 
		EdmoveTo(zz_x-_ed_top,zz_y+3);
		return 0;
	}
}
int main(){
	cout<<"main:start console c++ ide\n";
	SetConsoleTitle("Console C++ IDE");
	_ed_code::init();
	_ed_cpp::init();
	cwh::init();
	eview::init();
	srf::init();
	_ed_code::_edf_nopaste();
	cout<<"main:init all end\n";
	/*auto v = srf::py_near("bian");
	cout<<v.size()<<endl;
	for(auto i : v){
		cout<<i.hz<<" "<<i.py<<endl;
	}*/
	system("cls");
	/*string s;
	vector<pair<string, int>> v = {{"T1",0},{"T2",1},{"T3",2},{"T4",3},{"T5",4},{"T6",5},
	{"T1",0},{"T2",1},{"T3",2},{"T4",3},{"T5",4},{"T6",5}};
	string s1 = "test message";
	cwh::createConsoleInsideWindow(s1,cwh::DialogType::CUSTOM,&s,&v);*/
	_ed_code::_ed_flash();
	while(1){
		_ed_code::_ed_run();
	}
	return 0;
}

