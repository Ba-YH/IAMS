#include "../include/Record.h"

ostream &operator<<(ostream &os, const Record &r) {
    os << left << setw(40) << append(r.getTime(), 4);
    os << left << setw(30) << append(r.getShooterName(), 3);
    os << left << setw(30) << append(double2str(r.getFinalPrice(), 2), 3);
    os << left << setw(30) << append(to_string(r.getGoodId()), 4);
    os << endl;
    return os;
}

void to_json(nlohmann::json &j, const Record &r) {
    j = nlohmann::json{
            {"成交时间",   r.getTime()},
            {"拍品编号",   r.getGoodId()},
            {"最终成交价", r.getFinalPrice()},
            {"中拍者",     r.getShooterName()}
    };
}

void from_json(const nlohmann::json &j, Record &r) {
    j.at("成交时间").get_to(r.time);
    j.at("拍品编号").get_to(r.goodId);
    j.at("最终成交价").get_to(r.finalPrice);
    j.at("中拍者").get_to(r.shooterName);
}

int Record::getGoodId() const {
    return goodId;
}

void Record::setGoodId(int goodId) {
    Record::goodId = goodId;
}

const string &Record::getTime() const {
    return time;
}

void Record::setTime(const string &time) {
    Record::time = time;
}

const double &Record::getFinalPrice() const {
    return finalPrice;
}

void Record::setFinalPrice(const double &finalPrice) {
    Record::finalPrice = finalPrice;
}

const string &Record::getShooterName() const {
    return shooterName;
}

void Record::setShooterName(const string &shooterName) {
    Record::shooterName = shooterName;
}

string Record::getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    char timestamp[20];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localTime);
    return std::string(timestamp);
}