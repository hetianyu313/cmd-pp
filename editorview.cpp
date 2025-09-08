#include<bits/stdc++.h>
#include"editorcpp.cpp"
#include"editorapi.cpp"
#include"editorrgb.cpp"
#pragma once
using namespace std;
//editorview.cpp
#define cmdcol unsigned short
struct cmdcolEx{
	short r,g,b;//front
	short r1,g1,b1;//background
	int col24;
	bool sup_24=0;
	void tog(){
		if(sup_24){
			g_conc.SetRGB(r,g,b,0);
			g_conc.SetRGB(r1,g1,b1,1);
		}
		else{
			g_conc.SetRGBmap(col24);
		}
	}
};
cmdcolEx input_cmdcolEx(ifstream &ifs){
	int a;
	ifs>>a;
	if(a==-1){
		cmdcolEx rs;
		ifs>>rs.r>>rs.g>>rs.b>>rs.r1>>rs.g1>>rs.b1;
		rs.sup_24=1;
		rs.col24=15;
		printf("input_cmdcolEx:sup24 %d %d %d-%d %d %d\n",rs.r,rs.g,rs.b,rs.r1,rs.g1,rs.b1);
		return rs;
	}
	else{
		cmdcolEx rs;
		rs.sup_24=0;
		rs.col24=a;
		printf("input_cmdcolEx:not sup24 %d\n",rs.col24);
		return rs;
	}
}
class _ed_view{
public:
	cmdcolEx c_func;//1-
	cmdcolEx c_type;//2-
	cmdcolEx c_line;//3-
	cmdcolEx c_code;//4-
	cmdcolEx c_str;//5-
	cmdcolEx c_zs;//6-
	cmdcolEx c_def;//7-
	cmdcolEx c_sign;//8-
	cmdcolEx c_num;//9-
	bool b_func;
	bool b_type;
	bool b_zs;
	bool i_func;
	bool i_type;
	bool i_zs;
	bool u_func;
	bool u_type;
	bool u_zs;
	string v_name;
	string v_auth;
	string v_about;
};
_ed_view g_view = {
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
		/*ifs>>g_view.c_func>>
		g_view.c_type>>
		g_view.c_line>>
		g_view.c_code>>
		g_view.c_str>>
		g_view.c_zs>>
		g_view.c_def>>
		g_view.c_sign>>
		g_view.c_num;*/
		g_view.c_func=input_cmdcolEx(ifs);
		g_view.c_type=input_cmdcolEx(ifs);
		g_view.c_line=input_cmdcolEx(ifs);
		g_view.c_code=input_cmdcolEx(ifs);
		g_view.c_str=input_cmdcolEx(ifs);
		g_view.c_zs=input_cmdcolEx(ifs);
		g_view.c_def=input_cmdcolEx(ifs);
		g_view.c_sign=input_cmdcolEx(ifs);
		g_view.c_num=input_cmdcolEx(ifs);
		//getchar();
		string tmp = "";
		getline(ifs,tmp);
		getline(ifs,g_view.v_name);
		getline(ifs,g_view.v_auth);
		getline(ifs,g_view.v_about);
		ifs>>g_view.b_func>>g_view.b_type>>g_view.b_zs;//加粗设置
		ifs>>g_view.i_func>>g_view.i_type>>g_view.i_zs;//斜体设置
		ifs>>g_view.u_func>>g_view.u_type>>g_view.u_zs;//下划线设置
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
/*
testing code:
"string"
{};
+ - * / ^ | &
//zhushi
sort((()));
return 0;
0xff
#define mop int
string s;
*/
