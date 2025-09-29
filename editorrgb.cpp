#include <windows.h>
#include <iostream>
#include <climits>
#include <string>
#include <sstream>
#include <vector>
#pragma once
#include "editorcex.cpp"
using namespace std;
struct wincol {
    struct rgb { unsigned char r, g, b; } colors[16];

    // �� wincol תΪ�ַ��� "R:G:B|R:G:B|..."
    std::string to_str() const {
        std::ostringstream oss;
        for (int i = 0; i < 16; ++i) {
            oss << (int)colors[i].r << ":" << (int)colors[i].g << ":" << (int)colors[i].b;
            if (i != 15) oss << "|";
        }
        return oss.str();
    }

    // ���ַ������أ������Ƿ�ɹ�����������16����㲿�ֱ���ԭֵ����Ϊ0��
    bool load_str(const std::string &s) {
        // ��ʽ���� N ���� '|' �ָ���ÿ��Ϊ R:G:B
        std::vector<std::string> parts;
        {
            std::istringstream iss(s);
            std::string token;
            while (std::getline(iss, token, '|')) {
                parts.push_back(token);
            }
        }
        if (parts.empty()) return false;
        int n = (int)parts.size();
        if (n > 16) return false; // ����16����Ϊ����

        // ��ʱ��Ž���ֵ�������� 16 �򲻸Ķ�ʣ������ѡ��Ĭ��0��
        for (int i = 0; i < n; ++i) {
            std::string &p = parts[i];
            std::vector<std::string> rgbParts;
            std::istringstream iss(p);
            std::string t;
            while (std::getline(iss, t, ':')) rgbParts.push_back(t);
            if (rgbParts.size() != 3) return false;
            try {
                int rr = std::stoi(rgbParts[0]);
                int gg = std::stoi(rgbParts[1]);
                int bb = std::stoi(rgbParts[2]);
                if (rr < 0) rr = 0; if (rr > 255) rr = 255;
                if (gg < 0) gg = 0; if (gg > 255) gg = 255;
                if (bb < 0) bb = 0; if (bb > 255) bb = 255;
                colors[i].r = (unsigned char)rr;
                colors[i].g = (unsigned char)gg;
                colors[i].b = (unsigned char)bb;
            } catch (...) {
                return false;
            }
        }
        // ��� parts < 16������ѡ��ʣ����Ϊ 0 ����ԭֵ��������Ϊ 0
        for (int i = n; i < 16; ++i) {
            colors[i].r = colors[i].g = colors[i].b = 0;
        }
        return true;
    }
};
class ConsoleColor {
public:
    bool vt_support;  // �Ƿ�֧��VT����
    HANDLE hConsole;
    bool palette_supported;

    struct ConsoleColorMap { int r,g,b; WORD color; };
    ConsoleColorMap colors[16] = {
        {0,0,0, 0}, {0,0,128, 1}, {0,128,0, 2}, {0,128,128, 3},
        {128,0,0, 4}, {128,0,128, 5}, {128,128,0, 6}, {192,192,192, 7},
        {128,128,128, 8}, {0,0,255, 9}, {0,255,0, 10}, {0,255,255, 11},
        {255,0,0, 12}, {255,0,255, 13}, {255,255,0, 14}, {255,255,255, 15}
    };

    ConsoleColor() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        vt_support = false;
        palette_supported = false;

