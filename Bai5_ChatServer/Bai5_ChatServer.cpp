// Bai5_ChatServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//Viết chương trình chatroom server nhận tham số cổng từ
//dòng lệnh, nghe và phục vụ các client làm việc sau :
//-Nhận kết nối từ client, và vào vòng lặp hỏi tên client cho
//đến khi client gửi đúng cú pháp :
//“client_id: xxxxxxxx”
//	trong đó xxxxxxx là tên
//	- Sau đó vào vòng lặp nhận dữ liệu từ một client và gửi dữ
//	liệu đó đến các client còn lại ví dụ : client có id “abc” gửi
//	“xin chào” thì các client khác sẽ nhận được : “abc: xin chao”
//	hoặc có thể thêm thời gian vào trước ví dụ : “2014 / 05 / 06
//	11 : 00 : 00PM abc : xin chao

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include "winsock2.h"
#include <string>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET);
int CheckID(char*, SOCKET);

SOCKET clients[64];
string client_id[64];
int numClients;
char he[3] = ": ";

int main(int argc, char* argv[])
{

	int listenPort = atoi(argv[1]);
	WSAData wsa;
	SOCKET ListenSocket, ClientSocket;
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(listenPort);

	cout << "Hello World!\n";

	WSAStartup(MAKEWORD(2, 2), &wsa);
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(ListenSocket, (sockaddr *)&addr, sizeof(addr));
	listen(ListenSocket, 5);
	printf("Khoi dong server thanh cong\n");
	numClients = 0;
	while (true) {
		ClientSocket = accept(ListenSocket, NULL, NULL);
		printf("New client connected: %d\n", ClientSocket);
		clients[numClients] = ClientSocket;
		numClients++;

		CreateThread(0, 0, ClientThread, &ClientSocket, 0, 0);
	}

}

DWORD WINAPI ClientThread(LPVOID lpParameter) {
	SOCKET ClientSocket = *(SOCKET *)lpParameter;
	char buffer[1024];
	char bufferSend[1024];
	int ret;
	int res = 0;
	bool connected = false;

	while (connected == false) {
		printf("Vui long nhap client_id\n");
		ret = recv(ClientSocket, buffer, sizeof(buffer), 0);
		if (ret <= 0) {
			RemoveClient(ClientSocket);
			break;
		}
		buffer[ret] = 0;
		res = CheckID(buffer, ClientSocket);
		if (res == 1) {
			printf("Connect accepted\n");
			connected = true;
			break;
		}
	}

	while (connected == true) {
		ret = recv(ClientSocket, buffer, sizeof(buffer), 0);
		if (ret <= 0) {
			RemoveClient(ClientSocket);
			break;
		}
		buffer[ret] = 0;
		for (int i = 0; i < numClients; i++) {
			if (clients[i] == ClientSocket)
				strcpy_s(bufferSend, client_id[i].c_str());
		}
		strcat_s(bufferSend, he);
		strcat_s(bufferSend, buffer);
		if (connected) {
			printf("Receive: %s\n", buffer);
			for (int i = 0; i <= numClients; i++) {
				if (clients[i] != ClientSocket)
					send(clients[i], bufferSend, strlen(bufferSend), 0);
			}
		}
	}
	closesocket(ClientSocket);
	return 0;
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

int CheckID(char* buffer, SOCKET ClientSocket) {
	string RightSyntax = "client_id: ";
	string StringBuffer(buffer);
	StringBuffer.pop_back();
	cout << "Checking..." << endl;
	if (StringBuffer.compare(0, 11, RightSyntax) == 0) {
		cout << "Right" << endl;
		for (int i = 0; i < numClients; i++) {
			if (clients[i] == ClientSocket)
				client_id[i] = StringBuffer.substr(11);
		}
		return 1;
	}
	return 0;
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
