#include <iostream>

#pragma comment( lib, "ws2_32.lib" )

#include <Windows.h>
#include <conio.h>
#include <vector>

using namespace std;

#define MY_PORT    500

// ������ ���������� ����������  ���� ��� ������ ������ ����� ������ �������   
char* cbuff = new char(14); // ����� ����� ��� ������ ��������� �� ���� ��������
vector<SOCKET> all_sockets;

// ������ ��� ������ ���������� ��������
// ������������� 
#define PRINTNUSERS if (nclients)\
  cout << nclients << " user on-line" << endl;\
  else cout << "No User on line" << endl;

  // �������� �������, �������������
  // �������������� �������������
DWORD WINAPI WorkWithClient(LPVOID client_socket);

//����� ������ ���������� ��������� ��������� ���������� ����������
DWORD WINAPI CheckCommonBuffer(LPVOID client_socket);

// ���������� ���������� � ����������
// �������� ������������� 
int nclients = 0;

int main(int argc, char* argv[]) {
    char buff[1024];    // ����� ��� ��������� ����

    cout << "TCP SERVER DEMO" << endl;

    // ��� 1 - ������������� ���������� �������
    // �.�. ������������ �������� ����������
    // �� ������������ �� ���������� ��������� ��
    // ������� �����, �������������
    // � ���������  �� ��������� WSADATA.
    // ����� ����� ��������� ���������� ����
    // ����������, ������, ����� ������ ���� �� �����
    // ������������ �������� (��������� WSADATA
    // �������� 400 ����)
    if (WSAStartup(0x0202, (WSADATA*)&buff[0])) {
        // ������!
        cout << "Error WSAStartup " <<
            WSAGetLastError() << endl;
        return -1;
    }

    // ��� 2 - �������� ������
    SOCKET mysocket;
    // AF_INET     - ����� ���������
    // SOCK_STREAM  - ��������� ����� (�
    //      ���������� ����������)
    // 0      - �� ��������� ���������� TCP ��������
    if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // ������!
        cout << "Error socket " <<
            WSAGetLastError() << endl;

        WSACleanup();
        // �������������� ���������� Winsock
        return -1;
    }

    // ��� 3 ���������� ������ � ��������� �������
    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(MY_PORT);
    // �� �������� � ������� �������!!!
    local_addr.sin_addr.s_addr = 0;
    // ������ ��������� �����������
    // �� ��� IP-������

// �������� bind ��� ����������
    if (bind(mysocket, (sockaddr*)&local_addr,
        sizeof(local_addr))) {
        // ������
        cout << "Error bind " << WSAGetLastError() << endl;
        closesocket(mysocket);  // ��������� �����!
        WSACleanup();
        return -1;
    }

    // ��� 4 �������� �����������
    // ������ ������� � 0x100
    if (listen(mysocket, 20)) {
        // ������
        cout << "Error listen " << WSAGetLastError() << endl;

        closesocket(mysocket);
        WSACleanup();

        return -1;
    }

    cout << "Waiting for connection" << endl;

    // ��� 5 ��������� ��������� �� �������
    SOCKET client_socket;    // ����� ��� �������
    sockaddr_in client_addr;    // ����� �������
    // (����������� ��������)

// ������� accept ���������� �������� ������
// ���������
    int client_addr_size = sizeof(client_addr);

    // ���� ���������� �������� �� ����������� ��
    // �������
    /* accept - ������ ���������� � �� ��� ����� ���������
    (�� ���� �� ��� ������- ���� ����������� ������)
    ���� �� �������� ��������� ������ �� ����������*/

    while ((client_socket = accept(mysocket, (sockaddr*)
        &client_addr, &client_addr_size))) {
        all_sockets.push_back(client_socket);

        nclients++;      // ����������� �������
        // �������������� ��������

// �������� �������� ��� �����
        HOSTENT* hst;
        hst = gethostbyaddr((char*)
            &client_addr.sin_addr.s_addr, 4, AF_INET);

        // ����� �������� � �������
        cout<<"+"<<((hst) ? hst->h_name : "")<<"[ " << inet_ntoa(client_addr.sin_addr) << " ] new connect!\n";
        PRINTNUSERS

            // ����� ������ ������ ��� ����������� �������
            // ��, ��� ����� ������������� ������������
            // _beginthreadex ��, ��������� ������� �����
            // ������� ����������� �� ���������� ����� ��
            // ������, ����� �������� � CreateThread
            DWORD thID;
        CreateThread(NULL, NULL, WorkWithClient,
            &client_socket, NULL, &thID);
    }
    return 0;
}

void SendAll(const char* arr, int bytes_recv, int flags, SOCKET my_sock) {
    for (int i = 0; i < all_sockets.size(); i++) {
        if (all_sockets[i] != my_sock) {
            send(all_sockets[i], arr, bytes_recv, 0);
        }
    }
}

// ��� ������� ��������� � ��������� ������ �
// ���������� ���������� ��������������� �������
// ���������� �� ���������
DWORD WINAPI WorkWithClient(LPVOID client_socket){
    SOCKET my_sock;
    my_sock = ((SOCKET*)client_socket)[0];
    char buff[1024] = "123124";

    const char* buff_hello = "Hello, new member! This's our win socket chat!))\r\n";
#define sHELLO "Hello, new member! This's our win socket chat!))\r\n"

// ���������� ������� ����������� 
    send(my_sock, sHELLO, sizeof(sHELLO), 0);



    // ���� ���-�������: ����� ������ �� ������� �
    int bytes_recv;
    // ����������� �� �������

    while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && (bytes_recv != SOCKET_ERROR)) {
        
        if (bytes_recv == SOCKET_ERROR) {
            cout << "Socket error" << endl;
            nclients--; // ��������� ������� �������� ��������
 
            auto it = std::remove(all_sockets.begin(), all_sockets.end(), my_sock);
            all_sockets.erase(it, all_sockets.end());
            cout << "-disconnect" << endl; PRINTNUSERS

                // ��������� �����
                closesocket(my_sock);
            return 0;
        }

        cbuff = buff; // ����� � ���������� ���������� ����� ������ ���������
        // ������ � ���������
       
        SendAll(&buff[0], bytes_recv, 0, my_sock);
        cout << buff << std::endl; 
    }

    nclients--;
    auto it = std::remove(all_sockets.begin(), all_sockets.end(), my_sock);
    all_sockets.erase(it, all_sockets.end());
    cout << "-disconnect" << endl; PRINTNUSERS

    // ��������� �����
    closesocket(my_sock);

    return 0;
}
