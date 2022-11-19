#include <iostream>

#pragma comment( lib, "ws2_32.lib" )

#include <Windows.h>
#include <conio.h>
#include <vector>

using namespace std;

#define MY_PORT    500

// заведём глобальную переменную  ниже для обмена данным между нитями сервера   
char* cbuff = new char(14); // общий буфер для записи сообщений от всех клиентов
vector<SOCKET> all_sockets;

// макрос для печати количества активных
// пользователей 
#define PRINTNUSERS if (nclients)\
  cout << nclients << " user on-line" << endl;\
  else cout << "No User on line" << endl;

  // прототип функции, обслуживающий
  // подключившихся пользователей
DWORD WINAPI WorkWithClient(LPVOID client_socket);

//будет просто циклически проверять знкачение глобальной переменной
DWORD WINAPI CheckCommonBuffer(LPVOID client_socket);

// глобальная переменная – количество
// активных пользователей 
int nclients = 0;

int main(int argc, char* argv[]) {
    char buff[1024];    // Буфер для различных нужд

    cout << "TCP SERVER DEMO" << endl;

    // Шаг 1 - Инициализация Библиотеки Сокетов
    // Т.к. возвращенная функцией информация
    // не используется ей передается указатель на
    // рабочий буфер, преобразуемый
    // к указателю  на структуру WSADATA.
    // Такой прием позволяет сэкономить одну
    // переменную, однако, буфер должен быть не менее
    // полкилобайта размером (структура WSADATA
    // занимает 400 байт)
    if (WSAStartup(0x0202, (WSADATA*)&buff[0])) {
        // Ошибка!
        cout << "Error WSAStartup " <<
            WSAGetLastError() << endl;
        return -1;
    }

    // Шаг 2 - создание сокета
    SOCKET mysocket;
    // AF_INET     - сокет Интернета
    // SOCK_STREAM  - потоковый сокет (с
    //      установкой соединения)
    // 0      - по умолчанию выбирается TCP протокол
    if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // Ошибка!
        cout << "Error socket " <<
            WSAGetLastError() << endl;

        WSACleanup();
        // Деиницилизация библиотеки Winsock
        return -1;
    }

    // Шаг 3 связывание сокета с локальным адресом
    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(MY_PORT);
    // не забываем о сетевом порядке!!!
    local_addr.sin_addr.s_addr = 0;
    // сервер принимает подключения
    // на все IP-адреса

// вызываем bind для связывания
    if (bind(mysocket, (sockaddr*)&local_addr,
        sizeof(local_addr))) {
        // Ошибка
        cout << "Error bind " << WSAGetLastError() << endl;
        closesocket(mysocket);  // закрываем сокет!
        WSACleanup();
        return -1;
    }

    // Шаг 4 ожидание подключений
    // размер очереди – 0x100
    if (listen(mysocket, 20)) {
        // Ошибка
        cout << "Error listen " << WSAGetLastError() << endl;

        closesocket(mysocket);
        WSACleanup();

        return -1;
    }

    cout << "Waiting for connection" << endl;

    // Шаг 5 извлекаем сообщение из очереди
    SOCKET client_socket;    // сокет для клиента
    sockaddr_in client_addr;    // адрес клиента
    // (заполняется системой)

// функции accept необходимо передать размер
// структуры
    int client_addr_size = sizeof(client_addr);

    // цикл извлечения запросов на подключение из
    // очереди
    /* accept - держит управление и не даёт циклу вращаться
    (то есть не даёт потоку- нити выполняться вообще)
    пока не поступит очередной запрос на соединение*/

    while ((client_socket = accept(mysocket, (sockaddr*)
        &client_addr, &client_addr_size))) {
        all_sockets.push_back(client_socket);

        nclients++;      // увеличиваем счетчик
        // подключившихся клиентов

// пытаемся получить имя хоста
        HOSTENT* hst;
        hst = gethostbyaddr((char*)
            &client_addr.sin_addr.s_addr, 4, AF_INET);

        // вывод сведений о клиенте
        cout<<"+"<<((hst) ? hst->h_name : "")<<"[ " << inet_ntoa(client_addr.sin_addr) << " ] new connect!\n";
        PRINTNUSERS

            // Вызов нового потока для обслужвания клиента
            // Да, для этого рекомендуется использовать
            // _beginthreadex но, поскольку никаких вызов
            // функций стандартной Си библиотеки поток не
            // делает, можно обойтись и CreateThread
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

// Эта функция создается в отдельном потоке и
// обсуживает очередного подключившегося клиента
// независимо от остальных
DWORD WINAPI WorkWithClient(LPVOID client_socket){
    SOCKET my_sock;
    my_sock = ((SOCKET*)client_socket)[0];
    char buff[1024] = "123124";

    const char* buff_hello = "Hello, new member! This's our win socket chat!))\r\n";
#define sHELLO "Hello, new member! This's our win socket chat!))\r\n"

// отправляем клиенту приветствие 
    send(my_sock, sHELLO, sizeof(sHELLO), 0);



    // цикл эхо-сервера: прием строки от клиента и
    int bytes_recv;
    // возвращение ее клиенту

    while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && (bytes_recv != SOCKET_ERROR)) {
        
        if (bytes_recv == SOCKET_ERROR) {
            cout << "Socket error" << endl;
            nclients--; // уменьшаем счетчик активных клиентов
 
            auto it = std::remove(all_sockets.begin(), all_sockets.end(), my_sock);
            all_sockets.erase(it, all_sockets.end());
            cout << "-disconnect" << endl; PRINTNUSERS

                // закрываем сокет
                closesocket(my_sock);
            return 0;
        }

        cbuff = buff; // пишем в глобальную переменную чтобы другие участники
        // узнали о сообщении
       
        SendAll(&buff[0], bytes_recv, 0, my_sock);
        cout << buff << std::endl; 
    }

    nclients--;
    auto it = std::remove(all_sockets.begin(), all_sockets.end(), my_sock);
    all_sockets.erase(it, all_sockets.end());
    cout << "-disconnect" << endl; PRINTNUSERS

    // закрываем сокет
    closesocket(my_sock);

    return 0;
}
