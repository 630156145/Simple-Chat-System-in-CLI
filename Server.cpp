#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "winsock2.h"
#include <cstdlib>
#include <map>
#pragma comment(lib,"ws2_32.lib")//���ÿ��ļ�
using namespace std;

const int buf_size = 100;
const int port = 11111;//�˿ں�
map<string, int> userToSockMap;


DWORD WINAPI serverThread(LPVOID lpParamter) {
    SOCKET sock = (SOCKET)lpParamter;
    cout << "start sock " << sock << " at port " << port << endl;
    char recvBuf[buf_size];
    char sendBuf[buf_size];
    //�����û���Ϣ
    memset(recvBuf, 0, sizeof(recvBuf));
    recv(sock, recvBuf, sizeof(recvBuf), 0);
    string username = recvBuf;
    //�洢�û���Ϣ
    userToSockMap.insert(pair<string, int>(username, sock));
    cout << "User " << username << " logs in, socket is " << sock << endl;

    //�û�Ҫ����˭
    string targetUser;
    while (true) {
        memset(recvBuf, 0, sizeof(recvBuf));
        if (recv(sock, recvBuf, sizeof(recvBuf), 0) == SOCKET_ERROR) {
            cout << "Receiving message failed." << endl;
            exit(1);
        }
        targetUser = recvBuf;
        if (userToSockMap.count(targetUser) == 0) { //�û�������
            strcpy(sendBuf, "error");
            cout <<  "User " << targetUser << " does not exist." << endl;
            send(sock, sendBuf, sizeof(sendBuf), 0);
        }        
        else {
            send(sock, "1", 2, 0);
            break;
        }
    }

    int targetSock = userToSockMap[targetUser];
    cout << "user " << username << " will send message to user " << targetUser << ", socket is " << targetSock << endl;

    while (true) {  //��������
        memset(recvBuf, 0, sizeof(recvBuf));
        if (recv(sock, recvBuf, sizeof(recvBuf), 0) == SOCKET_ERROR) {
            cout << "Receiving message failed." << endl;
            break;
        }
        //���͸�Ŀ��
        if (send(targetSock, recvBuf, sizeof(recvBuf), 0) == SOCKET_ERROR) {
            cout << "Sending message \"" << recvBuf << "\" to user " << targetUser << " failed." << endl;
            break;
        }
        
        cout << "Message from user " << username << " \"" << recvBuf << "\" has sent to user " << targetUser << endl;
    }
    closesocket(sock);
    return 0;
}

int s_main() {
    cout << "Server starts." << endl;
    //��ʼ�����ӿ�
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSA startup error." << endl;
        exit(1);
    }

    //����socket
    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port); //1024���ϵĶ˿ں�
    //INADDR_ANY����ָ����ַΪ0.0.0.0�ĵ�ַ�������ַ��ʵ�ϱ�ʾ��ȷ����ַ�������е�ַ�����������ַ���� һ����˵���ڸ���ϵͳ�о������Ϊ0ֵ
    addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);

    SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
        cout << "Bind error." << endl;
        if (WSAGetLastError()==10048) cout<<"server has already started." << endl;
        exit(1);
    }

    if (listen(sockSrv, 10) == SOCKET_ERROR) {
        cout << "Listen error" << endl;
        exit(1);
    }

    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);

    while (true) {
        //�ȴ��ͻ�������
        SOCKET sockRecv = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
        if (sockRecv == SOCKET_ERROR) {
            cout << "Accept failed." << endl;
            break;
        }

        HANDLE hThread = CreateThread(NULL, 0, serverThread, (LPVOID)sockRecv, 0, NULL);
        CloseHandle(hThread);

    }

    closesocket(sockSrv); //�ر��׽���
    WSACleanup(); //�ͷų�ʼ��Ws2_32.dll���������Դ
    system("pause");
    return 0;
}