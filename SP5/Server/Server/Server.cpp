#include "Server.h"

#define BUFSIZE 2048
#define BASE_PATH_PIPE L"\\\\.\\pipe\\"

#define MESSAGE L"Message sent by server!"
#define MESSAGE_SIZE 24

#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21

#define PATH L"LogInfo.txt"

struct Param {
    HANDLE hpipe;
    DWORD threadID;
};

static char* getDtTm(char* buff) {
    time_t t = time(0);
    strftime(buff, DTTMSZ, DTTMFMT, localtime(&t));
    return buff;
}

void WriteToFile(LPWSTR strRequest, DWORD threadID);
void WriteTextToFile(std::wstring info);
DWORD WINAPI InstanceThread(_In_ LPVOID hPipe);

int main() {
    BOOL   isConnected = FALSE;
    HANDLE hPipe = NULL;
    DWORD  dwThreadId = 0;

    std::wstring namePipe(BASE_PATH_PIPE);
    std::wstring tmp;
    std::list<HANDLE> threads;

    //Get name of our pipe
    std::cout << "Write pipe name: " << std::endl;
    std::wcin >> tmp;

    namePipe.append(tmp);

    // The main loop creates an instance of the named pipe and 
    // then waits for a client to connect to it. When the client 
    // connects, a thread is created to handle communications 
    // with that client, and this loop is free to wait for the
    // next client connect request. It is an infinite loop.

    while (TRUE) {
        std::wcout << "\nPipe Server: Main thread awaiting client connection on " << namePipe << std::endl;
        //Create pipe
        hPipe = CreateNamedPipeW(
            namePipe.c_str(),         // pipe name 
            PIPE_ACCESS_DUPLEX,       // read/write access 
            PIPE_TYPE_MESSAGE |       // message type pipe 
            PIPE_READMODE_MESSAGE |   // message-read mode 
            PIPE_WAIT,                // blocking mode 
            PIPE_UNLIMITED_INSTANCES, // max. instances  
            BUFSIZE,                  // output buffer size 
            BUFSIZE,                  // input buffer size 
            INFINITE,                 // client time-out 
            NULL);                    // default security attribute 

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cout << "CreateNamedPipeW failed, GLE= " << GetLastError() << std::endl;
            return EXIT_FAILURE;
        }

        // Wait for the client to connect; if it succeeds, 
        // the function returns a nonzero value. If the function
        // returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

        isConnected = ConnectNamedPipe(hPipe, NULL);

        if (isConnected) {
            WriteTextToFile(L"Connected to pipe" + namePipe);
            std::cout << "Client connected, creating a processing thread." << std::endl;
            Param p1{ hPipe, dwThreadId };
            // Create a thread for this client. 
            HANDLE hThread = CreateThread(
                NULL,              // no security attribute 
                NULL,                 // default stack size 
                InstanceThread,    // thread proc
                &p1,    // thread parameter 
                NULL,                 // not suspended 
                &dwThreadId);      // returns thread ID 
            if (hThread == NULL) {
                std::cout << "CreateThread failed, GLE= " << GetLastError() << std::endl;
                return -1;
            } else {
                ++dwThreadId;
                threads.push_back(hThread);
            }
        } else {
            std::wcout << "Error of connection user" << std::endl;
            // The client could not connect, so close the pipe. 
            CloseHandle(hPipe);
        }
    }
    std::for_each(threads.cbegin(), threads.cend(), [](HANDLE h) {
        CloseHandle(h);
    });

    CloseHandle(hPipe);

    return EXIT_SUCCESS;
}

DWORD WINAPI InstanceThread(_In_ LPVOID hPipe) {
    Param* param = (Param*)hPipe;
    // This routine is a thread processing function to read from and reply to a client
    // via the open pipe connection passed from the main loop. Note this allows
    // the main loop to continue executing, potentially creating more threads of
    // of this procedure to run concurrently, depending on the number of incoming
    // client connections.

    //HANDLE hHeap = GetProcessHeap();
    //CHAR* pchRequest = (CHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(CHAR));
    //CHAR* pchReply = (CHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(CHAR));
    LPWSTR strRequest = new WCHAR[BUFSIZE + 1];
    memset(strRequest, 0, (BUFSIZE + 1) * sizeof(WCHAR));

    DWORD cbBytesRead = 0, cbWritten = 0;
    BOOL isSuccess = FALSE;

    // The thread's parameter is a handle to a pipe object instance. 

    HANDLE pipe = HANDLE(param->hpipe);

    // Print verbose messages. In production code, this should be for debugging only.
    std::cout << "Instance Thread created, receiving and processing messages." << std::endl;
    WriteTextToFile(L"Instance Thread created, receiving and processing messages." + std::to_wstring(param->threadID));
    // Loop until done reading
    while (true) {
        // Read client requests from the pipe. This simplistic code only allows messages
        // up to BUFSIZE characters in length.
        isSuccess = ReadFile(
            pipe,        // handle to pipe 
            strRequest,    // buffer to receive data 
            BUFSIZE * sizeof(WCHAR), // size of buffer 
            &cbBytesRead, // number of bytes read 
            NULL);        // not overlapped I/O 


        if (!isSuccess) {
            std::cout << "Instance Thread: client disconnected." << std::endl;
            WriteTextToFile(L"Instance Thread: client disconnected." + std::to_wstring(param->threadID));
            break;  
        } else {
            std::wcout << strRequest << std::endl;
        }

        // Process the incoming message.
        // Write the reply to the pipe. 
        isSuccess = WriteFile(
            pipe,        // handle to pipe 
            MESSAGE,     // buffer to write from 
            MESSAGE_SIZE * sizeof(wchar_t), // number of bytes to write 
            &cbWritten,   // number of bytes written 
            NULL);        // not overlapped I/O 
        
        if (!isSuccess || MESSAGE_SIZE * sizeof(wchar_t) != cbWritten) {
            std::cout << "InstanceThread WriteFile failed, GLE= " << GetLastError() << std::endl;
            break;
        } else {
            std::wcout << "Message has been sent to client!" << std::endl;
            WriteToFile(strRequest, param->threadID);
        }
    }

    if (strRequest)
        delete[] strRequest;

    CloseHandle(pipe);
    ExitThread(0);
}

void WriteToFile(LPWSTR strRequest, DWORD threadID) {
    char buff[DTTMSZ];
    std::wfstream filestr;
    filestr.open(PATH, std::wfstream::out | std::wfstream::app);

    // And this is how you call it:
    filestr << getDtTm(buff) << strRequest << " \t ThreadID: " << threadID << std::endl;

    filestr.close();
}

void WriteTextToFile(std::wstring info) {
    char buff[DTTMSZ];
    std::wfstream filestr;
    filestr.open(PATH, std::wfstream::out | std::wfstream::app);

    // And this is how you call it:
    filestr << getDtTm(buff) << info << std::endl;

    filestr.close();
}
