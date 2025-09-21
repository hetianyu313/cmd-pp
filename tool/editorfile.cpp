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
    // �ص�������ͨ������Ҫʵ�֣���������Ҫ�����������
    return FALSE;
}
string getsavefilename(char * filt) {
    OPENFILENAME ofn;       // ���� OPENFILENAME �ṹ��
    char szFile[260];       // ���ڱ����ļ����Ļ�����
    ZeroMemory(&ofn, sizeof(ofn));
    szFile[0] = '\0';
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // ����ָ�������ھ����������ΪNULL��ʾ�޸�����
    ofn.lpstrFile = szFile; // ָ���ļ���������
    ofn.lpstrFile[0] = '\0'; // ��ʼ��Ϊ���ַ���
    ofn.nMaxFile = sizeof(szFile); // ��������С
    ofn.lpstrFilter = filt; // �ļ����͹�����
    ofn.nFilterIndex = 1; // ����Ĭ�ϵ��ļ����͹���������
    ofn.lpstrFileTitle = NULL; // ����Ҫ�ļ������⣬��ΪNULL
    ofn.lpstrInitialDir = NULL; // ��ʼĿ¼����ΪNULL��ʾʹ��Ĭ��Ŀ¼
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; // ���öԻ���ѡ��
    // ��������������ѡ����� OFN_EXPLORER ��
    if (GetSaveFileName(&ofn)) { // ��ʾ�Ի��򲢻�ȡ���
        return std::string(szFile); // �����û�ѡ����ļ���
    } else {
        return ""; // �û�ȡ�������ʱ���ؿ��ַ���
    }
}
string getopenfilename(char * filt) {
    OPENFILENAME ofn;       // ���� OPENFILENAME �ṹ��
    char szFile[260];       // ���ڱ����ļ����Ļ�����
    ZeroMemory(&ofn, sizeof(ofn));
    szFile[0] = '\0';
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // ����ָ�������ھ����������ΪNULL��ʾ�޸�����
    ofn.lpstrFile = szFile; // ָ���ļ���������
    ofn.lpstrFile[0] = '\0'; // ��ʼ��Ϊ���ַ���
    ofn.nMaxFile = sizeof(szFile); // ��������С
    ofn.lpstrFilter = filt; // �ļ����͹�����
    ofn.nFilterIndex = 1; // ����Ĭ�ϵ��ļ����͹���������
    ofn.lpstrFileTitle = NULL; // ����Ҫ�ļ������⣬��ΪNULL
    ofn.lpstrInitialDir = NULL; // ��ʼĿ¼����ΪNULL��ʾʹ��Ĭ��Ŀ¼
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; // ���öԻ���ѡ��
    // ��������������ѡ����� OFN_EXPLORER ��
    if (GetOpenFileName(&ofn)) { // ��ʾ�Ի��򲢻�ȡ���
        return std::string(szFile); // �����û�ѡ����ļ���
    } else {
        return ""; // �û�ȡ�������ʱ���ؿ��ַ���
    }
}

class fileread {
public:
    fileread() : m_binary(true) {}
    ~fileread() { close(); }

    // ���ļ���binary = true ���Զ����Ʒ�ʽ��
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

    // �ر��ļ�
    void close() {
        if (m_if.is_open()) m_if.close();
        m_path.clear();
    }

    // �Ƿ��
    bool is_open() const {
        return m_if.is_open();
    }

    // ��ȡ�����ļ����ݣ����ļ�ͷ��ʼ�������� vector<char>
    std::vector<char> readbuf() {
        std::vector<char> buf;
        if (!m_if.is_open()) return buf;

        // ��ȡ�ļ�����
        std::streampos cur = m_if.tellg();
        m_if.seekg(0, std::ios::end);
        std::streampos end = m_if.tellg();
        if (end <= 0) {
            // ���ļ������
            m_if.clear(); // ��� eof ��־��
            m_if.seekg(cur); // ���Իָ�λ��
            return buf;
        }

        std::size_t len = static_cast<std::size_t>(end);
        buf.resize(len);

        m_if.seekg(0, std::ios::beg);
        if (len > 0) {
            m_if.read(&buf[0], static_cast<std::streamsize>(len));
            // �����ȡʧ�ܣ�����ʵ�ʶ�ȡ���٣������ص� vector ���������СΪ׼��
            std::streamsize got = m_if.gcount();
            if (static_cast<std::size_t>(got) != len) {
                buf.resize(static_cast<std::size_t>(got));
            }
        }

        // �ָ�ԭ����λ�ã���ѡ��readbuf ���ļ�ͷ��ȡ�Ὣָ���Ƶ��ļ�ĩ��
        // ���ﲻ�ָ��� cur���Ա��⸱���ã�����Ҫ����ȡ��ע���������лָ�ԭָ�룺
        // m_if.clear();
        // m_if.seekg(cur);

        return buf;
    }

    // �ӵ�ǰ�ļ�λ�ö�ȡ��� reqSize �ֽڣ�ʵ�ʶ�ȡ = min(ʣ��, reqSize)��
    std::vector<char> readsiz(std::size_t reqSize) {
        std::vector<char> buf;
        if (!m_if.is_open() || reqSize == 0) return buf;

        // ��ǰ pos
        std::streampos cur = m_if.tellg();
        if (cur == static_cast<std::streampos>(-1)) {
            // ��� tellg ʧ�ܣ���������Ϊ�ļ���ͷ
            m_if.clear();
            m_if.seekg(0, std::ios::beg);
            cur = m_if.tellg();
            if (cur == static_cast<std::streampos>(-1)) return buf;
        }

        // �ļ�ĩβλ��
        m_if.seekg(0, std::ios::end);
        std::streampos end = m_if.tellg();
        if (end == static_cast<std::streampos>(-1) || end <= cur) {
            // ����ĩβ�����
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
    
    //�����ļ�·��
    std::string getpath(){
    	return m_path;
	}

    // �����ļ��ܳ��ȣ��ֽڣ�
    std::size_t length() {
        if (!m_if.is_open()) return 0;
        std::streampos cur = m_if.tellg();
        m_if.seekg(0, std::ios::end);
        std::streampos end = m_if.tellg();
        if (end == static_cast<std::streampos>(-1)) {
            // ����
            m_if.clear();
            if (cur != static_cast<std::streampos>(-1)) m_if.seekg(cur);
            return 0;
        }
        std::size_t len = static_cast<std::size_t>(end);
        // �ָ���ǰλ��
        if (cur != static_cast<std::streampos>(-1)) {
            m_if.seekg(cur);
        } else {
            m_if.clear();
        }
        return len;
    }

    // �ԶԻ�����ʽѡ���ļ����򿪣������ⲿ�ṩ�� getopenfilename��
    // filt ��Ϊ nullptr ��ʹ��Ĭ�� "All\0*.*\0\0"
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
        // ���CRLF���з�
        if (charVec[i] == '\r' && i + 1 < charVec.size() && charVec[i+1] == '\n') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
            i++; // ������һ���ַ�(\n)
        }
        // ���LF���з�
        else if (charVec[i] == '\n') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += charVec[i];
        }
    }

    // ������һ��(���û���Ի��з���β)
    if (!current.empty()) {
        result.push_back(current);
    }

    return result;
}