        // ���Կ��� VT ���
        if (GetConsoleMode(hConsole, &mode)) {
            DWORD newMode = mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (SetConsoleMode(hConsole, newMode)) {
                vt_support = true; // �����ɹ���˵��֧��ANSIת��
            }
        }
    }

    int g_color_mode = 1;
    // ����ǰ��/������ɫ
    void SetRGB(int r, int g, int b, bool isBackground = false) {
        if (vt_support) {
            if (g_color_mode == 1) {
                // --- TrueColor 24bit ---
                if (isBackground) {
                    printf("\033[48;2;%d;%d;%dm", r, g, b);
                } else {
                    printf("\033[38;2;%d;%d;%dm", r, g, b);
                }
            }
            else {
                // --- 256ɫģʽ ---
                // ��RGBӳ�䵽xterm 256 palette
                int idx;
                if (r == g && g == b) {
                    // �Ҷ�: 232�C255 ��ʾ 24 �׻Ҷ�
                    idx = 232 + (r * 23 / 255);
                } else {
                    // ɫ����: 16�C231 ��ʾ 6x6x6 ��ɫ������
                    int R = r * 5 / 255;
                    int G = g * 5 / 255;
                    int B = b * 5 / 255;
                    idx = 16 + 36 * R + 6 * G + B;
                }
                if (isBackground)
                    printf("\033[48;5;%dm", idx);
                else
                    printf("\033[38;5;%dm", idx);
            }
        } else {
            // �ϻ��� �� 16ɫ����
            WORD closest = GetNearestColor(r, g, b);
            SetConsoleTextAttribute(hConsole, isBackground ? (closest << 4) : closest);
        }
    }

    void SetRGBmap(int a,int b = 0) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, (b << 4) | a);
    }

    // ������ɫ
    void Reset() {
        if (vt_support) {
        	SetRGBmap(7);
            //cout << "\033[0m";
        } else {
            SetConsoleTextAttribute(hConsole, 7); // Ĭ�ϻҰ�
        }
    }

    WORD GetNearestColor(int r, int g, int b) {
        WORD bestColor = 0;
        int minDist = INT_MAX;
        for (auto &c : colors) {
            int dr = r - c.r;
            int dg = g - c.g;
            int db = b - c.b;
            int dist = dr*dr + dg*dg + db*db;
            if (dist < minDist) {
                minDist = dist;
                bestColor = c.color;
            }
        }
        return bestColor;
    }
    // �ӵ�ǰ����̨��ȡ16ɫ��ɫ�嵽 wincol������ true ��ʾ�ɹ���
	bool ReadRGBmap(struct wincol &out) {
	    // ��̬���غ���
	    typedef BOOL(WINAPI *FN_GetConsoleScreenBufferInfoEx)(HANDLE, void*);
	    HMODULE hKernel = GetModuleHandleW(L"kernel32.dll");
	    if (!hKernel) return false;
	    FN_GetConsoleScreenBufferInfoEx fnGet = (FN_GetConsoleScreenBufferInfoEx)
	        GetProcAddress(hKernel, "GetConsoleScreenBufferInfoEx");
	    if (!fnGet) {
	        palette_supported = false;
	        return false;
	    }

	    // ���� CONSOLE_SCREEN_BUFFER_INFOEX�����ϵͳͷ���ṩҲ�ܹ�����
	    struct CONSOLE_SCREEN_BUFFER_INFOEX_LOCAL {
	        ULONG cbSize;
	        COORD dwSize;
	        COORD dwCursorPosition;
	        WORD wAttributes;
	        SMALL_RECT srWindow;
	        COORD dwMaximumWindowSize;
	        WORD wPopupAttributes;
	        BOOL bFullscreenSupported;
	        COLORREF ColorTable[16];
	    } info;
	    ZeroMemory(&info, sizeof(info));
	    info.cbSize = sizeof(info);

	    if (!fnGet(hConsole, &info)) {
	        palette_supported = false;
	        return false;
	    }

	    palette_supported = true;
	    for (int i = 0; i < 16; ++i) {
	        COLORREF c = info.ColorTable[i];
	        out.colors[i].r = GetRValue(c);
	        out.colors[i].g = GetGValue(c);
	        out.colors[i].b = GetBValue(c);
	    }
	    return true;
	}

	// �� wincol Ӧ�õ���ǰ����̨��16ɫ��ɫ�岢������Ч������ true ��ʾ�ɹ���
	bool RunRGBmap(const struct wincol &in) {
	    typedef BOOL(WINAPI *FN_GetConsoleScreenBufferInfoEx)(HANDLE, void*);
	    typedef BOOL(WINAPI *FN_SetConsoleScreenBufferInfoEx)(HANDLE, const void*);

	    HMODULE hKernel = GetModuleHandleW(L"kernel32.dll");
	    if (!hKernel) return false;
	    FN_GetConsoleScreenBufferInfoEx fnGet = (FN_GetConsoleScreenBufferInfoEx)
	        GetProcAddress(hKernel, "GetConsoleScreenBufferInfoEx");
	    FN_SetConsoleScreenBufferInfoEx fnSet = (FN_SetConsoleScreenBufferInfoEx)
	        GetProcAddress(hKernel, "SetConsoleScreenBufferInfoEx");
	    if (!fnGet || !fnSet) {
	        palette_supported = false;
	        return false;
	    }

	    struct CONSOLE_SCREEN_BUFFER_INFOEX_LOCAL {
	        ULONG cbSize;
	        COORD dwSize;
	        COORD dwCursorPosition;
	        WORD wAttributes;
	        SMALL_RECT srWindow;
	        COORD dwMaximumWindowSize;
	        WORD wPopupAttributes;
	        BOOL bFullscreenSupported;
	        COLORREF ColorTable[16];
	    } info;
	    ZeroMemory(&info, sizeof(info));
	    info.cbSize = sizeof(info);

	    if (!fnGet(hConsole, &info)) {
	        palette_supported = false;
	        return false;
	    }

	    // �滻 ColorTable
	    for (int i = 0; i < 16; ++i) {
	        const auto &c = in.colors[i];
	        info.ColorTable[i] = RGB(c.r, c.g, c.b);
	    }

	    if (!fnSet(hConsole, &info)) {
	        return false;
	    }

	    palette_supported = true;
	    return true;
	}

	// ��ӡ��ɫ�壨ʹ��24λ/256/16ɫ��ʽ��ʾÿһ����ɫ������������� out == nullptr ���ӡ��ǰ����̨����ɫ�����Զ�ȡ��
	void PrintRGBmap(const struct wincol *in = nullptr) {
	    wincol local;
	    const wincol *p = in;
	    if (!p) {
	        if (!ReadRGBmap(local)) {
	            // �����ȡʧ�ܣ�����Ĭ�� colors ����ת���� wincol��fallback��
	            for (int i = 0; i < 16; ++i) {
	                local.colors[i].r = colors[i].r;
	                local.colors[i].g = colors[i].g;
	                local.colors[i].b = colors[i].b;
	            }
	        }
	        p = &local;
	    }

	    // ��ӡ��������RGBֵ��ɫ��
	    for (int i = 0; i < 16; ++i) {
	        int r = p->colors[i].r;
	        int g = p->colors[i].g;
	        int b = p->colors[i].b;
	        // ��ӡ������ RGB �ı�
	        // ʹ�� ANSI 24bit ������ʾɫ�飨���֧��VT��������ʹ�� SetConsoleTextAttribute ����
	        std::ostringstream oss;
	        oss << i << ": " << r << "," << g << "," << b << "  ";
	        std::string info = oss.str();
			SetRGBmap(15);
	        std::cout << info;

	        if (vt_support) {
	            // �ñ���ɫ��ʾ�����ո�
	            if (g_color_mode == 1) {
	                printf("\033[48;2;%d;%d;%dm[+]\033[0m", r, g, b);
	            } else {
	                // 256ɫ
	                int idx;
	                if (r == g && g == b) idx = 232 + (r * 23 / 255);
	                else {
	                    int R = r * 5 / 255, G = g * 5 / 255, B = b * 5 / 255;
	                    idx = 16 + 36*R + 6*G + B;
	                }
	                printf("\033[48;5;%dm  \033[0m", idx);
	            }
	        } else {
	            // ������ʹ�ÿ���̨ 16 ɫ������ʾ�����ƣ�
	            WORD attr = GetNearestColor(r, g, b);
	            SetConsoleTextAttribute(hConsole, (attr << 4) | 0); // ��������
	            std::cout << "  ";
	            // �ָ�Ĭ��
	            SetConsoleTextAttribute(hConsole, 7);
	        }
			SetRGBmap(15);
	        std::cout << std::endl;
	    }
	}
};

