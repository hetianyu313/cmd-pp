#include<bits/stdc++.h>
#include<windows.h>
using namespace std;
int main(){
	system("pause");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for(int i = 0;i<256;i++){
		if(i%16==0){
			SetConsoleTextAttribute(hConsole,0);
			cout<<endl;
		}
        SetConsoleTextAttribute(hConsole,i);
		printf(" >%03d< ",i);
	}
    SetConsoleTextAttribute(hConsole,15);
	cout<<endl;
	system("pause");
	return 0;
}

