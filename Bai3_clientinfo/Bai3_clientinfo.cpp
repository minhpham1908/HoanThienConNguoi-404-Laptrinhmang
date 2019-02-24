// Bai3_clientinfo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include"winsock2.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#define INFO_BUFFER_SIZE 32767


using namespace std;

struct drivesInfo
{
	char computerName[128];
	char name[16];
	int totalSize[16];
	int freeSize[16];
};


int main(int argc, char** argv) 
{	
	struct drivesInfo info;
	int count=0;
	DWORD i;
	TCHAR  infoBuf[INFO_BUFFER_SIZE];
	DWORD  bufCharCount = INFO_BUFFER_SIZE;
	bufCharCount = INFO_BUFFER_SIZE;
	//Lay ten may tinh
	if (!GetComputerName(infoBuf, &bufCharCount))
		printf("he");
	_tprintf(TEXT("\nComputer name:      %s"), infoBuf);
	//Gan vao info
	wcstombs(info.computerName, infoBuf, wcslen(infoBuf)+1);
	
	// Lay ten tat ca cac o dia kem dung luong moi o
	const int BUFFER_SIZE = 255;
	WCHAR buffer[BUFFER_SIZE];
	DWORD test;
	//lay ten
	test = GetLogicalDriveStrings(BUFFER_SIZE, buffer);
	
	/*if (test != 0)
	{
		printf("GetLogicalDriveStrings() return value: %d, Error (if any): %d \n", test, GetLastError());
		printf("The logical drives of this machine are:\n");
		for (int i = 0; i < test; i++) {
			printf("%c", buffer[i]);

		}
		printf("\n");

	}
	else printf("GetLogicalDriveStrings() is failed lor!!! Error code: %d\n", GetLastError());*/
	
	char teno[4];
	wchar_t wtext[5];
	__int64 freeBytesToCaller, totalBytes, freeBytes;
	LPWSTR ptr;
	for (int i = 0; i < test; i=i+4) {
		teno[0] = buffer[i];
		teno[1] = ':';
		teno[2] = '\\';
		teno[3] = '\0';
		mbstowcs(wtext, teno, strlen(teno)+1);//Plus null
		ptr = wtext;
		wcout << ptr << endl;
		// lay dung luong moi o
		
		BOOL Result = GetDiskFreeSpaceEx(ptr, (PULARGE_INTEGER)&freeBytesToCaller, (PULARGE_INTEGER)&totalBytes, (PULARGE_INTEGER)&freeBytes);
		if (Result) {
			info.name[count] = buffer[i];
			info.totalSize[count] = (int) (totalBytes / (1024 * 1024 * 1024));
			info.freeSize[count] = (int) (freeBytes / (1024 * 1024 * 1024));
			count++;
		}
		/*{

			wprintf(L"\nGetDiskFreeSpaceExW reports:\n\n");

			wprintf(L"Available space to caller = %I64u GB\n", freeBytesToCaller / (1024 * 1024 * 1024));

			wprintf(L"Total space               = %I64u GB\n", totalBytes / (1024 * 1024 * 1024));

			wprintf(L"Free space on drive       = %I64u GB\n", freeBytes / (1024 * 1024 * 1024));

		}*/
	}
	info.name[count] = 0;

	string serverIP = argv[1];
	int port = atoi(argv[2]);
	printf("Connecting to %s:%u\n", serverIP.c_str(), port);
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
	serverAddr.sin_port = htons(port);
	int he = connect(client, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	printf("Ket noi thanh cong\n");
	send(client, (char *)&info, sizeof(info), 0);
	printf("Da gui thong diep\n");
	closesocket(client);
	WSACleanup();

	system("pause");
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
