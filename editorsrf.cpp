#include<bits/stdc++.h>
#pragma once
#include"editorapi.cpp"
#include"editorrgb.cpp"
#include"editorchar.cpp"
#include"editorcpp.cpp"
#include"editorview.cpp"
using namespace std;
struct _sr_py{
	string hz;//ºº×Ö 
	string py;//Æ´Òô 
};
namespace srf{
	vector<_sr_py> py;
	vector<_sr_py> py_near(string s){
		vector<_sr_py> v;
		for(_sr_py i : py){
			if(i.py==s){
				v.push_back(i);
			}
		}
		return v;
	}
	int init(){
		cout<<"srf.init:load py\n";
		py.clear();
		string fn = exedir_get()+"setting\\pinyin.ini";//coding:ANSI
		cout<<"srf.init:load from "<<fn<<"\n";
		ifstream ifs(fn.c_str());
		string a="",b="";
		int cnt = 0;
		while(1){
			ifs>>a;
			if(a=="stop") break;
			ifs>>b;
			py.push_back({a,b});
			cnt++;
		}
		cout<<"srf.init:loaded "<<cnt<<" keys\n";
	}
}
