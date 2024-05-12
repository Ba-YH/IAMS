#include "conio.h"
#include <locale>
#include <codecvt>
#include "src/Good.c++"
#include "src/User.c++"
#include "src/Record.c++"
#include "bits/stdc++.h"
#include "util/Output.h"
#include "util/Encryptor.h"

using namespace std;
using json = nlohmann::json;



/*----------------------------------------------数据定义区----------------------------------------------------*/

//声明Good类静态成员，主要用于格式化输出
int Good::maxDescribeCount;
int Good::maxNameCount;
int Good::maxCategoryCount;


bool loginStatus = false;                                      //登录状态
string currentUsername;                                        //当前用户名
vector<Record> records;                                        //交易记录
map<string, User> users;                                       //用户数据表
map<string, map<int, Good>> tables;                            //用户物品表
map<string, int> newGoodId;                                    //生成用户下一件物品的ID
hash<string> hasher;                                           //哈希函数
string goodsfilename = "../data/goods.json";                   //拍卖品数据文件位置
string usersfilename = "../data/users.json";                   //用户数据文件位置
string recordsfilename = "../data/records.json";               //日志保存位置

/*------------------------------------------------函数声明区--------------------------------------------------*/
/*辅助功能函数*/
void inline loginOrNot();                                      //判断是否登录
void inline misjudgment(string operationDescribe);             //误触判断
template<typename T>
T getInput(const string &prompt, int war = 0);                 //提示输入的同时获取用户输入，war表示是否需要警告
string secureInput(string prompt = "");                        //关闭输入回显的方式进行输入，默认提示为空

/*用户管理*/
void selectUserMenu();                                         //用户菜单项的选择与调用
void displayUserMenu();                                        //展示用户功能面板
void login();                                                  //登录账号
void registerUser();                                           //注册账号
void modifyPassword();                                         //修改密码
void banningUser();

void usersList();

/*物品管理*/
void selectGoodMenu();                                         //拍卖品菜单项的选择与调用
void displayGoodMenu();                                        //展示拍卖品功能面板
void selGoodMenuWhitAdmin();                                   //管理员的拍卖品功能面板
void disGoodMenuWhitAdmin();                                   //管理员的展示面板
void addGood();                                                //添加拍卖品
void removeGood();                                             //下架拍品
void deleteGood();                                             //删除拍卖品
void modifyGood();                                             //修改拍卖品信息
void displayMyGoods(map<int, Good> needTodisplay);             //展示所有拍卖品
void getGoodById(map<int, Good> goods);                        //根据编号查询物品
void getGoodByCategory(map<int, Good> goods);                  //根据类别查找
void getGoodByName(map<int, Good> goods);                      //根据名字查找
void findGood();                                               //集成查找功能
void findOpenGood();                                           //查看拍品
void uploadGood();                                             //上传个人物品到拍品
void batchAddGood();                                           //批量导入
void userAddGood(string username, Good good);                  //给指定用户添加拍品，因为使用过多所以封装为函数
bool isValidGood(const json &goodJson);                        //检查Good的保存格式是否正确
bool isValidGoods(const json &goodsJson);                      //检查导入文件内容是否正确


/*交易管理*/
void displayRecordMenu();                                      //展示交易记录功能面板
void selectRecordMenu();                                       //交易记录菜单项的选择与调用
void auction();                                                //模拟拍卖
void displayRecords();                                         //展示交易记录
void englandAuction(Good curGood);                             //英格兰式拍卖
void sealedBiddingAuction(Good curGood);                       //密封招标式拍卖
void transaction(string uploader,
                 string shootername, Good g, double price);    //交易->物品转让

/*文件读写操作*/
void loadData();                                               //读取文件
void saveToFile();                                             //保存到文件
void saveUsers();                                              //保存用户数据
void saveGoods();                                              //保存商品数据
void saveRecords();                                            //保存记录数据

/*首页*/
void systemIntroduction();                                     //系统介绍
void displayMainMenu();                                        //展示功能面板
void selectMainMenu();                                         //主菜单项的选择与调用
void exitTheSystem();                                          //退出系统

/*------------------------------------------------辅助功能函数区--------------------------------------------*/

void inline misjudgment(string operationDescribe) {
    char exitOrNot = getInput<char>("您正在进行的是" + operationDescribe + "操作！是否继续？(y/n)");
    if (exitOrNot == 'n') {
        selectMainMenu();
        return;
    }
    cout << endl;
}

void inline loginOrNot() {
    if (loginStatus == false) {
        outputWarning("您还没登录，执行此操作前请先登录！");
        selectMainMenu();
        return;
    }
}

template<typename T>
T getInput(const string &prompt, int war) {
    if (war == 0) cout << prompt;
    else outputWarning(prompt);
    T input;
    cin >> input;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return input;
}

//模版特化，针对string类型获取整行
template<>
string getInput<string>(const string &prompt, int war) {
    if (war == 0) cout << prompt;
    else outputWarning(prompt);
    string input;
    getline(cin, input);
    return input;
}

string secureInput(const std::string prompt) {
    std::cout << prompt;
    std::string input;
    char c;
    do {
        c = getch();
        if (c == '\r') {
            for (int i = 1; i <= input.size(); i++) {
                cout << "\b \b";
            }
            cout << aes_encrypt(input) << "\n";
            break;
        }
        if (c == '\b' || c == 127) {
            if (!input.empty()) {
                std::cout << "\b \b";
                input.pop_back();
            }
        } else if (c != '\r') {
            input.push_back(c);
            std::cout << '*';
        }
    } while (1);
    string res(input.begin(), input.end());
    return res;
}

