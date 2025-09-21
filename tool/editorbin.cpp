#include<bits/stdc++.h>
#include"editorfile.cpp"
#include<graphics.h>
#include<windows.h>
#define uint32 unsigned int
using namespace std;
fileread fr;
const int wd_h = 400,wd_w = 700;
int tgmode = 1;//0-b&w 1-color
int szone = 128*128;
int szsq = 128;
int szsf = 3;
int slpt = 500;
int main(){
	SetConsoleTitle("Binary Display Tool");
	cout<<"Input time of delay(ms):\n";
	cin>>slpt;
	cout<<"Input mode(1=color 0=B&W):\n";
	cin>>tgmode;
	HWND hwnd = initgraph(wd_w,wd_h,SHOWCONSOLE);
	settextstyle(24,0,"Arial");
	outtextxy(0,0,"Binary Display Tool");
	SetWindowText(hwnd,"Binary Display Tool");
	fr.winopen(1,(char*)"所有文件\0*\0\0");
	int sz = fr.length();
	int fc;
	if(tgmode==0)fc = ceil((double)sz/szone);
	else fc = ceil((double)sz/(szone*3));
    IMAGE img;
    img.Resize(wd_w,wd_h);
    string fn = fr.getpath().substr(fr.getpath().find_last_of("\\")+1);
	cleardevice();
    for(int i = 0;i<=50;i++){
    	char s[256];
    	memset(s,0,sizeof s);
    	sprintf(s,"Binary display will start in %.1fs        ",5.0-i/10.0);
		outtextxy(0,0,s);
		Sleep(100);
	}
	cleardevice();
	uint32 st = GetTickCount(),lt;
	for(int nf = 0;nf<fc;nf++){
		lt = st;
		vector<char> v = fr.readsiz((size_t)szone*(tgmode==0 ? 1 : 3));
        if (v.size() < szone) v.resize(szone,'\0');
        SetWorkingImage(&img);
		setfillcolor(RGB(0,0,0));solidrectangle(0,0,wd_w-1,wd_h-1);
		if(tgmode==0){
			for(int x = 0;x<szsq;x++){
				for(int y = 0;y<szsq;y++){
					int byt = v[x*szsq+y];
					setfillcolor(RGB(byt,byt,byt));
					solidrectangle(y*szsf,x*szsf,y*szsf+szsf-1,x*szsf+szsf-1);
				}
			}
		}
		else{
			for(int x = 0;x<szsq;x++){
				for(int y = 0;y<szsq;y++){
					int b1 = v[x*szsq*3+y*3];
					int b2 = v[x*szsq*3+y*3+1];
					int b3 = v[x*szsq*3+y*3+2];
					setfillcolor(RGB(b1,b2,b3));
					solidrectangle(y*szsf,x*szsf,y*szsf+szsf-1,x*szsf+szsf-1);
				}
			}
		}
		settextstyle(24,0,"Arial");
		string s;
		s = "Binary Display Tool";
		outtextxy(szsq*szsf+10,0,s.c_str());
		s = "File:"+fn;
		outtextxy(szsq*szsf+10,30,s.c_str());
		s = "Frame:"+to_string(nf+1)+"/"+to_string(fc);
		outtextxy(szsq*szsf+10,60,s.c_str());
		s = "Size:"+to_string(min((nf+1)*szone*(tgmode==0?1:3),sz))+"/"+to_string(sz)+"B";
		outtextxy(szsq*szsf+10,90,s.c_str());
		s = "导出方式:逐字节拼接";
		outtextxy(szsq*szsf+10,wd_h-90,s.c_str());
		s = (tgmode==0) ? "解码方式:灰度(0-255)" : "解码方式:RGB(0-255)";
		outtextxy(szsq*szsf+10,wd_h-60,s.c_str());
		s = "读取方式:自上而下,从左往右";
		outtextxy(szsq*szsf+10,wd_h-30,s.c_str());
		st = GetTickCount();
		uint32 nt = st-lt;
		uint32 ut = max((int)slpt-(int)nt-5,(int)0);
		uint32 rate = (uint32)((szone*(tgmode==0 ? 1 : 3))/(nt/1000.0))>>10;
		s = "Speed:"+to_string(rate)+"KB/s";
		outtextxy(szsq*szsf+10,120,s.c_str());
		SetWorkingImage(NULL);
		putimage(0,0,&img);
		if(ut)Sleep(ut);
		st+=ut;
	}
	cout<<"Binary display ended\n";
	Sleep(200);
	cleardevice();
	settextstyle(24,0,"Arial");
	outtextxy(0,0,"End");
	cout<<"Press enter to exit\n";
	getchar();getchar();
	return 0;
}

