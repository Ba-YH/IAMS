//物品类
#include "../include/Good.h"

using namespace std;

void toString(Good good) {
    std::ostringstream oss;
    oss << "物品名称：" << good.getGoodName() << endl;
    oss << "物品损坏程度：" << double2str(good.getDegreeOfImpairment(), 2) << endl;
    oss << "物品描述：" << good.getDescribe() << endl;
    oss << "物品价值：" << double2str(good.getAppraisal(), 4) << endl;
    oss << "物品分类：" << good.getCategory() << endl;
    cout << "\033[33m" << oss.str() << "\033[0m" << "\n\n";
}

//序列化方法
void to_json(nlohmann::json &j, const Good &g) {
    j = nlohmann::json{
            {"物品名称",     g.goodName},
            {"物品损坏程度", g.degreeOfImpairment},
            {"物品描述",     g.describe},
            {"物品价值",     g.appraisal},
            {"物品分类",     g.category},
            {"上传用户",       g.uploader},
            {"物品编号",     g.gid}
    };
}

// 反序列化方法
void from_json(const nlohmann::json &j, Good &g) {
    g.goodName = j.at("物品名称").get<string>();
    g.degreeOfImpairment = j.at("物品损坏程度").get<double>();
    g.describe = j.at("物品描述").get<string>();
    g.appraisal = j.at("物品价值").get<double>();
    g.category = j.at("物品分类").get<string>();
    g.uploader = j.at("上传用户").get<string>();
    g.gid = j.at("物品编号").get<int>();
}

void toString() {

}

//重载输出流
std::ostream &operator<<(ostream &os, const Good &g) {
    os << setw(30) << transfer(g.goodName, Good::maxNameCount);
    os << setw(20) << append(double2str(g.degreeOfImpairment, 2), 4);
    os << setw(50) << transfer(g.describe, Good::maxDescribeCount);
    os << setw(30) << append(double2str(g.appraisal, 4), 6);
    os << setw(30) << transfer(g.category, Good::maxCategoryCount);
    os << setw(20) << append(g.uploader, 4);
    return os;
}

const string &Good::getGoodName() const {
    return goodName;
}

void Good::setGoodName(const string &goodName) {
    Good::goodName = goodName;
}

double Good::getDegreeOfImpairment() const {
    return degreeOfImpairment;
}

void Good::setDegreeOfImpairment(double degreeOfImpairment) {
    Good::degreeOfImpairment = degreeOfImpairment;
}

const string &Good::getDescribe() const {
    return describe;
}

void Good::setDescribe(const string &describe) {
    Good::describe = describe;
}

double Good::getAppraisal() const {
    return appraisal;
}

void Good::setAppraisal(double appraisal) {
    Good::appraisal = appraisal;
}

const string &Good::getCategory() const {
    return category;
}

void Good::setCategory(const string &category) {
    Good::category = category;
}

//构造只初始化四个参数，在添加物品时得到编号，在上传时得到上传者属性
Good::Good(const string &goodName, double degreeOfImpairment, const string &describe, double appraisal,
           const string &category) : goodName(goodName), degreeOfImpairment(degreeOfImpairment), describe(describe),
                                     appraisal(appraisal), category(category), gid(-1) {}

const string &Good::getUploader() const {
    return uploader;
}

void Good::setUploader(const string &uploader) {
    Good::uploader = uploader;
}

int Good::getId() const {
    return gid;
}

void Good::setId(int id) {
    gid = id;
}

