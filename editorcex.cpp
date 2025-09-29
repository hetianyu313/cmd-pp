#include <windows.h>
#include <bits/stdc++.h>
#pragma once
#include "editorcfg.cpp"

// ����ԭ������
bool setConsoleTmd(BYTE alpha);
bool setConsolePmwz(BYTE fg, BYTE bg);
bool setConsoleCtwz(BYTE fg, BYTE bg);

// ������ SDK û�� CONSOLE_SCREEN_BUFFER_INFOEX�����Լ��ֶ�����һ��
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

// ����������������������������������������������������������������������������������������������������������������������������������������������������������
// 1. ���ÿ���̨����͸���ȣ�0 = ��ȫ͸��, 255 = ��ȫ��͸����
//    ���� true ��ʾ�ɹ���false ��ʾʧ�ܡ�
//    ע�⣺Windows 8 �����ϵ� conhost.exe ֧�� WS_EX_LAYERED��
bool setConsoleTmd(BYTE alpha) {
	HWND hwnd = GetConsoleWindow();
	if (!hwnd) return false;

	// �����ڼ��� WS_EX_LAYERED ��չ��ʽ
	LONG ex = GetWindowLong(hwnd, GWL_EXSTYLE);
	if (!(ex & WS_EX_LAYERED)) {
		SetWindowLong(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
	}

	// Ӧ�� alpha ֵ
	// LWA_ALPHA ��ʾ���޸� alpha ͨ��
	return SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA) != 0;
}

//����������������������������������������������������������������������������������������������������������������������������������������������������������
// ���ߣ���̬��ȡ Get/SetConsoleScreenBufferInfoEx��Vista+��
//����������������������������������������������������������������������������������������������������������������������������������������������������������
static auto ptrGetInfoEx = (decltype(&GetConsoleScreenBufferInfoEx))
                           GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetConsoleScreenBufferInfoEx");
static auto ptrSetInfoEx = (decltype(&SetConsoleScreenBufferInfoEx))
                           GetProcAddress(GetModuleHandleA("kernel32.dll"), "SetConsoleScreenBufferInfoEx");

// 2. ���á���Ļ���֡������޸� wAttributes ��ǰ4λ(fg)�͸�4λ(bg)
bool setConsolePmwz(BYTE fg, BYTE bg) {
	if (!ptrGetInfoEx || !ptrSetInfoEx) return false;

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFOEX info = {};
	info.cbSize = sizeof(info);
	if (!ptrGetInfoEx(hOut, &info)) return false;

	// color �����﷨�ǣ�color <BG><FG>����λ16���Ƹ�����һ����ɫ����(0�CF)
	fg &= 0x0F;  // ��֤�� 0�C15
	bg &= 0x0F;
	info.wAttributes = (WORD)((bg << 4) | fg);

	return ptrSetInfoEx(hOut, &info) != FALSE;
}

// 3. ���á��������֡������޸� wPopupAttributes ��ǰ4λ(fg)�͸�4λ(bg)
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
