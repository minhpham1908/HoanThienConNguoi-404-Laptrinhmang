// ChatServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ChatServer.h"
#include "winsock2.h"
#include <stdio.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define MAX_LOADSTRING 100
#define WM_SOCKET WM_USER + 1

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
SOCKET clients[64];
SOCKET clientsConnected[64];
char *ids[64];
int numClients;
int numClientsConnected;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	WSAData wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	strcpy((char*)szTitle, "Chat Server");
	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CHATSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATSERVER));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATSERVER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CHATSERVER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 600, 800, nullptr, nullptr, hInstance, nullptr);
	

	if (!hWnd)
	{
		return FALSE;
	}
	
	CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
		10, 10, 150, 420, hWnd, (HMENU)IDC_LIST_CLIENT, GetModuleHandle(NULL), NULL);
	CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
		180, 10, 400, 720, hWnd, (HMENU)IDC_LIST_MESSAGE, GetModuleHandle(NULL), NULL);
	int port = 9000;
	numClients = 0;
	numClientsConnected = 0;
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	WSAAsyncSelect(listener, hWnd, WM_SOCKET, FD_ACCEPT);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SOCKET:
	{
		if (WSAGETSELECTERROR(lParam))
		{
			closesocket(wParam);
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT)
		{
			SOCKADDR_IN clientAddr;
			int clientAddrLen = sizeof(clientAddr);

			SOCKET client = accept(wParam, (SOCKADDR *)&clientAddr, &clientAddrLen);
			char *ip = inet_ntoa(clientAddr.sin_addr);
			char portBuf[8];
			_itoa(ntohs(clientAddr.sin_port), portBuf, 10);
			char clientBuf[32];
			strcpy(clientBuf, ip);
			strcat(clientBuf, ":");
			strcat(clientBuf, portBuf);
			SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING,
				0, (LPARAM)clientBuf);
			SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, WM_VSCROLL, SB_BOTTOM, 0);
			printf("New client accepted: %d\n", client);
			const char *okMsg = "Hello client!\n";
			
			send(client, okMsg, strlen(okMsg), 0);
			clients[numClients] = client;
			numClients++;
			WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
		}

		if (WSAGETSELECTEVENT(lParam) == FD_READ) {
			char buffer[512];
			char cmd[64];
			char id[64];
			char tmp[64];
			int ret = recv(wParam, buffer, sizeof(buffer), 0);
			buffer[ret] = 0;
			if (buffer[ret - 1] = '\n')
				buffer[ret - 1] = 0;
			printf("Received: %d\n", wParam);
			int j = 0;
			for (; j < numClientsConnected; j++) {
				if (wParam == clientsConnected[j])
					break;
			}
			if (j == numClientsConnected) {
				//Chua dang nhap
				//Kiem tra cau lenh

				const char *errorMsg = "Loi cu phap. Hay nhap lai\n";
				ret = sscanf(buffer, "%s %s %s", cmd, id, tmp);
				if (ret == 2) {
					if (strcmp(cmd, "client_id:") == 0) {
						const char *okMsg = "Dung cu phap. Hay nhap thong diep muon gui.\n";
						send(wParam, okMsg, strlen(okMsg), 0);
						string connectNotification = " has joint the room.";
						char idBuffer[100];
						strcpy(idBuffer, id);
						strcat(idBuffer, connectNotification.c_str());
						SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, LB_ADDSTRING,
							0, (LPARAM)idBuffer);
						SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, WM_VSCROLL, SB_BOTTOM, 0);
						//luu thong tin dang nhap
						clientsConnected[numClientsConnected] = wParam;
						ids[numClientsConnected] = (char *)malloc(strlen(id) + 1);
						memcpy(ids[numClientsConnected], id, strlen(id) + 1);
						numClientsConnected++;
					}
					else
						send(wParam, errorMsg, strlen(errorMsg), 0);

				}
				else
					send(wParam, errorMsg, strlen(errorMsg), 0);
			}
			else {
				//da dang nhap
				char sendBuffer[256];
				char idBuffer[256];
				for (int i = 0; i < numClientsConnected; i++)
				{
					if (wParam == clientsConnected[i]) {
						sprintf(idBuffer, "%s : %s", ids[i], buffer);
						SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, LB_ADDSTRING,
							0, (LPARAM)idBuffer);
						SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, WM_VSCROLL, SB_BOTTOM, 0);
					}
				}
				for (int k = 0; k < numClients; k++) {
					if (wParam != clients[k])
					{
						sprintf(sendBuffer, "%s : %s\n", ids[k], buffer);
						send(clients[k], sendBuffer, strlen(sendBuffer), 0);
					}
					else
					{
						
					}
				}
			}

		}

		if (WSAGETSELECTEVENT(lParam) == FD_CLOSE) {
			closesocket(wParam);
		}

	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
