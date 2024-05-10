#指定cmake最小版本
cmake_minimum_required(VERSION 3.27)
#项目名称
project(itemAuctionManagementSystem)

set(CMAKE_CXX_STANDARD 17)


#设置源文件
set(SOURCE_FILE main.c++)
#设置OPENSSL根目录路径
set(${OPENSSL_ROOT_DIR} D:/Code/Libs/openssl)
#寻找OPENSSL
find_package(OpenSSL REQUIRED)
add_executable(main ${SOURCE_FILE})
# 链接 OpenSSL 库
target_link_libraries(main OpenSSL::Crypto)