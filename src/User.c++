#include "../include/User.h"
using namespace std;

void to_json(nlohmann::json &j, const User &u){
    j = {
            {"用户名", u.username},
            {"密码", u.password},
            {"竞拍成功率", u.sucessRate},
            {"参与竞拍次数", u.countOfSessions},
            {"不同种类拍品成功次数", u.difCategoryCnt}
    };
}
void from_json(const nlohmann::json &j, User &u){
    u.username = j.at("用户名").get<string>();
    u.password = j.at("密码").get<string>();
    u.sucessRate = j.at("竞拍成功率").get<double>();
    u.countOfSessions = j.at("参与竞拍次数").get<int>();
    u.difCategoryCnt = j.at("不同种类拍品成功次数").get<map<string,int>>();
}

