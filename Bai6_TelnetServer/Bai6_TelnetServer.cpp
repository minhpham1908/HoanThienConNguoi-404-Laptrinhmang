// Bai6_TelnetServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include "winsock2.h"
#include <string>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")


using namespace std;

DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET);
int CheckSignIn(char*);
bool directory_exists(char* buffer);
string cstringTocppString(char* buffer);
void storeDirToText(char* buffer);

SOCKET clients[64];
string client_id[64];
int numClients;
string line;

int main(int argc, char* argv[])
{
	int listenPort = 9000;
	// int listenPort = atoi(argv[1]);
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
	return 0;
}

DWORD WINAPI ClientThread(LPVOID lpParameter) {
	SOCKET ClientSocket = *(SOCKET *)lpParameter;
	char buffer[1024];
	int ret;
	int res = 0;

	while (true) {
		printf("Vui long nhap username va password\n");
		ret = recv(ClientSocket, buffer, sizeof(buffer), 0);
		if (ret <= 0) {
			RemoveClient(ClientSocket);
			break;
		}
		buffer[ret] = 0;
		res = CheckSignIn(buffer);
		if (res == 1) {
			printf("Connect accepted\n");
			break;
		}
	}
	while (true) {
		ret = recv(ClientSocket, buffer, sizeof(buffer), 0);
		if (ret <= 0) {
			RemoveClient(ClientSocket);
			break;
		}
		buffer[ret] = 0;
		printf("Receive: %s\n", buffer);
		storeDirToText(buffer);
		cout << "heoolo" << endl;
		string SendBufferString = cstringTocppString(buffer);
		string test = SendBufferString.substr(4) + "out.txt";
		string s1;
		int i = 0;
		string line;
		ifstream myfile(test);
		for (istreambuf_iterator<char, char_traits<char> > it(myfile.rdbuf());
			it != istreambuf_iterator<char, char_traits<char> >(); it++) {

			s1 += *it; //append at end of string
		}
		send(ClientSocket, s1.data(), strlen(s1.data()), 0);
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

string cstringTocppString(char* buffer) {
	string StringBuffer(buffer);
	StringBuffer.pop_back();
	return StringBuffer;
}
int CheckSignIn(char* buffer) {
	string StringBuffer = cstringTocppString(buffer);
	ifstream data;
	data.open("data.txt");

	cout << "Checking..." << endl;
	if (data.is_open())
	{
		while (getline(data, line))
		{
			if (StringBuffer.compare(line) == 0) {
				printf("Ket noi thanh cong\n");
				return 1;
			}
		}
		data.close();
	}
	return 0;
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