/*-------------------------------------------------用户管理区----------------------------------------------*/
void selectUserMenu() {
    //misjudgment("账号管理");
    int operation;
    while (true) {
        displayUserMenu();
        operation = getInput<int>("请选择要执行的功能编号[1~4之间的整数]：");
        if (currentUsername != "admin" and (operation < 1 or operation > 4)) {
            outputWarning("输入错误，请检查输入是否是[1~4]之间的整数！");
            continue;
        }
        switch (operation) {
            case 1:
                login();
                break;
            case 2:
                registerUser();
                break;
            case 3:
                modifyPassword();
                break;
            case 4:
                if (currentUsername == "admin") banningUser();
                else return;
                break;
            case 5:
                usersList();
                break;
            case 6:
                return;
            default:
                outputWarning("输入错误，请检查输入是否是[1~6]之间的整数！\n");
        }
        //saveUsers();
    }
}

void displayUserMenu() {
    cout << "\n\n\n\n";
    cout << "----------------------欢迎使用物品竞拍管理系统----------------------\n";
    string space, describe = "用户管理：";
    cout << describe;
    if (loginStatus) {
        for (int i = 1; i <= 57 - currentUsername.size() - describe.size() / 2; i++) space += " ";
        cout << space << currentUsername << "已登录\n\n";
    } else {
        for (int i = 1; i <= 57 - describe.size() / 2; i++) space += " ";
        cout << space << "未登录\n\n";
    }
    if (loginStatus == false)
        cout << right << setw(30) << "1. 用户登录";
    else
        cout << right << setw(30) << "1. 切换账号";
    cout << right << setw(30) << "2. 用户注册\n";
    if (currentUsername != "admin") {
        cout << right << setw(30) << "3. 修改密码" << right << setw(30) << "4. 返回首页\n";
    } else {
        cout << right << setw(30) << "3. 修改密码" << right << setw(30) << "4. 封禁用户\n";
        cout << right << setw(30) << "5. 用户信息" << right << setw(30) << "6. 返回首页\n";
    }
    cout << "--------------------------------------------------------------------\n";
}

void login() {
    //判断是否需要切换账号
    string username, password;
    if (loginStatus) {
        string switchAccounts = getInput<string>("当前账号" + currentUsername + "已登录！是否需要切换账号？(y/n)", 1);
        if (switchAccounts == "y" || switchAccounts.empty());
        else return;
    }
    logInAgain:
    username = getInput<string>("请输入用户名：");
    string prompt = append("请输入密", 1) + "码：";
    if (!users.count(username)) {
        //可以直接回车
        string registerOrNot = getInput<string>("用户名未找到！是否直接使用此用户名进行注册？(y/n)", 1);
        if (registerOrNot == "y" || registerOrNot.empty()) {
            password = getInput<string>(prompt);
            //添加用户,计算新的用户ID->添加到用户ID表->添加到用户表
            User addUser(username, password);
            users[username] = addUser;
            outputHint("注册成功，请重新登录！\n");
        } else {
            outputWarning("请检查用户名并重新登录！\n\n");
        }
        goto logInAgain;
    } else {
        //获取登录用户ID
        string correctPassword = users[username].password;
        int numberOfAttempts = 0;
        while (true) {
            password = getInput<string>(prompt);
            if (password == correctPassword) {
                currentUsername = username;
                outputHint("登录成功！");
                if (currentUsername == "admin") {
                    outputHint("您已登录管理员账号，可以对公开拍品进行管理");
                } else {
                    outputHint("您已经登录普通账号，可以对自己的物品进行管理");
                }
                loginStatus = true;
                break;
            } else {
                outputWarning("密码错误，请重新输入！\n");
                numberOfAttempts++;
                if (numberOfAttempts == 3) {
                    outputWarning("错误次数过多，请重新登录！\n");
                    goto logInAgain;
                }
            }
        }
    }
}

void registerUser() {
    string username = getInput<string>("请输入用户名：");
    if (username == "admin" || users.count(username)) {
        outputWarning("该用户名已存在，请重新输入！\n");
        return registerUser();
    }
    inputAgain:
    string password = getInput<string>(append("请输入密", 1) + "码：");
    string password2 = getInput<string>(append("请确认密", 1) + "码：");
    if (password2 == password) {
        User addUser(username, password);
        users[username] = addUser;
        outputHint("注册成功！\n");
    } else {
        outputWarning("两次输入密码不一致，请重新输入！\n");
        goto inputAgain;
    }
}

void modifyPassword() {
    if (loginStatus == false) {
        outputWarning("您还没有登录！");
        return;
    }
    inputAgain:
    string password = getInput<string>("请输入原来密码：");
    if (password != users[currentUsername].password) {
        outputWarning("密码错误，请重新输入！\n");
        goto inputAgain;
    }
    string newpassword = getInput<string>("请输入新的密码：");
    string newpassword2 = getInput<string>("请再次确认密码：");
    if (newpassword2 == newpassword2) {
        users[currentUsername].password = newpassword;
        outputHint("修改成功！");
    } else {
        outputWarning("两次输入密码不一致，请重新输入！\n");
        modifyPassword();
    }
}

void usersList() {
    //TODO 三线表的信息展示输出
}

void banningUser() {
    string shortline;
    for (int i = 1; i <= 53; i++) shortline += '-';
    int uid = 0;
    map<string, User> userId;
    //先展示用户列表
    cout << "\n\n";
    cout << shortline << "--\n|";
    cout << left << setw(30) << "用户编号" << left << setw(30) << "用户名" << "|\n";
    cout << "|" << shortline << "|\n";
    for (auto [username, user]: users) {
        if (username == "admin") continue;
        string id = int2str(++uid, 6);
        userId[id] = user;
        cout << "|";
        cout << left << setw(30) << append(id, 4);
        cout << left << setw(30) << append(user.username, 3);
        cout << "|\n";
    }
    cout << shortline << "--\n";

    string selectUserId = getInput<string>("请输入要封禁的用户编号：");
    if (userId.count(selectUserId)) {
        users[userId[selectUserId].username].isAvailable = false;
        outputHint("编号为［" + selectUserId + "]的用户已封禁");
    } else {
        outputWarning("没有编号为［" + selectUserId + "］的用户\n");
    }
}

