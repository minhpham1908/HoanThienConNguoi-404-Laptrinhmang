// Bai4_serverinfo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "winsock2.h"


using namespace std;

struct drivesInfo
{
	char computerName[128];
	char name[16];
	__int64 totalSize[16];
	__int64 freeSize[16];
};

int main(int argc, char **argv)
{
    std::cout << "Hello World!\n"; 
	int port = atoi(argv[1]);
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
	char buf[256];
	int ret;

	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;

		buf[ret] = 0;
		printf("Received: %s\n", buf);
	}
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
