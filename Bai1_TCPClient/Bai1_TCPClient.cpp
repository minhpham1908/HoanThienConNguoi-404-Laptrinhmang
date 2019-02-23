// Bai1_TCPClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"


using namespace std;
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char* argv[]) {
	string serverIP = argv[1];
	int port = atoi(argv[2]);
	printf("Connecting to %s:%u\n", serverIP.c_str(), port);
	//Khoi tao winsock
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//Khoi tao socket
	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Khoi tao gia tri dich den
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
	serverAddr.sin_port = htons(port);

	//Ket noi den server

	int he = connect(client, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	char buf[256];
	int ret;
	ret = recv(client, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s\n", buf);

	while (true) {

		printf("Nhap du lieu can gui:\n");
		gets_s(buf);

		send(client, buf, strlen(buf), 0);

		if (strncmp(buf, "exit", 4) == 0) break;
	}
	closesocket(client);
	WSACleanup();
	system("pause");
	return 0;
}
