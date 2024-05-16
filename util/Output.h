// Created by 巴云浩 on 2024/4/23.


#ifndef OUTPUT_H
#define OUTPUT_H

#include "string"
#include "utf8.h"
#include "iomanip"


int utf8len(std::string s);                                         //返回UTF8编码下的字符串长度
int countChineseCharacters(const std::string& text);                //获取字符串中中文字符的数量
void inline outputWarning(const std:: string &warning);             //当输入有误时，红色提示错误信息
void inline outputHint(const std::string &hint);                    //当功能顺利完成时，绿色提示成功信息
void inline outputHighlight(const std::string &hint);               //系统对用户的高亮提示消息
std::string transfer(std::string s, int maxLen);                    //通过中文括号填充为指定长度的字符串
std::string append(std::string s,int n);                            //字符串末尾填充若干中文括号
std::string double2str(double value,int reserveDigits);             //将浮点数保留reserveDigits位小数并转换为字符串
std::string int2str(int value,int digits);                          //将整数转换为指定位数的字符串，不足补前导0
std::string to8(std::string value);

inline int utf8len(std::string s) {
    return utf8::distance(s.begin(), s.end());
}
int countChineseCharacters(const std::string& text) {
    return (text.size()-utf8len(text))/2;
}
void inline outputWarning(const std::string &warning) {
    std::cout <<"❗"<< "\033[31m" << warning << "\033[0m";
}
void inline outputHint(const std::string &hint) {
    std::cout << "\033[32m" << hint << "\033[0m"<<std::endl;
}

void inline outputHighlight(const std::string &hint) {
    std::cout << "\033[33m" << hint << "\033[0m"<<std::endl;
}
inline std::string transfer(std::string s, int maxCount) {
    int codepoint = 12288;
    std::string chineseSpace;
    utf8::utf32to8(&codepoint, &codepoint + 1, std::back_inserter(chineseSpace));
    while (countChineseCharacters(s) < maxCount){
        s += chineseSpace;
    }
    return s;
}
inline std::string append(std::string s,int n){
    int codepoint = 12288;
    std::string chineseSpace;
    utf8::utf32to8(&codepoint, &codepoint + 1, std::back_inserter(chineseSpace));
    for(int i=1;i<=n;i++){
        s+=chineseSpace;
    }
    return s;
}

inline std::string double2str(double value,int reserveDigits) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(reserveDigits) << value;
    return oss.str();
}

inline std::string int2str(int value,int digits) {
    std::ostringstream oss;
    oss << std::setw(digits) << std::setfill('0') << value;
    return oss.str();
}

inline std::string to8(std::string value){
    std::string res;
    utf8::utf16to8(value.begin(), value.end(), std::back_inserter(res));
    return res;
}
#endif //物品竞拍管理_OUTPUT_H
