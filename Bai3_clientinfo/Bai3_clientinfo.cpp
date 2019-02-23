// Bai3_clientinfo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#define INFO_BUFFER_SIZE 32767
#pragma comment(lib, "user32.lib")
void printError(TCHAR* msg);

int main()
{
	/*
	DWORD i;
	TCHAR  infoBuf[INFO_BUFFER_SIZE];
	DWORD  bufCharCount = INFO_BUFFER_SIZE;

    std::cout << "Hello World!\n"; 
	bufCharCount = INFO_BUFFER_SIZE;
	if (GetComputerName(infoBuf, &bufCharCount))
	_tprintf(TEXT("\nComputer name:      %s"), infoBuf);
	if (GetSystemDirectory(infoBuf, INFO_BUFFER_SIZE))
	_tprintf(TEXT("\nSystem Directory:   %s"), infoBuf);
	if (GetUserName(infoBuf, &bufCharCount))
		_tprintf(TEXT("\nUser name:      %s\n"), infoBuf);
	
	
	SYSTEM_INFO siSysInfo;

	// Copy the hardware information to the SYSTEM_INFO structure. 

	GetSystemInfo(&siSysInfo);

	// Display the contents of the SYSTEM_INFO structure. 

	printf("Hardware information: \n");
	printf("  OEM ID: %u\n", siSysInfo.dwOemId);
	printf("  Number of processors: %u\n",
		siSysInfo.dwNumberOfProcessors);
	printf("  Page size: %u\n", siSysInfo.dwPageSize);
	printf("  Processor type: %u\n", siSysInfo.dwProcessorType);
	printf("  Minimum application address: %lx\n",
		siSysInfo.lpMinimumApplicationAddress);
	printf("  Maximum application address: %lx\n",
		siSysInfo.lpMaximumApplicationAddress);
	printf("  Active processor mask: %u\n",
		siSysInfo.dwActiveProcessorMask);

	const int BUFFER_SIZE = 255;
	WCHAR buffer[BUFFER_SIZE];
	GetLogicalDriveStrings(BUFFER_SIZE, buffer);
	printf("%s", buffer);
	*/

	const int BUFFER_SIZE = 255;
	WCHAR buffer[BUFFER_SIZE];
	GetLogicalDriveStrings(BUFFER_SIZE, buffer);

	printf("%s", buffer);

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
