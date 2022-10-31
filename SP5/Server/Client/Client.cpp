#include "Client.h"

#define BUFSIZE 256

int main(int argc, TCHAR* argv[]) {
    HANDLE hPipe = NULL;
    DWORD dwMode; //pipe's mode
    BOOL isSuccess = FALSE;
    DWORD iCountIO = 0;

    LPWSTR str = new WCHAR[BUFSIZE];
    memset(str, 0, (BUFSIZE) * 2);

    std::wstring pipeName;
    std::wcout << "Write pipe name: " << std::endl;
    std::getline(std::wcin, pipeName);

    std::wstring pipe = L"\\\\.\\pipe\\";
    pipe.append(pipeName);

    std::wcout << "Pipe's path: " << pipe << std::endl;

    // Try to open a named pipe; wait for it, if necessary. 

    while (true) {
        std::wstring message;
        std::cout << "Write message: " << std::endl;
        std::getline(std::wcin, message);

        hPipe = CreateFile(
            pipe.c_str(),   // pipe name 
            GENERIC_READ |  // read and write access 
            GENERIC_WRITE,
            0,              // no sharing 
            NULL,           // default security attributes
            OPEN_EXISTING,  // opens existing pipe 
            0,              // default attributes 
            NULL);          // no template file 

        // Break if the pipe handle is valid. 

        if (hPipe == INVALID_HANDLE_VALUE)
            return EXIT_FAILURE;

        // Exit if an error other than ERROR_PIPE_BUSY occurs. 

        if (GetLastError() == ERROR_PIPE_BUSY) {
            std::cout << "Could not open pipe. GLE= " << GetLastError() << std::endl;
            return -1;
        }

        // All pipe instances are busy, so wait for 20 seconds. 

        if (!WaitNamedPipe(pipe.c_str(), 20000)) {
            std::cout << "Could not open pipe: 20 second wait timed out." << std::endl;
            return -1;
        }

        dwMode = PIPE_READMODE_MESSAGE;

        isSuccess = SetNamedPipeHandleState(
            hPipe,    // pipe handle 
            &dwMode,  // new pipe mode 
            NULL,     // don't set maximum bytes 
            NULL);    // don't set maximum time 

        if (!isSuccess) {
            std::cout << "Can't change pipe's mode. GLE= " << GetLastError() << std::endl;
            return EXIT_FAILURE;
        }

        isSuccess = WriteFile(
            hPipe,                  // pipe handle 
            message.c_str(),             // message 
            (message.size() + 1) * sizeof(wchar_t),              // message length 
            &iCountIO,             // bytes written 
            NULL);                  // not overlapped 

        if (!isSuccess || iCountIO != (message.size() + 1) * sizeof(wchar_t)) {
            std::cout << "WriteFile to pipe failed. GLE= " << GetLastError() << std::endl;
            return EXIT_FAILURE;
        }
        // Read from the pipe. 
        do {
            isSuccess = ReadFile(
                hPipe,    // pipe handle 
                str,    // buffer to receive reply 
                BUFSIZE * sizeof(wchar_t),  // size of buffer 
                &iCountIO,  // number of bytes read 
                NULL);    // not overlapped 


            if (isSuccess && iCountIO > 0) {
                std::wcout << L"Message from server: " << str << std::endl;
            }
        } while (!isSuccess);
    }

    // The pipe connected; change to message-read mode. 

    CloseHandle(hPipe);
    delete[] str;
    system("pause");

    return EXIT_SUCCESS;
}
