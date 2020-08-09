CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ChatSystem)
#把当前目录(.)下所有源代码文件和头文件加入变量SRC_LIST
AUX_SOURCE_DIRECTORY(. SRC_LIST)
#生成应用程序 ChatSystem (在windows下会自动生成ChatSystem.exe)
ADD_EXECUTABLE(ChatSystem ${SRC_LIST})