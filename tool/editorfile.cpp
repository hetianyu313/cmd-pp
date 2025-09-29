#include<bits/stdc++.h>
#include <windows.h>
#include <commdlg.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstddef>
#include <iostream>//-lcomdlg32 -lcomctl32
#pragma once
#pragma comment(lib, "Comdlg32.lib")
#pragma comment(lib, "comctl32.lib")
using namespace std;
BOOL CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData){
    // 回调函数，通常不需要实现，除非你需要处理特殊情况
    return FALSE;
}
string getsavefilename(char * filt) {
    OPENFILENAME ofn;       // 创建 OPENFILENAME 结构体
    char szFile[260];       // 用于保存文件名的缓冲区
    ZeroMemory(&ofn, sizeof(ofn));
    szFile[0] = '\0';
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // 可以指定父窗口句柄，这里设为NULL表示无父窗口
    ofn.lpstrFile = szFile; // 指向文件名缓冲区
    ofn.lpstrFile[0] = '\0'; // 初始化为空字符串
    ofn.nMaxFile = sizeof(szFile); // 缓冲区大小
    ofn.lpstrFilter = filt; // 文件类型过滤器
    ofn.nFilterIndex = 1; // 设置默认的文件类型过滤器索引
    ofn.lpstrFileTitle = NULL; // 不需要文件名标题，设为NULL
    ofn.lpstrInitialDir = NULL; // 初始目录，设为NULL表示使用默认目录
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; // 设置对话框选项
    // 这里可以添加其他选项，例如 OFN_EXPLORER 等
    if (GetSaveFileName(&ofn)) { // 显示对话框并获取结果
        return std::string(szFile); // 返回用户选择的文件名
    } else {
        return ""; // 用户取消或出错时返回空字符串
    }
}
string getopenfilename(char * filt) {
    OPENFILENAME ofn;       // 创建 OPENFILENAME 结构体
    char szFile[260];       // 用于保存文件名的缓冲区
    ZeroMemory(&ofn, sizeof(ofn));
    szFile[0] = '\0';
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // 可以指定父窗口句柄，这里设为NULL表示无父窗口
    ofn.lpstrFile = szFile; // 指向文件名缓冲区
    ofn.lpstrFile[0] = '\0'; // 初始化为空字符串
    ofn.nMaxFile = sizeof(szFile); // 缓冲区大小
    ofn.lpstrFilter = filt; // 文件类型过滤器
    ofn.nFilterIndex = 1; // 设置默认的文件类型过滤器索引
    ofn.lpstrFileTitle = NULL; // 不需要文件名标题，设为NULL
    ofn.lpstrInitialDir = NULL; // 初始目录，设为NULL表示使用默认目录
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; // 设置对话框选项
    // 这里可以添加其他选项，例如 OFN_EXPLORER 等
    if (GetOpenFileName(&ofn)) { // 显示对话框并获取结果
        return std::string(szFile); // 返回用户选择的文件名
    } else {
        return ""; // 用户取消或出错时返回空字符串
    }
}

class fileread {
public:
    fileread() : m_binary(true) {}
    ~fileread() { close(); }

    // 打开文件，binary = true 则以二进制方式打开
    bool open(const std::string &path, bool binary = true) {
        close();
        std::ios::openmode mode = std::ios::in;
        if (binary) mode |= std::ios::binary;
        m_if.open(path.c_str(), mode);
        if (!m_if.is_open()) {
            m_path.clear();
            m_binary = true;
            return false;
        }
        m_path = path;
        m_binary = binary;
        return true;
    }

    // 关闭文件
    void close() {
        if (m_if.is_open()) m_if.close();
        m_path.clear();
    }

    // 是否打开
    bool is_open() const {
        return m_if.is_open();
    }

    // 读取整个文件内容（从文件头开始），返回 vector<char>
    std::vector<char> readbuf() {
        std::vector<char> buf;
        if (!m_if.is_open()) return buf;

        // 获取文件长度
        std::streampos cur = m_if.tellg();
        m_if.seekg(0, std::ios::end);
        std::streampos end = m_if.tellg();
        if (end <= 0) {
            // 空文件或错误
            m_if.clear(); // 清除 eof 标志等
            m_if.seekg(cur); // 尝试恢复位置
            return buf;
        }

        std::size_t len = static_cast<std::size_t>(end);
        buf.resize(len);

        m_if.seekg(0, std::ios::beg);
        if (len > 0) {
            m_if.read(&buf[0], static_cast<std::streamsize>(len));
            // 如果读取失败，可能实际读取更少，但返回的 vector 以所请求大小为准。
            std::streamsize got = m_if.gcount();
            if (static_cast<std::size_t>(got) != len) {
                buf.resize(static_cast<std::size_t>(got));
            }
        }

        // 恢复原来的位置（可选，readbuf 从文件头读取会将指针移到文件末）
        // 这里不恢复到 cur，以避免副作用；若需要可以取消注释下面两行恢复原指针：
        // m_if.clear();
        // m_if.seekg(cur);

        return buf;
    }

