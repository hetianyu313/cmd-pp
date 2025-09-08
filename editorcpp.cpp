#include<bits/stdc++.h>
#include<conio.h>
#include<sstream>
#pragma once
#include"editorapi.cpp"
#include"editorrgb.cpp"
#include"editorstr.cpp"
#include"editorchar.cpp"
using namespace std;
namespace _ed_cpp{
	string _ed_gpp_path = "";//g++ path
	string _ed_in_f = "";    //cpp path
	string _ed_out_f = "";   //exe path
	string _ed_yh = "-std=c++14 -O2 -s";
	string _ed_include = "";
	string _ed_yh2 = "-static-libgcc";
	void getexedir(){
		char path[MAX_PATH];
	    DWORD len = GetModuleFileName(NULL, path, MAX_PATH);
	    string t = string(path, len);
	    exedir = t.substr(0,t.find_last_of("\\"))+"\\";
	    cout<<"getexedir:exedir is "<<exedir<<endl;
	}
	void init(){
		getexedir();
		string fn = exedir+"setting\\mingw_g++.ini";
		cout<<"_ed_cpp:file path="<<fn<<"\n";
		ifstream ifs(fn.c_str());
		getline(ifs,_ed_gpp_path);
		getline(ifs,_ed_yh);
		getline(ifs,_ed_yh2);
		getline(ifs,_ed_include);
		ifs.close();
	}
	void write_by(){
		getexedir();
		string fn = exedir+"setting\\mingw_g++.ini";
		cout<<"_ed_cpp:file path="<<fn<<"\n";
		ofstream ofs(fn.c_str());
		ofs<<_ed_gpp_path<<endl;
		ofs<<_ed_yh<<endl;
		ofs<<_ed_yh2<<endl;
		ofs<<_ed_include<<endl;
		ofs.close();
	}
	string _gen_cmd(string gpp_path,string in_f,string out_f,
		string yh,string include,string yh2){
		stringstream ss("");
		ss
		<<"\""<<gpp_path<<"\" \""<<in_f<<"\" -o \""
		<<out_f<<"\" "<<yh<<" "<<include<<" "
		<<yh2;
		return ss.str();
	}
	bool runProcess1(const string &commandLine) {
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
	bool runProcess(const string &commandLine) {
	    STARTUPINFOA si;
	    PROCESS_INFORMATION pi;
	    ZeroMemory(&si, sizeof(si));
	    si.cb = sizeof(si);
	    ZeroMemory(&pi, sizeof(pi));
	    // 注意：CreateProcess 需要可写的缓冲区
	    char *cmd = _strdup(commandLine.c_str());
	    // 创建进程
	    BOOL success = CreateProcessA(
	        NULL,   // 可执行文件名（NULL=从命令行里解析第一个token）
	        cmd,    // 命令行（必须可写）
	        NULL,   // 安全属性
	        NULL,   // 安全属性
	        TRUE,   // 是否继承句柄
	        0,      // 创建参数
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
	    // 等待进程结束
	    WaitForSingleObject(pi.hProcess, INFINITE);
	    // 关闭句柄
	    CloseHandle(pi.hProcess);
	    CloseHandle(pi.hThread);
	    return true;
	}
	struct CompileError {
	    string file;
	    int line = -1;
	    int column = -1;
	    string type;   // "error" / "warning" / "note"
	    string message;
	};
	string runAndCapture(const string& commandLine) {
	    SECURITY_ATTRIBUTES saAttr;
	    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	    saAttr.bInheritHandle = TRUE;
	    saAttr.lpSecurityDescriptor = NULL;

	    HANDLE hRead, hWrite;
	    if (!CreatePipe(&hRead, &hWrite, &saAttr, 0)) {
	        return "ERROR: CreatePipe failed\n";
	    }
	    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

	    STARTUPINFOA si;
	    PROCESS_INFORMATION pi;
	    ZeroMemory(&si, sizeof(si));
	    si.cb = sizeof(si);
	    si.dwFlags |= STARTF_USESTDHANDLES;
	    si.hStdOutput = hWrite;
	    si.hStdError = hWrite;   // 把 stderr 也合并
	    ZeroMemory(&pi, sizeof(pi));

	    char* cmd = _strdup(commandLine.c_str());
	    BOOL success = CreateProcessA(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	    free(cmd);
	    if (!success) {
	        CloseHandle(hRead);
	        CloseHandle(hWrite);
	        return "ERROR: CreateProcess failed\n";
	    }
	    CloseHandle(hWrite); // 父进程只读

	    string output;
	    char buffer[4096];
	    DWORD bytesRead;
	    while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
	        buffer[bytesRead] = 0;
	        output += buffer;
	    }
	    WaitForSingleObject(pi.hProcess, INFINITE);
	    CloseHandle(pi.hProcess);
	    CloseHandle(pi.hThread);
	    CloseHandle(hRead);

	    return output;
	}
	void t_displayerror(vector<CompileError> v,string fn){
		g_conc.SetRGBmap(15);
		cout<<"find "<<v.size()<<" CompileErrors\n";
		int lv[5] = {0,0,0,0,0};
		for(int i = 0;i<(int)v.size();i++){
			CompileError e = v[i];
			g_conc.SetRGBmap(7);
			cout<<"Log "<<i<<": level=";
			g_conc.SetRGBmap(15);
			string el = e.type;
			if(el=="error") g_conc.SetRGBmap(12);
			else if(el=="warning") g_conc.SetRGBmap(6);
			else if(el=="note") g_conc.SetRGBmap(3);
			else if(el=="linker") g_conc.SetRGBmap(10);
			else g_conc.SetRGBmap(10);
			cout<<el<<" ";
			g_conc.SetRGBmap(7);
			if(el!="linker")printf(" line=%d row=%d\n",e.line,e.column);
			else cout<<"\n";
			g_conc.SetRGBmap(8);
			if(el!="linker"){
				cout<<"in file ";
				if(e.file==fn) g_conc.SetRGBmap(2);
				cout<<e.file<<endl;
			}
			if(el=="error") g_conc.SetRGBmap(14);
			else if(el=="warning") g_conc.SetRGBmap(6);
			else if(el=="note") g_conc.SetRGBmap(15);
			else if(el=="linker") g_conc.SetRGBmap(10);
			else g_conc.SetRGBmap(135);
			cout<<e.message<<endl;
			
			if(el=="error")lv[0]++;
			else if(el=="warning")lv[1]++;
			else if(el=="note")lv[2]++;
			else if(el=="linker")lv[4]++;
			else lv[3]++;
		}
		g_conc.SetRGBmap(8);
		cout<<"Errors:"<<lv[0]<<endl;
		cout<<"Warnings:"<<lv[1]<<endl;
		cout<<"Notes:"<<lv[2]<<endl;
		cout<<"Linker:"<<lv[4]<<endl;
		cout<<"Unknown:"<<lv[3]<<endl;
		g_conc.SetRGBmap(15);
	}
	/*vector<CompileError> parseErrors(const string& compilerOutput) {
	    vector<CompileError> errs;
	    regex re(R"((.*):(\d+):(\d+):\s+(warning|error|note):\s+(.*))");
	    smatch m;
	    istringstream iss(compilerOutput);
	    string line;
	    while (getline(iss, line)) {
	        if (regex_match(line, m, re)) {
	            CompileError e;
	            e.file = m[1].str();
	            e.line = stoi(m[2].str());
	            e.column = stoi(m[3].str());
	            e.type = m[4].str();
	            e.message = m[5].str();
	            errs.push_back(e);
	        }
	    }
	    return errs;
	}*/
	vector<CompileError> parseErrors(const string& compilerOutput) {
	    vector<CompileError> errs;
	    // 典型编译错误: file.cpp:line:col: message...
	    regex reCompile(R"((.*):(\d+):(\d+):\s+(warning|error|note):\s+(.*))");
	    // 链接错误: 任意路径 + ld.exe: <消息>
	    regex reLinker(R"(([^:]+ld\.exe):\s+(.*))");
	    regex reOther(R"(([^:]+(.*)\.exe):\s+(.*))");
	    smatch m;
	    istringstream iss(compilerOutput);
	    string line;
	    while (getline(iss, line)) {
	        // 去掉 Windows 的 \r
	        if (!line.empty() && line.back()=='\r') line.pop_back();

	        if (regex_match(line, m, reCompile)) {
	            CompileError e;
	            e.file   = m[1].str();
	            e.line   = stoi(m[2].str());
	            e.column = stoi(m[3].str());
	            e.type   = m[4].str();
	            e.message= m[5].str();
	            errs.push_back(e);
	        }
	        else if (regex_search(line, m, reLinker)) {
	            CompileError e;
	            e.file   = m[1].str();
	            e.line   = -1;
	            e.column = -1;
	            e.type   = "linker";
	            e.message= m[2].str();
	            errs.push_back(e);
	        }
	        else if (regex_search(line, m, reOther)) {
	            CompileError e;
	            e.file   = m[1].str();
	            e.line   = -1;
	            e.column = -1;
	            e.type   = "other";
	            e.message= line;
	            errs.push_back(e);
	        }
	    }
	    return errs;
	}
	std::vector<CompileError> _ed_compile_fileEx(bool run = false) {
	    system("cls");
	    //clearInputBuffer();
	    EdmoveTo(0,0); g_conc.SetRGBmap(15);
	    cout << lan_str(301) << "\n";
	    unsigned int ts = GetTickCount();
	    string in = _ed_in_f;
	    string out = in.substr(0, in.find_last_of(".")) + ".exe";
	    string cmd = _gen_cmd(_ed_gpp_path, in, out, _ed_yh, _ed_include, _ed_yh2);
	    cout << "compile cmd: " << AnsiToUtf8(cmd) << endl;
	    string output = AnsiToUtf8(runAndCapture(cmd));
	    //cout << "compiler output: \n" << output << endl;
	    unsigned int es = GetTickCount();
	    cout << "time: " << (es - ts) << "ms\n";
	    auto errors = parseErrors(output);
	    if (run) {
	        string runcmd = "\"" + exedir + "tool\\ConsolePauser.exe\" \""+out+"\"";
	        runProcess1(runcmd);
	    }
	    return errors;
	}
	void _ed_test(bool run = 0){
		g_conc.SetRGBmap(15);
		system("cls");
		clearInputBuffer();
		auto v = _ed_compile_fileEx(run);
		t_displayerror(v,_ed_in_f);
		edt_pause();
	}
	void _ed_complete_file(bool run = 0){
		g_conc.SetRGBmap(15);
		system("cls");
		clearInputBuffer();
		EdmoveTo(0,0);
		cout<<lan_str(301)<<"\n";
		unsigned int ts = GetTickCount();
		string in=_ed_in_f,out;
		//cin>>out;
		out = in.substr(0,in.find_last_of("."))+".exe";
		cout<<lan_str(302)<<":"<<_ed_gpp_path<<endl;
		cout<<lan_str(223)<<":"<<out<<"\n";
		/*cmd = "cd \""+_ed_gpp_path.substr(0,_ed_gpp_path.find_last_of("\\"))+"\"";
		cout<<"命令:"<<cmd<<endl;
		system(cmd.c_str());*/
		string cmd = _gen_cmd(_ed_gpp_path,in,out,_ed_yh,_ed_include,_ed_yh2);
		//replace(cmd.begin(), cmd.end(), '\\', '/');
		cout<<lan_str(227)<<AnsiToUtf8(cmd)<<endl;
		runHooks("beforeRun",_ed_in_f);
		cout<<lan_str(304)<<lan_str(303)<<"\n";
		runProcess(cmd);
		cout<<lan_str(305)<<lan_str(303)<<"\n";
		unsigned int es = GetTickCount();
		unsigned int dt = es - ts;
		cout<<lan_str(303)<<lan_str(307)<<":"<<dt<<"ms\n";
		if(run){
			cout<<lan_str(304)<<lan_str(306)<<"\n";
			cmd = "\""+exedir+"tool\\ConsolePauser.exe\" \""+out+"\"";
			cout<<lan_str(227)<<AnsiToUtf8(cmd)<<endl;
			runProcess1(cmd);
			//system(cmd.c_str());
			runHooks("onRun",_ed_in_f);
		}
		edt_pause();
	}
}
