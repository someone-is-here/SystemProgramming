#include <iostream>
#pragma comment( lib, "ws2_32.lib" )
#include <Windows.h>
#include <conio.h>
#include <string>


#define PORT 500
#define SERVERADDR "127.0.0.1"

char buff[1024];

using namespace std;

DWORD WINAPI MyThreadFunction(LPVOID lpParam) {
    SOCKET my_sock = (SOCKET)lpParam;
    int nsize;
    while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR) {
        // ставим завершающий ноль в конце строки 
        buff[nsize] = 0;

        // выводим на экран 
        cout << "Recived: " << buff << endl;
    }
}
DWORD WINAPI MyThreadFunctionForInput(LPVOID lpParam) {
    SOCKET my_sock = (SOCKET)lpParam;

    while (true) {

       string str;
       getline(cin, str);

       if (!strcmp(str.c_str(), "quit")) {
           // Корректный выход
           cout << "Exit..." << endl;

           closesocket(my_sock);
           WSACleanup();

           return 0;
       }
       
       send(my_sock, str.c_str(), str.size(), 0);
       cout << "Your message successfully sent" << endl;
    }
}

int main() {
    cout << "TCP DEMO CLIENT" << endl;

    // Winsock initialization
    if (WSAStartup(0x202, (WSADATA*)&buff[0])) {
        cout << "WSAStart error " << WSAGetLastError() << endl;
        return -1;
    }

    // socket creation
    SOCKET my_sock;
    my_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (my_sock < 0) {
        cout << "Socket() error " << WSAGetLastError() << endl;
        return -1;
    }

    // Шаг 3 - установка соединения

    // заполнение структуры sockaddr_in
    // указание адреса и порта сервера
    sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    HOSTENT* hst;

    // преобразование IP адреса из символьного в
    // сетевой формат
    if (inet_addr(SERVERADDR) != INADDR_NONE) {
        dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
    } else {
        // попытка получить IP адрес по доменному
        // имени сервера
        if (hst = gethostbyname(SERVERADDR)) {
            // hst->h_addr_list содержит не массив адресов,
            // а массив указателей на адреса
            ((unsigned long*)&dest_addr.sin_addr)[0] =
                ((unsigned long**)hst->h_addr_list)[0][0];
        } else {
            cout << "Invalid address " << SERVERADDR << endl;

            closesocket(my_sock);
            WSACleanup();

            return -1;
        }
    }

    // адрес сервера получен – пытаемся установить
    // соединение 
    if (connect(my_sock, (sockaddr*)&dest_addr, sizeof(dest_addr))) {
        cout << "Connect error " << WSAGetLastError() << endl;
        return -1;
    }

    cout << "Connected with " << SERVERADDR << " successfully!" << endl
        << "Type quit for quit" << endl << endl;

    cout << "If you want to send message just type!" << endl;

    DWORD thID;
    //CreateThread(NULL, NULL, GetNewMessage, &my_sock, NULL, &thID);

    //// Шаг 4 - чтение и передача сообщений
    HANDLE  hThreadArray[2];
    DWORD   dwThreadIdArray[2];
    hThreadArray[0] = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        MyThreadFunction,       // thread function name
        (LPVOID)my_sock,          // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadIdArray[0]);   // returns the thread identifier 

    hThreadArray[1] = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        MyThreadFunctionForInput,       // thread function name
        (LPVOID)my_sock,          // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadIdArray[1]);   // returns the thread identifier 

    WaitForMultipleObjects(1, hThreadArray, TRUE, INFINITE);

    cout << "Recv error " << WSAGetLastError() << endl;

    closesocket(my_sock);
    WSACleanup();

    return -1;
}