/*------------------------------------------------商品管理区-----------------------------------------------*/
void selectGoodMenu() {
    loginOrNot();
    //misjudgment("商品管理");
    int operation;
    while (true) {
        displayGoodMenu();
        operation = getInput<int>("请选择要输入的编号[1~8之间的整数]：");
        switch (operation) {
            case 1:
                addGood();
                break;
            case 2:
                batchAddGood();
                break;
            case 3:
                deleteGood();
                break;
            case 4:
                modifyGood();
                break;
            case 5:
                findGood();
                break;
            case 6:
                findOpenGood();
                break;
            case 7:
                uploadGood();
                break;
            case 8:
                return;
            default:
                outputWarning("输入错误，请检查输入是否是[1~8]之间的整数！\n");
        }
        //saveGoods();
    }

}

void displayGoodMenu() {
    cout << "\n\n\n\n";
    cout << "----------------------欢迎使用物品竞拍管理系统----------------------\n";
    string space, describe = "物品管理：";
    cout << describe;
    if (loginStatus) {
        for (int i = 1; i <= 57 - currentUsername.size() - describe.size() / 2; i++) space += " ";
        cout << space << currentUsername << "已登录\n\n";
    }
    cout << right << setw(30) << "1. 添加物品" << right << setw(30) << "2. 批量导入" << endl;
    cout << right << setw(30) << "3. 删除物品" << right << setw(30) << "4. 修改物品" << endl;
    cout << right << setw(30) << "5. 查看物品" << right << setw(30) << "6. 查询拍品" << endl;
    cout << right << setw(30) << "7. 上传拍品" << right << setw(30) << "8. 返回首页" << endl;
    cout << "--------------------------------------------------------------------\n";
}

void selGoodMenuWhitAdmin() {
    loginOrNot();
    //misjudgment("商品管理");
    int operation;
    while (true) {
        disGoodMenuWhitAdmin();
        operation = getInput<int>("请选择要输入的编号[1~4之间的整数]：");
        switch (operation) {
            case 1:
                removeGood();
                break;
            case 2:
                modifyGood();
                break;
            case 3:
                findOpenGood();
                break;
            case 4:
                return;
            default:
                outputWarning("输入错误，请检查输入是否是[1~4]之间的整数！\n");
        }
        //saveGoods();
    }

}

void disGoodMenuWhitAdmin() {
    cout << "\n\n\n\n";
    cout << "----------------------欢迎使用物品竞拍管理系统----------------------\n";
    string space, describe = "拍品管理：";
    cout << describe;
    if (loginStatus) {
        for (int i = 1; i <= 57 - currentUsername.size() - describe.size() / 2; i++) space += " ";
        cout << space << currentUsername << "已登录\n\n";
    }
    cout << right << setw(30) << "1. 下架拍品" << right << setw(30) << "2. 修改拍品" << endl;
    cout << right << setw(30) << "3. 查询拍品" << right << setw(30) << "4. 返回首页" << endl;
    cout << "--------------------------------------------------------------------\n";
}

void userAddGood(string username, Good g) {
    //如果已经有编号，则直接添加，确保不变性
    if (g.getId() != -1) {
        tables[username][g.getId()] = g;
        return;
    }
    //第一件商品编号为用户名哈希，后面递增（确保唯一性）
    int goodId;
    if (!newGoodId.count(username)) {
        goodId = stoi(to_string(hasher(username)).substr(0, 6));
    } else goodId = newGoodId[username] + 1;
    newGoodId[username] = goodId + 1;
    //在添加物品时设置
    g.setId(goodId);
    tables[username][goodId] = g;
}

void addGood() {
    string goodName = getInput<string>("请输入商品名称：");
    double degreeOfImpairment = getInput<double>("请输入物品磨损程度(0~1之间的两位小数(会自动取舍)：");
    string describe = getInput<string>("请输入物品描述(注意详略得当)：");
    double appraisal = getInput<double>("请输入物品估价(单位：万元)：");
    inputAgain:
    cout << "当前拍卖系统支持的类别如下：\n";
    int id = 0;
    for (string category: categories) {
        cout << "\t" << ++id << ": " << category << endl;
    }
    int selId = getInput<int>("请选择物品类别：");
    if (selId < 1 || selId > 7) {
        outputWarning("请输入正确的选项");
        goto inputAgain;
    }
    string category = categories[selId - 1];
    Good good(goodName, degreeOfImpairment, describe, appraisal, category);
    good.setUploader(currentUsername);//没上传是持有者，上传就是上传者

    userAddGood(currentUsername, good);
    outputHint("物品信息添加成功！");

    while (true) {
        string continueOrNot = getInput<string>("是否需要继续添加物品？(y/n)");
        if (continueOrNot == "y" || continueOrNot.empty()) {
            addGood();
        } else {
            break;
        }
    }
}

void removeGood() {
    map<int, Good> goods = tables["admin"];
    displayMyGoods(goods);
    int selId = getInput<int>("请选择要下架的拍品编号：");
    if (tables["admin"].count(selId)) {
        Good g = tables["admin"][selId];
        userAddGood(g.getUploader(), g);
        tables["admin"].erase(selId);
    } else {
        outputWarning("没有此编号的拍品，请重新输入！");
        removeGood();
    }
}

void displayMyGoods(map<int, Good> needToDisplay) {
    if (needToDisplay.size() == 0) {
        outputHint("当前还没有任何物品！");
        return;
    }
    auto maxNameCount = 3;
    auto maxDescribeCount = 2;
    auto maxCategoryCount = 2;
    for (const auto &[goodId, good]: needToDisplay) {
        maxNameCount = max(maxNameCount, countChineseCharacters(good.getGoodName()));
        maxDescribeCount = max(maxDescribeCount, countChineseCharacters(good.getDescribe()));
        maxCategoryCount = max(maxCategoryCount, countChineseCharacters(good.getCategory()));
    }
    Good::maxCategoryCount = maxCategoryCount;
    Good::maxNameCount = maxNameCount;
    Good::maxDescribeCount = maxDescribeCount;
    int sumCount = maxCategoryCount + maxNameCount + maxDescribeCount;
    string line;
    for (int i = 1; i <= 200 - sumCount - 4 - 4 - 6 - 4; i++) line += "-";
    cout << line << "--\n|";
    cout << left << setw(20) << "物品编号";
    cout << left << setw(30) << transfer("物品名", maxNameCount);
    cout << left << setw(20) << "损耗程度";
    cout << left << setw(50) << transfer("描述", maxDescribeCount);
    cout << left << setw(30) << "估价（万元）";
    cout << left << setw(30) << transfer("种类", maxCategoryCount);
    cout << left << setw(20) << "上传用户" << "|\n";
    cout << "|" << line << "|\n";
    int curId = 0;
    for (const auto &[goodId, good]: needToDisplay) {
        cout << "|";
        cout << left << setw(20) << append(to_string(goodId), 4);
        cout << good;
        cout << "|\n";
    }
    cout << line << "--\n";
}

