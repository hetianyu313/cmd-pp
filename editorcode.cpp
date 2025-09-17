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
#include"editorlang.cpp"
#include"editorsrf.cpp"
#include"editorxgj.cpp"
using namespace std;
//-std=c++14 -O2 -s -m32
namespace _ed_code{
	int _ed_width,_ed_height;
	int _ed_top = 0;//顶端的行数 
	int _ed_line = 20;//界面行数 
	int zz_x = 0,zz_y = 0;//指针行\列 
	vector<string> v = {""};
	string g_ktip = "";//按键提示 
	string g_doc = "";
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
	unordered_map<string, vector<string>> docMap;
	bool fc_zdbq = 1;//当前状态是否允许自动补全,比如 " 中不补全
	
	void load_docmap(){
		string fn = exedir_get()+"setting\\docmap.ini";
		cout<<"load_docmap:load from "<<fn<<endl;
		docMap.clear();
		string s,t;
		ifstream ifs(fn.c_str());
		while(1){
			getline(ifs,s);
			if(s=="") break;
			vector<string> sv;
			while(1){
				getline(ifs,t);
				sv.push_back(t);
				if(t=="")break;
			}
			docMap[s] = sv;
		}
		cout<<"load_docmap:loaded "<<docMap.size()<<" items of docMap\n";
	}
	void _ed_outcol(string s,bool &f_dhzs,int col = -1){//utf8 string
		bool f_define = 0;
		bool f_str = 0;
		bool f_zs = 0;
		bool f_num = 0;
		char lc = 0;
		string gjz = "";
		string last_kw = "";
		int cnt = 0;
		if(f_dhzs){
			g_view.c_zs.tog();
			TogBold(g_view.b_zs);
			TogItalic(g_view.i_zs);
			TogUnderline(g_view.u_zs);
		}
		string u8buf;   // 用于缓存一个 UTF8 字符
	    auto flush_u8buf = [&](bool count = true) {
	        if (!u8buf.empty()) {
	            printf("%s", u8buf.c_str());
	            if (count) {
		            if ((unsigned char)u8buf[0] < 0x80) {
		                cnt += 1;     // 英文1列
		            } else {
		                cnt += 2;     // 汉字在ANSI里是两个字节
		            }
		        }
	            u8buf.clear();
	        }
	    };
		for(char c : s){
			bool no_putchar_this = 0;
			if(f_dhzs){
				if(lc=='*'&&c=='/'){
					f_dhzs = 0;
					TogBold(0);
					TogItalic(0);
					TogUnderline(0);
					g_view.c_code.tog();
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
				g_view.c_zs.tog();
				TogBold(g_view.b_zs);
				TogItalic(g_view.i_zs);
				TogUnderline(g_view.u_zs);
				no_putchar_this = 1;
				putchar(c);
			}
			else if(!f_define && !f_str && !f_zs && !f_dhzs && c=='#' && lc!='\''){
				f_define = 1;
				if(col==zz_x)fc_zdbq = 0;
				g_view.c_def.tog();
			}
			else if(!f_define && !f_zs && !f_dhzs && lc!='\\' && c=='\"'){
				f_str = !f_str;
				if(!f_str){
					no_putchar_this = 1;
					putchar(c);	
					if(cnt>zz_y&&col==zz_x)fc_zdbq = 1;
					g_view.c_code.tog();
				}
				else{
					if(cnt<=zz_y&&col==zz_x)fc_zdbq = 0;
					g_view.c_str.tog();
				}
			}
			else if(!f_define && !f_str && !f_zs && !f_dhzs && lc=='/' && c=='/'){
				no_putchar_this = 1;
				g_view.c_zs.tog();
				TogBold(g_view.b_zs);
				TogItalic(g_view.i_zs);
				TogUnderline(g_view.u_zs);
				COORD pos = EdgetPosition();
				EdmoveTo(pos.Y,pos.X-1);
				putchar('/');
				putchar('/');
				f_zs = 1;
				if(cnt<=zz_y&&col==zz_x)fc_zdbq = 0;
			}
			else if(!f_num && (isdigit(c)) && (mp_c_qh[lc] || lc==0 || lc==' ') && !f_define && !f_str && !f_zs && !f_dhzs){
				f_num = 1;
				g_view.c_num.tog();
				//if(cnt==zz_y&&col==zz_x)fc_zdbq = 0;
			}
			else if(f_num && !(isdigit(c)||c=='.'||c=='L'||c=='F'||c=='U'||c=='x'||c=='X'||c=='b'||c=='l'
			||c=='A'||c=='B'||c=='C'||c=='D'||c=='E'||c=='a'||c=='b'||c=='c'||c=='d'||c=='e'||c=='f')){
				f_num = 0;
				g_view.c_code.tog();
				//if(cnt==zz_y&&col==zz_x)fc_zdbq = 0;
			}
			if(!f_define && !f_str && !f_zs && !f_dhzs && !f_num){
	            if(mp_c_qh[c]==1||c==' '||(unsigned int)cnt==s.size()-1){
	                if(gjz.size()>0){
	                    if(mp_c_hs1[gjz]){
	                        COORD pos = EdgetPosition();
	                        EdmoveTo(pos.Y,pos.X-gjz.size());
	                        g_view.c_type.tog();
							TogBold(g_view.b_type);
							TogItalic(g_view.i_type);
							TogUnderline(g_view.u_type);
	                        for(char c : gjz) putchar(c);
							g_view.c_code.tog();
	                        TogBold(0);
							TogItalic(0);
							TogUnderline(0);
	                        last_kw = gjz; // <-- 记录
	                    }
	                    else if(mp_c_hs2[gjz]){
	                        COORD pos = EdgetPosition();
	                        EdmoveTo(pos.Y,pos.X-gjz.size());
	                        g_view.c_func.tog();
							TogBold(g_view.b_func);
							TogItalic(g_view.i_func);
							TogUnderline(g_view.u_func);
	                        for(char c : gjz) putchar(c);
	                        g_view.c_code.tog();
	                        TogBold(0);
							TogItalic(0);
							TogUnderline(0);
	                        last_kw = gjz; // <-- 记录
	                    }
	                }
	                if(mp_c_qh[c]){
	                    g_view.c_sign.tog();
	                }
	                gjz = "";
	            }
	            else{
				g_view.c_code.tog();
	                gjz += c;
	            }
	        }
	        lc = c;
		    /*if(!no_putchar_this){
		    	putchar(c);
			}*/
			// 收集到 u8buf
	        
	        if (!no_putchar_this) {
	            u8buf.push_back(c);
	            // 判断是否组成了完整 UTF-8 字符
	            if ( (u8buf[0] & 0x80) == 0 ) {
	                // 单字节 ASCII
	                flush_u8buf();
	            }
	            else if ( (u8buf[0] & 0xE0) == 0xC0 && u8buf.size() == 2 ) {
	                flush_u8buf();
	            }
	            else if ( (u8buf[0] & 0xF0) == 0xE0 && u8buf.size() == 3 ) {
	                flush_u8buf();
	            }
	            else if ( (u8buf[0] & 0xF8) == 0xF0 && u8buf.size() == 4 ) {
	                flush_u8buf();
	            }
	            // 否则还没收集完，继续等下一个字节
	        }
	        else cnt++;
			if(col == zz_x && cnt<=zz_y && !last_kw.empty()){
		        g_doc = last_kw;
		    }
		}
		TogBold(0);
		TogItalic(0);
		TogUnderline(0);
	}
	const string fl = "                                                                                                                        ";
	//120字符占位 
	void filllinePRO(int i, int r=30, int g=30, int b=30){
	    EdmoveTo(i,0);
	    // 置背景色
	    printf("\x1b[48;2;%d;%d;%dm", r, g, b);
	    // 填充颜色
	    cout<<fl;
	    //EdmoveTo(i,0);
	}
	void fillline(int i){
		filllinePRO(i,g_view.c_code.r1,g_view.c_code.g1,g_view.c_code.b1);
		/*COORD coord = {0, (SHORT)i}; // 第 i 行
		DWORD written;
		FillConsoleOutputCharacterA(hConsole, ' ', _ed_width, coord, &written);
		FillConsoleOutputAttribute(hConsole,g_view.c_code.col24, _ed_width, coord, &written);
		EdmoveTo(i,0);
		g_view.c_code.tog();*/
	}
	void _edf_doc_display(const string& key){
	    if(docMap.find(key) == docMap.end()){
	    	g_doc.clear();
	    	return;
		}
	    //EdmoveTo(_ed_line+6, 0); 
	    //g_conc.SetRGBmap(120);
	    //cout<<string(_ed_width, ' ')<<endl;
	    vector<string> &lines = docMap[key];
	    fillline(_ed_line+5);
	    fillline(_ed_line+6);
	    fillline(_ed_line+7);
	    fillline(_ed_line+8);
	    fillline(_ed_line+9);
	    for(size_t i=0;i<lines.size();i++){
	        EdmoveTo(_ed_line+5+i,0);
	        g_conc.SetRGBmap(120);
	        cout<<lines[i];
	    }
	    g_conc.SetRGBmap(15);
	}
	vector<string> fileList;
    int fileSel = 0;
    void load_file_list(const string& dir){
        fileList.clear();
        WIN32_FIND_DATA ffd;
        HANDLE hFind = FindFirstFile((dir + "\\*").c_str(), &ffd);
        if(hFind == INVALID_HANDLE_VALUE) return;
        do {
            if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                fileList.push_back(ffd.cFileName);
            }
        } while(FindNextFile(hFind, &ffd));
        FindClose(hFind);
    }
    void _ed_draw_filetree(){
        int startX = _ed_width - 30;
        g_conc.SetRGBmap(128);
        for(int i=0;i<_ed_line;i++){
            EdmoveTo(i, startX);
            cout << string(30,' ');
        }
        EdmoveTo(0, startX);
        g_conc.SetRGBmap(143);
        cout << " FILES ";
        g_conc.SetRGBmap(15);
        int maxShow = _ed_line-2;
        for(int i=0;(unsigned int)i<fileList.size() && i<maxShow;i++){
            EdmoveTo(i+1, startX);
            if(i==fileSel){
                g_conc.SetRGBmap(112);
            }else{
                g_conc.SetRGBmap(15);
            }
            string name = fileList[i];
            if(name.size()>28) name = name.substr(0,27)+"~";
            cout << " " << setw(28) << left << name;
        }
        g_conc.SetRGBmap(15);
    }
    HANDLE hConsole;
	void _ed_flash(){
		EdShowConsoleCursor(0);
		EdmoveTo(0,0);
		bool f_dhzs = 0;
		fc_zdbq = 1;
		g_doc.clear();
		for(int i = 0;i<=_ed_line;i++){
			int cl = i+_ed_top;
			fillline(i);
			if(v.size()-1<(unsigned int)cl)continue;
			EdmoveTo(i,0);
			g_view.c_line.tog();
			printf("%-3d",cl);
			g_view.c_code.tog();
			_ed_outcol(AnsiToUtf8(v[cl]),f_dhzs,cl==zz_x?cl:-1);
		}
		//帮助
		if(g_doc!=""){
		    _edf_doc_display(g_doc);
		}
		EdShowConsoleCursor(1);
	}
	void _edf_up(){
		if(zz_x > 0){
			zz_x--;
			if((unsigned int)zz_y > v[zz_x].size()){
				zz_y = v[zz_x].size();
			}
			if(zz_x < _ed_top){
				_ed_top = zz_x;
			}
		}
	}
	void _edf_down(){
		if((unsigned int)zz_x<v.size()-1){
			if(v[zz_x+1].size()>=(unsigned int)zz_y){
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
		if((unsigned int)zz_y<v[zz_x].size()){
			zz_y++;
		}
	}
	bool gf_atsj = 1;
	int get_indent_level(int line) {
	    int res = 0;
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
	{'(',')'}/*,{'{','}'},{'<','>'}*/,{'[',']'},{'\"','\"'},
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
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (msg == WM_CONTEXTMENU) {
			return 0;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}	
	void _edf_nopaste() {
		HWND hConsole = GetConsoleWindow();
		LONG_PTR style = GetWindowLongPtr(hConsole, GWL_STYLE);
		SetWindowLongPtr(hConsole, GWL_STYLE, style & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
		SetWindowLongPtr(hConsole, GWLP_WNDPROC, (LONG_PTR)WndProc);
		HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
		DWORD mode;
		GetConsoleMode(hInput, &mode);
		SetConsoleMode(hInput, mode & ~ENABLE_QUICK_EDIT_MODE);
		HMENU hMenu = GetSystemMenu(hConsole, FALSE);
		if (hMenu) {
			DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
			DeleteMenu(hMenu, SC_MINIMIZE, MF_BYCOMMAND);
			DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
			DrawMenuBar(hConsole);
		}
	}
	void _edf_back() {
		if (v.empty() || (unsigned int)zz_x >= v.size()) return;
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
			if((unsigned int)zz_y>v[zz_x].size()) zz_y = v[zz_x].size();
		}
	}
	void _edf_pgdn() {
		if((unsigned int)_ed_top + _ed_line < v.size()) {
			_ed_top++;
			zz_x++;
			// 光标位置校正（包含边界检查）
			if((unsigned int)zz_y>v[zz_x].size()) zz_y = v[zz_x].size();
		}
	}
	void _edf_insert(){
		system("cls");
		clearInputBuffer();
		int c;
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		cout<<lan_str(222)<<"\n";
		string s = "";
		int cnt = 0;
		while(1){
			c = _getch();
			if(c==13){
				v.insert(v.begin()+zz_x,Utf8ToAnsi(s));
				zz_x++;
				s = "";
			}
			else if(c==27){
				if(s!="") v.insert(v.begin()+zz_x,Utf8ToAnsi(s));
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
	string g_save_encoding = "ansi";
	string g_save_eol = "lf";
	void _edf_save_file(){
  		system("cls");
		char c;
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		string fn = _ed_cpp::_ed_in_f;
		if(fn==""){
			cout<<lan_str(223)<<"\n";
			clearInputBuffer();
			getline(cin,fn);
		}
		else{
			fn = fn.substr(0,fn.find_last_of("."))+".cpp";
		}
		cout<<lan_str(224)<<":"<<fn<<endl;
		ofstream ofs(fn, ios::binary);
		string eol = (g_save_eol=="crlf")? "\r\n" : "\n";
		string data="";
		for(size_t i=0;i<v.size();i++){
		    data += v[i];
		    if(i+1<v.size()) data += eol;
		}
		if(g_save_encoding=="ansi"){
		    ofs<<(data);
		}else if(g_save_encoding=="utf8"){
		    ofs<<AnsiToUtf8(data);
		}else if(g_save_encoding=="utf16"){
		    wstring w = AnsiToUtf16(data);
		    ofs.write((const char*)w.c_str(), w.size()*sizeof(wchar_t));
		}
		_ed_cpp::_ed_in_f = fn;//自动设置
		ofs.close();
		//file saved successfully
		cout<<lan_str(225)<<endl;
		runHooks("onSave", fn);
		edt_pause();
	}
	vector<string> readFileToVector(const string& filename) {
		vector<string> lines;
		ifstream file(filename);
		if (!file.is_open()) {
			cerr << lan_str(226)<< filename << endl;
			return lines;
		}
		string line;
		while (getline(file, line)) {
			lines.push_back(line);
		}
		file.close();
		return lines;
	}
	/*void _edf_load_file(){
		system("cls");
		clearInputBuffer();
		char c;
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		cout<<lan_str(223)<<"\n";
		string fn;
		getline(cin,fn);
		_ed_cpp::_ed_in_f = fn;//自动设置 
		v.clear();
		v = readFileToVector(fn);
		zz_x = 0;
		zz_y = 0;
		_ed_top = 0;
		edt_pause();
	}*/
	vector<string> splitString(const string& s, const string& delim="\n") {
	    vector<string> result;
	    size_t start = 0;
	    if(delim == "\r\n") {
	        size_t pos = 0;
	        while((pos = s.find("\r\n", start)) != string::npos){
	            result.push_back(s.substr(start, pos - start));
	            start = pos + 2; // 跳过\r\n
	        }
	        if(start <= s.size()) result.push_back(s.substr(start));
	    }
	    else { // 一般情况 \n
	        size_t pos = 0;
	        while((pos = s.find('\n', start)) != string::npos){
	            string line = s.substr(start, pos - start);
	            // 去掉行尾 \r （兼容 Windows 文本）
	            if(!line.empty() && line.back()=='\r')
	                line.pop_back();
	            result.push_back(line);
	            start = pos + 1; // 跳过\n
	        }
	        if(start <= s.size()) {
	            string last = s.substr(start);
	            if(!last.empty() && last.back()=='\r')
	                last.pop_back();
	            result.push_back(last);
	        }
	    }

	    return result;
	}
	void _edf_load_file(){
	    system("cls");
	    clearInputBuffer();
	    EdmoveTo(0,0); g_conc.SetRGBmap(135);
	    cout<<lan_str(223)<<"\n";  // 输入文件名
	    string fn;
	    getline(cin,fn);
	    _ed_cpp::_ed_in_f = fn;
	    v.clear();
	    //cout<<lan_str(242)<<"\n1.ANSI\n2.UTF8\n3.UTF16\n";
	    //int ec=0;cin>>ec;cin.ignore();
	    ifstream file(fn, ios::binary);
	    if(!file.is_open()){ cerr<<"cannot open:"<<fn<<endl; edt_pause(); return; }
	    stringstream buffer; buffer<<file.rdbuf();
	    string data = buffer.str();
	    file.close();
	    if(g_save_encoding=="ansi"){ // ANSI
	        v = splitString(data,g_save_eol=="crlf"?"\r\n":"\n");
	    }else if(g_save_encoding=="utf8"){ // UTF8
	        string ansi = Utf8ToAnsi(data);
	        v = splitString(ansi,g_save_eol=="crlf"?"\r\n":"\n");
	    }else if(g_save_encoding=="utf16"){ // UTF16
	        wstring ws;
	        ws.assign((wchar_t*)data.data(), data.size()/2);
	        string ansi = Utf16ToAnsi(ws);
	        v = splitString(ansi,g_save_eol=="crlf"?"\r\n":"\n");
	    }
	    zz_x=0;zz_y=0;_ed_top=0;
	    runHooks("onOpen",fn);
	    edt_pause();
	}
	void _edf_set_encoding(){
	    system("cls"); clearInputBuffer();
	    cout<<lan_str(242)<<"\n1.ANSI\n2.UTF8\n3.UTF16\n";
	    int ec;cin>>ec;
	    if(ec==1) g_save_encoding="ansi";
	    if(ec==2) g_save_encoding="utf8";
	    if(ec==3) g_save_encoding="utf16";
	    cout<<lan_str(243)<<"\n1.CRLF(Windows)\n2.LF(Unix)\n";
	    int e;cin>>e;
	    if(e==1) g_save_eol="crlf"; else g_save_eol="lf";
	    cout<<"result: "<<g_save_encoding<<", "<<g_save_eol<<endl;
	    edt_pause();
	}
	void _edf_about(){
		system("cls");
		clearInputBuffer();
		cout<<"CMD++\nby HeTianYu\nLanguage:C++ (MinGW64)\n";
		EdWriteUrl("GithubURL","https://github.com/hetianyu313/cmd-pp");
		cout<<" hetianyu313"<<endl;
		EdWriteUrl("Zfworld_123oj","http://123.60.188.246");
		cout<<endl;
		edt_pause();
	}
	void _edf_obfus(){
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0);
		g_conc.SetRGBmap(135);
		cout<<lan_str(223)<<"\n";
		string fn;
		getline(cin,fn);
		g_conc.SetRGBmap(78);
		cout<<lan_str(234)<<endl<<lan_str(235)<<endl;//代码混淆功能不推荐使用
		g_conc.SetRGBmap(135);
		edt_pause();cout<<"\n";
		string cmd = "\""+exedir+"tool\\CodeObfuscator.exe\" \""+_ed_cpp::_ed_in_f+"\" \""+fn+"\"";
		cout<<lan_str(227)<<cmd<<endl;
		int r = _ed_cpp::runProcess(cmd.c_str());
		cout<<lan_str(228)<<r<<endl;//result after running
		edt_pause();
	}
	void _edf_cstyle(){
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0);
		g_conc.SetRGBmap(135);
		//code path
		cout<<lan_str(223)<<endl;
		string fn;
		getline(cin,fn);
		g_conc.SetRGBmap(78);
		cout<<lan_str(236)<<endl<<lan_str(237)<<endl;//代码整理功能不推荐使用
		g_conc.SetRGBmap(135);
		edt_pause();cout<<"\n";
		string cmd = "\""+exedir+"tool\\cpp_code_nai.exe\" \""+_ed_cpp::_ed_in_f+"\" \""+fn+"\"";
		cout<<lan_str(227)<<cmd<<endl;
		int r = _ed_cpp::runProcess(cmd.c_str());
		cout<<lan_str(228)<<r<<endl;
		edt_pause();
	}
	void _edf_curto(){
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0);
		g_conc.SetRGBmap(135);
		cout<<"cursor line number:\n";
		int ln;
		cin>>ln;
		if(ln<0||(unsigned int)ln>=v.size()){
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
		//将文件复制到剪切板
		cout<<lan_str(221);
		cout<<"\n0.ansi 1.utf8 2.utf16\n";
		int ec = 0;
		cin>>ec;
		string s = "";
		for(int i = 0;i<(int)v.size();i++){
			s+=v[i];
			if((unsigned int)i!=v.size()-1)s+="\n";
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
		//已复制到剪切板
		cout<<lan_str(220)<<"\n";
		edt_pause();
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
		cout<<lan_str(208)<<endl;
		string keyword;
		getline(cin, keyword);
		if(keyword.empty()) {
			//Empty input
			cout<<lan_str(219)<<endl;
			edt_pause();
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
			edt_pause();
		}
		//edt_pause();
	}
	// 查找下一个（基于上一次关键字/位置）
	void _edf_find_next() {
		if(_last_find.empty()) {
			system("cls");
			cout <<lan_str(215)<< "\n";//No previous search. Use find first.
			edt_pause();
			return;
		}
		bool found = false;
		int fx = -1, fy = -1;
		// 从当前位置继续往后查找
		for(size_t i = _last_x; i < v.size(); i++) {
			size_t startPos = ((unsigned int)i == _last_x ? _last_y + 1 : 0);
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
			//Next found at line
			cout<<lan_str(218)<<fx<<lan_str(216)<<fy<<lan_str(217)<<fx<<endl;
		} else {
			system("cls");
			//No more matches
			cout<<lan_str(209)<<"\n";
			edt_pause();
		}
		//edt_pause();
		Sleep(200);
	}
	// 替换（一个或全部）
	void _edf_replace() {
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0); g_conc.SetRGBmap(135);
		string from, to;
		cout << lan_str(208);
		getline(cin, from);
		if(from.empty()) {
			cout <<lan_str(209) <<"\n";
			edt_pause();
			return;
		}
		cout << lan_str(207);
		getline(cin, to);
		//换一个   换全部
		cout<<lan_str(210)<<"\n"<<lan_str(211)<<"\n"<<lan_str(212)<<"\n";
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
		cout <<lan_str(213)<< cnt <<lan_str(214)<< "\n";
		edt_pause();
	}
	void _edf_bycs(){//修改参数
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0);
		g_conc.SetRGBmap(135);
		string s = _ed_cpp::_ed_yh;
		bool flag = 1;
		while(flag){
			fillline(0);fillline(1);
			EdmoveTo(0,0);
			cout<<lan_str(229)<<"\n"<<lan_str(230)<<":"<<s;
			int c = _getch();
			switch(c){
				case 8:
					if(s.size()>0) s.erase(s.size()-1);
					break;//backspace
				case 13: flag=0; break;//enter
				case 27: flag=0 ;break;//escape
				case 0:c=_getch();break;
				case 0xe0:c=_getch();break;
				default:
					s+=(char)c;
					break;
			}
		}
		_ed_cpp::_ed_yh = s;
		_ed_cpp::write_by();
		cout<<"_edf_bycs:set to "<<s<<endl;
	}
	void _edf_find_regex() {//正则查找
	    system("cls");
	    clearInputBuffer();
	    EdmoveTo(0,0); g_conc.SetRGBmap(135);
	    cout<<lan_str(238)<<"\n";
	    string pattern;
	    getline(cin, pattern);
	    if(pattern.empty()) {
	        cout <<lan_str(219)<< endl;
	        edt_pause();
	        return;
	    }
	    regex reg;
	    try {
	        reg = regex(pattern);
	    } catch(const regex_error& e) {
	        cout <<lan_str(239)<< e.what() << endl;
	        edt_pause();
	        return;
	    }
	    bool found = false;
	    for(size_t i=0; i<v.size(); i++) {
	        auto begin = sregex_iterator(v[i].begin(), v[i].end(), reg);
	        auto end = sregex_iterator();
	        if(begin != end) {
	            zz_x = (int)i;
	            zz_y = (int)begin->position();
	            if(zz_x < _ed_top) _ed_top = zz_x;
	            else if(zz_x >= _ed_top + _ed_line) _ed_top = max(0, zz_x - _ed_line/2);
	            cout <<lan_str(216)<< zz_x <<lan_str(217)<< zz_y << endl;
	            found = true;
	            Sleep(200);
	            break;
	        }
	    }
	    if(!found) {
	        cout <<lan_str(209)<< pattern << endl;
	        edt_pause();
	    }
	}
	void _edf_replace_regex() {//z正则替换
	    system("cls");
	    clearInputBuffer();
	    EdmoveTo(0,0); g_conc.SetRGBmap(135);
	    cout<<lan_str(238)<<"\n";
	    string pattern;
	    getline(cin, pattern);
	    cout <<lan_str(207)<<"\n";;
	    string replacement;
	    getline(cin, replacement);

	    regex reg;
	    try {
	        reg = regex(pattern);
	    } catch(const regex_error& e) {
	        cout << lan_str(239) << e.what() << endl;
	        edt_pause();
	        return;
	    }

	    cout <<lan_str(240)<< "\n";//mode
	    char c = _getch();
	    int cnt = 0;

	    if(c == '1') {
	        for(size_t i=0; i<v.size(); i++) {
	            smatch m;
	            if(regex_search(v[i], m, reg)) {
	                v[i].replace(m.position(), m.length(), replacement);
	                zz_x = (int)i;
	                zz_y = m.position() + replacement.size();
	                cnt = 1;
	                break;
	            }
	        }
	    } else if(c == '2') {
	        for(size_t i=0;i<v.size();i++) {
	            string replaced = regex_replace(v[i], reg, replacement);
	            if(replaced != v[i]) {
	                v[i] = replaced;
	                cnt++;
	            }
	        }
	    }
	    cout <<lan_str(213)<< cnt <<lan_str(214)<< endl;
	    edt_pause();
	}
	void _edf_escape(){
		system("cls");
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		cout<<lan_str(206)<<"\n";
		//"0.back\n1.insert\n2.exit cmd++\n3.save file\n4.load file\n5.complete\n6.complete and run\n7.about\n";
		//"8.obfuscate code\n9.move cursor\na.copy to clipboard\nb.set view of IDE\n";
		//"c.code style\nd.find first\ne.find\nf.replace\ng.set language of IDE\n";
		//"h.set compete info m.tool\n";
		cout<<lan_str(231)<<"\n";
		cout<<lan_str(232)<<"\n";
		cout<<lan_str(233)<<"\n";
		cout<<lan_str(241)<<"\n";
		cout<<lan_str(245)<<"\n";
		int c = _getch();
		switch (c){
			case '0':return;break;
			case '1':_edf_insert();break;
			case '2':exit(0);break;
			case '3':_edf_save_file();break;
			case '4':_edf_load_file();break;
			case '5':_ed_cpp::_ed_complete_file(0);break;
			case '6':_ed_cpp::_ed_test(1);break;
			case '7':_edf_about();break;
			case '8':_edf_obfus();break;
			case '9':_edf_curto();break;
			case 'a':_edf_jqb();break;
			case 'b':eview::_ev_main();break;
			case 'c':_edf_cstyle();break;
			case 'd':_edf_find();break;
			case 'e':_edf_find_next();break;
			case 'f':_edf_replace();break;
			case 'g':elang::_el_main();break;
			case 'h':_edf_bycs();break;
			case 'i': _edf_find_regex(); break;
			case 'j': _edf_replace_regex(); break;
			case 'k': _edf_set_encoding(); break;
			case 'l': _ed_cpp::_ed_test(0); break;
			case 'm': xgj::main(v); break;
		}
		g_conc.SetRGBmap(15);
		system("cls");
	}
	void put_ed_run(){
		printf("line:%d row:%d top:%d size:%d tot:%d %s %s\n",zz_x,zz_y,_ed_top,_ed_line,v.size(),g_save_encoding.c_str(),g_save_eol.c_str());
	}
	void _edf_zhcn_display(vector<_sr_py> v,int p,string ci){
		g_conc.SetRGBmap(8);
		fillline(_ed_line+1);
		fillline(_ed_line+2);
		fillline(_ed_line+3);
		fillline(_ed_line+4);
		g_ktip = lan_str(201);//home:上一页 end:下一页 escape:退出中文输入 0~9:选择 a-z:拼音输入 A-Z:直接输入
		EdmoveTo(_ed_line+1,0);
		cout<<fl<<endl<<fl<<endl;
		EdmoveTo(_ed_line+1,0);
		put_ed_run();
		cout<<g_ktip<<"\n"; 
		g_conc.SetRGBmap(112);
		EdmoveTo(_ed_line+3,0);
		cout<<fl<<"\n"<<fl<<"\n"<<fl;
		EdmoveTo(_ed_line+3,0);
		string s = (lan_str(205));
		for(int i = 0;i<10;i++){
			int l = p*10+i;
			if((unsigned int)l<v.size())s+=to_string(((i+1)%10))+"."+AnsiToUtf8(v[l].hz)+" ";
		}
		cout<<(s)<<"\n"<<lan_str(202)<<p<<"/"<<v.size()/10<<"("<<lan_str(203)<<v.size()<<")";
		cout<<lan_str(204)<<ci;
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
				if((int)c==71&&((unsigned int)page*10+1<vp.size())){//home
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
			}
			else if(c==13){
				if(vp.size()>0){ 
					for(char i : vp[0].hz){//add string to code
						_edf_addch(i);
					}
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
				if(vp.size()>(unsigned int)n){
                    string t = (vp[n].hz);
					for(unsigned char i : t){//add string to code
						_edf_addch(i);
					}
					//str+=vp[n].hz;
					vp.clear();
					ci= "";
					_ed_flash();
				}
			}
			if((unsigned int)page>vp.size()/10) page = ((int)vp.size())/10;
			if(page<=0) page=0;
			_edf_zhcn_display(vp,page,ci);
		}
		system("cls");
		_ed_flash();
	}
	void _edf_flash1(){
		system("cls");
		DWORD mode;
		GetConsoleMode(hConsole, &mode);
		SetConsoleMode(hConsole, ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hConsole, &csbi);
		clearInputBuffer();
		clearOutputBuffer(NULL);
		ResetFormat();
		SetConsoleOutputCP(CP_UTF8);
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
	string fldir = "";
	string getCurrentToken(){
	    if(zz_x < 0 || (unsigned int)zz_x >= v.size()) return "";
	    string &line = v[zz_x];
	    if((unsigned int)zz_y > line.size()) return "";
	    int start = zz_y - 1;
	    while(start >= 0 && (isalnum(line[start]) || line[start]=='_')){
	        start--;
	    }
	    start++;
	    return line.substr(start, zz_y - start);
	}
	string findCompletion(const string& prefix){
	    if(prefix.empty()) return "";
	    for(auto &kv : mp_c_hs1){
	        if(kv.first.rfind(prefix, 0) == 0){
	            return kv.first;
	        }
	    }
	    for(auto &kv : mp_c_hs2){
	        if(kv.first.rfind(prefix, 0) == 0){
	            return kv.first;
	        }
	    }
	    return "";
	}
	void doCompletion(){
	    string token = getCurrentToken();
	    string comp = findCompletion(token);
	    if(comp.empty() || comp == token) return;
	
	    string &line = v[zz_x];
	    int start = zz_y - token.size();
	    if(start < 0) start = 0;
	    line.erase(start, token.size());
	    line.insert(start, comp);
	    zz_y = start + comp.size();
	}
	HANDLE hInput;
	int _ed_runEx(){
	    INPUT_RECORD record;
	    DWORD read;
	    g_ktip = lan_str(200);
	    while (true) {
	        ReadConsoleInput(hInput, &record, 1, &read);
	        if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
	            int c = record.Event.KeyEvent.wVirtualKeyCode;
	            switch (c) {
	                case VK_UP:    _edf_up(); break;
	                case VK_DOWN:  _edf_down(); break;
	                case VK_LEFT:  _edf_left(); break;
	                case VK_RIGHT: _edf_right(); break;
	                case VK_HOME:  _edf_home(); break;
	                case VK_END:   _edf_end(); break;
	                case VK_PRIOR: _edf_pgup(); break;  // PageUp
	                case VK_NEXT:  _edf_pgdn(); break;  // PageDown
	                case VK_F1:    _edf_insert(); break;
	                case VK_F2:    _edf_zhcn(); break;
	                case VK_F3:    _edf_flash1(); break;
	                case VK_F4:    doCompletion(); break;
	                case VK_F9:    _ed_cpp::_ed_test(0); break;
	                case VK_F11:   _ed_cpp::_ed_test(1); break;
	                case VK_ESCAPE: _edf_escape(); break;
	                case VK_RETURN: _edf_enter(); break;
	                case VK_BACK:   _edf_back(); break;
	                default: {
	                    char ascii = record.Event.KeyEvent.uChar.AsciiChar;
	                    if ((ascii >= 32 && ascii <= 126)||ascii=='\t') {
	                        _edf_addch(ascii, 1);
	                    }
	                } break;
	            }
	            break;
	        }
	    }
	    _ed_flash();
	    g_view.c_line.tog();
	    fillline(_ed_line+1);
	    fillline(_ed_line+2);
	    fillline(_ed_line+3);
	    fillline(_ed_line+4);
	    EdmoveTo(_ed_line+1,0);
	    put_ed_run();
	    cout << g_ktip << " doc:" << g_doc;
	    EdmoveTo(zz_x-_ed_top, zz_y+3);
	    return 0;
	}
}
int main(){
	cout<<"main:start console c++ ide\n";
	SetConsoleTitle("Console C++ IDE");
	_ed_code::hInput = GetStdHandle(STD_INPUT_HANDLE);
	_ed_code::hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	_ed_code::init();
	_ed_cpp::init();
	cout<<"main:display system info\n";
	system((exedir+"\\tool\\editorsys.exe").c_str());
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	cwh::init();
	eview::init();
	srf::init();
	_ed_code::_edf_nopaste();
	_ed_code::load_docmap();
	load_hooks();
	elang::init();
	xgj::init();
	cout<<"main:vt support = "<<g_vt_support<<endl;
	if(g_vt_support==0){
		cout<<lan_str(244)<<endl;
	}
	PrintGradientText("main:init all end\n",217,192,80,53,212,217);
	PrintGradientText("Console C++ IDE by hetianyu313\n",217,192,80,53,212,217);
	g_view.c_line.tog();
	edt_pause();
	system("cls");
	//Sleep(500);
	_ed_code::_ed_flash();
	while(1){
		_ed_code::_ed_runEx();
	}
	return 0;
}

