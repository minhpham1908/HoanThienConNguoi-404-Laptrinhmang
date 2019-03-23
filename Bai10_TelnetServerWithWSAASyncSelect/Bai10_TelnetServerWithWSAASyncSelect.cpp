// Bai10_TelnetServerWithWSAASyncSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")

using namespace std;



#define WM_SOCKET WM_USER + 1

BOOL CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

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

	WNDCLASS wndclass;
	const CHAR *providerClass = "AsyncSelect";
	HWND window;

	wndclass.style = 0;
	wndclass.lpfnWndProc = (WNDPROC)WinProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = (LPCWSTR)providerClass;

	if (RegisterClass(&wndclass) == 0)
		return NULL;

	// Create a window
	if ((window = CreateWindow((LPCWSTR)providerClass, L"", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, NULL, NULL)) == NULL)
		return -1;

	WSAAsyncSelect(listener, window, WM_SOCKET, FD_ACCEPT);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

BOOL CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SOCKET)
	{
		if (WSAGETSELECTERROR(lParam))
		{
			closesocket(wParam);
			return FALSE;
		}

		if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT)
		{
			SOCKET client = accept(wParam, NULL, NULL);
			printf("New client accepted: %d\n", client);

			const char *msg = "Hello client\n";
			send(client, msg, strlen(msg), 0);
			clients[numClients] = client;
			numClients++;
			WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_READ)
		{
			char buf[256];
			char fileBuf[1024];
			int ret = recv(wParam, buf, sizeof(buf), 0);
			buf[ret] = 0;
			if (buf[ret - 1] = '\n')
				buf[ret - 1] = 0;
			printf("Received from %d: %s\n", wParam, buf);
			int j = 0;
			for (; j < numClientsConnected; j++) {
				if (wParam == clientsConnected[j])
					break;
			}
			if (j == numClientsConnected) {
				int found = 0;
				FILE *f = fopen("data.txt", "r");
				while (fgets(fileBuf, sizeof(fileBuf), f)) {
					if (fileBuf[strlen(fileBuf)-1] = '\n')
						fileBuf[strlen(fileBuf)-1] = 0;
					if (strcmp(buf, fileBuf) == 0) {
						found = 1;
						break;
					}
				}
				fclose(f);
				if (found == 1) {
					string msg = "Dang nhap thanh cong. Hay nhap lenh.\n";
					send(wParam, msg.c_str(), msg.length(), 0);

					clientsConnected[numClientsConnected] = wParam;
					numClientsConnected++;
				}
				else {
					string msg = "Dang nhap that bai. Hay thu lai.\n";
					send(wParam, msg.c_str(), msg.length(), 0);
				}
			}
			else {
				storeDirToText(buf);
				string SendBufferString = cstringTocppString(buf);
				string test = SendBufferString.substr(4) + "out.txt";
				FILE *f = fopen(test.c_str(), "r");
				while (fgets(fileBuf, sizeof(fileBuf), f))
				{
					send(wParam, fileBuf, strlen(fileBuf), 0);
				}
				fclose(f);
			}
		}
		else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
		{
			closesocket(wParam);
		}
	}

	return TRUE;
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

