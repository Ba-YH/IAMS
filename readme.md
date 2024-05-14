### 目录
1. [项目介绍](#项目介绍)
2. [上手指南](#上手指南)
   * [下载到本地](#1-下载到本地)
   * [项目依赖](#2-项目依赖)
   * [注意事项](#3-注意事项) 
3. [文件目录说明](#文件目录说明)
4. [最后](#最后)



### 项目介绍
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;本项目为大一下学期的高级程序设计课程设计作业，主要功能为实现一个物品惊竞拍管理系统，由于没有使用到GUI库，需要使用终端进行交互。
通过选择相应菜单项来实现相应功能的调用。
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;该项目使用多权限的用户系统，包括管理员和普通用户，普通用户可以是买家和卖家。管理员可以对拍品和用户进行管理，包括下架和修改拍品。
普通用户只能管理个人物品，但可以将物品上传到拍卖系统并参与到由管理员主持拍卖活动中。
### 上手指南
#### 1. 下载到本地
```shell
git clone git@github.com:Ba-YH/IAMS.git
```
#### 2. 项目依赖
1. 本项目用到的外部头文件为[nlohmann/json.hpp](https://github.com/nlohmann/json/releases/tag/v3.11.3)和[ut8.h](https://github.com/sheredom/utf8.h)请自行下载并添加到编译器目录中
2. 使用到的外部库为[OpenSSL](https://github.com/openssl/openssl/releases)，请自行下载并链接到
   <br>
   1. 如果你使用gcc/g++/clang/clang++编译，请删除CMakeLists.txt，并将openssl的lib目录添加到编译器目录中
   2. 如果你使用cmake编译，请在CMakeLists.txt中将`your_openssl_path`替换为您openssl的下载目录
      ```cmake
      set(${OPENSSL_ROOT_DIR} your_openssl_path)
      ```
#### 3. 注意事项
由于本项目全局使用utf-8编码，所以文件编码必须为utf-8.在运行时，终端的编码必须为utf-8，否则会出现乱码
<br>
请在运行前，使用`chcp 65001`改变终端编码，也可以编写并执行如下的批处理文件
```shell
@echo off
chcp 65001 > null
.\main.exe
```

### 文件目录说明
```markdown
IAMS                      
│  .gitignore             
│  CMakeLists.txt         
│  main.c++               
│  readme.md
│
├─data
│   ├─goods.json
│   ├─records.json
│   ├─test.json
│   └─users.json
│
├─include
│   ├─Good.h
│   ├─Record.h
│   └─User.h
│
├─src
│   ├─Good.c++
│   ├─Record.c++
│   └─User.c++
│
└─util
    ├─Encryptor.h
    └─Output.h

```
### 最后
本项目仅供学习交流，如果您对项目有任何的问题或者建议，欢迎在issus中提出
