//
// Created by 巴云浩 on 2024/4/30.
//

#ifndef ITEMAUCTIONMANAGER_USER_H
#define ITEMAUCTIONMANAGER_USER_H

#include "bits/stdc++.h"
#include "nlohmann/json.hpp"
#include "../util/Output.h"
using namespace std;

const vector<string> categories={"艺术品", "古董与收藏品", "房地产", "文娱和体育纪念品", "奢侈品", "金银珠宝","虚拟产品"};
class User{
public:
    User(){}
    ~User(){}
    // 无参构造函数
    User(const string &username, const string &password) :
        username(username), password(password),isAvailable(true), sucessRate(0.00), countOfSessions(0),countOfSuccesses(0){
        for(auto &c : categories){
            difCategoryCnt[c] = 0;
        }
    }

    // 拷贝构造函数
    User(const User &other) :
        username(other.username),
        password(other.password),
        sucessRate(other.sucessRate),
        isAvailable(other.isAvailable),
        countOfSessions(other.countOfSessions),
        countOfSuccesses(other.countOfSuccesses),
        difCategoryCnt(other.difCategoryCnt){}



    friend void to_json(nlohmann::json &j, const User &u);
    friend void from_json(const nlohmann::json &j, User &u);

    string username;
    string password;
    double sucessRate{};
    int countOfSessions{};
    int countOfSuccesses{};
    map<string,int> difCategoryCnt;
    map<string,double> difCategorySucessRate;
    bool isAvailable;  //默认可用
};
#endif //ITEMAUCTIONMANAGER_USER_H
