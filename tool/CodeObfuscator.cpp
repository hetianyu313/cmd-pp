#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <random>
using namespace std;
mt19937 rand_main;
string generate_random_name(){
	string name;
    name += 'a'+(rand_main()%26);
    for (int i = 0; i < 9; ++i) {
        int val = rand_main()%36;
        name += (val < 10) ? ('0' + val) : ('a' + (val - 10));
    }
    return name;
}
bool is_valid_identifier(const string& token) {
    if (token.empty()) return false;
    if (!isalpha(token[0]) && token[0] != '_') return false;
    return all_of(token.begin(), token.end(), [](char c) {
        return isalnum(c) || c == '_';
    });
}
void process_content(string& content) {
    map<string, string> name_map;
    stringstream result;
    bool in_string = false;
    bool in_comment = false;
    bool in_macro = false;
    bool in_preprocessor = false;
    string current_token;
    char last_char = 0;
    char quote_char = 0;

    // 新增行状态
    int current_line_length = 0; 
    bool current_line_macro = false; 
    bool line_first_nonspace_found = false;

    auto is_unescaped_quote = [&](size_t pos) {
        int backslash_count = 0;
        for (int j = (int)pos - 1; j >= 0 && content[j] == '\\'; --j) {
            backslash_count++;
        }
        // 偶数表示未转义的引号
        return (backslash_count % 2 == 0);
    };

    for (size_t i = 0; i < content.size(); ++i) {
        char c = content[i];

        // 处理字符串或字符常量
        if ((c == '"' || c == '\'') && is_unescaped_quote(i)) {
            if (!in_string && !in_comment && !in_macro) {
                // 输出已有 token
                if (!current_token.empty()) {
                    if (is_valid_identifier(current_token)) {
                        if (name_map.find(current_token) == name_map.end()) {
                            name_map[current_token] = generate_random_name();
                        }
                        result << name_map[current_token];
                        current_line_length += name_map[current_token].size();
                    } else {
                        result << current_token;
                        current_line_length += current_token.size();
                    }
                    current_token.clear();
                }
                in_string = true;
                quote_char = c;
            }
            else if (in_string && c == quote_char) {
                in_string = false;
            }
            result << c;
            current_line_length++;
            continue;
        }
        if (in_string) {
            result << c;
            current_line_length++;
            continue;
        }

        // 处理注释
        if (!in_macro && c == '/' && i+1 < content.size()) {
            if (content[i+1] == '/' || content[i+1] == '*') {
                in_comment = true;
            }
        }
        if (in_comment) {
            result << c;
            current_line_length++;
            if (c == '\n') {
                in_comment = false;
                current_line_length = 0;
                current_line_macro = false;
                line_first_nonspace_found = false;
            } else if (c == '*' && i+1 < content.size() && content[i+1] == '/') {
                result << content[++i];
                current_line_length++;
                in_comment = false;
            }
            continue;
        }

        // 处理宏/预处理指令
        if (c == '#' && !in_string && !in_comment) {
            if (!line_first_nonspace_found) {
                current_line_macro = true;
            }
            in_macro = true;
            in_preprocessor = true;
            if (!current_token.empty()) {
                result << current_token;
                current_line_length += current_token.size();
                current_token.clear();
            }
        }
        if (in_macro) {
            result << c;
            current_line_length++;
            if (c == '\n') {
                in_macro = false;
                in_preprocessor = false;
                current_line_length = 0;
                current_line_macro = false;
                line_first_nonspace_found = false;
            }
            continue;
        }

        // 普通 token 输出
        if (isalnum(c) || c == '_') {
            current_token += c;
        } else {
            // 先输出 token
            if (!current_token.empty()) {
                if (is_valid_identifier(current_token)) {
                    if (name_map.find(current_token) == name_map.end()) {
                        name_map[current_token] = generate_random_name();
                    }
                    result << name_map[current_token];
                    current_line_length += name_map[current_token].size();
                } else {
                    result << current_token;
                    current_line_length += current_token.size();
                }
                current_token.clear();
            }

            if (isspace(c)) {
                if (c == '\n') {
                    // 判断是否折叠
                    if (current_line_length <= 150 && !current_line_macro) {
                        if (last_char != ' ' && last_char != '\n') {
                            result << ' ';
                            current_line_length += 1;
                        }
                        // 不真正换行
                    } else {
                        result << '\n';
                        current_line_length = 0;
                        current_line_macro = false;
                        line_first_nonspace_found = false;
                    }
                } else { // 空格或 Tab
                    if (last_char != ' ' && last_char != '\n') {
                        result << ' ';
                        current_line_length += 1;
                    }
                }
            } else {
                result << c;
                current_line_length++;
                if (!line_first_nonspace_found && !isspace(c)) {
                    line_first_nonspace_found = true;
                    if (c == '#') {
                        current_line_macro = true;
                    }
                }
            }
        }
        last_char = c;
    }

    // 输出最后的 token
    if (!current_token.empty()) {
        if (is_valid_identifier(current_token)) {
            if (name_map.find(current_token) == name_map.end()) {
                name_map[current_token] = generate_random_name();
            }
            result << name_map[current_token];
        } else {
            result << current_token;
        }
    }

    cout << "map size:" << name_map.size() << endl;

    stringstream ss1;
    for (auto &it : name_map) {
        ss1 << "#define " << it.second << " " << it.first << "\n";
    }
    content = ss1.str() + result.str();
}
int main(int argc, char* argv[]) {
	rand_main = mt19937(time(0));
	string s1,s2;
	cout<<"CodeObfuscator\nby zfworld hty\n";
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        cout<<"输入文件名 输出文件名 分两行输入：\n";
        getline(cin,s1);
        getline(cin,s2);
    }
    else{
    	s1 = argv[1];
    	s2 = argv[2];
	}
    ifstream in_file(s1.c_str());
    if (!in_file) {
        cerr << "Error opening input file" << endl;
        return 1;
    }
    string content((istreambuf_iterator<char>(in_file)), 
                   istreambuf_iterator<char>());
    in_file.close();
    process_content(content);
    ofstream out_file(s2.c_str());
    if (!out_file) {
        cerr << "Error opening output file" << endl;
        return 1;
    }
    out_file << content;
    out_file.close();
    cout << "Obfuscation completed successfully" << endl;
    return 0;
}

