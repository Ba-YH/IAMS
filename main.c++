#include "conio.h"
#include "locale"
#include "codecvt"
#include "windows.h"
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

string uploaderItem="上传用户";
bool loginStatus = false;                                      //登录状态
string currentUsername;                                        //当前用户名
vector<Record> records;                                        //交易记录
map<string, User> users;                                       //用户数据表
map<string, map<int, Good>> tables;                            //用户物品表
map<string, int> maxGoodId;                                    //记录用户最大的物品编号
hash<string> hasher;                                           //哈希函数
string goodsfilename = "../data/goods.json";                   //拍卖品数据文件位置
string usersfilename = "../data/users.json";                   //用户数据文件位置
string recordsfilename = "../data/records.json";               //日志保存位置
string buttonColor="31";
/*------------------------------------------------函数声明区--------------------------------------------------*/
/*辅助功能函数*/
void inline loginOrNot();                                      //判断是否登录
void inline pressAnyKey();                                     //任意键继续
void inline contentHint(const string& content);                //提示当前页面
template<typename T>
T getInput(const string &prompt, int war = 0);                 //提示输入的同时获取用户输入，war表示是否需要警告
string secureInput(string prompt = "",int isEncrypt=0);        //关闭输入回显的方式进行输入，默认提示为空


/*用户管理*/
void selectUserMenu();                                         //用户菜单项的选择与调用
void displayUserMenu(int pos);                                 //展示用户功能面板
void selUserMenuWithAdmin();                                   //管理员用户菜单项的选择与调用
void disUserMenuWithAdmin(int pos);                             //展示管理员的用户功能面板
void login();                                                  //登录账号
void registerUser();                                           //注册账号
void modifyPassword();                                         //修改密码
void banningUser();                                            //封禁用户
void usersList();                                              //用户信息


/*物品管理*/
void selectGoodMenu();                                         //拍卖品菜单项的选择与调用
void displayGoodMenu(int pos);                                 //展示拍卖品功能面板
void selGoodMenuWithAdmin();                                   //管理员的拍卖品功能面板
void disGoodMenuWithAdmin(int pos);                            //管理员的展示面板
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



/*交易管理*/
void displayRecordMenu(int pos);                               //展示交易记录功能面板
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
void saveGoods();                                              //保存物品数据
void saveRecords();                                            //保存记录数据

/*首页*/
void systemIntroduction();                                     //系统介绍
void displayMainMenu(int pos);                                 //展示功能面板
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
        pressAnyKey();
        selectMainMenu();
        return;
    }
}

void inline pressAnyKey() {
    outputHint("\n\n按任意键继续...");
    char ch = _getch();
    system("cls");
    return;
}

