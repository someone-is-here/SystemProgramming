#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <conio.h>

using namespace std;

int main() {
	STARTUPINFO sti = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	DWORD excode;

	wstring CommandLine(L"Child.exe");
	LPWSTR lpwCMDLine = &CommandLine[0];

	cout << "Parent process start" << endl;

	cout << pi.dwProcessId << " " << pi.hProcess << endl;

	if (!CreateProcess(NULL,
		lpwCMDLine,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&sti,
		&pi)) {
		cout << "Unable to generate process" << endl;
		return -1;
	}

	Sleep(10000);

	GetExitCodeProcess(pi.hProcess, &excode);


	if (excode != STILL_ACTIVE) {
		cout << "Done:)" << endl;
	} else {
		cout << "Process still running" << endl;
	}
	cout << "Parent process end!" << endl;

	_getch();
}