// Bai7_ChatServerWithSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")


using namespace std;
void RemoveClient(SOCKET);

SOCKET clients[10];
int numClients;
SOCKET clientsConnected[10];
int numClientsConnected;
int main()
{
	numClients = 0;
	numClientsConnected = 0;
	std::cout << "Hello World!\n";
	WSAData wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int listenPort = 9000;
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(listenPort);

	bind(listener, (sockaddr *)&addr, sizeof(addr));
	listen(listener, 5);
	const char *errorMsg = "Loi cu phap. Hay nhap lai\n";

	char cmd[64];
	char id[64];
	char tmp[64];
	char *ids[64];
	char sendBuffer[256];
	fd_set master;
	while (true)
	{
		FD_ZERO(&master);
		FD_SET(listener, &master);
		for (int i = 0; i < numClients; i++)
			FD_SET(clients[i], &master);

		int ret = select(0, &master, NULL, NULL, NULL);
		if (ret == SOCKET_ERROR)
			break;
		if (ret > 0) {

			if (FD_ISSET(listener, &master)) {
				SOCKET client = accept(listener, NULL, NULL);
				printf("New client accepted: %d\n", client);
				clients[numClients] = client;
				numClients++;
			}

			for (int i = 0; i < numClients; i++)
			{
				bool connected = false;
				for (int j = 0; j < numClientsConnected; j++)
				{
					if (clients[i] == clientsConnected[j] && FD_ISSET(clients[i], &master)) {

						cout << "da ket noi\n" << endl;
						char recvBuffer[256];
						connected = true;
						ret = recv(clients[i], recvBuffer, sizeof(recvBuffer), 0);
						recvBuffer[ret] = 0;
						printf("Received: %s\n", recvBuffer);
						if (ret <= 0)
						{
							RemoveClient(clients[i]);
							continue;
						}
						for (int k = 0; k < numClients; k++) {
							if (clients[k] != clients[i])
							{
								sprintf(sendBuffer, "%s : %s", ids[k], recvBuffer);
								send(clients[k], sendBuffer, strlen(sendBuffer), 0);
							}
						}
					}
				}

				if (FD_ISSET(clients[i], &master) && !connected) {
					cout << "chua ket noi\n" << endl;
					char recvBuffer[256];
					ret = recv(clients[i], recvBuffer, sizeof(recvBuffer), 0);
					recvBuffer[ret] = 0;
					printf("Received: %s\n", recvBuffer);
					if (ret <= 0)
					{
						RemoveClient(clients[i]);
						continue;
					}
					ret = sscanf(recvBuffer, "%s %s %s", cmd, id, tmp);
					if (ret = 2)
					{
						if (strcmp(cmd, "client_id:") == 0)
						{
							string okMsg = "Dung cu phap. Hay nhap thong diep muon gui.\n";
							send(clients[i], okMsg.c_str(), okMsg.length(), 0);

							clientsConnected[numClientsConnected] = clients[i];
							ids[numClientsConnected] = id;
							numClientsConnected++;
						}
						else
							send(clients[i], errorMsg, strlen(errorMsg), 0);
					}
					else
						send(clients[i], errorMsg, strlen(errorMsg), 0);
				}
			}
		}
	}
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