void inline contentHint(const string& content){
    //当文本显示在正中间
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int consoleWidth;

    // 获取控制台屏幕缓冲区信息
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    // 计算文本的起始位置
    std::string text = "当前为"+content+"页面";
    int textWidth = text.length();
    int xPosition = (consoleWidth - textWidth) / 2; // 控制台宽度的一半减去文本长度的一半

    // 设置光标位置到屏幕最顶行的中间
    COORD coord;
    coord.X = xPosition;
    coord.Y = 0; // 第一行
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    // 输出文本
    outputHint(text);

    // 恢复光标位置
    coord.X = 0;
    coord.Y = 3;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
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

string secureInput(const std::string prompt,int isEncrypt) {
    std::cout << prompt;
    std::string input;
    char c;
    do {
        c = getch();
        if (c == '\r') {
            if(isEncrypt) {
                for (int i = 1; i <= input.size(); i++) {
                    cout << "\b \b";
                }
                cout << aes_encrypt(input)<<endl;
            }
            cout<<endl;
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
    int x = 1, y = 1;
    char operation;
    while (true) {
        int pos = (x - 1) * 2 + y;
        displayUserMenu(pos);
        operation = _getch();
        system("cls");
        switch (operation) {
            case '1':
                return;
                break;
            case '2':
                registerUser();
                break;
            case '3':
                modifyPassword();
                break;
            case '4':
                return;
            case 'w':
                if (x != 1) x--;
                break;
            case 's':
                if (x != 2) x++;
                break;
            case 'a':
                if (y != 1) y--;
                break;
            case 'd':
                if (y != 2) y++;
                break;
            case '\r':
                switch (pos) {
                    case 1:
                        login();
                        return;
                    case 2:
                        registerUser();
                        break;
                    case 3:
                        modifyPassword();
                        break;
                    case 4:
                        return;
                }
                break;
        }
        //saveGoods();
    }
}

void displayUserMenu(int pos) {
    //@formatter:off
    cout << "\n\n\n\n";
    string line,shortline;
    string value="用户管理：";
    for(int i=1; i<=25; i++) shortline+='-';
    for(int i=1; i<=72; i++) line += '-';
    cout<<shortline<<"欢迎使用物品竞拍管理系统"<<shortline<<"\n";
    if(loginStatus){
        cout<<"|"<<value<<right<<setw(80-value.size())<<(currentUsername+"已登录")<<"|\n";
    }else{
        cout<<"|"<<value<<right<<setw(80-value.size())<<("未登录")<<"|\n";
    }
    cout<<"|"<<right<<setw(80)<<append("",8)<<"|\n";
    auto put=[&](string value,int id,int width)->string{
        ostringstream os;
        if(id==pos) os<<"\033["<<buttonColor<<"m"<<setw(30)<<value<<"\033[0m";
        else  os<<setw(30)<<value;
        return os.str();
    };
    string loginOrRegister = loginStatus ? "1. 切换账号" : "1. 用户登录";
    cout<<"|"<<right<<put(loginOrRegister,1,30)<<right<<put("2. 用户注册",2,30)<<setw(20)<<""<<"|\n";
    cout<<"|"<<right<<put("3. 修改密码",3,30)<<right<<put("4. 返回首页",4,30)<<setw(20)<<""<<"|\n";
    cout<<line << "--\n";
    //@formatter:on
}

void selUserMenuWithAdmin() {
    int x = 1, y = 1;
    char operation;
    while (true) {
        int pos = (x - 1) * 2 + y;
        disUserMenuWithAdmin(pos);
        operation = _getch();
        system("cls");
        switch (operation) {
            case '1':
                login();
                return;
            case '2':
                registerUser();
                break;
            case '3':
                modifyPassword();
                break;
            case '4':
                banningUser();
                break;
            case '5':
                usersList();
                break;
            case '6':
                return;
            case 'w':
                if (x != 1) x--;
                break;
            case 's':
                if (x != 3) x++;
                break;
            case 'a':
                if (y != 1) y--;
                break;
            case 'd':
                if (y != 2) y++;
                break;
            case '\r':
                switch (pos) {
                    case 1:
                        login();
                        return;
                    case 2:
                        registerUser();
                        break;
                    case 3:
                        modifyPassword();
                        break;
                    case 4:
                        banningUser();
                        break;
                    case 5:
                        usersList();
                        break;
                    case 6:
                        return;
                }
                break;
        }
        //saveGoods();
    }
}

void disUserMenuWithAdmin(int pos) {
    //@formatter:off
    cout << "\n\n\n\n";
    string line,shortline;
    string value="用户管理：";
    for(int i=1; i<=25; i++) shortline+='-';
    for(int i=1; i<=72; i++) line += '-';
    cout<<shortline<<"欢迎使用物品竞拍管理系统"<<shortline<<"\n";
    if(loginStatus){
        cout<<"|"<<value<<right<<setw(80-value.size())<<(currentUsername+"已登录")<<"|\n";
    }else{
        cout<<"|"<<value<<right<<setw(80-value.size())<<("未登录")<<"|\n";
    }
    cout<<"|"<<right<<setw(80)<<append("",8)<<"|\n";
    auto put=[&](string value,int id,int width)->string{
        ostringstream os;
        if(id==pos) os<<"\033["<<buttonColor<<"m"<<setw(30)<<value<<"\033[0m";
        else  os<<setw(30)<<value;
        return os.str();
    };
    string loginOrRegister = loginStatus ? "1. 切换账号" : "1. 用户登录";
    cout<<"|"<<right<<put(loginOrRegister,1,30)<<right<<put("2. 用户注册",2,30)<<setw(20)<<""<<"|\n";
    cout<<"|"<<right<<put("3. 修改密码",3,30)<<right<<put("4. 封禁用户",4,30)<<setw(20)<<""<<"|\n";
    cout<<"|"<<right<<put("5. 用户信息",5,30)<<right<<put("6. 返回首页",6,30)<<setw(20)<<""<<"|\n";
    cout<<line << "--\n";
    //@formatter:on
}

void login() {
    contentHint("用户登录");
    //判断是否需要切换账号
    string username, password;
    if (loginStatus) {
        string switchAccounts = getInput<string>("当前账号" + currentUsername + "已登录！是否需要切换账号？(y/n)", 1);
        if (switchAccounts == "y" || switchAccounts.empty());
        else {
            pressAnyKey();
            return;
        }
    }
    logInAgain:
    username = getInput<string>("请输入用户名：");
    string prompt = append("请输入密", 1) + "码：";
    if (users.count(username) and users[username].isAvailable == false) {
        outputWarning("当前用户已被封禁，限制登录！\n如果您对本次处罚有疑问，请联系管理员！\n\n");
        pressAnyKey();
        return;
    }
    if (!users.count(username)) {
        //可以直接回车
        string registerOrNot = getInput<string>("用户名未找到！是否直接使用此用户名进行注册？(y/n)", 1);
        if (registerOrNot == "y" || registerOrNot.empty()) {
            inputAgain:
            password = (prompt);
            string passwordAgain = getInput<string>("再次确认密码：");
            if (passwordAgain == password) {
                //添加用户,计算新的用户ID->添加到用户ID表->添加到用户表
                User addUser(username, password);
                users[username] = addUser;
                outputHint("注册成功，请重新登录！\n");
            } else {
                outputWarning("两次输入密码不一致，请重新输入！\n");
                goto inputAgain;
            }
        } else {
            outputWarning("请检查用户名并重新登录！\n\n");
        }
        goto logInAgain;
    } else {
        //获取登录用户ID
        string correctPassword = users[username].password;
        int numberOfAttempts = 0;
        while (true) {
            password = secureInput(prompt);
            if (password == correctPassword) {
                currentUsername = username;
                outputHint("\n登录成功！");
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
    pressAnyKey();
    if(currentUsername=="admin") selUserMenuWithAdmin();
    else selectUserMenu();
}

void registerUser() {
    contentHint("用户注册");
    string username = getInput<string>("请输入用户名：");
    if (username == "admin" || users.count(username)) {
        outputWarning("该用户名已存在，请重新输入！\n");
        return registerUser();
    }
    inputAgain:
    string password =  secureInput(append("请输入密", 1) + "码：");
    string password2 = secureInput(append("请确认密", 1) + "码：");
    if (password2 == password) {
        User addUser(username, password);
        users[username] = addUser;
        outputHint("注册成功！\n");
    } else {
        outputWarning("两次输入密码不一致，请重新输入！\n");
        goto inputAgain;
    }
    pressAnyKey();
}

void modifyPassword() {
    contentHint("修改密码");
    if (loginStatus == false) {
        outputWarning("您还没有登录！\n");
        pressAnyKey();
        return;
    }
    inputAgain:
    string password = getInput<string>("请输入原来密码：");
    if (password != users[currentUsername].password) {
        outputWarning("密码错误，请重新输入！\n");
        goto inputAgain;
    }
    string newpassword  = secureInput("请输入新的密码：");
    string newpassword2 = secureInput("请再次确认密码：");
    if (newpassword2 == newpassword2) {
        users[currentUsername].password = newpassword;
        outputHint("修改成功！");
    } else {
        outputWarning("两次输入密码不一致，请重新输入！\n");
        modifyPassword();
    }
    pressAnyKey();
}

void usersList() {
    contentHint("用户信息展示");
    string line;
    for (int i = 1; i <= 100 - 17; i++) line += '-';
    cout << line << "--\n";
    cout << "|";
    cout << left << setw(20) << "用户名" << left << setw(30) << "密码" << left << setw(25) << "参与拍卖次数";
    cout << left << setw(25) << "拥有物品个数";
    cout << "|\n";
    cout << "|" << line << "|\n";
    for (auto [username, user]: users) {
        if (username == "admin") continue;
        cout << "|";
        cout << left << setw(20) << append(username, 3);
        cout << left << setw(30) << append(user.password, 2);
        cout << left << setw(25) << append(to_string(user.countOfSessions), 6);
        cout << left << setw(25) << append(to_string(tables[username].size()), 6);
        cout << "|\n";
    }
    cout << line << "--\n\n\n";
    cout<<"\n\n所有用户物品一览表:\n";
    for(auto [username,user]: users){
        if(username=="admin") continue;
        uploaderItem="持有用户";
        cout<<"\n\n用户"<<username<<"的所有物品如下：\n";
        map<int,Good> need2dis=tables[username];
        if(need2dis.size()==0){
            cout<<"目前还没有物品！\n\n";
        }else {
            displayMyGoods(tables[username]);
        }
    }
    pressAnyKey();
}

void banningUser() {
    contentHint("封禁用户");
    string shortline;
    for (int i = 1; i <= 90 - 11; i++) shortline += '-';
    int uid = 0;
    map<string, User> userId;
    //先展示用户列表
    cout << "\n\n当前用户状态：\n";
    cout << shortline << "--\n|";
    cout << left << setw(30) << "用户编号" << left << setw(30) << "用户名" << left << setw(30) << "用户状态";
    cout << "|\n";
    cout << "|" << shortline << "|\n";
    for (auto [username, user]: users) {
        if (username == "admin") continue;
        string id = int2str(++uid, 6);
        userId[id] = user;
        cout << "|";
        std::ostringstream oss;
        oss << left << setw(30) << append(id, 4);
        oss << left << setw(30) << append(user.username, 3);
        string val;
        if (user.isAvailable == true) val = "可用";
        else val = "封禁";
        oss << left << setw(30) << append(val, 2);
        if (val == "可用") {
            cout << "\033[32m" << oss.str() << "\033[0m";
        } else {
            cout << "\033[31m" << oss.str() << "\033[0m";
        }
        cout << "|\n";
    }
    cout << shortline << "--\n";

    string selectUserId = getInput<string>("请输入要封禁/解封的用户编号：");
    if (userId.count(selectUserId)) {
        string uername = userId[selectUserId].username;
        if (users[uername].isAvailable)
            outputHint("编号为［" + selectUserId + "］的用户已封禁");
        else
            outputHint("编号为［" + selectUserId + "］的用户已解封");
    } else {
        outputWarning("没有编号为［" + selectUserId + "］的用户\n");
    }
    pressAnyKey();
}

/*------------------------------------------------物品管理区-----------------------------------------------*/
void selectGoodMenu() {
    loginOrNot();
    char operation;
    int x = 1, y = 1;
    while (true) {
        int pos = (x - 1)*2 + y;
        displayGoodMenu(pos);
        operation = _getch();
        system("cls");
        switch (operation) {
            case '1':
                addGood();
                break;
            case '2':
                batchAddGood();
                break;
            case '3':
                deleteGood();
                break;
            case '4':
                modifyGood();
                break;
            case '5':
                findGood();
                break;
            case '6':
                findOpenGood();
                break;
            case '7':
                uploadGood();
                break;
            case '8':
                return;
            case 'w':
                if (x != 1) x--;
                break;
            case 's':
                if (x != 4) x++;
                break;
            case 'a':
                if (y != 1) y--;
                break;
            case 'd':
                if (y != 2) y++;
                break;
            case '\r':
                switch (pos) {
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
                }
                break;
        }
        //saveGoods();
    }

}

void displayGoodMenu(int pos) {
    //@formatter:off
    cout << "\n\n\n\n";
    string line,shortline;
    string value="物品管理：";
    for(int i=1; i<=25; i++) shortline+='-';
    for(int i=1; i<=72; i++) line += '-';
    cout<<shortline<<"欢迎使用物品竞拍管理系统"<<shortline<<"\n";
    if(loginStatus){
        cout<<"|"<<value<<right<<setw(80-value.size())<<(currentUsername+"已登录")<<"|\n";
    }else{
        cout<<"|"<<value<<right<<setw(80-value.size())<<("未登录")<<"|\n";
    }
    auto put=[&](string value,int id,int width)->string{
        ostringstream os;
        if(id==pos) os<<"\033["<<buttonColor<<"m"<<setw(30)<<value<<"\033[0m";
        else  os<<setw(30)<<value;
        return os.str();
    };
    cout<<"|"<<right<<setw(80)<<append("",8)<<"|\n";
    cout<<"|"<<right<<put("1. 添加物品",1,30)<<right<<put("2. 批量导入",2,30)<<setw(20)<<""<<"|\n";
    cout<<"|"<<right<<put("3. 删除物品",3,30)<<right<<put("4. 修改物品",4,30)<<setw(20)<<""<<"|\n";
    cout<<"|"<<right<<put("5. 查看物品",5,30)<<right<<put("6. 查询拍品",6,30)<<setw(20)<<""<<"|\n";
    cout<<"|"<<right<<put("7. 上传拍品",7,30)<<right<<put("8. 返回首页",8,30)<<setw(20)<<""<<"|\n";
    cout<<line << "--\n";
    //@formatter:on
}

void selGoodMenuWithAdmin() {
    loginOrNot();
    int x = 1, y = 1;
    char operation;
    while (true) {
        int pos = (x - 1) * 2 + y;
        disGoodMenuWithAdmin(pos);
        operation = _getch();
        system("cls");
        switch (operation) {
            case '1':
                removeGood();
                break;
            case '2':
                modifyGood();
                break;
            case '3':
                findOpenGood();
                break;
            case '4':
                return;
            case 'w':
                if (x != 1) x--;
                break;
            case 's':
                if (x != 2) x++;
                break;
            case 'a':
                if (y != 1) y--;
                break;
            case 'd':
                if (y != 2) y++;
                break;
            case '\r':
                switch (pos) {
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
                }
                break;
        }
        //saveGoods();
    }

}

void disGoodMenuWithAdmin(int pos) {
    //@formatter:off
    cout << "\n\n\n\n";
    string line,shortline;
    string value="物品管理：";
    for(int i=1; i<=25; i++) shortline+='-';
    for(int i=1; i<=72; i++) line += '-';
    cout<<shortline<<"欢迎使用物品竞拍管理系统"<<shortline<<"\n";
    if(loginStatus){
        cout<<"|"<<value<<right<<setw(80-value.size())<<(currentUsername+"已登录")<<"|\n";
    }else{
        cout<<"|"<<value<<right<<setw(80-value.size())<<("未登录")<<"|\n";
    }
        auto put=[&](string value,int id,int width)->string{
        ostringstream os;
        if(id==pos) os<<"\033["<<buttonColor<<"m"<<setw(30)<<value<<"\033[0m";
        else  os<<setw(30)<<value;
        return os.str();
    };
    cout<<"|"<<right<<setw(80)<<append("",8)<<"|\n";
    cout<<"|"<<right<<put("1. 下架物品",1,30)<<right<<put("2. 修改物品",2,30)<<setw(20)<<""<<"|\n";
    cout<<"|"<<right<<put("3. 查询拍品",3,30)<<right<<put("4. 返回首页",4,30)<<setw(20)<<""<<"|\n";
    cout<<line << "--\n";
    //@formatter:on
}

void userAddGood(string username, Good g) {
    //如果已经有编号，则直接添加，确保不变性
    if (g.getId() != -1) {
        tables[username][g.getId()] = g;
        return;
    }
    //第一件商品编号为用户名哈希，后面递增（确保唯一性）
    int goodId;
    if (!maxGoodId.count(username)) {
        goodId = stoi(to_string(hasher(username)).substr(0, 6));
    } else {
        goodId = maxGoodId[username] + 1;
    }
    //设置为当前物品编号
    maxGoodId[username] = goodId;
    //在添加物品时设置
    g.setId(goodId);
    tables[username][goodId] = g;
}

void addGood() {
    contentHint("添加物品");
    outputWarning("添加物品必须合法合规，否则经管理员审查后给与封号处理\n\n");
    string goodName = getInput<string>("请输入物品名称：");
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

    pressAnyKey();
}

void removeGood() {
    contentHint("下架物品");
    map<int, Good> goods = tables["admin"];
    cout << "\n\n当前所有拍品的信息如下：\n";
    displayMyGoods(goods);
    int selId = getInput<int>("请选择要下架的拍品编号：");
    if (tables["admin"].count(selId)) {
        Good g = tables["admin"][selId];
        userAddGood(g.getUploader(), g);
        tables["admin"].erase(selId);
        outputHint("编号为［" + to_string(selId) + "］的拍品已下架！\n");
    } else {
        outputWarning("没有此编号的拍品，请重新输入！");
        removeGood();
    }
    pressAnyKey();
}

void displayMyGoods(map<int, Good> needToDisplay) {
    if (needToDisplay.size() == 0) {
        outputHint("当前还没有任何物品！\n\n");
        pressAnyKey();
        return;
    }
    auto maxNameCount = 7;
    auto maxDescribeCount = 16;
    auto maxCategoryCount = 8;
    for (const auto &[goodId, good]: needToDisplay) {
        maxNameCount = max(maxNameCount, countChineseCharacters(good.getGoodName()));
        maxDescribeCount = max(maxDescribeCount, countChineseCharacters(good.getDescribe()));
        //maxCategoryCount = max(maxCategoryCount, countChineseCharacters(good.getCategory()));
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
    cout << left << setw(20) << uploaderItem << "|\n";
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
    contentHint("删除物品");
    map<int, Good> currentGoods = tables[currentUsername];
    //判断是否有物品
    if (currentGoods.size() == 0) {
        outputWarning("当前没有物品可以删除，请先添加物品！\n\n");
        pressAnyKey();
        return;
    }

    delAgain:
    currentGoods = tables[currentUsername];
    if (currentGoods.size() == 0) {
        outputWarning("您已删除完全部物品！");
        pressAnyKey();
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

    pressAnyKey();
}

void modifyGood() {
    contentHint("修改物品");
    map<int, Good> currentGoods = tables[currentUsername];

    //检查是否有物品
    if (currentGoods.size() == 0) {
        cout << "当前没有物品可以修改，请先添加物品！\n\n";
        pressAnyKey();
        return;
    }

    //继续修改
    modifyAgain:
    currentGoods = tables[currentUsername];
    cout << "\n\n所有物品的信息如下：\n";
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
            "\n当前估价为（万元）：" + double2str(selGood.getAppraisal(), 4)
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

    pressAnyKey();
}

void getGoodById(map<int, Good> goods) {
    int selId = getInput<int>("请输入物品编号:");
    if (!goods.count(selId)) {
        outputWarning("没有查询到该物品，如果忘记编号建议使用[按照类型/名称查询]\n\n");
        pressAnyKey();
        return;
    }
    map<int, Good> selGoods;
    selGoods[selId] = goods[selId];
    outputHint("\n编号为" + ("[" + to_string(selId) + "]") + "的物品信息如下：");
    displayMyGoods(selGoods);
    pressAnyKey();
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
        outputWarning("没有找到类别为［" + specifyCatagory + "］的物品！\n\n\n");
        pressAnyKey();
        return;
    }
    outputHint("\n［" + specifyCatagory + "］类的物品信息如下：");
    displayMyGoods(selGoods);
    pressAnyKey();
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
        outputWarning("没有找到名为［" + specifyGoodName + "］的物品！\n\n\n");
        pressAnyKey();
        return;
    }
    outputHint("\n名为［" + specifyGoodName + "］的物品信息如下：");
    displayMyGoods(selGoods);
    pressAnyKey();
}

void findGood() {
    uploaderItem="持有用户";
    map<int, Good> goods = tables[currentUsername];
    string line;
    for (int i = 1; i <= 20 - 6; i++) line += "-";
    int pos = 1;
    auto put = [&](string value, int id, int width) -> string {
        ostringstream os;
        if (id == pos) os << "\033[33m" << left << setw(width) << value << "\033[0m";
        else os << left << setw(width) << value;
        return os.str();
    };
    auto dis = [&]() -> void {
        cout << line << "--\n";
        cout << "|" << put(append("按编号查找", 1), 1, 20) << "|\n";
        cout << "|" << put(append("按名称查找", 1), 2, 20) << "|\n";
        cout << "|" << put(append("按类别查找", 1), 3, 20) << "|\n";
        cout << "|" << put("查看所有拍品", 4, 20) << "|\n";
        cout << "|" << put(append("返回",4), 5, 20) << "|\n";
        cout << line << "--\n";
    };
    while (1) {
        dis();
        char c = _getch();
        switch (c) {
            case 'w':
                if (pos != 1) pos--;
                system("cls");
                break;
            case 's':
                if (pos != 5) pos++;
                system("cls");
                break;
            case '\r':
                switch (pos) {
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
                        pressAnyKey();
                        break;
                    case 5:
                        system("cls");
                        goto out;
                }
        }
    }
    out:;
}

void findOpenGood() {
    uploaderItem="上传用户";
    map<int, Good> goods = tables["admin"];
    string line;
    for (int i = 1; i <= 20 - 6; i++) line += "-";
    int pos = 1;
    auto put = [&](string value, int id, int width) -> string {
        ostringstream os;
        if (id == pos) os << "\033[33m" << left << setw(width) << value << "\033[0m";
        else os << left << setw(width) << value;
        return os.str();
    };
    auto dis = [&]() -> void {
        cout << line << "--\n";
        cout << "|" << put(append("按编号查找", 1), 1, 20) << "|\n";
        cout << "|" << put(append("按名称查找", 1), 2, 20) << "|\n";
        cout << "|" << put(append("按类别查找", 1), 3, 20) << "|\n";
        cout << "|" << put("查看所有拍品", 4, 20) << "|\n";
        cout << "|" << put(append("返回",4), 5, 20) << "|\n";
        cout << line << "--\n";
    };
    while (1) {
        dis();
        char c = _getch();
        switch (c) {
            case 'w':
                if (pos != 1) pos--;
                system("cls");
                break;
            case 's':
                if (pos != 5) pos++;
                system("cls");
                break;
            case '\r':
                switch (pos) {
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
                        pressAnyKey();
                        break;
                    case 5:
                        system("cls");
                        goto out;
                }
        }
    }
    out:;
}

void uploadGood() {
    contentHint("上传物品");
    map<int, Good> currentGoods = tables[currentUsername];
    //判断是否有物品
    if (currentGoods.size() == 0) {
        outputWarning("当前没有物品可以上传，请先添加物品！\n\n");
        pressAnyKey();
        return;
    }


    uploadAgain:
    currentGoods = tables[currentUsername];
    if (currentGoods.size() == 0) {
        outputWarning("您已上传完全部物品！\n\n");
        pressAnyKey();
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

    pressAnyKey();
}


void batchAddGood() {
    contentHint("批量添加物品");
    inputAgain:
    string fileName = getInput<string>("请输入文件路径，注意只能是JSON格式文件：");
    size_t dotPos = fileName.find_last_of(".");
    //检查文件格式
    if (dotPos == string::npos || fileName.substr(dotPos + 1) != "json") {
        outputWarning("文件格式错误，请检查文件格式是否为JSON！\n");
        goto inputAgain;
    }
    //检查路径合法性
    ifstream inputfile(fileName);
    if (!inputfile.is_open()) {
        outputWarning("文件打开失败，请检查文件路径是否正确并重新输入！\n");
        goto inputAgain;
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
    } catch (json::parse_error &ex) {
        //文件读取异常
        outputWarning("文件内容错误，请检查文件内容是否正确！\n");
        pressAnyKey();
        return;
    }
    inputfile.close();
    outputHint("批量导入成功！");
    pressAnyKey();
}


/*-------------------------------------------------交易管理区----------------------------------------------*/
void selectRecordMenu() {
    //misjudgment("交易管理");
    char operation;
    int x = 1, y = 1;
    while (true) {
        int pos = (x - 1) * 2 + y;
        displayRecordMenu(pos);
        operation = _getch();
        system("cls");
        switch (operation) {
            case '1':
                displayRecords();
                break;
            case '2':
                auction();
                break;
            case '3':
                return;
            case 'w':
                if (x != 1) x--;
                break;
            case 's':
                if ((x==1 and y==2) or x==2);
                else x++;
                break;
            case 'a':
                if (y != 1) y--;
                break;
            case 'd':
                if (x != 2 and y != 2) y++;
                break;
            case '\r':
                switch (pos) {
                    case 1:
                        displayRecords();
                        break;
                    case 2:
                        auction();
                        break;
                    case 3:
                        return;
                }
                break;
        }
        //saveRecords();
    }
}

void displayRecordMenu(int pos) {
    //@formatter:off
    cout << "\n\n\n\n";
    string line,shortline;
    string value="交易管理：";
    for(int i=1; i<=25; i++) shortline+='-';
    for(int i=1; i<=72; i++) line += '-';
    cout<<shortline<<"欢迎使用物品竞拍管理系统"<<shortline<<"\n";
    if(loginStatus){
        cout<<"|"<<value<<right<<setw(80-value.size())<<(currentUsername+"已登录")<<"|\n";
    }else{
        cout<<"|"<<value<<right<<setw(80-value.size())<<("未登录")<<"|\n";
    }
    auto put=[&](string value,int id,int width)->string{
        ostringstream os;
        if(id==pos) os<<"\033["<<buttonColor<<"m"<<setw(30)<<value<<"\033[0m";
        else  os<<setw(30)<<value;
        return os.str();
    };
    cout<<"|"<<right<<setw(80)<<append("",8)<<"|\n";
    cout<<"|"<<right<<put("1. 交易记录",1,30)<<right<<put("2. 开始竞拍",2,30)<<right<<setw(20)<<""<<"|\n";
    cout<<"|"<<right<<put("3. 返回首页",3,30)<<right<<put(append("", 4),0,30)<<right<<setw(20)<<""<<"|\n";
    cout << line << "--\n";
    //@formatter:on
}

void englandAuction(Good curGood) {
    contentHint("英式拍卖");
    srand(time(0));
    double startingPrice = getInput<double>("\n\n请设置起拍价（万元）：");
    double minIncrement = getInput<double>("请设置最小加价幅度（万元）：");
    double reservePrice = getInput<double>("请设置保留价（万元）：");

    string uploder = curGood.getUploader();
    double apprisal = curGood.getAppraisal();

    vector<string> participants;
    for (const auto [username, user]: users) {
        if (username == "admin" || username == uploder || user.isAvailable == false) continue;
        participants.push_back(username);
    }
    int number = participants.size();

    double prePrice = startingPrice;
    string preUsername;
    outputHighlight("\n\n下面由用户进行出价\n");

    //出价顺序全排列
    int numberOfAbandonments = 0;
    vector<int> indexs(number);
    iota(indexs.begin(), indexs.end(), 0);
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rng(seed);
    shuffle(indexs.begin(), indexs.end(), rng);
    set<string> abandonUsers, joinUsers;
    while (1) {
        for (int index: indexs) {
            inputAgain:
            string username = participants[index];
            double curNeed = prePrice + minIncrement;
            cout << setw(10) << username;
            string hint = "［当前最低出价为 " + double2str(curNeed, 4)
                          + "(万元)" + (curNeed > apprisal ? "  \033[33m已超出估价\033[0m" : "") + "]:";
            string price = getInput<string>(hint);
            if (price.empty()) {
                abandonUsers.insert(username);
            } else if (stod(price) < curNeed) {
                outputWarning("当前出价低于最低出价，请重新出价或者放弃！\n");
                goto inputAgain;
            } else {
                abandonUsers.clear();
                joinUsers.insert(username);
                prePrice = stod(price);
                preUsername = username;
            }
            if (abandonUsers.size() == participants.size() - 1) {
                goto next;
            }
        }
    }
    next:
    for (string username: joinUsers) {
        users[username].countOfSessions++;
    }
    //结束信息
    outputHighlight("无人继续加价，最终竞买价为：" + double2str(prePrice, 4));
    if (prePrice < reservePrice) {
        outputWarning("\n由于最终成交价小于物品的保留价，交易失败！\n");
    } else {
        transaction(uploder, preUsername, curGood, prePrice);
    }
    pressAnyKey();
}

void sealedBiddingAuction(Good curGood) {
    contentHint("招标式拍卖");
    outputHighlight("\n竞买人将出价加密发送给拍卖人\n");
    map<string, int> bidPrice;
    string uploader = curGood.getUploader();
    string category = curGood.getCategory();
    vector<string> participants;
    for(auto [username,user] : users){
        if(username=="admin" || username==uploader || user.isAvailable==false) continue;
        participants.push_back(username);
    }
    //预测拍卖成功率
    auto predictionSuccessRate = [&](string curUsername) -> double {
        int effectiveNumber = participants.size();
        double P_other_fails = 1.0;
        for (auto otherUsername : participants) {
            if(otherUsername==curUsername) continue;
            P_other_fails *= (1 - users[otherUsername].difCategorySucessRate[category]);
        }
        double lambda = 1.0 / effectiveNumber;
        double P_win = lambda * P_other_fails + (1 - lambda) * users[curUsername].difCategorySucessRate[category];
        return P_win;
    };

    for (const auto [username, user]: users) {
        if (username == uploader || username == "admin" || user.isAvailable == false) continue;
        double P_win = predictionSuccessRate(username);
        cout << "当前由" << username << "用户递交竞价，回车可放弃竞买！\n";
        cout << "系统预测当前竞买成功率为：" << "\033[33m" << double2str(P_win, 4) << "\033[0m";
        //只有这里需要加密
        string price = secureInput("\n如果你想成功竞拍，请斟酌您的递价（万元）：",1);
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

    outputHighlight("\n\n拍卖人选择竞买人，若没有满意的递价请回车放弃选择！");
    int id = 0;
    map<string, string> id2user;
    string line;
    for (int i = 1; i <= 90 - 14; i++) line += '-';
    cout << line << "--\n|";
    cout << left << setw(20) << "编号" << left << setw(30) << "参与竞买人" << setw(40) << "递交价（万元）" << "|\n";
    cout << "|" << line << "|\n";
    for (auto [username, price]: sortedBidPrice) {
        string curId = int2str(++id, 2);
        id2user[curId] = username;
        cout << "|";
        cout << left << setw(20) << append(curId, 2);
        cout << left << setw(30) << append(username, 5);
        cout << left << setw(40) << append(double2str(price, 4), 7);
        cout << "|\n";
    }
    cout << line << "--\n";

    //记录交易结果
    string selId = getInput<string>("拍卖人最终选择的竞买人的编号，回车表示放弃竞买！");
    string username = id2user[selId];
    if (selId.empty()) {
        outputWarning("拍卖人没有选择竞买人，竞买失败！");
    } else if (bidPrice.count(username)) {
        transaction(uploader, username, curGood, bidPrice[username]);
    } else {
        outputWarning("没有找到竞买记录，竞买失败！");
    }
    pressAnyKey();
}

void auction() {
    contentHint("选择拍品");
    //选择拍品->选择拍卖方式->拍卖->记录结果
    map<int, Good> openGoods = tables["admin"];
    cout << "\n\n所有拍品的信息如下：\n";
    displayMyGoods(openGoods);
    int selId = getInput<int>("选择要竞拍的拍品编号：");
    Good currentItem = openGoods[selId];
    toString(currentItem);
    outputHint("\n\n按任意键继续...");
    char ch=_getch();
    system("cls");

    string line;
    for (int i = 1; i <= 20 - 6; i++) line += "-";
    int pos = 1;
    auto put = [&](string value, int id, int width) -> string {
        ostringstream os;
        if (id == pos) os << "\033[33m" << left << setw(width) << value << "\033[0m";
        else os << left << setw(width) << value;
        return os.str();
    };
    auto dis = [&]() -> void{
        contentHint("选择拍卖方式");
        cout << line << "--\n";
        cout << "|" << put(append("英格兰式拍卖", 1), 1, 20) << "|\n";
        cout << "|" << put("密封招标式拍卖", 2, 20) << "|\n";
        cout << line << "--\n";
    };
    while (1) {
        dis();
        char c = _getch();
        system("cls");
        switch (c) {
            case 'w':
                if (pos != 1) pos--;
                break;
            case 's':
                if (pos != 2) pos++;
                break;
            case '\r':
                switch (pos) {
                    case 1:
                        englandAuction(currentItem);
                        break;
                    case 2:
                        sealedBiddingAuction(currentItem);
                        break;
                }
                goto out;
        }
    }
    out:;
    //更新用户信息
    for(auto [username,user] : users){
        int cnt=user.countOfSessions;
        int get=0;
        for(auto category : categories){
            get+=user.difCategoryCnt[category];
            user.difCategorySucessRate[category]=1.0*user.difCategoryCnt[category]/cnt;
        }
        user.sucessRate=get/cnt;
    }
}

void transaction(string uploader, string shootername, Good g, double price) {
    //竞买成功提示
    string goodNews = "恭喜" + shootername + "以" + double2str(price, 2) + "万元的成交价,将" +
                      "用户" + uploader + "上传的［" + g.getGoodName() + "］收入囊中";
    outputHighlight(goodNews);
    //物品转移
    tables["admin"].erase(g.getId());
    g.setAppraisal(price * 0.9);
    g.setUploader(shootername);
    userAddGood(shootername, g);
    users[shootername].difCategoryCnt[g.getCategory()]++;
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
    for (int i = 1; i <= 132+30-8; i++) line += '-';
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
    cout << "\n\n交易记录如下：\n";
    cout << line << "--\n|";
    cout << left << setw(20) << "交易单号" << left << setw(40) << "交易时间";
    cout << left << setw(30) << "中拍者" << left << setw(30) << "成交价";
    cout << left << setw(30) << "拍品编号"<< left << setw(30) << transfer("拍品种类",8)<<"|\n";
    cout << "|" << line << "|" << endl;
    for (const auto &record: records) {
        cout << "|";
        std::ostringstream oss;
        oss << left << setw(20) << append(int2str(++tranNumber, 6), 4);
        oss << record;
        oss << left << setw(30) << transfer(tables[record.getShooterName()][record.getGoodId()].getCategory(),8);
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
    cout << line << "--\n\n\n";
    next:
    outputHint("\n\n按任意键继续查看统计信息");
    char ch=_getch();
    system("cls");
    contentHint("统计信息");
    cout << "竞拍物品数量：" << records.size() << endl;
    cout << "交易总额：" << totalPrice <<" 万元"<< endl;

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
        oss << left << setw(30) << append(to_string(count), 4);
        //非管理员会突出显示自己的排名
        if (username == currentUsername) cout << "\033[33m" << oss.str() << "\033[0m";
        else cout << oss.str();
        cout << "|\n";
    }
    cout << shortline << "--\n\n\n";
    pressAnyKey();
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


    //加载各用户的物品表
    ifstream goodsFile(goodsfilename);
    goodsFile >> j;
    if (!j.empty()) {
        for (const auto &[username, innerJ]: j.items()) {
            map<int, Good> goods;
            for (const auto &[goodId, goodJson]: innerJ.items()) {
                Good good = goodJson.get<Good>(); //使用反序列化从JSON格式读取对象
                goods[stoi(goodId)] = good;
                maxGoodId[username] = max(maxGoodId[username], stoi(goodId));
                if (username == "admin") {
                    string uploader = good.getUploader();
                    maxGoodId[uploader] = max(maxGoodId[uploader], stoi(goodId));
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
            Record record1=record.get<Record>();
            string name=record1.getShooterName();
            maxGoodId[name]=max(maxGoodId[name],record1.getGoodId());
            records.push_back(record1);
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
void displayMainMenu(int pos) {
    //@formatter:off
    cout<<"                             ,,                                            \n";
    cout<<"`7MMF'     A     `7MF'     `7MM                                            \n";
    cout<<"  `MA     ,MA     ,V         MM                                            \n";
    cout<<"   VM:   ,VVM:   ,V .gP\"Ya   MM  ,p6\"bo   ,pW`Wq.`7MMpMMMb.pMMMb.  .gP\"Ya  \n";
    cout<<"    MM.  M' MM.  M',M'   Yb  MM 6M'  OO  6W'   `Wb MM    MM    MM ,M'   Yb \n";
    cout<<"    `MM A'  `MM A' 8M\"\"\"\"\"\"  MM 8M       8M     M8 MM    MM    MM 8M\"\"\"\"\"\" \n";
    cout<<"     :MM;    :MM;  YM.    ,  MM YM.    , YA.   ,A9 MM    MM    MM YM.    , \n";
    cout<<"      VF      VF    `Mbmmd'.JMML.YMbmd'   `Ybmd9'.JMML  JMML  JMML.`Mbmmd' \n";



    cout << "\n\n\n\n";
    string line,shortline;
    string value="首页：";
    for(int i=1; i<=25; i++) shortline+='-';
    for(int i=1; i<=72; i++) line += '-';
    cout<<shortline<<"欢迎使用物品竞拍管理系统"<<shortline<<"\n";
    if(loginStatus){
        cout<<"|"<<value<<right<<setw(80-value.size()-2)<<(currentUsername+"已登录")<<"|\n";
    }else{
        cout<<"|"<<value<<right<<setw(80-value.size()-2)<<("未登录")<<"|\n";
    }
    string recordsValue=currentUsername=="admin"?"3. 交易管理":"3. 交易记录";
    auto put=[&](string value,int id,int width)->string{
        ostringstream os;
        if(id==pos) os<<"\033["<<buttonColor<<"m"<<setw(30)<<value<<"\033[0m";
        else  os<<setw(30)<<value;
        return os.str();
    };
    cout<<"|"<<right<<setw(80)<<append("",8)<<"|\n";
    cout<<"|"<<right<<put("1. 用户管理",1,30)<<right<<put("2. 物品管理",2,30)<<right<<setw(20)<<""<<"|\n";
    cout<<"|"<<right<<put(recordsValue,3,30)<<right<<put("4. 关于系统",4,30)<<right<<setw(20)<<""<<"|\n";
    cout<<"|"<<right<<put("5. 退出系统",5,30)<<right<<put(append("",4),0,30)<<right<<setw(20)<<""<<"|\n";
    cout << line << "--\n";
    //@formatter:on
}

void systemIntroduction() {
    // @formatter:off  关闭格式化
    //cout<<"           _____                    _____                    _____                    _____                 \n";
    //cout<<"          /\\    \\                  /\\    \\                  /\\    \\                  /\\    \\                \n";
    //cout<<"         /::\\    \\                /::\\    \\                /::\\____\\                /::\\    \\               \n";
    //cout<<"         \\:::\\    \\              /::::\\    \\              /::::|   |               /::::\\    \\              \n";
    //cout<<"          \\:::\\    \\            /::::::\\    \\            /:::::|   |              /::::::\\    \\             \n";
    //cout<<"           \\:::\\    \\          /:::/\\:::\\    \\          /::::::|   |             /:::/\\:::\\    \\            \n";
    //cout<<"            \\:::\\    \\        /:::/__\\:::\\    \\        /:::/|::|   |            /:::/__\\:::\\    \\           \n";
    //cout<<"            /::::\\    \\      /::::\\   \\:::\\    \\      /:::/ |::|   |            \\:::\\   \\:::\\    \\          \n";
    //cout<<"   ____    /::::::\\    \\    /::::::\\   \\:::\\    \\    /:::/  |::|___|______    ___\\:::\\   \\:::\\    \\         \n";
    //cout<<"  /\\   \\  /:::/\\:::\\    \\  /:::/\\:::\\   \\:::\\    \\  /:::/   |::::::::\\    \\  /\\   \\:::\\   \\:::\\    \\        \n";
    //cout<<" /::\\   \\/:::/  \\:::\\____\\/:::/  \\:::\\   \\:::\\____\\/:::/    |:::::::::\\____\\/::\\   \\:::\\   \\:::\\____\\       \n";
    //cout<<" \\:::\\  /:::/    \\::/    /\\::/    \\:::\\  /:::/    /\\::/    / ~~~~~/:::/    /\\:::\\   \\:::\\   \\::/    /       \n";
    //cout<<"  \\:::\\/:::/    / \\/____/  \\/____/ \\:::\\/:::/    /  \\/____/      /:::/    /  \\:::\\   \\:::\\   \\/____/        \n";
    //cout<<"   \\::::::/    /                    \\::::::/    /               /:::/    /    \\:::\\   \\:::\\    \\            \n";
    //cout<<"    \\::::/____/                      \\::::/    /               /:::/    /      \\:::\\   \\:::\\____\\           \n";
    //cout<<"     \\:::\\    \\                      /:::/    /               /:::/    /        \\:::\\  /:::/    /           \n";
    //cout<<"      \\:::\\    \\                    /:::/    /               /:::/    /          \\:::\\/:::/    /            \n";
    //cout<<"       \\:::\\    \\                  /:::/    /               /:::/    /            \\::::::/    /             \n";
    //cout<<"        \\:::\\____\\                /:::/    /               /:::/    /              \\::::/    /              \n";
    //cout<<"         \\::/    /                \\::/    /                \\::/    /                \\::/    /               \n";
    //cout<<"          \\/____/                  \\/____/                  \\/____/                  \\/____/                \n";
    //cout<<"\n\n";


    cout<<"\n\n";
    cout<<"  `7MMF'      db      `7MMM.     ,MMF' .M\"\"\"bgd   \n";
    cout<<"    MM       ;MM:       MMMb    dPMM  ,MI    \"Y   \n";
    cout<<"    MM      ,V^MM.      M YM   ,M MM  `MMb.       \n";
    cout<<"    MM     ,M  `MM      M  Mb  M' MM    `YMMNq.   \n";
    cout<<"    MM     AbmmmqMA     M  YM.P'  MM  .     `MM   \n";
    cout<<"    MM    A'     VML    M  `YM'   MM  Mb     dM   \n";
    cout<<"  .JMML..AMA.   .AMMA..JML. `'  .JMML.P\"Ybmmd\"    \n";
    cout<<"\n\n";


    cout<<"内部版本号 #IAMS_1.0,  2024年5月12日构建\n";
    cout<<"Designer: Ba Yun-Hao  版权所有 ©2024-2099    \n";
    cout<<"本项目已在GitHub上开开源：";
    cout<<"\033[33m"<<"https://github.com/Ba-YH/IAMS\n"<<"\033[0m";
    cout<<"如果你对项目使用有任何疑问，请在GitHub中查看!\n";
    cout<<"如果你发现该项目的任何问题或是需求新的功能，欢迎您在issues提出!\n\n\n";
    // @formatter:on   开启格式化
    pressAnyKey();
}

void selectMainMenu() {
    char operation;
    int x = 1, y = 1;
    while (true) {
        int pos = (x - 1) * 2 + y;
        displayMainMenu(pos);
        operation = _getch();
        system("cls");
        switch (operation) {
            case '1':
                if (currentUsername == "admin") selUserMenuWithAdmin();
                else selectUserMenu();
                break;
            case '2':
                if (currentUsername == "admin") selGoodMenuWithAdmin();
                else selectGoodMenu();
                break;
            case '3':
                if (currentUsername == "admin") selectRecordMenu();
                else displayRecords();
                break;
            case '4':
                systemIntroduction();
                break;
            case '5':
                exitTheSystem();
                break;
            case 'w':
                if (x != 1) x--;
                break;
            case 's':
                if ((x==2 and y==2) or x == 3);
                else x++;
                break;
            case 'a':
                if (y != 1) y--;
                break;
            case 'd':
                if (x != 3 and y != 2) y++;
                break;
            case '\r':
                switch (pos) {
                    case 1:
                        if (currentUsername == "admin") selUserMenuWithAdmin();
                        else selectUserMenu();
                        break;
                    case 2:
                        if (currentUsername == "admin") selGoodMenuWithAdmin();
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
                }
                break;
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
