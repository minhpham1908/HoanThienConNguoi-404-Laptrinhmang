// Bai2_TCPServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//



#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include "winsock2.h"
#include <stdio.h>
using namespace std;

int main(int argc, char** argv) {
	//convert port sang int
	int port = atoi(argv[1]);
	string hello = argv[2];
	string data = argv[3];
	
	ofstream clientrecv (data);

	//Doc du lieu tu file
	string s1;
	int i = 0;
	string line;
	ifstream myfile (hello);
	for (istreambuf_iterator<char, char_traits<char> > it(myfile.rdbuf());
		it != istreambuf_iterator<char, char_traits<char> >(); it++) {

		s1 += *it; //append at end of string
	}
	cout << s1.data() << endl;

	/*chao.seekg(0, ios::end);
	data.reserve(chao.tellg());
	chao.seekg(0, ios::beg);
	data.assign((istreambuf_iterator<char>(chao)), istreambuf_iterator<char>());
	chao.close();*/

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);
	printf("khoi dong thanh cong server\n");
	SOCKET client = accept(listener, NULL, NULL);
	printf("Co ket noi den\n");
	send(client, s1.data(), strlen(s1.data()), 0);
	char buf[256];
	int ret;

	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;

		buf[ret] = 0;
		printf("Received: %s\n", buf);
		if (clientrecv.is_open()) {
			clientrecv << buf << endl;
		}
	}
	clientrecv.close();
	closesocket(client);
	closesocket(listener);

	WSACleanup();

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
