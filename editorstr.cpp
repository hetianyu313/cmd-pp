#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
// 区分大小写的替换
std::string replaceAll(const std::string& str, 
                      const std::string& from, 
                      const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    while((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

// 不区分大小写的替换
std::string replaceAllIgnoreCase(std::string str,
                               const std::string& from,
                               const std::string& to) {
    std::string lowerStr = str;
    std::string lowerFrom = from;
    
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    std::transform(lowerFrom.begin(), lowerFrom.end(), lowerFrom.begin(), ::tolower);
    
    size_t pos = 0;
    while((pos = lowerStr.find(lowerFrom, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        lowerStr.replace(pos, from.length(), to);
        pos += to.length();
    }
    return str;
}