void deleteGood() {
    map<int, Good> currentGoods = tables[currentUsername];
    //判断是否有物品
    if (currentGoods.size() == 0) {
        outputWarning("当前没有物品可以删除，请先添加物品！");
        return;
    }

    delAgain:
    currentGoods = tables[currentUsername];
    if (currentGoods.size() == 0) {
        outputWarning("您已删除完全部商品！");
        return;
    }

    inputAgain:
    outputHint("\n所有物品的信息如下：\n");
    displayMyGoods(currentGoods);
    int delId = getInput<int>("您当前共有" + to_string(currentGoods.size()) + "个物品，请输入您想删除物品的编号！");
    if (currentGoods.count(delId)) {
        tables[currentUsername].erase(delId);
        outputHint("已成功删除编号为［" + to_string(delId) + "]的物品\n");
    } else {
        outputWarning("输入的编号不存在，请重新输入！\n");
        goto inputAgain;
    }

    while (true) {
        string continueOrNot = getInput<string>("是否需要继续删除物品？(y/n)");
        if (continueOrNot == "y" || continueOrNot.empty()) {
            goto delAgain;
        } else {
            break;
        }
    }
}

void modifyGood() {
    map<int, Good> currentGoods = tables[currentUsername];

    //检查是否有物品
    if (currentGoods.size() == 0) {
        cout << "当前没有物品可以修改，请先添加物品！";
        return;
    }

    //继续修改
    modifyAgain:
    currentGoods = tables[currentUsername];
    outputHint("\n所有物品的信息如下：\n");
    displayMyGoods(currentGoods);
    int modifyId = getInput<int>("您当前共有" + to_string(currentGoods.size()) + "个物品，请输入您想修改物品的编号：");
    if (!currentGoods.count(modifyId)) {
        outputWarning("编号不存在，请输入正确的编号！\n");
        goto modifyAgain;
    }
    cout << "您当前正在修改编号为" << modifyId << "的物品，请输入新的物品信息：" << endl;
    Good selGood = currentGoods[modifyId];
    //修改操作
    string goodName = getInput<string>("\n当前物品名称为：" + selGood.getGoodName() + "\n输入新的物品名称：");
    if (!goodName.empty()) selGood.setGoodName(goodName);
    string degreeOfImpairments = getInput<string>(
            "\n当前损耗程度为：" + double2str(selGood.getDegreeOfImpairment(), 2)
            + "\n请输入物品损耗程度(0~1之间的两位小数(会自动取舍)：");
    if (!degreeOfImpairments.empty()) selGood.setDegreeOfImpairment(stod(degreeOfImpairments));
    string describe = getInput<string>("\n当前的描述为：" + selGood.getDescribe()
                                       + "\n请输入新的描述（注意详略得当）：");
    if (!describe.empty()) selGood.setDescribe(describe);
    string apprisals = getInput<string>(
            "\n当前估价为（万元）：" + to_string(selGood.getAppraisal())
            + "\n请输入新的估价（万元）：");
    if (!apprisals.empty()) selGood.setAppraisal(stod(apprisals));

    cout << "\n当前类别为：" + selGood.getCategory() + "\n当前拍卖系统的类别如下：\n";
    int id = 0;
    for (string catagory: categories) {
        cout << "\t" << ++id << ": " << catagory << endl;
    }
    string selId = getInput<string>("请选择修改后的类别编号：");
    if (selId.empty());
    else if (stod(selId) > 7 || stod(selId) <= 0) {
        outputWarning("输入有误，请重新输入！");
    } else {
        selGood.setCategory(categories[stoi(selId) - 1]);
    }
    tables[currentUsername][modifyId] = selGood;
    outputHint("编号为［" + to_string(modifyId) + "］的物品已完成修改！\n");

    while (true) {
        string continueOrNot = getInput<string>("是否需要继续修改物品？(y/n)");
        if (continueOrNot == "y" || continueOrNot.empty()) {
            goto modifyAgain;
        } else {
            break;
        }
    }
}

void getGoodById(map<int, Good> goods) {
    int selId = getInput<int>("请输入物品编号:");
    if (!goods.count(selId)) {
        outputWarning("没有查询到该商品，如果忘记编号建议使用[按照类型/名称查询]");
        return;
    }
    map<int, Good> selGoods;
    selGoods[selId] = goods[selId];
    outputHint("\n编号为" + ("[" + to_string(selId) + "]") + "的物品信息如下：\n");
    displayMyGoods(selGoods);
}

void getGoodByCategory(map<int, Good> goods) {
    //获取输入
    inputAgain:
    cout << "当前拍卖系统的所有类别如下：\n";
    int id = 0;
    for (string catagory: categories) {
        cout << "\t" << ++id << ":" << catagory << endl;
    }
    int selId = getInput<int>("请选择要查询的类别编号：");
    if (selId < 0 || selId > 7) {
        outputWarning("输入有误，检查输入是否为[1,6]之间的整数！\n");
        goto inputAgain;
    }
    string specifyCatagory = categories[selId - 1];

    //选择要展示的物品
    map<int, Good> selGoods;
    for (const auto [goodId, good]: goods) {
        if (good.getCategory() == specifyCatagory) {
            selGoods[goodId] = good;
        }
    }
    if (selGoods.size() == 0) {
        outputWarning("您还没有该类型物品哦~\n");
        return;
    }
    outputHint(("\n［" + specifyCatagory + "]") + "］类的物品信息如下：\n");
    displayMyGoods(selGoods);
}

