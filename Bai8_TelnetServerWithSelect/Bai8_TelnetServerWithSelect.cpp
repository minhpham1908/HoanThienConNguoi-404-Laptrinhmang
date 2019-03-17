// Bai8_TelnetServerWithSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")

using namespace std;


void RemoveClient(SOCKET);
bool directory_exists(char* buffer);
string cstringTocppString(char* buffer);
void storeDirToText(char* buffer);

SOCKET clients[64];
int numClients;
SOCKET clientsConnected[64];

int numClientsConnected;

int main()
{
	std::cout << "Hello World!\n";
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	numClients = 0;
	fd_set master;
	int ret;
	char recvBuffer[256];
	char fileBuf[1024];
	/*char cmdBuf[256];
	char pathBuf[256];*/
	while (true) {
		FD_ZERO(&master);
		FD_SET(listener, &master);
		for (int i = 0; i < numClients; i++) {
			FD_SET(clients[i], &master);
		}
		ret = select(0, &master, NULL, NULL, NULL);
		if (ret == SOCKET_ERROR)
			break;
		if (ret > 0) {
			if (FD_ISSET(listener, &master)) {
				SOCKET client = accept(listener, NULL, NULL);
				printf("New client accepted: %d\n", client);
				clients[numClients] = client;
				numClients++;
			}
		}
		for (int i = 0; i < numClients; i++) {
			bool connected = false;
			for (int j = 0; j < numClientsConnected; j++) {
				if (FD_ISSET(clients[i], &master) && clients[i] == clientsConnected[j]) {
					ret = recv(clients[i], recvBuffer, sizeof(recvBuffer), 0);
					connected = true;
					recvBuffer[ret] = 0;
					if (recvBuffer[ret - 1] == '\n')
						recvBuffer[ret - 1] = 0;
					if (ret <= 0) {
						RemoveClient(clients[i]);
						break;
					}

					printf("Received: %s\n", recvBuffer);

					storeDirToText(recvBuffer);
					string SendBufferString = cstringTocppString(recvBuffer);
					string test = SendBufferString.substr(4) + "out.txt";
					FILE *f = fopen(test.c_str(), "r");
					while (fgets(fileBuf, sizeof(fileBuf), f))
					{
						send(clients[i], fileBuf, strlen(fileBuf), 0);
					}
					fclose(f);


				}
			}
			if (FD_ISSET(clients[i], &master) && !connected) {
				ret = recv(clients[i], recvBuffer, sizeof(recvBuffer), 0);
				recvBuffer[ret] = 0;
				if (ret <= 0) {
					RemoveClient(clients[i]);
					break;
				}
				printf("Received: %s\n", recvBuffer);
				int found = 0;
				FILE *f = fopen("data.txt", "r");
				while (fgets(fileBuf, sizeof(fileBuf), f)) {
					if (strcmp(recvBuffer, fileBuf) == 0) {
						found = 1;
						break;
					}
				}
				fclose(f);
				if (found == 1)
				{
					string msg = "Dang nhap thanh cong. Hay nhap lenh.\n";
					send(clients[i], msg.c_str(), msg.length(), 0);

					clientsConnected[numClientsConnected] = clients[i];
					numClientsConnected++;
					continue;
				}
				else
				{
					string msg = "Dang nhap that bai. Hay thu lai.\n";
					send(clients[i], msg.c_str(), msg.length(), 0);
				}
			}

		}

	}
	closesocket(listener);

}


void RemoveClient(SOCKET client)
{
	// Tim chi so cua phan tu
	int i = 0;
	for (; i < numClients; i++)
		if (clients[i] == client)
			break;

	// Xoa phan tu
	if (i < numClients - 1)
		clients[i] = clients[numClients - 1];
	numClients--;
}

string cstringTocppString(char* buffer) {
	string StringBuffer(buffer);
	StringBuffer.pop_back();
	return StringBuffer;
}
bool directory_exists(char* buffer)
{
	string StringBuffer = cstringTocppString(buffer);
	string test = StringBuffer.substr(4);
	const std::string &directory = test;
	DWORD ftyp = GetFileAttributesA(directory.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

void storeDirToText(char* buffer) {
	if (directory_exists(buffer)) {
		cout << "Chinh xac" << endl;
		string StringBuffer = cstringTocppString(buffer);
		StringBuffer += " > " + StringBuffer.substr(4) + "out.txt";
		system(StringBuffer.c_str());
	}
	else cout << "Khong ton tai duong dan nay" << endl;
}

