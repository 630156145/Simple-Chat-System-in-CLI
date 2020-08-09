#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "winsock2.h"
#include <map>
#pragma comment(lib,"ws2_32.lib")//引用库文件
using namespace std;

const int buf_size = 100;
const int port = 11111;//端口号


//接收服务器发来的消息
DWORD WINAPI recvThread(LPVOID lpParamter) {
    SOCKET sockRecv = (SOCKET)lpParamter;
    char recvBuf[buf_size];

    while (true) {
        memset(recvBuf, 0, sizeof(recvBuf));
        if (recv(sockRecv, recvBuf, sizeof(recvBuf), 0) == SOCKET_ERROR) {
            cout << "Receiving message failed." << endl;
            break;
        }
        cout << "[Receive message] " << recvBuf << endl;
    }

    closesocket(sockRecv);
    return 0;
}


//向服务器发送信息
DWORD WINAPI sendThread(LPVOID lpParamter) {
    cout << "Please enter message." << endl;
    SOCKET sockSend = (SOCKET)lpParamter;
    char sendBuf[buf_size];    
    while (true) {
        cin >> sendBuf;
        if (send(sockSend, sendBuf, sizeof(sendBuf), 0) == SOCKET_ERROR) {
            cout << "Sending message failed." << endl;
            break;
        }
    }

    closesocket(sockSend);
    return 0;
}


int c_main() {
    map<string, string> userInfoMap;
    userInfoMap.insert(pair<string, string>("AA", "11"));
    userInfoMap.insert(pair<string, string>("BB", "22"));

    cout << "Client starts." << endl;
   
    //初始化链接库
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSA startup error." << endl;
        return -1;
    }

    //创建socket
    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port); //端口号
    addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1"); //IP地址

    SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

    //向服务器发出连接请求
    if (connect(sockClient, (struct  sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET) {
        cout << "Connect error." << endl;
        if (WSAGetLastError()==10061) cout<<"no server." << endl;
        return -1;
    }

    //获取用户名
    cout << "Enter username: ";
    string username;
    cin >> username;
    while (userInfoMap.count(username) == 0) {
        cout << "Username does not exist, try again: ";
        cin >> username;
    }
    //验证密码
    cout << "Enter password: ";
    string password;
    cin >> password;
    while (password != userInfoMap[username]) {
        cout << "Password is wrong, try again: ";
        cin >> password;
    }

    char buf[buf_size];
    //发送用户名
    strcpy(buf, username.data());
    send(sockClient, buf, sizeof(buf), 0);

    //获取发送对象的id，如果对方id未登录，需要重新输入
    while (true) {
        cout << "Enter username you want to send to: ";
        string targetUser;
        cin >> targetUser;
        strcpy(buf, targetUser.data());
        if (send(sockClient, buf, sizeof(buf), 0) == SOCKET_ERROR) {
            cout << "Sending message failed." << endl;
            continue;
        }
        memset(buf, 0, sizeof(buf));
        if (recv(sockClient, buf, sizeof(buf), 0) == SOCKET_ERROR) {
            cout << "Receiving message failed." << endl;
            return -1;
        }
        if (strcmp(buf, "error")==0) {
            cout << "User does not exist." << endl;
        }
        else break;
    }

    HANDLE hThreadSend = CreateThread(NULL, 0, sendThread, (LPVOID)sockClient, 0, NULL);
    HANDLE hThreadRecv = CreateThread(NULL, 0, recvThread, (LPVOID)sockClient, 0, NULL);

    while (true){}

    closesocket(sockClient); //关闭套接字
    WSACleanup();//释放初始化Ws2_32.dll所分配的资源
    return 0;
}