void getGoodByName(map<int, Good> goods) {
    map<int, Good> selGoods;
    string specifyGoodName = getInput<string>("请输入要查询的物品名称:");
    for (const auto [goodId, good]: goods) {
        if (good.getGoodName() == specifyGoodName) {
            selGoods[goodId] = good;
        }
    }
    if (selGoods.size() == 0) {
        outputWarning("没有找到名为［" + specifyGoodName + "］的物品！");
        return;
    }
    outputHint("\n名为［" + specifyGoodName + "］的商品信息如下：\n");
    displayMyGoods(selGoods);
}

void findGood() {
    map<int, Good> goods = tables[currentUsername];
    findAgain:
    cout << "请选择查找类型[1~4之间的整数]：\n";
    cout << "  1. 按编号查找\n" << "  2. 按名称查找\n" << "  3. 按类别查找\n" << "  4. 查看所有物品\n";
    int selId = 0;
    cin >> selId;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    switch (selId) {
        case 1:
            getGoodById(goods);
            break;
        case 2:
            getGoodByName(goods);
            break;
        case 3:
            getGoodByCategory(goods);
            break;
        case 4:
            outputHint("\n所有物品的信息如下：");
            displayMyGoods(goods);
            break;
        default:
            outputWarning("输入有误，检查输入是否为[1,4]之间的整数！\n");
            goto findAgain;
    }
}

void findOpenGood() {
    map<int, Good> goods = tables["admin"];

    findAgain:
    cout << "请选择查找类型[1~4之间的整数]\n";
    cout << "  1. 按编号查找\n" << "  2. 按名称查找\n" << "  3. 按类别查找\n" << "  4. 查看所有拍品\n";
    int selId = 0;
    cin >> selId;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    switch (selId) {
        case 1:
            getGoodById(goods);
            break;
        case 2:
            getGoodByName(goods);
            break;
        case 3:
            getGoodByCategory(goods);
            break;
        case 4:
            outputHint("\n所有拍品的信息如下：");
            displayMyGoods(goods);
            break;
        default:
            outputWarning("输入有误，检查输入是否为[1,4]之间的整数！");
            goto findAgain;
    }
}

void uploadGood() {
    map<int, Good> currentGoods = tables[currentUsername];
    //判断是否有物品
    if (currentGoods.size() == 0) {
        outputWarning("当前没有物品可以上传，请先添加物品！\n");
        return;
    }


    uploadAgain:
    currentGoods = tables[currentUsername];
    if (currentGoods.size() == 0) {
        outputWarning("您已上传完全部物品！\n");
        return;
    }

    inputAgain:
    displayMyGoods(currentGoods);
    int delId = getInput<int>("您当前共有" + to_string(currentGoods.size()) + "个物品，请输入您想上传物品的编号！");
    if (currentGoods.count(delId)) {
        Good curGood = currentGoods[delId];
        tables[currentUsername].erase(delId);
        userAddGood("admin", curGood);
        outputHint("已成功将编号为［" + to_string(delId) + "］的物品上传到拍卖系统！\n");
    } else {
        outputWarning("输入有误，请检查您的输入！\n");
        goto inputAgain;
    }

    while (true) {
        string continueOrNot = getInput<string>("是否需要继续上传物品？(y/n)");
        if (continueOrNot == "y" || continueOrNot.empty()) {
            goto uploadAgain;
        } else {
            break;
        }
    }
}

bool isValidGood(const json &goodJson) {
    //检查JSON格式是否正确
    return goodJson.contains("物品名称") && goodJson.contains("物品描述") &&
           goodJson.contains("物品价值") && goodJson.contains("物品分类")
           && goodJson.contains("物品损坏程度");
}

bool isValidGoods(const json &goodsJson) {
    if (!goodsJson.is_array()) {
        return false;
    }
    for (const auto &goodJson: goodsJson) {
        if (!isValidGood(goodJson)) {
            return false;
        }
    }
    return true;
}

void batchAddGood() {
    string fileName = getInput<string>("请输入文件的绝对路径，注意只能是JSON格式文件：");
    size_t dotPos = fileName.find_last_of(".");
    //检查文件格式
    if (dotPos == string::npos || fileName.substr(dotPos + 1) != "json") {
        outputWarning("文件格式错误，请检查文件格式是否为JSON！\n");
        return batchAddGood();
    }
    //检查路径合法性
    ifstream inputfile(fileName);
    if (!inputfile.is_open()) {
        outputWarning("文件打开失败，请检查文件路径是否正确并重新输入！\n");
        batchAddGood();
    }
    //文件读取过程使用异常检测
    try {
        json j;
        inputfile >> j;
        for (const auto &goodJson: j) {
            Good g = goodJson.get<Good>();
            g.setUploader(currentUsername);
            userAddGood(currentUsername, g);
        }
        inputfile.close();
        outputHint("批量导入成功！");
    } catch (json::parse_error &ex) {
        //文件读取异常
        outputWarning("文件内容错误，请检查文件内容是否正确！\n");
        return;
    }
}


/*-------------------------------------------------交易管理区----------------------------------------------*/
void selectRecordMenu() {
    //misjudgment("交易管理");
    int operation;
    while (true) {
        displayRecordMenu();
        operation = getInput<int>("请选择要输入的编号[1~3之间的整数]：");
        switch (operation) {
            case 1:
                displayRecords();
                break;
            case 2:
                auction();
                break;
            case 3:
                return;
            default:
                outputWarning("输入错误，请检查输入是否是[1~3]之间的整数！");
        }
        //saveRecords();
    }
}

