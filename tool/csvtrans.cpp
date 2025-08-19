#include<bits/stdc++.h>
using namespace std;
int main(){
	ifstream ifs("pinyin.csv");
	ofstream ofs("pinyin.ini");
	string s;
	int cnt = 0;
	while(1){
		getline(ifs,s);
		if(s=="")break;
		string ci="",py="";
		int i = 0;
		//cout<<"s:"<<s<<endl;
		for(;i<(int)s.size();i++){
			char c = s[i];
			if(c!=',') ci+=c;
			else break;
		}
		//cout<<ci<<endl;
		i++;
		for(;i<(int)s.size();i++){
			char c = s[i];
			if(c!='/') py+=c;
			if(i==s.size()-1){
				ofs<<ci<<" "<<py<<"\n";
				//cout<<ci<<" "<<py<<"\n";
				py = "";
			}
			else if(c=='/'){
				ofs<<ci<<" "<<py<<"\n";
				py = "";			
			}
		}
		if(cnt%100==0) cout<<cnt<<endl;
		cnt++;
	}
	return 0;
}

