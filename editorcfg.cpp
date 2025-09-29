#pragma once

#include <windows.h>        // for COLORREF, RGB
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <cctype>
#include <iostream>

class Settings {
public:
    Settings() = default;
    ~Settings() { if (m_ifs.is_open()) m_ifs.close(); }

    // 打开配置文件
    bool open(const std::string& filename) {
        clear();
        m_ifs.open(filename);
        return m_ifs.is_open();
    }

    // 读取并解析
    void read(bool ts = true) {
        if (!m_ifs.is_open()) return;
        std::string line;
        while (std::getline(m_ifs, line)) {
            trim(line);
            if (line.empty() || line.front() == '#')
                continue;

            std::istringstream iss(line);
            std::string type;
            if (!(iss >> type))
                continue;
            std::string rest;
            std::getline(iss, rest);
            trim(rest);

            if (type == "COLOR") {
                parseColor(rest, ts);
                continue;
            }

            // 解析 INT/FLOAT/STR
            auto eq = rest.find('=');
            if (eq == std::string::npos)
                continue;
            std::string keyStr = rest.substr(0, eq);
            std::string valStr = rest.substr(eq + 1);
            trim(keyStr);
            trim(valStr);
            if (keyStr.empty() || valStr.empty())
                continue;

            int key = 0;
            try { key = std::stoi(keyStr); }
            catch (...) { continue; }

            if (type == "INT") {
                try {
                    m_ints[key] = std::stoi(valStr);
                    if (ts) std::cout << "cfg: INT key="<<key<<","<<valStr<<"\n";
                } catch(...) {}
            }
            else if (type == "FLOAT") {
                try {
                    m_floats[key] = std::stof(valStr);
                    if (ts) std::cout << "cfg: FLOAT key="<<key<<","<<valStr<<"\n";
                } catch(...) {}
            }
            else if (type == "STR") {
                m_strs[key] = valStr;
                if (ts) std::cout << "cfg: STR key="<<key<<","<<valStr<<"\n";
            }
            // else: 忽略未知 type
        }
        m_ifs.close();
    }

    int getInt(int key) const {
        auto it = m_ints.find(key);
        return it == m_ints.end() ? 0 : it->second;
    }

    float getFloat(int key) const {
        auto it = m_floats.find(key);
        return it == m_floats.end() ? 0.0f : it->second;
    }

    std::string getStr(int key) const {
        auto it = m_strs.find(key);
        if (it != m_strs.end())
            return it->second;
        return "[missing:" + std::to_string(key) + "]";
    }

    // 新增：获取 COLORREF，默认返回黑色 (RGB(0,0,0))
    COLORREF getColor(int key) const {
        auto it = m_colors.find(key);
        return it == m_colors.end() ? RGB(0,0,0) : it->second;
    }

private:
    // 清理状态
    void clear() {
        if (m_ifs.is_open()) m_ifs.close();
        m_ints.clear();
        m_floats.clear();
        m_strs.clear();
        m_colors.clear();
    }

    // 去掉两端空白
    static void trim(std::string& s) {
        auto is_space = [](char c){ return std::isspace(static_cast<unsigned char>(c)); };
        while (!s.empty() && is_space(s.front())) s.erase(s.begin());
        while (!s.empty() && is_space(s.back()))  s.pop_back();
    }

    // 解析 COLOR 行
    void parseColor(const std::string& rest, bool ts) {
        // rest 示例： "BYTE 1=255,0,128" 或 "HEX 2=#00FF7F"
        std::istringstream iss(rest);
        std::string subType;
        if (!(iss >> subType)) return;

        std::string tail;
        std::getline(iss, tail);
        trim(tail);
        auto eq = tail.find('=');
        if (eq == std::string::npos) return;

        std::string keyStr = tail.substr(0, eq);
        std::string valStr = tail.substr(eq + 1);
        trim(keyStr);
        trim(valStr);
        if (keyStr.empty() || valStr.empty()) return;

        int key = 0;
        try { key = std::stoi(keyStr); }
        catch(...) { return; }

        COLORREF color = RGB(0,0,0);
        if (subType == "BYTE") {
            // 格式 R,G,B
            int r, g, b;
            char comma1, comma2;
            std::istringstream vs(valStr);
            if (vs >> r >> comma1 >> g >> comma2 >> b
                && comma1==',' && comma2==','
                && (0<=r&&r<=255) && (0<=g&&g<=255) && (0<=b&&b<=255)) {
                color = RGB(r,g,b);
            }
        }
        else if (subType == "HEX") {
            // 格式 #RRGGBB
            if (valStr.size() == 7 && valStr[0]=='#') {
                unsigned long u = 0;
                try {
                    u = std::stoul(valStr.substr(1), nullptr, 16);
                    int r = (u >> 16) & 0xFF;
                    int g = (u >> 8 ) & 0xFF;
                    int b = (u      ) & 0xFF;
                    color = RGB(r,g,b);
                } catch(...) {}
            }
        }

        m_colors[key] = color;
        if (ts) {
            std::cout << "cfg: COLOR " << subType
                      << " key="<<key<<",0x"
                      << std::hex << std::uppercase
                      << ((color & 0xFF)<<16 | (color & 0xFF00) | ((color>>16)&0xFF))
                      << std::dec << "\n";
        }
    }

    std::ifstream                       m_ifs;
    std::unordered_map<int,int>         m_ints;
    std::unordered_map<int,float>       m_floats;
    std::unordered_map<int,std::string> m_strs;
    std::unordered_map<int,COLORREF>    m_colors;
};
#include "editorapi.cpp"
Settings g_cfg;
namespace cfg{
	void init(){
		string fn = exedir+"setting\\cfg.ini";
		cout<<"cfg.init:open config file "<<fn<<endl;
		g_cfg.open(fn);
		g_cfg.read();
	}
}
