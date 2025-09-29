#include <windows.h>
#include <bits/stdc++.h>
#pragma once
#include "editorcfg.cpp"

// 函数原型声明
bool setConsoleTmd(BYTE alpha);
bool setConsolePmwz(BYTE fg, BYTE bg);
bool setConsoleCtwz(BYTE fg, BYTE bg);

// 如果你的 SDK 没有 CONSOLE_SCREEN_BUFFER_INFOEX，就自己手动声明一个
/*#ifndef _CONSOLE_SCREEN_BUFFER_INFOEX
#define _CONSOLE_SCREEN_BUFFER_INFOEX
typedef struct {
    ULONG      cbSize;
    COORD      dwSize;
    COORD      dwCursorPosition;
    WORD       wAttributes;
    SMALL_RECT srWindow;
    COORD      dwMaximumWindowSize;
    WORD       wPopupAttributes;
    BOOL       bFullscreenSupported;
    COLORREF   ColorTable[16];
} CONSOLE_SCREEN_BUFFER_INFOEX;
#endif*/

// —————————————————————————————————————————————————————————————————————————————
// 1. 设置控制台窗口透明度（0 = 完全透明, 255 = 完全不透明）
//    返回 true 表示成功，false 表示失败。
//    注意：Windows 8 及以上的 conhost.exe 支持 WS_EX_LAYERED。
bool setConsoleTmd(BYTE alpha) {
	HWND hwnd = GetConsoleWindow();
	if (!hwnd) return false;

	// 给窗口加上 WS_EX_LAYERED 扩展样式
	LONG ex = GetWindowLong(hwnd, GWL_EXSTYLE);
	if (!(ex & WS_EX_LAYERED)) {
		SetWindowLong(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
	}

	// 应用 alpha 值
	// LWA_ALPHA 表示仅修改 alpha 通道
	return SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA) != 0;
}

//—————————————————————————————————————————————————————————————————————————————
// 工具：动态获取 Get/SetConsoleScreenBufferInfoEx（Vista+）
//—————————————————————————————————————————————————————————————————————————————
static auto ptrGetInfoEx = (decltype(&GetConsoleScreenBufferInfoEx))
                           GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetConsoleScreenBufferInfoEx");
static auto ptrSetInfoEx = (decltype(&SetConsoleScreenBufferInfoEx))
                           GetProcAddress(GetModuleHandleA("kernel32.dll"), "SetConsoleScreenBufferInfoEx");

// 2. 设置“屏幕文字”——修改 wAttributes 的前4位(fg)和高4位(bg)
bool setConsolePmwz(BYTE fg, BYTE bg) {
	if (!ptrGetInfoEx || !ptrSetInfoEx) return false;

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFOEX info = {};
	info.cbSize = sizeof(info);
	if (!ptrGetInfoEx(hOut, &info)) return false;

	// color 命令语法是：color <BG><FG>，两位16进制各代表一个颜色索引(0–F)
	fg &= 0x0F;  // 保证在 0–15
	bg &= 0x0F;
	info.wAttributes = (WORD)((bg << 4) | fg);

	return ptrSetInfoEx(hOut, &info) != FALSE;
}

// 3. 设置“弹出文字”——修改 wPopupAttributes 的前4位(fg)和高4位(bg)
bool setConsoleCtwz(BYTE fg, BYTE bg) {
	if (!ptrGetInfoEx || !ptrSetInfoEx) return false;

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFOEX info = {};
	info.cbSize = sizeof(info);
	if (!ptrGetInfoEx(hOut, &info)) return false;

	fg &= 0x0F;
	bg &= 0x0F;
	info.wPopupAttributes = (WORD)((bg << 4) | fg);

	return ptrSetInfoEx(hOut, &info) != FALSE;
}

namespace cex{
	void init(){
		cout<<"cex.init:init cex from setting\n";
		setConsoleTmd(g_cfg.getInt(100));
		setConsolePmwz(g_cfg.getInt(101),g_cfg.getInt(102));
		setConsoleCtwz(g_cfg.getInt(103),g_cfg.getInt(104));
	}
}