ConsoleColor g_conc;

// �����ӡ
void PrintGradientText(const string& text,
                       int r1, int g1, int b1,
                       int r2, int g2, int b2) {
    int len = text.size();
    if (len == 0) return;

    for (int i = 0; i < len; i++) {
        double t = (double)i / (len - 1); // ��һ [0,1]
        int r = r1 + (int)((r2 - r1) * t);
        int g = g1 + (int)((g2 - g1) * t);
        int b = b1 + (int)((b2 - b1) * t);

        g_conc.SetRGB(r, g, b);
        cout << text[i];
    }
    g_conc.Reset();
}
void PrintGradientText(const std::string& text,
                       COLORREF startColor,
                       COLORREF endColor)
{
    // ��ȡ��ͨ��
    int r1 = GetRValue(startColor);
    int g1 = GetGValue(startColor);
    int b1 = GetBValue(startColor);

    int r2 = GetRValue(endColor);
    int g2 = GetGValue(endColor);
    int b2 = GetBValue(endColor);

    // �������а汾
    PrintGradientText(text, r1, g1, b1, r2, g2, b2);
}
#include <tlhelp32.h>
#include <string>
#include <cstdarg>

//-----------------------------------------------------------------------------
// Simple debug�\printer to the VS Output window or any debugger attached.
//-----------------------------------------------------------------------------
static void DebugPrint(const wchar_t* fmt, ...)
{
    wchar_t buffer[512];
    va_list args;
    va_start(args, fmt);
    wvsprintfW(buffer, fmt, args);
    va_end(args);
    OutputDebugStringW(buffer);
}

