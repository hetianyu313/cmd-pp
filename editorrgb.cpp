#include <windows.h>
#include <iostream>
#pragma once
using namespace std;
class ConsoleColor {
public:
    void SetRGB(int r, int g, int b, bool isBackground = false) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFOEX csbi;
        csbi.cbSize = sizeof(csbi);
        GetConsoleScreenBufferInfoEx(hConsole, &csbi);
        
        // 转换RGB到控制台最近似颜色
        WORD closest = GetNearestColor(r, g, b);
        SetConsoleTextAttribute(hConsole, isBackground ? (closest << 4) : closest);
    }
    struct ConsoleColorMap { int r,g,b; WORD color; };
    ConsoleColorMap colors[16] = {
        {0,0,0, 0}, {0,0,128, 1}, {0,128,0, 2}, {0,128,128, 3},
        {128,0,0, 4}, {128,0,128, 5}, {128,128,0, 6}, {192,192,192, 7},
        {128,128,128, 8}, {0,0,255, 9}, {0,255,0, 10}, {0,255,255, 11},
        {255,0,0, 12}, {255,0,255, 13}, {255,255,0, 14}, {255,255,255, 15}
    };
    void SetRGBmap(int a,int b = 0) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, (b << 4) | a);
    }
    WORD GetNearestColor(int r, int g, int b) {
        // Windows控制台标准16色映射

        WORD bestColor = 0;
        int minDist = INT_MAX;
        
        for(auto &c : colors) {
            int dist = (r-c.r)*(r-c.r) + (g-c.g)*(g-c.g) + (b-c.b)*(b-c.b);
            if(dist < minDist) {
                minDist = dist;
                bestColor = c.color;
            }
        }
        return bestColor;
    }
};

ConsoleColor g_conc;
