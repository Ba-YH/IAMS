//
// Created by 巴云浩 on 2024/4/30.
//

#ifndef ITEMAUCTIONMANAGER_RECORDS_H
#define ITEMAUCTIONMANAGER_RECORDS_H
#include "bits/stdc++.h"
#include "nlohmann/json.hpp"
#include "../util/output.h"

using namespace std;

class Record {
public:
    Record() {}

    ~Record() {}

    string getCurrentTime();  //获取当前时间片
    Record(int goodId, double finalPrice, string shooterName) :
            goodId(goodId), finalPrice(finalPrice), shooterName(shooterName) {
        time = getCurrentTime();
        this->setGoodId(goodId);
    }

    int getGoodId() const;

    void setGoodId(int goodId);

    const string &getTime() const;

    void setTime(const string &time);

    const double &getFinalPrice() const;

    void setFinalPrice(const double &finalPrice);

    const string &getShooterName() const;

    void setShooterName(const string &shooterName);

    //序列化与反序列化
    friend void to_json(nlohmann::json &j, const Record &r);

    friend void from_json(const nlohmann::json &j, Record &r);

    friend ostream &operator<<(ostream &os, const Record &r);

private:
    int goodId;
    string time;
    double finalPrice;
    string shooterName;

};
#endif //ITEMAUCTIONMANAGER_RECORDS_H
