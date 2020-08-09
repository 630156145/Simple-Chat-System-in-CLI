#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "winsock2.h"
#include <map>
#pragma comment(lib,"ws2_32.lib")//引用库文件
using namespace std;

const int buf_size = 100;
const int port = 11111;//端口号
map<string, int> userToSockMap;


DWORD WINAPI serverThread(LPVOID lpParamter) {
    SOCKET sock = (SOCKET)lpParamter;
    cout << "Start socket " << sock << " at port " << port << endl;
    char recvBuf[buf_size];
    char sendBuf[buf_size];
    //接受用户信息
    memset(recvBuf, 0, sizeof(recvBuf));
    recv(sock, recvBuf, sizeof(recvBuf), 0);
    string username = recvBuf;
    //存储用户信息
    userToSockMap.insert(pair<string, int>(username, sock));
    cout << "User " << username << " logs in, socket is " << sock << endl;

    //用户要发给谁
    string targetUser;
    while (true) {
        memset(recvBuf, 0, sizeof(recvBuf));
        if (recv(sock, recvBuf, sizeof(recvBuf), 0) == SOCKET_ERROR) {
            cout << "Receiving message failed." << endl;
            return -1;
        }
        targetUser = recvBuf;
        if (userToSockMap.count(targetUser) == 0) { //用户不存在
            strcpy(sendBuf, "error");
            cout << "To user " << username << " - User " << targetUser << " does not exist." << endl;
            send(sock, sendBuf, sizeof(sendBuf), 0);
        }        
        else {
            send(sock, "1", 2, 0);
            break;
        }
    }

    int targetSock = userToSockMap[targetUser];
    cout << "User " << username << " will send message to user " << targetUser << endl;

    while (true) {  //接收数据
        memset(recvBuf, 0, sizeof(recvBuf));
        if (recv(sock, recvBuf, sizeof(recvBuf), 0) == SOCKET_ERROR) {
            cout << "Receiving message failed." << endl;
            break;
        }
        //发送给目标
        if (send(targetSock, recvBuf, sizeof(recvBuf), 0) == SOCKET_ERROR) {
            cout << "Sending message \"" << recvBuf << "\" to user " << targetUser << " failed." << endl;
            break;
        }
        
        cout << "Message from user " << username << " \"" << recvBuf << "\" has been sent to user " << targetUser << endl;
    }
    closesocket(sock);
    return 0;
}

int s_main() {
    cout << "Server starts." << endl;
    //初始化链接库
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSA startup error." << endl;
        return -1;
    }

    //创建socket
    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port); //1024以上的端口号
    //INADDR_ANY就是指定地址为0.0.0.0的地址，这个地址事实上表示不确定地址，或“所有地址”、“任意地址”。 一般来说，在各个系统中均定义成为0值
    addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);

    SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
        cout << "Bind error." << endl;
        if (WSAGetLastError()==10048) cout<<"server has already started." << endl;
        return -1;
    }

    if (listen(sockSrv, 10) == SOCKET_ERROR) {
        cout << "Listen error" << endl;
        return -1;
    }

    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);

    while (true) {
        //等待客户请求到来
        SOCKET sockRecv = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
        if (sockRecv == SOCKET_ERROR) {
            cout << "Accept failed." << endl;
            break;
        }

        HANDLE hThread = CreateThread(NULL, 0, serverThread, (LPVOID)sockRecv, 0, NULL);
        CloseHandle(hThread);

    }

    closesocket(sockSrv); //关闭套接字
    WSACleanup(); //释放初始化Ws2_32.dll所分配的资源
    return 0;
}