    // 从当前文件位置读取最多 reqSize 字节（实际读取 = min(剩余, reqSize)）
    std::vector<char> readsiz(std::size_t reqSize) {
        std::vector<char> buf;
        if (!m_if.is_open() || reqSize == 0) return buf;

        // 当前 pos
        std::streampos cur = m_if.tellg();
        if (cur == static_cast<std::streampos>(-1)) {
            // 如果 tellg 失败，尝试设置为文件开头
            m_if.clear();
            m_if.seekg(0, std::ios::beg);
            cur = m_if.tellg();
            if (cur == static_cast<std::streampos>(-1)) return buf;
        }

        // 文件末尾位置
        m_if.seekg(0, std::ios::end);
        std::streampos end = m_if.tellg();
        if (end == static_cast<std::streampos>(-1) || end <= cur) {
            // 到达末尾或错误
            m_if.clear();
            m_if.seekg(cur);
            return buf;
        }

        std::size_t remaining = static_cast<std::size_t>(end - cur);
        std::size_t toRead = std::min(remaining, reqSize);
        if (toRead == 0) {
            m_if.clear();
            m_if.seekg(cur);
            return buf;
        }

        buf.resize(toRead);
        m_if.seekg(cur);
        m_if.read(&buf[0], static_cast<std::streamsize>(toRead));
        std::streamsize actually = m_if.gcount();
        if (static_cast<std::size_t>(actually) != toRead) {
            buf.resize(static_cast<std::size_t>(actually));
        }

        return buf;
    }
    
    //返回文件路径
    std::string getpath(){
    	return m_path;
	}

    // 返回文件总长度（字节）
    std::size_t length() {
        if (!m_if.is_open()) return 0;
        std::streampos cur = m_if.tellg();
        m_if.seekg(0, std::ios::end);
        std::streampos end = m_if.tellg();
        if (end == static_cast<std::streampos>(-1)) {
            // 错误
            m_if.clear();
            if (cur != static_cast<std::streampos>(-1)) m_if.seekg(cur);
            return 0;
        }
        std::size_t len = static_cast<std::size_t>(end);
        // 恢复当前位置
        if (cur != static_cast<std::streampos>(-1)) {
            m_if.seekg(cur);
        } else {
            m_if.clear();
        }
        return len;
    }

    // 以对话框形式选择文件并打开（调用外部提供的 getopenfilename）
    // filt 若为 nullptr 则使用默认 "All\0*.*\0\0"
    bool winopen(bool binary = true, char *filt = nullptr) {
        static char defaultFilter[] = "All files\0*.*\0\0";
        char *useFilter = filt ? filt : defaultFilter;
        std::string path = getopenfilename(useFilter);
        if (path.empty()) return false;
        return open(path, binary);
    }

private:
    std::ifstream m_if;
    std::string m_path;
    bool m_binary;
};
std::vector<std::string> convertVectorCharToString(const std::vector<char>& charVec) {
    std::vector<std::string> result;
    std::string current;

    for (size_t i = 0; i < charVec.size(); ++i) {
        // 检查CRLF换行符
        if (charVec[i] == '\r' && i + 1 < charVec.size() && charVec[i+1] == '\n') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
            i++; // 跳过下一个字符(\n)
        }
        // 检查LF换行符
        else if (charVec[i] == '\n') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += charVec[i];
        }
    }

    // 添加最后一行(如果没有以换行符结尾)
    if (!current.empty()) {
        result.push_back(current);
    }

    return result;
}

class filewrite {
public:
    filewrite() = default;
    explicit filewrite(const std::string& path, bool binary = true) {
        open(path, binary);
    }
    ~filewrite() {
        close();
    }
    bool open(const std::string& path, bool binary = true) {
        close();
        std::ios::openmode mode = std::ios::out;
        if (binary) mode |= std::ios::binary;
        ofs_.open(path, mode);
        if (ofs_.is_open()) {
            path_ = path;
            return true;
        } else {
            path_.clear();
            return false;
        }
    }
    bool winopen(char* filt, bool binary = true) {
        std::string chosen = getopenfilename(filt);
        if (chosen.empty()) return false;
        return open(chosen, binary);
    }

    void close() {
        if (ofs_.is_open()) {
            ofs_.close();
        }
        path_.clear();
    }

    bool is_open() const {
        return ofs_.is_open();
    }

    bool writebuf(const std::vector<char>& buf) {
        if (!is_open()) return false;
        if (buf.empty()) return true;
        ofs_.write(buf.data(), static_cast<std::streamsize>(buf.size()));
        return static_cast<bool>(ofs_);
    }
    bool writestr(const std::vector<std::string>& strs, const std::string& sep = "\n") {
        if (!is_open()) return false;
        for (size_t i = 0; i < strs.size(); ++i) {
            const std::string& s = strs[i];
            if (!s.empty()) {
                ofs_.write(s.data(), static_cast<std::streamsize>(s.size()));
                if (!ofs_) return false;
            }
            if (!sep.empty() && i + 1 < strs.size()) {
                ofs_.write(sep.data(), static_cast<std::streamsize>(sep.size()));
                if (!ofs_) return false;
            }
        }
        return static_cast<bool>(ofs_);
    }
    std::string getpath() const {
        return path_;
    }

private:
    std::ofstream ofs_;
    std::string path_;
};
