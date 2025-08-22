#include<bits/stdc++.h>
#define cmdcol unsigned short
#include"editorcpp.cpp"
#include"editorapi.cpp"
#include"editorrgb.cpp"
#pragma once
using namespace std;
//editorview.cpp
class _ed_view{
public:
	cmdcol c_func;//1-
	cmdcol c_type;//2-
	cmdcol c_line;//3-
	cmdcol c_code;//4-
	cmdcol c_str;//5-
	cmdcol c_zs;//6-
	cmdcol c_def;//7-
	cmdcol c_sign;//8-
	cmdcol c_num;//9-
	string v_name;
	string v_auth;
	string v_about;
};
_ed_view g_view = {
	7,9,7,15,1,3,2,12,5,
	"Classic Plus",
	"__builtin__",
	"Dev-C++ 5.11"
};//builtin view
string exedir_get(){
	return exedir;
}
namespace eview{
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
	string _ev_getmrview(){
		string fn = exedir_get()+"setting\\view_mr.ini";
		cout<<"load from:"<<fn<<endl;
		ifstream ifs(fn.c_str());
		string s;
		getline(ifs,s);
		ifs.close();
		cout<<"_ev_getmrview:result="<<s<<"\n";
		return s;
	}
	void _ev_setmrview(string s){
		string fn = exedir_get()+"setting\\view_mr.ini";
		cout<<"load from:"<<fn<<endl;
		ofstream ofs(fn.c_str());
		ofs.clear();
		ofs<<s;
		ofs.close();
		cout<<"_ev_setmrview:set mr view to "<<s<<"\n";
		return ;
	}
	void _ev_loadview(string s){
		string fn = exedir_get()+"setting\\"+s+".view";
		cout<<"load from:"<<fn<<endl;
		ifstream ifs(fn.c_str());
		ifs>>g_view.c_func>>
		g_view.c_type>>
		g_view.c_line>>
		g_view.c_code>>
		g_view.c_str>>
		g_view.c_zs>>
		g_view.c_def>>
		g_view.c_sign>>
		g_view.c_num;
		//getchar();
		string tmp = "";
		getline(ifs,tmp);
		getline(ifs,g_view.v_name);
		getline(ifs,g_view.v_auth);
		getline(ifs,g_view.v_about);
		ifs.close();
		cout<<"_ev_loadview:loaded view file\n";
	}
	int init(){
		string s = _ev_getmrview();
		cout<<"eview.init:view="<<s<<endl;
		_ev_loadview(s);
		cout<<"eview.init:loaded view file\n";
		return 0;
	}
	void _ev_main(){
		clearInputBuffer();
		system("cls");
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		string s = "";
		while(1){
			cout<<lan_str(400)<<"\n0.exit\n1.set IDE view\n2.help\n3.now view\n";
			cin>>s;
			if(s=="0"){
				cout<<"_ev_main:return to codemain\n";
				return ;
			}
			else if(s=="1"){
				cout<<lan_str(401)<<"\n";
				vector<string> v = GetFilesWithExtension((exedir_get()+"setting"),".view");
				for(string i : v){
					cout<<" - "<<i<<endl;
				}
				cout<<lan_str(405)<<":";
				string t = "";
				//cin>>t;
				clearInputBuffer();
				getline(cin,t);
				_ev_setmrview(t);
				cout<<"_ev_main:reload view\n";
				init();
			}
			else if(s=="2"){
				cout<<lan_str(402)<<"\n";
				cout<<lan_str(403)<<"\n";
			}
			else if(s=="3"){
				cout<<lan_str(404)<<"\n";
				cout<<"name:"<<g_view.v_name<<endl;
				cout<<"auth:"<<g_view.v_auth<<endl;
				cout<<"about:"<<g_view.v_about<<endl;
			}
			else return;
		}
	}
}

