#include <windows.h>
#include <iostream>
#include <climits>
#pragma once
using namespace std;

class ConsoleColor {
public:
    bool vt_support;  // 是否支持VT序列
    HANDLE hConsole;

    ConsoleColor() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        vt_support = false;
        if (GetConsoleMode(hConsole, &mode)) {
            DWORD newMode = mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (SetConsoleMode(hConsole, newMode)) {
                vt_support = true; // 开启成功，说明支持ANSI转义
            }
        }
    }

    // 设置前景/背景颜色
    void SetRGB(int r, int g, int b, bool isBackground = false) {
        if (vt_support) {
            // 直接使用 ANSI TrueColor
            if (isBackground) {
            	printf("\033[48;2;%d;%d;%dm",r,g,b);
                //cout << "\033[48;2;" << r << ";" << g << ";" << b << "m";
            } else {
            	printf("\033[38;2;%d;%d;%dm",r,g,b);
                //cout << "\033[38;2;" << r << ";" << g << ";" << b << "m";
            }
        } else {
            // 老环境 → 使用最近16色替代
            WORD closest = GetNearestColor(r, g, b);
            SetConsoleTextAttribute(hConsole, isBackground ? (closest << 4) : closest);
        }
    }
    void SetRGBmap(int a,int b = 0) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, (b << 4) | a);
    }

    // 重置颜色
    void Reset() {
        if (vt_support) {
            cout << "\033[0m";
        } else {
            SetConsoleTextAttribute(hConsole, 7); // 默认灰白
        }
    }

    struct ConsoleColorMap { int r,g,b; WORD color; };
    ConsoleColorMap colors[16] = {
        {0,0,0, 0}, {0,0,128, 1}, {0,128,0, 2}, {0,128,128, 3},
        {128,0,0, 4}, {128,0,128, 5}, {128,128,0, 6}, {192,192,192, 7},
        {128,128,128, 8}, {0,0,255, 9}, {0,255,0, 10}, {0,255,255, 11},
        {255,0,0, 12}, {255,0,255, 13}, {255,255,0, 14}, {255,255,255, 15}
    };

    WORD GetNearestColor(int r, int g, int b) {
        WORD bestColor = 0;
        int minDist = INT_MAX;
        for (auto &c : colors) {
            int dist = (r-c.r)*(r-c.r) + (g-c.g)*(g-c.g) + (b-c.b)*(b-c.b);
            if (dist < minDist) {
                minDist = dist;
                bestColor = c.color;
            }
        }
        return bestColor;
    }
};

ConsoleColor g_conc;

void PrintGradientText(const string& text,
                       int r1, int g1, int b1,
                       int r2, int g2, int b2) {
    int len = text.size();
    if (len == 0) return;

    for (int i = 0; i < len; i++) {
        double t = (double)i / (len - 1); // ???? [0,1]
        int r = r1 + (int)((r2 - r1) * t);
        int g = g1 + (int)((g2 - g1) * t);
        int b = b1 + (int)((b2 - b1) * t);

        g_conc.SetRGB(r, g, b);
        cout << text[i];
    }
    g_conc.Reset();
}
