#include<bits/stdc++.h>
#include<windows.h>
using namespace std;
#pragma once
// ANSI -> UTF-16
std::wstring AnsiToUtf16(const std::string& str, UINT codePage = CP_ACP) {
    int len = MultiByteToWideChar(codePage, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(len - 1, L'\0');
    MultiByteToWideChar(codePage, 0, str.c_str(), -1, &wstr[0], len);
    return wstr;
}

// UTF-16 -> ANSI
std::string Utf16ToAnsi(const std::wstring& wstr, UINT codePage = CP_ACP) {
    int len = WideCharToMultiByte(codePage, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(len - 1, '\0');
    WideCharToMultiByte(codePage, 0, wstr.c_str(), -1, &str[0], len, nullptr, nullptr);
    return str;
}

// UTF-8 -> UTF-16
std::wstring Utf8ToUtf16(const std::string& str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(len - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
    return wstr;
}

// UTF-16 -> UTF-8
std::string Utf16ToUtf8(const std::wstring& wstr) {
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(len - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], len, nullptr, nullptr);
    return str;
}

// ANSI -> UTF-8
std::string AnsiToUtf8(const std::string& str, UINT codePage = CP_ACP) {
    return Utf16ToUtf8(AnsiToUtf16(str, codePage));
}

// UTF-8 -> ANSI
std::string Utf8ToAnsi(const std::string& str, UINT codePage = CP_ACP) {
    return Utf16ToAnsi(Utf8ToUtf16(str), codePage);
}
