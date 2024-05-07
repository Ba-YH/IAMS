#指定cmake最小版本
cmake_minimum_required(VERSION 3.27)
#项目名称
project(itemAuctionManagementSystem)

set(CMAKE_CXX_STANDARD 17)

#忽略警告信息
#如果不是中文的JSON，简易开启
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -fexec-charset=gbk")

#设置源文件
set(SOURCE_FILES main.c++)
set(OPENSSL_USE_STATIC_LIBS TRUE)
#使用OpenSSL库
find_package(OpenSSL REQUIRED)
#设置由源文件生成的可执行文件名称
add_executable(main ${SOURCE_FILES})
#链接OpenSSL库，注意与find_package的大小写相同
target_link_libraries(main OpenSSL::Crypto)

