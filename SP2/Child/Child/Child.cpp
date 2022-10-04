#include <iostream>
#include <Windows.h>
#include <stdio.h>

using namespace std;

int main() {

	cout << "Child process start" << endl;
	cout << "Child process running..." << endl;
	//cout << "Child process sleeping..." << endl;
	Sleep(10000);
	//cout << "Child process awake!" << endl;
	cout << "Child process end!" << endl;

	exit(0);
}