//-----------------------------------------------------------------------------
// GetParentProcessName
// Given our PID, finds the parent PID and retrieves its executable name.
//-----------------------------------------------------------------------------
static bool GetParentProcessName(DWORD pid, std::wstring& outName)
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        DebugPrint(L"GetParentProcessName: CreateToolhelp32Snapshot failed (err=%u)\n", GetLastError());
        return false;
    }

    PROCESSENTRY32W pe = { sizeof(pe) };
    DWORD parentPid = 0;

    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (pe.th32ProcessID == pid) {
                parentPid = pe.th32ParentProcessID;
                break;
            }
        } while (Process32NextW(hSnap, &pe));
    }

    if (!parentPid) {
        DebugPrint(L"GetParentProcessName: no parent PID found for %u\n", pid);
        CloseHandle(hSnap);
        return false;
    }

    // find parent entry
    Process32FirstW(hSnap, &pe);
    do {
        if (pe.th32ProcessID == parentPid) {
            outName = pe.szExeFile;
            DebugPrint(L"GetParentProcessName: parent of %u is %s (pid=%u)\n",
                       pid, pe.szExeFile, parentPid);
            break;
        }
    } while (Process32NextW(hSnap, &pe));

    CloseHandle(hSnap);
    return !outName.empty();
}

//-----------------------------------------------------------------------------
// IsWindowsTerminalHost
// Returns non�\zero if running under the modern Windows Terminal.
// Emits debug info for each check.
//-----------------------------------------------------------------------------
int IsWindowsTerminalHost()
{
    DebugPrint(L"=== IsWindowsTerminalHost() begin ===\n");

    // --------- Method 1: WT_SESSION env var -------------
    DebugPrint(L"Checking WT_SESSION environment variable...\n");
    DWORD len = GetEnvironmentVariableW(L"WT_SESSION", nullptr, 0);
    if (len > 0) {
        DebugPrint(L"Found WT_SESSION (length=%u)\n", len);
        DebugPrint(L"=== Windows Terminal detected (Method 1) ===\n");
        return 1;
    }

    // --------- Method 2: Window class name -------------
    DebugPrint(L"Checking console window class name...\n");
    HWND hwnd = GetConsoleWindow();
    if (hwnd) {
        wchar_t clsName[64] = {};
        if (GetClassNameW(hwnd, clsName, _countof(clsName))) {
            DebugPrint(L"Console window class: %s\n", clsName);
            if (wcscmp(clsName, L"CASCADIA_WINDOW_CLASS") == 0) {
                DebugPrint(L"=== Windows Terminal detected (Method 2) ===\n");
                return 1;
            }
        }
    } else {
        DebugPrint(L"No console window handle available.\n");
    }

    // --------- Method 3: Parent process name -------------
    DebugPrint(L"Checking parent process name for wt.exe / WindowsTerminal.exe...\n");
    {
        DWORD myPid = GetCurrentProcessId();
        std::wstring parentName;
        if (GetParentProcessName(myPid, parentName)) {
            if (_wcsicmp(parentName.c_str(), L"wt.exe") == 0 ||
                _wcsicmp(parentName.c_str(), L"WindowsTerminal.exe") == 0)
            {
                DebugPrint(L"Parent process is %s\n", parentName.c_str());
                DebugPrint(L"=== Windows Terminal detected (Method 3) ===\n");
                return 1;
            }
        }
    }

    // --------- Method 4: WT_PROFILE_ID env var -------------
    DebugPrint(L"Checking WT_PROFILE_ID environment variable...\n");
    if (GetEnvironmentVariableW(L"WT_PROFILE_ID", nullptr, 0) > 0) {
        DebugPrint(L"Found WT_PROFILE_ID\n");
        DebugPrint(L"=== Windows Terminal detected (Method 4) ===\n");
        return 1;
    }

    // --------- Method 5: TERM == xterm-256color -------------
    DebugPrint(L"Checking TERM environment variable...\n");
    {
        wchar_t termBuf[64] = {};
        if (GetEnvironmentVariableW(L"TERM", termBuf, _countof(termBuf)) > 0) {
            DebugPrint(L"TERM=%s\n", termBuf);
            if (_wcsicmp(termBuf, L"xterm-256color") == 0) {
                DebugPrint(L"=== Windows Terminal detected (Method 5) ===\n");
                return 1;
            }
        }
    }

    DebugPrint(L"=== Windows Terminal NOT detected ===\n");
    return 0;
}
