#include <bits/stdc++.h>
#include <Lmcons.h>
#include "editorapi.cpp"
#include "editorchar.cpp"
#include "editorview.cpp"
#include "tool\editorfile.cpp"

//editorxgj.cpp 2025.09
using namespace std;
namespace xgj{
	int getrandyq(int YEAR, int MONTH, int DAY, const string &S){//HASH
	    string key = to_string(YEAR)+ "-" + to_string(MONTH)+ "-" + to_string(DAY)+ "-" + S;
	    unsigned long hash = 5381;
	    for (char c : key){
	        hash = ((hash<<5)+ hash)+ (unsigned char)c;
	    }
	    return (int)(hash % 32768);
	}
	struct Date{
	    int year;
	    int month;
	    int day;
	};
	Date GetDate(){
	    SYSTEMTIME st;
	    GetLocalTime(&st);
	    Date d;
	    d.year  = st.wYear;
	    d.month = st.wMonth;
	    d.day   = st.wDay;
	    return d;
	}
	std::string GetUserNameString(){
	    wchar_t buffer[UNLEN + 1];
	    DWORD size = UNLEN + 1;
	    if (GetUserNameW(buffer, &size)){
	        return Utf16ToUtf8(std::wstring(buffer));//utf8
	    }else{
	        return "Unknown";
	    }
	}
	struct ys_item{
		string a;
		string b;
	};
	vector<ys_item> uy1;
	vector<ys_item> uy2;
	int uy_init_1(){
		string fn = exedir_get()+"setting\\yunshi1.ini";
		cout<<"load from:"<<fn<<endl;
		ifstream ifs(fn.c_str());
		string a,b;
		vector<ys_item> v;
		while(1){
			getline(ifs,a);
			if(a=="")break;
			getline(ifs,b);
			v.push_back({a,b});
		}
		ifs.close();
		uy1.clear();
		uy1 = v;
		cout<<"uy_init_1:items="<<v.size()<<"\n";
		return 0;
	}
	int uy_init_2(){
		string fn = exedir_get()+"setting\\yunshi2.ini";
		cout<<"load from:"<<fn<<endl;
		ifstream ifs(fn.c_str());
		string a,b;
		vector<ys_item> v;
		while(1){
			getline(ifs,a);
			if(a=="")break;
			getline(ifs,b);
			v.push_back({a,b});
		}
		ifs.close();
		uy2.clear();
		uy2 = v;
		cout<<"uy_init_2:items="<<v.size()<<"\n";
		return 0;
	}
	struct resultys{
	    vector<ys_item> yi; // 宜
	    vector<ys_item> ji; // 忌
	};
	resultys get_yunshi_items(int mode){
	    Date dt = GetDate();
	    string uname = GetUserNameString();
	    auto nextRand = [&](int idx){
	        return getrandyq(dt.year,dt.month,dt.day,uname+"_"+to_string(idx*(dt.day%10+1))+"-"+to_string((int)(dt.month*131+idx+dt.year*129+dt.day)%256));
	    };
	    resultys res;
	    if(mode!=1){ // 需要抽取宜
	        unordered_set<int> used;
	        for(int k=0;k<2 &&!uy1.empty();k++){
	            int r;
	            do{
	                r = nextRand(k)% uy1.size();
	            }while(used.count(r));
	            used.insert(r);
	            res.yi.push_back(uy1[r]);
	        }
	    }
	    if(mode!=2){ // 需要抽取忌
	        unordered_set<int> used;
	        for(int k=0;k<2 &&!uy2.empty();k++){
	            int r;
	            do{
	                r = nextRand(100+k)% uy2.size();
	            }while(used.count(r));
	            used.insert(r);
	            res.ji.push_back(uy2[r]);
	        }
	    }
	    return res;
	}
	int u_yunshi(){
		clearInputBuffer();
		system("cls");
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		string s = GetUserNameString()+lan_str(602);
		cout<<s<<endl;
		Date dt = GetDate();
		cout<<dt.year<<"-"<<dt.month<<"-"<<dt.day<<endl;
		int rand_ys = getrandyq(dt.year,dt.month,dt.day,GetUserNameString())%100;
		int ys_tp = -1, ys_mode = 0;
		if(rand_ys<=8){
			g_conc.SetRGBmap(244);
			ys_tp = 0;
			ys_mode = 1;
		}
		else if(rand_ys<=20){
			g_conc.SetRGBmap(252);
			ys_tp = 1;
		}
		else if(rand_ys<=35){
			g_conc.SetRGBmap(252);
			ys_tp = 2;
		}
		else if(rand_ys<=50){
			g_conc.SetRGBmap(248);
			ys_tp = 3;
		}
		else if(rand_ys<=65){
			g_conc.SetRGBmap(242);
			ys_tp = 4;
		}
		else if(rand_ys<=80){
			g_conc.SetRGBmap(242);
			ys_tp = 5;
		}
		else if(rand_ys<=92){
			g_conc.SetRGBmap(250);
			ys_tp = 6;
		}
		else{
			g_conc.SetRGBmap(251);
			ys_tp = 7;
			ys_mode = 2;
		}
		cout<<lan_str(603)<<" "<<lan_str(690+ys_tp)<<" "<<lan_str(603);
		g_conc.SetRGBmap(135);
		cout<<endl;
		//put yunshi
        resultys r = get_yunshi_items(ys_mode);
        g_conc.SetRGBmap(139);
        if(ys_mode==1){
        	cout<<lan_str(606)<<":"<<lan_str(605)<<"\n\n\n\n";
		}
		else{
			for(ys_item i : r.yi){
				cout<<lan_str(606)<<":"<<i.a<<"\n   ";
				TogItalic(1);
				cout<<i.b<<endl;
				TogItalic(0);
			}
		}
		g_conc.SetRGBmap(128);
        if(ys_mode==2){
        	cout<<lan_str(607)<<":"<<lan_str(605)<<"\n\n\n\n";//unuse 604
		}
		else{
			for(ys_item i : r.ji){
				cout<<lan_str(607)<<":"<<i.a<<"\n   ";
				TogItalic(1);
				cout<<i.b<<endl;
				TogItalic(0);
			}
		}
		g_conc.SetRGBmap(135);
		edt_pause();
	}
	void init(){
		cout<<"xgj.init:init xgj\n";
		uy_init_1();
		uy_init_2();
		cout<<"xgj.init:init end\n";
	}
	struct FileStats{
        int totalLines = 0;
        int codeLines = 0;
        int commentLines = 0;
        int emptyLines = 0;
    };
    bool isComment(const string &line){
        string s = line;
        // 去掉前导空格
        s.erase(0, s.find_first_not_of(" \t\r\n"));
        if(s.empty())return false;
        return (s.rfind("//",0)==0 ||s.rfind("/*",0)==0 ||s.rfind("*",0)==0);
    }
    FileStats analyzeOpenFile(const vector<string> &ed_lines){
        FileStats stat;
        auto &vec = ed_lines; // 当前缓存在编辑器里的行
        for(auto &line : vec){
            stat.totalLines++;
            string t = line;
            if(!t.empty()&&t.back()=='\r')t.pop_back();
            if(t.empty()||all_of(t.begin(),t.end(),::isspace)){
                stat.emptyLines++;
            }else if(isComment(t)){
                stat.commentLines++;
            }else{
                stat.codeLines++;
            }
        }
        return stat;
    }
    void u_codestats_file(const vector<string> &ed_lines){
        clearInputBuffer();
        system("cls");
        EdmoveTo(0,0); g_conc.SetRGBmap(135);
        cout<<lan_str(608)<<endl;//unuse 609
        FileStats s = analyzeOpenFile(ed_lines);
        g_conc.SetRGBmap(139);
        cout<<lan_str(610)<<s.totalLines<<endl;
        cout<<lan_str(611)<<s.codeLines<<endl;
        cout<<lan_str(612)<<s.commentLines<<endl;
        cout<<lan_str(613)<<s.emptyLines<<endl;
        g_conc.SetRGBmap(135);
        edt_pause();
    }
    // ========== AES 128 ECB 实现 ==========
    // S盒
    static const uint8_t sbox[256] = {
	 0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
	 0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
	 0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
	 0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
	 0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
	 0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
	 0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
	 0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
	 0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
	 0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
	 0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
	 0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
	 0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
	 0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
	 0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
	 0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
	};
	static const uint8_t rsbox[256] = {
	 0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
	 0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
	 0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
	 0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
	 0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
	 0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
	 0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
	 0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
	 0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
	 0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
	 0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
	 0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
	 0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
	 0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
	 0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
	 0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
	};
    static const uint8_t Rcon[11] = {
        0x8d,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36
    };
    // AES参数
    const int Nb = 4;  // 每组4列
    const int Nk = 4;  // key 4词 = 16字节
    const int Nr = 10; // AES-128 共10轮
    using state_t = uint8_t[4][4];
    vector<uint8_t> RoundKey;
    uint8_t Key[16];
    // ========= 工具函数 ==========
    uint8_t xtime(uint8_t x) {
        return (uint8_t)((x<<1) ^ (((x>>7) & 1) * 0x1b));
    }
    void KeyExpansion() {
        RoundKey.resize(176);
        int i=0;
        while(i < Nk*4){
            RoundKey[i] = Key[i];
            i++;
        }
        uint8_t temp[4];
        while(i < Nb*(Nr+1)*4){
            for(int j=0;j<4;j++) temp[j] = RoundKey[(i-4)+j];
            if(i % (Nk*4) == 0){
                // RotWord
                uint8_t t = temp[0];
                temp[0]=temp[1]; temp[1]=temp[2]; temp[2]=temp[3]; temp[3]=t;
                // SubWord
                for(int j=0;j<4;j++) temp[j] = sbox[temp[j]];
                temp[0] = temp[0] ^ Rcon[i/(Nk*4)];
            }
            for(int j=0;j<4;j++){
                RoundKey[i] = RoundKey[i - Nk*4] ^ temp[j];
                i++;
            }
        }
    }
    void AddRoundKey(int round, state_t &state){
        for(int c=0;c<4;c++){
           for(int r=0;r<4;r++){
              state[r][c] ^= RoundKey[round*Nb*4 + c*4 + r];
           }
        }
    }
    void SubBytes(state_t &state){
        for(int r=0;r<4;r++) for(int c=0;c<4;c++)
            state[r][c] = sbox[state[r][c]];
    }
    void InvSubBytes(state_t &state){
        for(int r=0;r<4;r++) for(int c=0;c<4;c++)
            state[r][c] = rsbox[state[r][c]];
    }
    void ShiftRows(state_t &s){
        uint8_t t;
        // row1
        t=s[1][0]; s[1][0]=s[1][1]; s[1][1]=s[1][2]; s[1][2]=s[1][3]; s[1][3]=t;
        // row2
        t=s[2][0]; s[2][0]=s[2][2]; s[2][2]=t; t=s[2][1]; s[2][1]=s[2][3]; s[2][3]=t;
        // row3
        t=s[3][3]; s[3][3]=s[3][2]; s[3][2]=s[3][1]; s[3][1]=s[3][0]; s[3][0]=t;
    }
    void InvShiftRows(state_t &s){
        uint8_t t;
        // row1
        t=s[1][3]; s[1][3]=s[1][2]; s[1][2]=s[1][1]; s[1][1]=s[1][0]; s[1][0]=t;
        // row2
        t=s[2][0]; s[2][0]=s[2][2]; s[2][2]=t; t=s[2][1]; s[2][1]=s[2][3]; s[2][3]=t;
        // row3
        t=s[3][0]; s[3][0]=s[3][1]; s[3][1]=s[3][2]; s[3][2]=s[3][3]; s[3][3]=t;
    }
    void MixColumns(state_t &s){
        for(int c=0;c<4;c++){
            uint8_t a0=s[0][c], a1=s[1][c], a2=s[2][c], a3=s[3][c];
            uint8_t r0 = xtime(a0)^xtime(a1)^a1^a2^a3;
            uint8_t r1 = a0^xtime(a1)^xtime(a2)^a2^a3;
            uint8_t r2 = a0^a1^xtime(a2)^xtime(a3)^a3;
            uint8_t r3 = xtime(a0)^a0^a1^a2^xtime(a3);
            s[0][c]=r0; s[1][c]=r1; s[2][c]=r2; s[3][c]=r3;
        }
    }
	uint8_t multiply(uint8_t x, uint8_t y) {
	    uint8_t r = 0;
	    while(y){
	        if(y & 1) r ^= x;
	        x = (x<<1) ^ ((x & 0x80) ? 0x1b : 0);
	        y >>= 1;
	    }
	    return r;
	}
	void InvMixColumns(state_t &s) {
	    for(int c=0;c<4;c++){
	        uint8_t a0=s[0][c], a1=s[1][c], a2=s[2][c], a3=s[3][c];
	        s[0][c] = multiply(a0,0x0e)^multiply(a1,0x0b)^multiply(a2,0x0d)^multiply(a3,0x09);
	        s[1][c] = multiply(a0,0x09)^multiply(a1,0x0e)^multiply(a2,0x0b)^multiply(a3,0x0d);
	        s[2][c] = multiply(a0,0x0d)^multiply(a1,0x09)^multiply(a2,0x0e)^multiply(a3,0x0b);
	        s[3][c] = multiply(a0,0x0b)^multiply(a1,0x0d)^multiply(a2,0x09)^multiply(a3,0x0e);
	    }
	}
    // 主要加密函数
    void Cipher(uint8_t *input, uint8_t *output){
        state_t state;
        for(int i=0;i<16;i++) state[i%4][i/4] = input[i];
        AddRoundKey(0, state);
        for(int round=1;round<Nr;round++){
            SubBytes(state);
            ShiftRows(state);
            MixColumns(state);
            AddRoundKey(round,state);
        }
        SubBytes(state);
        ShiftRows(state);
        AddRoundKey(Nr,state);

        for(int i=0;i<16;i++) output[i]= state[i%4][i/4];
    }
    void InvCipher(uint8_t *input, uint8_t *output) {
	    state_t state;
	    for(int i=0;i<16;i++) state[i%4][i/4] = input[i];

	    AddRoundKey(Nr,state);
	    for(int round=Nr-1; round>=1; round--){
	        InvShiftRows(state);
	        InvSubBytes(state);
	        AddRoundKey(round,state);
	        InvMixColumns(state);
	    }
	    InvShiftRows(state);
	    InvSubBytes(state);
	    AddRoundKey(0,state);

	    for(int i=0;i<16;i++) output[i] = state[i%4][i/4];
	}
    // 对外接口：ECB模式
    vector<uint8_t> AES_Encrypt(const vector<uint8_t>& data,const string& key) {
        // 取前16字节key
        memset(Key,0,16);
        memcpy(Key, key.data(), min((int)key.size(),16));
        KeyExpansion();
        vector<uint8_t> in = data;
        while(in.size()%16!=0) in.push_back(16-in.size()%16); // padding
        vector<uint8_t> out(in.size());
        for(size_t i=0;i<in.size();i+=16){
            Cipher((uint8_t*)&in[i], &out[i]);
        }
        return out;
    }
    vector<uint8_t> AES_Decrypt(const vector<uint8_t>& data,const string& key) {
        memset(Key,0,16);
        memcpy(Key, key.data(), min((int)key.size(),16));
        KeyExpansion();
        vector<uint8_t> out(data.size());
        for(size_t i=0;i<data.size();i+=16){
            InvCipher((uint8_t*)&data[i], &out[i]);
        }
        // 去padding
        if(!out.empty()){
            uint8_t pad=out.back();
            if(pad<=16) out.resize(out.size()-pad);
        }
        return out;
    }
    string bin2hex(const vector<uint8_t>& v){
        static const char*dig="0123456789ABCDEF";
        string s;
        for(auto b:v){
            s.push_back(dig[b>>4]);
            s.push_back(dig[b&0xf]);
        }
        return s;
    }
    vector<uint8_t> hex2bin(const string &hex){
        vector<uint8_t> out;
        for(size_t i=0;i+1<hex.size();i+=2){
            int v = stoi(hex.substr(i,2),nullptr,16);
            out.push_back((uint8_t)v);
        }
        return out;
    }
    void u_aes_tool(){
        clearInputBuffer();
        system("cls");
        EdmoveTo(0,0); g_conc.SetRGBmap(135);
        cout<<"[ AES tool ]\n";
        cout<<"1.encode\n2.decode\n";
        int m;cin>>m;cin.ignore();
        cout<<lan_str(614); string key; getline(cin,key);
        cout<<lan_str(615); string text; getline(cin,text);
        if(m==1){
            vector<uint8_t> data(text.begin(),text.end());
            auto res = AES_Encrypt(data,key);
            cout<<lan_str(616);
			g_conc.SetRGBmap(11);
			cout<<bin2hex(res);
			g_conc.SetRGBmap(135);
			cout<<endl;
        }else{
            auto bin = hex2bin(text);
            auto res = AES_Decrypt(bin,key);
            string out(res.begin(),res.end());
            cout<<lan_str(617);
            g_conc.SetRGBmap(11);
			cout<<out;
			g_conc.SetRGBmap(135);
			cout<<endl;
        }
        edt_pause();
    }
    // 颜色实验 (调用外部工具)
    void u_color_test() {
        clearInputBuffer();
        system("cls");
        EdmoveTo(0,0);
        g_conc.SetRGBmap(135);
        cout << "[ Color Tool ]" << endl;
        // 构造命令执行 colortest.exe
        string cmd = "\"" + exedir + "tool\\colortest.exe\"";
        cout << "command:" << cmd << endl;
        // 调用外部程序
        int ret = system(cmd.c_str());
        g_conc.SetRGBmap(135);
        cout << "exit value: " << ret << endl;
        if(ret!=0)edt_pause();
        else Sleep(250);
    }
    int board2048[4][4];
    // 数字对应色表
    unordered_map<int,int> colorMap = {
        {2,8},      {4,7},      {8,6},      {16,4},      {32,5},      {64,13},
        {128,9},    {256,3},    {512,11},   {1024,15},   {2048,14}
    };
    // 随机放置新方块 (2 或 4)
    void addRandomTile() {
        vector<pair<int,int>> empty;
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                if(board2048[i][j]==0) empty.push_back({i,j});
        if(empty.empty()) return;
        auto p = empty[rand()%empty.size()];
        board2048[p.first][p.second] = (rand()%10==0)?4:2;
    }
	bool tk_usedai = 0;
    // 绘制棋盘
    void drawBoard(string details,int stepc = 0) {
        system("cls");
        //clearInputBuffer();
        //EdmoveTo(0,0);
        //clearOutputBuffer(NULL);
        cout<<"=== Console 2048 ===\n\n";
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                int val=board2048[i][j];
                if(val==0){
                    g_conc.SetRGBmap(8);
                    cout << setw(6) << ".";
                }else{
                    int col = colorMap.count(val)?colorMap[val]:10;
                    g_conc.SetRGBmap(col);
                    cout << setw(6) << val;
                }
            }
            cout<<"\n\n";
        }
        g_conc.SetRGBmap(7);
        cout<<"[^ V < >]Move\n[ESC]Back IDE\n[TAB]Auto step\n";
		if(stepc){
			cout<<"step count:"<<stepc<<"\n";
		}
        g_conc.SetRGBmap(14);
        if(tk_usedai) cout<<"[Auto step used]\n";
        if(details.size()>0){
        	g_conc.SetRGBmap(7);
        	cout<<details;
		}
        g_conc.SetRGBmap(15);
    }
    // 向左移动
    bool moveLeft(){
        bool moved=false;
        for(int i=0;i<4;i++){
            vector<int> line;
            for(int j=0;j<4;j++) if(board2048[i][j]) line.push_back(board2048[i][j]);

            for(int j=0;j+1<(int)line.size();j++){
                if(line[j]==line[j+1]){ line[j]*=2; line[j+1]=0; j++; }
            }
            vector<int> newl;
            for(int x:line) if(x) newl.push_back(x);
            while(newl.size()<4) newl.push_back(0);

            for(int j=0;j<4;j++){
                if(board2048[i][j]!=newl[j]) moved=true;
                board2048[i][j]=newl[j];
            }
        }
        return moved;
    }
    void rotateCW(){ // 顺时针旋转 90°
        int tmp[4][4];
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                tmp[j][3-i]=board2048[i][j];
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                board2048[i][j]=tmp[i][j];
    }
    bool moveRight(){ rotateCW();rotateCW(); bool ok=moveLeft(); rotateCW();rotateCW(); return ok;}
    bool moveUp(){ rotateCW();rotateCW();rotateCW(); bool ok=moveLeft(); rotateCW(); return ok;}
    bool moveDown(){ rotateCW(); bool ok=moveLeft(); rotateCW();rotateCW();rotateCW(); return ok;}
    bool canMove() {
        for(int i=0;i<4;i++)for(int j=0;j<4;j++) if(board2048[i][j]==0) return true;
        for(int i=0;i<4;i++)for(int j=0;j<3;j++) if(board2048[i][j]==board2048[i][j+1]) return true;
        for(int j=0;j<4;j++)for(int i=0;i<3;i++) if(board2048[i][j]==board2048[i+1][j]) return true;
        return false;
    }
	int countEmpty() {
	    int cnt=0;
	    for(int i=0;i<4;i++)
	        for(int j=0;j<4;j++)
	            if(board2048[i][j]==0) cnt++;
	    return cnt;
	}
	int smoothness() {
	    int score=0;
	    for(int i=0;i<4;i++){
	        for(int j=0;j<4;j++){
	            if(board2048[i][j]==0) continue;
	            if(j+1<4 && board2048[i][j+1]!=0){
	                score -= abs(board2048[i][j]-board2048[i][j+1]);
	            }
	            if(i+1<4 && board2048[i+1][j]!=0){
	                score -= abs(board2048[i][j]-board2048[i+1][j]);
	            }
	        }
	    }
	    return score;
	}
	int maxInCorner() {
	    int maxTile=0;
	    int posi=0,posj=0;
	    for(int i=0;i<4;i++)
	        for(int j=0;j<4;j++)
	            if(board2048[i][j]>maxTile){
	                maxTile=board2048[i][j]; posi=i; posj=j;
	            }
	    if((posi==0||posi==3) && (posj==0||posj==3)) return maxTile;
	    return 0;
	}
	int monotonicity() {
	    int score=0;
	    for(int i=0;i<4;i++){
	        int inc=0, dec=0;
	        for(int j=0;j<3;j++){
	            if(board2048[i][j]>board2048[i][j+1]) dec+=board2048[i][j]-board2048[i][j+1];
	            else inc+=board2048[i][j+1]-board2048[i][j];
	        }
	        score -= min(inc,dec);
	    }
	    for(int j=0;j<4;j++){
	        int inc=0, dec=0;
	        for(int i=0;i<3;i++){
	            if(board2048[i][j]>board2048[i+1][j]) dec+=board2048[i][j]-board2048[i+1][j];
	            else inc+=board2048[i+1][j]-board2048[i][j];
	        }
	        score -= min(inc,dec);
	    }
	    return -score;
	}
	struct EvalDetail {
	    int empty;
	    int smooth;
	    int corner;
	    int mono;
	    int maxTile;
	    double total;
	};

	EvalDetail evaluateBoardDetail() {
	    EvalDetail ed;
	    ed.empty = countEmpty();
	    ed.smooth = smoothness();
	    ed.corner = maxInCorner();
	    ed.mono = monotonicity();
	    ed.maxTile=0;
	    for(int i=0;i<4;i++)for(int j=0;j<4;j++)
	        ed.maxTile = max(ed.maxTile, board2048[i][j]);

	    ed.total = ed.empty * 1000.0
	             + ed.smooth * 0.5
	             + ed.corner * 5.0
	             + ed.mono * 1.0
	             + ed.maxTile * 1.0;

	    return ed;
	}
	double evaluateBoard() {
	    /*int emptyCount = countEmpty();
	    int smooth = smoothness();
	    int corner = maxInCorner();
	    int mono = monotonicity();
	    int maxTile=0;
	    for(int i=0;i<4;i++)for(int j=0;j<4;j++)
	        maxTile = max(maxTile, board2048[i][j]);
	    double score = emptyCount * 1000.0
	                 + smooth * 0.5
	                 + corner * 5.0
	                 + mono * 1.0
	                 + maxTile * 1.0;
	    return score;*/
	    return evaluateBoardDetail().total;
	}
	double searchExpectimax(int depth, bool isPlayer){
	    if(depth==0 || !canMove()) return evaluateBoard();

	    if(isPlayer){
	        double best=-1e9;
	        int backup[4][4]; memcpy(backup, board2048, sizeof(board2048));
	        vector<function<bool()>> moves = {moveLeft, moveRight, moveUp, moveDown};
	        for(int d=0; d<4; d++){
	            memcpy(board2048, backup, sizeof(board2048));
	            bool moved = moves[d]();
	            if(!moved) continue;
	            double val = searchExpectimax(depth-1,false);
	            best = max(best,val);
	        }
	        memcpy(board2048, backup, sizeof(board2048));
	        return best;
	    }else{
	        vector<pair<int,int>> empty;
	        for(int i=0;i<4;i++)
	            for(int j=0;j<4;j++)
	                if(board2048[i][j]==0) empty.push_back({i,j});

	        if(empty.empty()) return evaluateBoard();

	        double total=0;
	        int backup[4][4]; memcpy(backup, board2048, sizeof(board2048));
			for(const auto &p : empty){
			    int r = p.first;
			    int c = p.second;
			    board2048[r][c] = 2;
			    total += 0.9 * searchExpectimax(depth-1,true);
			    board2048[r][c] = 4;
			    total += 0.1 * searchExpectimax(depth-1,true);
			    board2048[r][c] = 0;
			}
	        memcpy(board2048, backup, sizeof(board2048));
	        return total;
	    }
	}
	int bestMoveExpectimax(int depth, string &S){
	    vector<function<bool()>> moves = {moveLeft, moveRight, moveUp, moveDown};
	    int backup[4][4]; memcpy(backup, board2048, sizeof(board2048));

	    double best=-1e9;
	    int bestDir=-1;
	    stringstream ss;
	    char t[260];

	    for(int d=0; d<4; d++){
	        memcpy(board2048, backup, sizeof(board2048));
	        bool moved = moves[d]();
	        if(!moved){
	            ss<<"dir="<<d<<" invalid\n";
	            continue;
	        }
	        double val = searchExpectimax(depth-1,false);

	        // 保存详细当前局面评分
	        EvalDetail ed = evaluateBoardDetail();
	        memset(t,0,sizeof t);
	        sprintf(t,"dir=%d empty=%d smooth=%d corner=%d mono=%d maxtile=%d eval=%.2lf\n",d,ed.empty,ed.smooth,ed.corner,ed.mono,ed.maxTile,val);
	        ss<<(string)t;
	        if(val>best){
	            best=val;
	            bestDir=d;
	        }
	    }
	    memcpy(board2048, backup, sizeof(board2048));
	    S = ss.str();
	    return bestDir;
	}
	// 主循环 (用 _getch 控制)
	void game_2048() {
	    g_conc.SetRGBmap(15);
	    system("cls");
	    clearInputBuffer();
	    EdmoveTo(0,0);
	    memset(board2048,0,sizeof(board2048));
	    srand(time(0));
	    addRandomTile();
	    addRandomTile();
	    tk_usedai = 0;
		string details = "";
	    int stepc = 0;
	    while(true){
	        drawBoard(details,stepc);
	    	details = "";
	        if(!canMove()){
	            cout<<"\nGame Over! Press ESC return...\n";
	            while(true){
	                int c = _getch();
	                if(c==27) return; // ESC
	            }
	        }
	        int c = _getch();
	        if(c==27) return; // ESC
			if(c==9){ // Tab
			    tk_usedai=1;
			    int dir = bestMoveExpectimax(4, details);
			    bool moved=false;
			    if(dir==0) moved=moveLeft();
			    else if(dir==1) moved=moveRight();
			    else if(dir==2) moved=moveUp();
			    else if(dir==3) moved=moveDown();
			    if(moved) addRandomTile();
			    if(moved) stepc++;
			}
	        if(c==0 || c==0xE0){ // 扩展键 (方向键)
	            c = _getch();    // 读取扫描码
	            bool moved=false;
	            if(c==75) moved=moveLeft();   // ←
	            if(c==77) moved=moveRight();  // →
	            if(c==72) moved=moveUp();     // ↑
	            if(c==80) moved=moveDown();   // ↓
	            if(moved) addRandomTile();
	            if(moved) stepc++;
	        }
	    }
	}
	bool u_runProcess(const string &commandLine) {
	    STARTUPINFOA si;
	    PROCESS_INFORMATION pi;
	    ZeroMemory(&si, sizeof(si));
	    si.cb = sizeof(si);
	    ZeroMemory(&pi, sizeof(pi));
	    // 注意：CreateProcess 需要可写缓冲区
	    char *cmd = _strdup(commandLine.c_str());
	    BOOL success = CreateProcessA(
	        NULL,   // 可执行文件路径（NULL=命令行第一个 token）
	        cmd,    // 命令行（必须可写）
	        NULL,   // 进程安全属性
	        NULL,   // 线程安全属性
	        TRUE,   // 是否继承句柄
	        CREATE_NEW_CONSOLE ,      // 创建选项
	        NULL,   // 环境变量
	        NULL,   // 当前目录
	        &si,    // 启动参数
	        &pi     // 进程信息
	    );
	    free(cmd);
	    if (!success) {
	        cerr << "CreateProcess failed. Error code: " << GetLastError() << endl;
	        return false;
	    }
	    CloseHandle(pi.hProcess);
	    CloseHandle(pi.hThread);
	    return true;
	}
	void u_copy_brain(){
		clearInputBuffer();
		system("cls");
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		cout<<lan_str(618)<<endl;
		edt_pause();
		string fn = exedir+"tool\\editorbin.exe";
		int r = u_runProcess(fn);
		if(r==0){
			cout<<"\nError:starting process\n";
			edt_pause();
		}
	}
	void main(const vector<string> &ed_lines={""}){
		clearInputBuffer();
		system("cls");
		EdmoveTo(0,0);g_conc.SetRGBmap(135);
		cout<<lan_str(600)<<endl<<lan_str(601)<<endl;
		int i;
		cin>>i;
		if(i==1){
			u_yunshi();
		}
		else if(i==2){
			u_codestats_file(ed_lines);
		}
		else if(i==3){
			u_aes_tool();
		}
		else if(i==4){
			u_color_test();
		}
		else if(i==5){
			game_2048();
		}
		else if(i==6){
			u_copy_brain();
		}
	}
}