void displayRecordMenu() {
    cout << "\n\n\n\n";
    cout << "----------------------欢迎使用物品竞拍管理系统----------------------\n";
    string space, describe = "交易管理：";
    cout << describe;
    if (loginStatus) {
        for (int i = 1; i <= 57 - currentUsername.size() - describe.size() / 2; i++) space += " ";
        cout << space << currentUsername << "已登录\n\n";
    } else {
        for (int i = 1; i <= 57 - describe.size() / 2; i++) space += " ";
        cout << space << "未登录\n\n";
    }
    cout << right << setw(30) << "1. 交易记录" << right << setw(30) << "2. 开始竞拍" << endl;
    cout << right << setw(30) << "3. 返回首页" << endl;
    cout << "----------------------------------------------------------------------\n";
}

void englandAuction(Good curGood) {
    srand(time(0));
    double startingPrice = getInput<double>("\n\n请设置起拍价（万元）：");
    double minIncrement = getInput<double>("请设置最小加价幅度（万元）：");
    double reservePrice = getInput<double>("请设置保留价（万元）：");

    string uploder = curGood.getUploader();
    double apprisal = curGood.getAppraisal();

    vector<string> participants;
    for (const auto [username, user]: users) {
        if (username == "admin" || username == uploder) continue;
        participants.push_back(username);
    }
    int number = participants.size();

    double prePrice = startingPrice;
    string preUsername;
    outputHighlight("~~~~~~~~~~~~~~~~~~~~~~!!!用户进行出价!!!~~~~~~~~~~~~~~~~~~~~~");
    while (1) {
        //获取随机全排列 -> 遍历用户获得出价
        int numberOfAbandonments = 0;
        vector<int> indexs(number);
        iota(indexs.begin(), indexs.end(), 0);
        unsigned seed = chrono::system_clock::now().time_since_epoch().count();
        default_random_engine rng(seed);
        shuffle(indexs.begin(), indexs.end(), rng);

        for (int index: indexs) {
            string username = participants[index];
            double curNeed = prePrice + minIncrement;
            string hint = username + "［当前最低出价为 " + double2str(curNeed, 4)
                          + "(万元)" + (curNeed > apprisal ? "  \033[33m已超出估价\033[0m" : "") + "］：";
            string price = getInput<string>(hint);
            if (price.empty()) {
                numberOfAbandonments++;
            } else {
                prePrice = stod(price);
                preUsername = username;
            }
        }
        if (numberOfAbandonments == number - 1) break;
    }
    //结束信息
    outputHighlight("无人继续加价，最终成交价为：" + double2str(prePrice, 4));
    if (prePrice < reservePrice) {
        outputWarning("\n由于最终成交价小于物品的保留价，拍卖失败！");
    } else {
        transaction(uploder, preUsername, curGood, prePrice);
    }
}

void sealedBiddingAuction(Good curGood) {
    string hint;
    hint = "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~!!!竞买人将出价加密发送给拍卖人!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
    outputHighlight(hint);
    map<string, int> bidPrice;
    string uploader = curGood.getUploader();
    string category = curGood.getCategory();

    //预测拍卖成功率
    auto predictionSuccessRate = [&](string curUsername) -> double {
        int effectiveNumber = 0;
        double P_other_fails = 1.0;
        for (auto [otherUsername, otherUser]: users) {
            if (otherUsername == "admin" || otherUsername == uploader) continue;
            effectiveNumber++;
            P_other_fails *= (1 - otherUser.difCategorySucessRate[category]);
        }
        double lambda = 1.0 / effectiveNumber;
        double P_win = lambda * P_other_fails + (1 - lambda) * users[curUsername].difCategorySucessRate[category];
        return P_win;
    };

    for (const auto [username, user]: users) {
        if (username == uploader || username == "admin") continue;
        double P_win = predictionSuccessRate(username);
        cout << "尊敬的" << username << "用户您好，系统综合预测您的竞买成功率为："
             << "\033[33m" << double2str(P_win, 4) << "\033[0m";

        string price = secureInput("\n如果你想成功竞拍，请斟酌您的递价（万元）：");
        cout << "\n";
        if (!price.empty()) {
            bidPrice[username] = stod(price);
            users[username].countOfSessions++;
        }
    }


    //按递价由大到小展示
    vector<pair<string, double>> sortedBidPrice(bidPrice.begin(), bidPrice.end());
    sort(sortedBidPrice.begin(), sortedBidPrice.end(),
         [](pair<string, int> a, pair<string, int> b) {
             return a.second > b.second;
         });

    hint = "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~!!!拍卖人选择竞买人!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
    outputHighlight(hint);

    cout << left << setw(30) << "参与竞买人" << setw(30) << "递交价（万元）" << endl;
    for (auto [username, price]: sortedBidPrice) {
        cout << left << setw(30) << append(username, 5);
        cout << left << setw(30) << append(to_string(price), 7);
        cout << endl;
    }

    //记录交易结果
    string sel = getInput<string>("拍卖人最终选择的竞买人：");
    if (sel.empty()) {
        cout << "拍卖人没有选择竞买人，竞买失败！";
    } else if (bidPrice.count(sel)) {
        transaction(uploader, sel, curGood, bidPrice[sel]);
    } else {
        outputWarning("该用户没有提交竞买价，竞买失败！");
    }
}

void auction() {
    //选择拍品->选择拍卖方式->拍卖->记录结果
    map<int, Good> openGoods = tables["admin"];
    cout << "\n\n所有拍品的信息如下：\n";
    displayMyGoods(openGoods);
    int selId = getInput<int>("选择要竞拍的拍品编号：");
    Good currentItem = openGoods[selId];
    toString(currentItem);

    cout << "当前拍卖系统的竞拍方式如下：\n";
    cout << "\t1. 英格兰式拍卖\n";
    cout << "\t2. 密封招标式拍卖\n";
    selId = getInput<int>("请选择竞拍方式：");
    if (selId == 1) {
        englandAuction(currentItem);
    } else if (selId == 2) {
        sealedBiddingAuction(currentItem);
    } else {
        outputWarning("输入有误，请检查输入！\n");
    }
}

