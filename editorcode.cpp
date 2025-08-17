#include<bits/stdc++.h>
#include<conio.h>
#include<windows.h>
#include"editorapi.cpp"
#include"editorcpp.cpp"
#include"editorrgb.cpp"
using namespace std;
namespace _ed_code{
	int _ed_width,_ed_height;
	int _ed_top = 0;//顶端的行数 
	int _ed_line = 0;//界面行数 
	int zz_x = 0,zz_y = 0;//指针行\列 
	vector<string> v = {""};
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
	{"wchar_t",1},{"wstring",1},
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
	};
	unordered_map<char,bool> mp_c_qh = {
	{'+',1},{'-',1},{'*',1},{'/',1},{'%',1},{'^',1},{'|',1},{'{',1},{'}',1},{'[',1},
	{']',1},{'(',1},{')',1},{'&',1},{'!',1},{'~',1},{'<',1},{'>',1},{',',1},{'.',1},
	{'=',1},{';',1},{':',1},
	};
	
	void _ed_outcol(string s,bool &f_dhzs){
		bool f_define = 0;
		bool f_str = 0;
		bool f_zs = 0;
		char lc = 0;
		string gjz = "";
		int cnt = 0;
		if(f_dhzs) g_conc.SetRGBmap(0x03);
		for(char c : s){
			bool no_putchar_this = 0;
			if(f_dhzs){
				if(lc=='*'&&c=='/'){
					f_dhzs = 0;
					g_conc.SetRGBmap(0x0f);
				}
				else{
					no_putchar_this = 1;
					putchar(c);
				}
			}
			else if(!f_dhzs && !f_str && !f_zs && lc=='/' && c=='*'){
				f_dhzs = (bool)1;
				g_conc.SetRGBmap(0x03);
				no_putchar_this = 1;
				putchar(c);
			}
			else if(!f_define && !f_str && !f_zs && !f_dhzs && c=='#' && lc!='\''){
				f_define = 1;
				g_conc.SetRGBmap(0x02);
			}
			else if(!f_define && !f_zs && !f_dhzs && lc!='\\' && c=='\"'){
				f_str = !f_str;
				if(!f_str){
					no_putchar_this = 1;
					putchar(c);	
				}
				g_conc.SetRGBmap(f_str ? 0x01 : 0x0f);
			}
			else if(!f_define && !f_str && !f_zs && !f_dhzs && lc=='/' && c=='/'){
				no_putchar_this = 1;
				g_conc.SetRGBmap(0x03);
				COORD pos = EdgetPosition();
				EdmoveTo(pos.Y,pos.X-1);
				putchar('/');
				putchar('/');
				f_zs = 1;
			}
			if(!f_define && !f_str && !f_zs && !f_dhzs){
				if(mp_c_qh[c]==1||c==' '||cnt==s.size()-1){
					if(mp_c_hs1[gjz]&&gjz.size()>0){
						//EdmoveLeft(gjz.size());
						COORD pos = EdgetPosition();
						//cerr<<"pos x="<<pos.X<<" y="<<pos.Y<<" sz="<<gjz.size();
						EdmoveTo(pos.Y,pos.X-gjz.size());
						g_conc.SetRGBmap(0x09);
						for(char c : gjz){
							putchar(c);
						}
						g_conc.SetRGBmap(0x0F);
					}
					else if(mp_c_hs2[gjz]&&gjz.size()>0){
						//EdmoveLeft(gjz.size());
						COORD pos = EdgetPosition();
						//cerr<<"pos x="<<pos.X<<" y="<<pos.Y<<" sz="<<gjz.size();
						EdmoveTo(pos.Y,pos.X-gjz.size());
						g_conc.SetRGBmap(0x07);
						for(char c : gjz){
							putchar(c);
						}
						g_conc.SetRGBmap(0x0F);
					}
					if(mp_c_qh[c]){
						g_conc.SetRGBmap(0x0C);
					}
					gjz = "";
				}
				else{
					g_conc.SetRGBmap(0x0f);
					gjz+=c;
				}
			} 
			lc = c;//last char
			cnt++;
			if(!no_putchar_this)putchar(c);
		}
	}
	void _ed_flash(){
		system("cls");
		EdmoveTo(0,0);
		bool f_dhzs = 0;
		for(int i = 0;i<=_ed_line;i++){
			int cl = i+_ed_top;
			if(v.size()-1<cl)continue;
			EdmoveTo(i,0);
			g_conc.SetRGBmap(7);
			printf("%-3d",cl);
			g_conc.SetRGBmap(15);
			//cout<<v[cl]<<endl;
			_ed_outcol(v[cl],f_dhzs);
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
	void _edf_enter(){
		string s = v[zz_x].substr(zz_y);
		v[zz_x].erase(v[zz_x].begin()+zz_y,v[zz_x].end());
		v.insert(v.begin()+zz_x+1,(string)s);
		zz_x++;
		zz_y = 0; 
		if(zz_x>_ed_top+_ed_line) _ed_top = zz_x-_ed_line;
	}
	void _edf_addch(char c){
		if(c=='\t'){
			v[zz_x].insert(v[zz_x].begin()+zz_y,32);zz_y++;
			v[zz_x].insert(v[zz_x].begin()+zz_y,32);zz_y++;
			v[zz_x].insert(v[zz_x].begin()+zz_y,32);zz_y++;
			v[zz_x].insert(v[zz_x].begin()+zz_y,32);zz_y++;
			//cout<<"ssss\n";
		}
		else {
			v[zz_x].insert(v[zz_x].begin()+zz_y,c);zz_y++;
		}
	}
	void _edf_back() {
		// 安全校验
		if(v.empty() || zz_x >= v.size()) return;
	    // 情况1：当前行有字符可删除（普通退格）
	    if(zz_y > 0) {
	        v[zz_x].erase(zz_y-1, 1);  // 删除前一个字符
	        zz_y--;
	    }
	    // 情况2：行首且不是第一行（跨行退格）
	    else if(zz_x > 0) {
	        int prev_len = v[zz_x-1].size();  // 保存上一行长度
	        v[zz_x-1] += v[zz_x];      // 合并两行内容
	        v.erase(v.begin()+zz_x);    // 删除当前行
	        // 更新光标位置
	        zz_x--;
	        zz_y = prev_len;
	        // 视口调整逻辑
	        if(zz_x < _ed_top) {
	            _ed_top = max(0, zz_x - _ed_line/2);  // 保持光标在视口中央
	        }
	    }
	    // 情况3：第一行行首（无操作）
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
			else if(c==27||c==0 ||c==0xE0)break;
			else if(c=='\t'){
				s+="    "; 
			}
			else{
				s+=c;
			}
			printf("input %d bytes\n",cnt);
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
	std::vector<std::string> readFileToVector(const std::string& filename) {
	    std::vector<std::string> lines;
	    std::ifstream file(filename);
	    
	    if (!file.is_open()) {
	        std::cerr << "Error opening file: " << filename << std::endl;
	        return lines;
	    }
	    
	    std::string line;
	    while (std::getline(file, line)) {
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
	void _eff_about(){
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
		cout<<"     这是最后一次提醒,你可以关闭窗口     \n";
		g_conc.SetRGBmap(135);
		system("pause");
		string cmd = "\""+_ed_cpp::exedir+"tool\\CodeObfuscator.exe\" \""+_ed_cpp::_ed_in_f+"\" \""+fn+"\"";
		cout<<"命令:"<<cmd<<endl;
		int r = _ed_cpp::runProcess(cmd.c_str());
		cout<<"执行结果:"<<r<<endl;
		system("pause");
	}
	void _edf_escape(){
		system("cls");
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		cout<<"选择完成后如果没有反应按一次回车,部分功能需要保存后使用\n";
		cout<<"0.exit\n1.insert\n2.leave cmd++\n3.save file\n4.load file\n5.complete\n6.complete and run\n7.about\n";
		cout<<"8.obfuscate code\n";
		int c = _getch();
		switch (c){
			case '0':return;break;
			case '1':_edf_insert();break;
			case '2':exit(0);break;
			case '3':_edf_save_file();break;
			case '4':_edf_load_file();break;
			case '5':_ed_cpp::_ed_complete_file(0);break;
			case '6':_ed_cpp::_ed_complete_file(1);break;
			case '7':_eff_about();break;
			case '8':_edf_obfus();break;
		}
		g_conc.SetRGBmap(15);
		system("cls");
	}
	int _ed_run(){ 
	    int c = _getch(); 
	    //cout<<c<<endl;
	    if(zz_x<0||zz_x>v.size()){
	    	zz_x = _ed_top;
		}
		else{
		    if(c == 0 || c == 0xE0){
		        c = _getch();
		    	//cout<<"="<<c<<endl;
		        switch(c){
		            case 72: _edf_up(); break;    // VK_UP
		            case 80: _edf_down(); break;  // VK_DOWN
		            case 75: _edf_left(); break;  // VK_LEFT
		            case 77: _edf_right(); break; // VK_RIGHT
		            case 71: _edf_home(); break; // home
		            case 79: _edf_end(); break;  // end
		            case 0x49: _edf_pgup(); break; // pgup
		            case 0x51: _edf_pgdn(); break;  // pgdn
		            case 0x3b: _edf_insert(); break;  // f1
		        }
		    }
		    else {
		        switch(c){
		            case 8: _edf_back(); break;  //backspace
		            case 13: _edf_enter(); break;//enter
		            case 27: _edf_escape(); break;     //escape
		            default: _edf_addch(c); break;
		        }
		    }			
		}

	    _ed_flash();
	    g_conc.SetRGB(255,128,64);
	    EdmoveTo(_ed_line+1,0);
	    printf("_ed_run zz:%d,%d top:%d line:%d vs:%d",zz_x,zz_y,_ed_top,_ed_line,v.size());
	    EdmoveTo(zz_x-_ed_top,zz_y+3);
	    return 0;
	}
}
int main(){
	_ed_code::init();
	_ed_cpp::init();
	_ed_code::_ed_flash();
	while(1){
		_ed_code::_ed_run();
	}
	return 0;
}

