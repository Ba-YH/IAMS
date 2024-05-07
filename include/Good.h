//
// Created by 巴云浩 on 2024/4/30.
//

#ifndef ITEMAUCTIONMANAGER_GOOD_H
#define ITEMAUCTIONMANAGER_GOOD_H
//物品类
#include "utf8.h"
#include "bits/stdc++.h"
#include "nlohmann/json.hpp"
#include "../util/output.h"

using namespace std;

class Good {
public:
    //静态成员变量，为了格式化输出
    static int maxNameCount;
    static int maxDescribeCount;
    static int maxCategoryCount;

    Good() {}

    ~Good() {}

    //有参构造函数
    Good(const string &goodName, double degreeOfImpairment, const string &describe, double appraisal,
         const string &category);

    //拷贝构造函数，浅拷贝
    Good(const Good &other)
            : goodName(other.goodName),
              degreeOfImpairment(other.degreeOfImpairment),
              describe(other.describe),
              appraisal(other.appraisal),
              category(other.category) ,
              uploader(other.uploader),
              gid(other.gid){}

    //getter和setter
    const string &getGoodName() const;

    void setGoodName(const string &goodName);

    double getDegreeOfImpairment() const;

    void setDegreeOfImpairment(double degreeOfImpairment);

    const string &getDescribe() const;

    void setDescribe(const string &describe);

    double getAppraisal() const;

    void setAppraisal(double appraisal);

    const string &getCategory() const;

    void setCategory(const string &category);

    int getId() const;

    void setId(int id);

    //声明为友元函数，便于访问私有成员   序列化与反序列化
    friend void to_json(nlohmann::json &j, const Good &g);

    friend void from_json(const nlohmann::json &j, Good &g);

    //重载流输出函数
    friend ostream &operator<<(ostream &os, const Good &good);

    void toString(){};


    const string &getUploader() const;

    void setUploader(const string &uploader);

private:
    string goodName;
    double degreeOfImpairment{};
    string describe;
    double appraisal{};
    string category;
    string uploader;
    int gid;
};

#endif //ITEMAUCTIONMANAGER_GOOD_H