void transaction(string uploader, string shootername, Good g, double price) {
    //竞买成功提示
    string goodNews = "恭喜" + shootername + "以" + double2str(price, 2) + "万元的成交价，将［"
                      + g.getGoodName() + "］收入囊中";
    outputHighlight(goodNews);
    //物品转移
    tables[uploader].erase(g.getId());
    g.setAppraisal(price * 0.9);
    g.setUploader("");
    userAddGood(shootername, g);
    //更新用户信息
    users[shootername].difCategoryCnt[g.getCategory()]++;
    users[shootername].countOfSuccesses++;
    users[shootername].sucessRate = users[shootername].countOfSuccesses / users[shootername].countOfSessions;

    //交易记录
    Record record(g.getId(), price, shootername);
    records.push_back(record);
}

void displayRecords() {
    loginOrNot();
    //不区分用户权限，都可以查看全部交易信息，非管理员(用户)会着重显示自己的交易记录
    double totalPrice = 0;
    string line, shortline;
    int tranNumber = 0;
    for (int i = 1; i <= 132; i++) line += '-';
    for (int i = 1; i <= 61; i++) shortline += '-';
    map<string, int> cnt;
    for (const auto &[username, id]: users) {
        if (username != "admin")
            cnt[username] = 0;
    }
    if (records.size() == 0) {
        outputHint("\n当前还没有用户交易记录！\n");
        goto next;
    }
    //以三线表的形式展示交易记录
    cout << "交易记录如下：\n";
    cout << line << "--\n|";
    cout << left << setw(20) << "交易单号" << left << setw(40) << "交易时间";
    cout << left << setw(30) << "中拍者" << left << setw(30) << "成交价";
    cout << left << setw(30) << "拍品编号" << "|\n";
    cout << "|" << line << "|" << endl;
    for (const auto &record: records) {
        cout << "|";
        std::ostringstream oss;
        oss << left << setw(20) << append(int2str(++tranNumber, 6), 4);
        oss << record;
        //高亮显示自己的交易记录
        if (record.getShooterName() == currentUsername) {
            outputHighlight(oss.str());
        } else {
            cout << oss.str();
        }
        cout << "|\n";
        cnt[record.getShooterName()]++;
        totalPrice += record.getFinalPrice();
    }
    cout << line << "--\n\n";
    next:
    //统计交易信息
    cout << "竞拍物品数量：" << records.size() << endl;
    cout << "交易总额：" << totalPrice << endl;

    //统计排名
    vector<pair<string, int>> shootCnt(cnt.begin(), cnt.end());
    sort(shootCnt.begin(), shootCnt.end(),
         [&](const pair<string, int> &a, const pair<string, int> &b) {
             return a.second > b.second || (a.second == b.second && a.first < b.first);
         });
    map<int, int> ranks;
    int rank = 1;
    for (int i = 0; i < shootCnt.size(); ++i) {
        if (i > 0 && shootCnt[i].second != shootCnt[i - 1].second) {
            rank = i + 1;
        }
        ranks[shootCnt[i].second] = rank;
    }
    cout << "所有用户中拍次数排名如下：\n";
    //输出用户排名，如果非管理员突出显示自己的位置
    cout << shortline << "--\n";
    cout << "|" << left << setw(10) << "排名" << left << setw(30) << "用户名" << left << setw(30) << "中拍次数"
         << "|\n";
    cout << "|" << shortline << "|\n";
    for (const auto &[username, count]: shootCnt) {
        cout << "|";
        std::ostringstream oss;
        oss << left << setw(10) << append(int2str(ranks[count], 3), 2);
        oss << left << setw(30) << append(username, 3);
        oss << left << setw(30) << append(to_string(count), 4) << "|\n";
        //非管理员会突出显示自己的排名
        if (username == currentUsername) cout << "\033[33m" << oss.str() << "\033[0m";
        else cout << oss.str();
    }
    cout << shortline << "--\n";
}

/*-------------------------------------------------文件操作区----------------------------------------------*/
void loadData() {
    json j;
    //加载用户表
    ifstream usersFile(usersfilename);
    usersFile >> j;
    if (!j.empty()) {
        for (const auto &[username, userJson]: j.items()) {
            User user = userJson.get<User>(); //使用反序列化从JSON格式读取对象
            //在读取时加载成功率
            if (user.countOfSessions != 0) {
                for (auto category: categories) {
                    user.difCategorySucessRate[category]
                            = user.difCategoryCnt[category] / (double) user.countOfSessions;
                }
            }
            users[username] = user;
        }
    }
    usersFile.close();


    //加载各用户的商品表
    ifstream goodsFile(goodsfilename);
    goodsFile >> j;
    if (!j.empty()) {
        for (const auto &[username, innerJ]: j.items()) {
            map<int, Good> goods;
            for (const auto &[goodID, goodJson]: innerJ.items()) {
                Good good = goodJson.get<Good>(); //使用反序列化从JSON格式读取对象
                goods[stoi(goodID)] = good;
                //更新用户下一个商品ID
                newGoodId[username] = max(newGoodId[username], stoi(goodID));
                if (username == "admin") {
                    newGoodId[good.getUploader()] = max(newGoodId[good.getUploader()], stoi(goodID));
                }
            }
            tables[username] = goods;
        }
    }
    goodsFile.close();

    //加载records
    ifstream recordsFile(recordsfilename);
    recordsFile >> j;
    if (!j.empty()) {
        for (auto &record: j) {
            records.push_back(record.get<Record>());
        }
        recordsFile.close();
    }
}

void saveUsers() {
    json j = json::object();
    ofstream UserFile(usersfilename, std::ios::out | std::ios::trunc);
    for (const auto &[username, user]: users) {
        j[username] = user;
    }
    UserFile << j.dump(4);
    UserFile.close();
}

void saveGoods() {
    json j = json::object();
    for (const auto &[tableKey, innerMap]: tables) {
        json innerj;
        for (const auto &[goodId, good]: innerMap) {
            innerj[to_string(goodId)] = good; //使用序列化将对象转换为JSON格式
        }
        j[tableKey] = innerj;
    }
    ofstream tablesFile(goodsfilename, std::ios::out | std::ios::binary);
    tablesFile << j.dump(4);
    tablesFile.close();
}

