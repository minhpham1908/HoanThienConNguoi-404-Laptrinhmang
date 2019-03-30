// Bai12_TelnetServerWithWSAEventSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
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

int main() {
	std::cout << "Hello World!\n";
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	SOCKET sockets[64];
	WSAEVENT events[64];
	int numEvents = 0;
	WSAEVENT newEvent = WSACreateEvent();
	WSAEventSelect(listener, newEvent, FD_ACCEPT);
	sockets[numEvents] = listener;
	events[numEvents] = newEvent;
	numEvents++;

	int ret;
	char buf[512];

	while (true) {
		ret = WSAWaitForMultipleEvents(numEvents, events, FALSE, 0, FALSE);
		if (ret == WSA_WAIT_FAILED)
			break;

		int index = ret - WSA_WAIT_EVENT_0;
		for (int i = index; i < numEvents; i++) {
			ret = WSAWaitForMultipleEvents(1, &events[i], TRUE, 0, FALSE);
			if (ret == WSA_WAIT_FAILED)
				continue;
			if (ret == WSA_WAIT_TIMEOUT)
				continue;

			WSANETWORKEVENTS networkEvents;
			WSAEnumNetworkEvents(sockets[i], events[i], &networkEvents);
			WSAResetEvent(events[i]);

			if (networkEvents.lNetworkEvents & FD_ACCEPT) {
				if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0) {
					printf("FD_ACCEPT failed\n");
					continue;
				}
				if (numEvents == WSA_MAXIMUM_WAIT_EVENTS) {
					printf("Too many connection\n");
					continue;
				}

				SOCKET client = accept(sockets[i], NULL, NULL);

				newEvent = WSACreateEvent();
				WSAEventSelect(client, newEvent, FD_READ | FD_CLOSE);

				const char *msg = "Hello client\n";
				send(client, msg, strlen(msg), 0);
				clients[numClients] = client;
				numClients++;

				events[numEvents] = newEvent;
				sockets[numEvents] = client;
				numEvents++;
				printf("New client connected %d\n", (int)client);
			}

			if (networkEvents.lNetworkEvents & FD_READ) {
				if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
				{
					printf("FD_READ failed\n");
					continue;
				}

				ret = recv(sockets[i], buf, sizeof(buf), 0);
				if (ret <= 0) {
					printf("FD_READ failed\n");
					continue;
				}

				buf[ret] = 0;
				if (buf[ret - 1] = '\n') {
					buf[ret - 1] = 0;
				}
				printf("Received: %s\n", buf);

				char fileBuf[1024];
				int j = 0;
				for (; j < numClientsConnected; j++) {
					if (sockets[i] == clientsConnected[j])
						break;
				}
				if (j == numClientsConnected) {
					int found = 0;
					FILE *f = fopen("data.txt", "r");
					while (fgets(fileBuf, sizeof(fileBuf), f)) {
						if (fileBuf[strlen(fileBuf) - 1] = '\n')
							fileBuf[strlen(fileBuf) - 1] = 0;
						if (strcmp(buf, fileBuf) == 0) {
							found = 1;
							break;
						}
					}
					fclose(f);
					if (found == 1) {
						string msg = "Dang nhap thanh cong. Hay nhap lenh.\n";
						send(sockets[i], msg.c_str(), msg.length(), 0);

						clientsConnected[numClientsConnected] = sockets[i];
						numClientsConnected++;
					}
					else {
						string msg = "Dang nhap that bai. Hay thu lai.\n";
						send(sockets[i], msg.c_str(), msg.length(), 0);
					}
				}
				else {
					storeDirToText(buf);
					string SendBufferString = cstringTocppString(buf);
					string test = SendBufferString.substr(4) + "out.txt";
					FILE *f = fopen(test.c_str(), "r");
					while (fgets(fileBuf, sizeof(fileBuf), f))
					{
						send(sockets[i], fileBuf, strlen(fileBuf), 0);
					}
					fclose(f);
				}
			}

			if (networkEvents.lNetworkEvents & FD_CLOSE) {
				if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0) {
					printf("FD_CLOSE failed\n");
					continue;
				}

				closesocket(sockets[i]);

				printf("Client disconnected\n");
			}
		}
	}
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

