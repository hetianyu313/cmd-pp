#include"editorapi.cpp"
namespace elang{
	std::vector<std::string> GetFilesWithExtension(const std::string& folderPath, const std::string& extension) {
	    std::vector<std::string> files;
	    WIN32_FIND_DATAA findData;
	    HANDLE hFind;
	    
	    std::string searchPath = folderPath + "\\*" + extension;
	    hFind = FindFirstFileA(searchPath.c_str(), &findData);
	    
	    if (hFind != INVALID_HANDLE_VALUE) {
	        do {
	            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
	                files.push_back(findData.cFileName);
	            }
	        } while (FindNextFileA(hFind, &findData));
	        FindClose(hFind);
	    }
	    
	    return files;
	}
	string _el_getmrlang(){
		string fn = exedir_get()+"setting\\lang_mr.ini";
		cout<<"load from:"<<fn<<endl;
		ifstream ifs(fn.c_str());
		string s;
		getline(ifs,s);
		ifs.close();
		cout<<"_el_getmrlang:result="<<s<<"\n";
		return s;
	}
	void _el_setmrlang(string s){
		string fn = exedir_get()+"setting\\lang_mr.ini";
		cout<<"load from:"<<fn<<endl;
		ofstream ofs(fn.c_str());
		ofs.clear();
		ofs<<s;
		ofs.close();
		cout<<"_el_setmrlang:set mr view to "<<s<<"\n";
		return ;
	}
	int init(){
		string s = _el_getmrlang();
		langMap.clear(); 
		cout<<"elang.init:view="<<s<<endl;
		load_language(s);
		cout<<"elang.init:loaded language file\n";
		return 0;
	}
	void _el_main(){
		clearInputBuffer();
		system("cls");
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		string s = "";
		while(1){
			g_conc.SetRGBmap(135);
			cout<<lan_str(500)<<"\n0.exit\n1.set language\n2.help\n3.now language\n";
			cin>>s;
			if(s=="0"){
				cout<<"_el_main:return to codemain\n";
				return ;
			}
			else if(s=="1"){
				cout<<lan_str(501)<<"\n";
				vector<string> v = GetFilesWithExtension((exedir_get()+"setting"),".lang");
				for(string i : v){
					cout<<" - "<<i<<endl;
				}
				cout<<lan_str(505)<<":";
				string t = "";
				//cin>>t;
				clearInputBuffer();
				getline(cin,t);
				_el_setmrlang(t);
				cout<<"_el_main:reload language\n";
				init();
			}
			else if(s=="2"){
				cout<<lan_str(502)<<"\n";
				cout<<lan_str(503)<<"\n";
			}
			else if(s=="3"){
				cout<<lan_str(504)<<"\n";
				cout<<"name:"<<lan_str(101)<<endl;
				cout<<"local name:"<<lan_str(100)<<endl;
				cout<<"about:"<<lan_str(102)<<endl;
			}
			else return;
		}
	}
}