void saveRecords() {
    json j = json::array();
    for (const auto &record: records) {
        j.push_back(record);
    }
    ofstream recordsFile(recordsfilename, std::ios::out | std::ios::trunc);
    recordsFile << j.dump(4);
    recordsFile.close();
}

/*-------------------------------------------------首页----------------------------------------------------*/
void displayMainMenu() {
    cout << "\n\n\n\n";
    string title = "----------------------欢迎使用物品竞拍管理系统----------------------\n";
    string space, describe = "首页：";
    cout << title << describe;
    if (loginStatus) {
        for (int i = 1; i <= 57 - currentUsername.size() - utf8len(describe); i++) space += " ";
        cout << space << currentUsername << "已登录\n\n";
    } else {
        for (int i = 1; i <= 57 - utf8len(describe); i++) space += " ";
        cout << space << "未登录\n\n";
    }
    cout << right << setw(30) << "1. 用户管理" << right << setw(30) << "2. 物品管理\n";
    if (currentUsername != "admin") {
        cout << right << setw(30) << "3. 交易记录";
    } else {
        cout << right << setw(30) << "3. 交易管理";
    }
    cout << right << setw(30) << "4. 关于系统\n";
    cout << right << setw(30) << "5. 退出系统" << endl;//最后输入结束采用endl 前面换行用\n
    string line;
    for (int i = 1; i <= title.size() - 13; i++) line += '-';
    cout << line << endl;
}

void systemIntroduction() {
    // @formatter:off  关闭格式化
    cout<<"           _____                    _____                    _____                    _____                 \n";
    cout<<"          /\\    \\                  /\\    \\                  /\\    \\                  /\\    \\                \n";
    cout<<"         /::\\    \\                /::\\    \\                /::\\____\\                /::\\    \\               \n";
    cout<<"         \\:::\\    \\              /::::\\    \\              /::::|   |               /::::\\    \\              \n";
    cout<<"          \\:::\\    \\            /::::::\\    \\            /:::::|   |              /::::::\\    \\             \n";
    cout<<"           \\:::\\    \\          /:::/\\:::\\    \\          /::::::|   |             /:::/\\:::\\    \\            \n";
    cout<<"            \\:::\\    \\        /:::/__\\:::\\    \\        /:::/|::|   |            /:::/__\\:::\\    \\           \n";
    cout<<"            /::::\\    \\      /::::\\   \\:::\\    \\      /:::/ |::|   |            \\:::\\   \\:::\\    \\          \n";
    cout<<"   ____    /::::::\\    \\    /::::::\\   \\:::\\    \\    /:::/  |::|___|______    ___\\:::\\   \\:::\\    \\         \n";
    cout<<"  /\\   \\  /:::/\\:::\\    \\  /:::/\\:::\\   \\:::\\    \\  /:::/   |::::::::\\    \\  /\\   \\:::\\   \\:::\\    \\        \n";
    cout<<" /::\\   \\/:::/  \\:::\\____\\/:::/  \\:::\\   \\:::\\____\\/:::/    |:::::::::\\____\\/::\\   \\:::\\   \\:::\\____\\       \n";
    cout<<" \\:::\\  /:::/    \\::/    /\\::/    \\:::\\  /:::/    /\\::/    / ~~~~~/:::/    /\\:::\\   \\:::\\   \\::/    /       \n";
    cout<<"  \\:::\\/:::/    / \\/____/  \\/____/ \\:::\\/:::/    /  \\/____/      /:::/    /  \\:::\\   \\:::\\   \\/____/        \n";
    cout<<"   \\::::::/    /                    \\::::::/    /               /:::/    /    \\:::\\   \\:::\\    \\            \n";
    cout<<"    \\::::/____/                      \\::::/    /               /:::/    /      \\:::\\   \\:::\\____\\           \n";
    cout<<"     \\:::\\    \\                      /:::/    /               /:::/    /        \\:::\\  /:::/    /           \n";
    cout<<"      \\:::\\    \\                    /:::/    /               /:::/    /          \\:::\\/:::/    /            \n";
    cout<<"       \\:::\\    \\                  /:::/    /               /:::/    /            \\::::::/    /             \n";
    cout<<"        \\:::\\____\\                /:::/    /               /:::/    /              \\::::/    /              \n";
    cout<<"         \\::/    /                \\::/    /                \\::/    /                \\::/    /               \n";
    cout<<"          \\/____/                  \\/____/                  \\/____/                  \\/____/                \n";
    cout<<"\n\n";
    cout<<"内部版本号 #IAMS_1.0,  2024年5月12日构建\n";
    cout<<"Designer: Ba Yun-Hao  版权所有 ©2024-2099    \n";
    cout<<"本项目已在GitHub上开开源：";
    cout<<"\033[33m"<<"https://github.com/Ba-YH/IAMS\n"<<"\033[0m";
    cout<<"如果你对项目使用有任何疑问，请在GitHub中查看!\n";
    cout<<"如果你发现该项目的任何问题或是需求新的功能，欢迎您在issues提出!\n";
    // @formatter:on   开启格式化
}

void selectMainMenu() {
    int operation;
    while (true) {
        displayMainMenu();
        operation = getInput<int>("请选择要输入的编号[1~4之间的整数]：");
        switch (operation) {
            case 1:
                selectUserMenu();
                break;
            case 2:
                if (currentUsername == "admin") selGoodMenuWhitAdmin();
                else selectGoodMenu();
                break;
            case 3:
                if (currentUsername == "admin") selectRecordMenu();
                else displayRecords();
                break;
            case 4:
                systemIntroduction();
                break;
            case 5:
                exitTheSystem();
                break;
            default:
                outputWarning("输入有误，请检查输入是否是[1~4]之间的整数！");
        }
    }
}

void exitTheSystem() {
    saveUsers();
    saveGoods();
    saveRecords();
    outputHint("正在退出系统，欢迎您下次使用！");
    exit(0);
}

int main(void) {
    //先从JSON数据文件加载信息
    loadData();
    selectMainMenu();
    return 0;
}
