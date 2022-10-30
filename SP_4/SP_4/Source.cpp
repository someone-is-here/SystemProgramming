#include <Windows.h>
#include <string.h>
#include <ctime>
#include <iostream>
#include <vector>

#define SIZE_BUFFER 100000

#define PATH L"NotSorted.txt"
#define PATH2 L"SortedSync.txt"
#define PATH3 L"SortedAsync.txt"

using namespace std;

struct Param {
    char* arr;
    int size;
};

void ShowArray(char* arr, int size=SIZE_BUFFER) {
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

int CreateFileWithValues() {
    //Открываем файл
    HANDLE hFile = CreateFile(PATH,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    OVERLAPPED olf = { 0 }; //Структура, в которой задана позиция в файле
    //Аналог LARGE_INTEGER
    //hEvent - ставим NULL
    //Offset младший разряд того, куда поставить курсор. 32-бит
    //OffsetHigh - старший разряд того, куда поставить курсор. 32-бит
    //Internal - ставим NULL(0). Это - возвращаемое значение, куда записывается новая поз. курсора
    //32-бит, младший разряд
    //InternalHigh - тоже что и предыдущее, только старший разряд

    //Как объеденять и разъеденять:
    LARGE_INTEGER li = { 0 };
    //LowPart - младший 32-битный разряд
    //HighPart - старший 32-битный разряд
    //QuadPart - 64 битное число

    CHAR* str = (CHAR*)calloc(SIZE_BUFFER + 1, sizeof(CHAR));

    for (int i = 0; i < SIZE_BUFFER; i++) {
        str[i] = rand() % ('z' - 'a' + 1) + 'a';
    }

    DWORD iNumWrite = 0;
    if (!WriteFile(hFile, str, SIZE_BUFFER, &iNumWrite, &olf))
        return 3;
    if (olf.Internal == -1 && GetLastError())
        return 4;

    CloseHandle(hFile);

    return 0;
}

DWORD WINAPI SelectionSort(LPVOID lpParam) {
    HANDLE hStdout = NULL;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE) {
        return 1;
    }
    Param* p = (Param*) lpParam;

    char* arr = p->arr;
    int iter = 0;
    for (int i = 0; i < p->size; i++, ++iter) {
        int min = i;
        for (int j = i + 1; j < p->size; j++, ++iter) {
            if (arr[j] < arr[min]) {
                min = j;
            }
        }
        if (min != i) {
            int temp = arr[i];
            arr[i] = arr[min];
            arr[min] = temp;
        }
    }

  //  ShowArray(arr, p->size);

    return 0;
}

int SyncReadSortWrite() {
    HANDLE hFile = CreateFile(PATH,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    OVERLAPPED olf = { 0 };

    unsigned int startTime = clock();

    LPSTR buffer = (CHAR*)calloc(SIZE_BUFFER, sizeof(CHAR));
    DWORD iNumRead = 0;//Обязательный параметр. получает кол-во считанных байт

    if (!ReadFile(hFile, buffer, SIZE_BUFFER, &iNumRead, &olf))
        return 13;
    if (olf.Internal == -1 && GetLastError())
        return 2;

    olf.Offset += iNumRead;//добавляем кол-во прочитанных байт

    Param p = { buffer, SIZE_BUFFER };

    HANDLE th_selection_sort = CreateThread(NULL, 0, SelectionSort, &p, 1, 0);

    WaitForSingleObject(th_selection_sort, INFINITE);


    CloseHandle(th_selection_sort);
    CloseHandle(hFile);

    HANDLE hFile2 = CreateFile(PATH2,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    LARGE_INTEGER li = { 0 };
    //LowPart - младший 32-битный разряд
    //HighPart - старший 32-битный разряд
    //QuadPart - 64 битное число
    olf = { 0 };

    DWORD iNumWrite = 0;
    if (!WriteFile(hFile2, buffer, SIZE_BUFFER, &iNumWrite, &olf))
        return 3;
    if (olf.Internal == -1 && GetLastError())
        return 4;


    CloseHandle(hFile2);

    unsigned int endTime = clock();

    cout << "Sync time: " << endTime - startTime << endl;
    
    return 0;
}

char* EndSorting(Param p1, Param p2, Param p3, Param p4) {
    size_t size = p1.size;
    int i1 = 0, i2 = 0, i3 = 0, i4 = 0;
    int i = 0;
    CHAR* result = (CHAR*)calloc(size * 4, sizeof(CHAR));

    while (i1 != size || i2 != size || i3 != size || i4 != size) {
        char ch1 = 127, ch2 = 127, ch3 = 127, ch4 = 127;
        if (i1 != size) {
            ch1 = p1.arr[i1];
        }
        if (i2 != size) {
            ch2 = p2.arr[i2];
        }
        if (i3 != size) {
            ch3 = p3.arr[i3];
        }
        if (i4 != size) {
            ch4 = p4.arr[i4];
        }

        char min_val = min(min(ch1, ch2), min(ch3, ch4));
        if (min_val == ch1) {
            result[i] = p1.arr[i1];
            i1++; i++;
            continue;
        }
        if (min_val == ch2) {
            result[i] = p2.arr[i2];
            i2++; i++;
            continue;
        }
        if (min_val == ch3) {
            result[i] = p3.arr[i3];
            i3++; i++;
            continue;
        }
        if (min_val == ch4) {
            result[i] = p4.arr[i4];
            i4++; i++;
            continue;
        }
    }

    return result;
}

BOOL ReadFromFileAsync(){
    unsigned int startTime = clock();

    BOOL bResult = FALSE;

    HANDLE hFile = CreateFile(PATH,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE){
        cout << "Error opening file" << endl;
        return FALSE;
    }

    DWORD dwLineSize = SIZE_BUFFER / 4; // size of each line, in bytes

    LPSTR bFirstLineBuf = (CHAR*)calloc(dwLineSize, sizeof(CHAR));
    LPSTR bSecondLineBuf = (CHAR*)calloc(dwLineSize, sizeof(CHAR));
    LPSTR bThirdLineBuf = (CHAR*)calloc(dwLineSize, sizeof(CHAR));
    LPSTR bFourthLineBuf = (CHAR*)calloc(dwLineSize, sizeof(CHAR));

    OVERLAPPED oReadFirstLine = { 0 };
    OVERLAPPED oReadSecondLine = { 0 };
    OVERLAPPED oReadThirdLine = { 0 };
    OVERLAPPED oReadFourthLine = { 0 };

    oReadFirstLine.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!oReadFirstLine.hEvent) {
        cout << "Error creating I/O event for reading first line" << endl;
        return -1;
    }
    oReadFirstLine.Offset = 0; // offset of first line

    oReadSecondLine.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!oReadSecondLine.hEvent){
        cout << "Error creating I/O event for reading second line" << endl;
        return -1;
    }
    oReadSecondLine.Offset = dwLineSize; // offset of second line


    oReadThirdLine.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!oReadThirdLine.hEvent) {
        cout << "Error creating I/O event for reading third line" << endl;
        return -1;
    }
    oReadThirdLine.Offset = dwLineSize * 2; // offset of third line

    oReadFourthLine.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!oReadFourthLine.hEvent) {
        cout << "Error creating I/O event for reading forth line" << endl;
        return -1;
    }

    oReadFourthLine.Offset = dwLineSize * 3; // offset of fourth line

    DWORD iNumRead1 = 0;//Обязательный параметр. получает кол-во считанных байт
    DWORD iNumRead2 = 0;//Обязательный параметр. получает кол-во считанных байт
    DWORD iNumRead3 = 0;//Обязательный параметр. получает кол-во считанных байт
    DWORD iNumRead4 = 0;//Обязательный параметр. получает кол-во считанных байт

    if (!ReadFile(hFile, &bFirstLineBuf[0], dwLineSize, &iNumRead1, &oReadFirstLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read first line" << endl;
            return -1;
        }
    }

    if (!ReadFile(hFile, &bSecondLineBuf[0], dwLineSize, &iNumRead2, &oReadSecondLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read second line" << endl;
            return -1;
        }
    }

    if (!ReadFile(hFile, &bThirdLineBuf[0], dwLineSize, &iNumRead3, &oReadThirdLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read third line" << endl;
            return -1;
        }
    }

    if (!ReadFile(hFile, &bFourthLineBuf[0], dwLineSize, &iNumRead4, &oReadFourthLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read forth line" << endl;
            CancelIo(hFile);
            return -1;
        }
    }

    
    // perform some stuff asynchronously  

    HANDLE hEvents[4];
    hEvents[0] = oReadFirstLine.hEvent;
    hEvents[1] = oReadSecondLine.hEvent;
    hEvents[2] = oReadThirdLine.hEvent;
    hEvents[3] = oReadFourthLine.hEvent;

    DWORD dwWaitRes = WaitForMultipleObjects(_countof(hEvents), hEvents, TRUE, INFINITE);
    
    if (dwWaitRes == WAIT_FAILED) {
        cout << "Error waiting for I/O to finish" << endl;
        CancelIo(hFile);
        return -2;
    }


    if (oReadFirstLine.hEvent) CloseHandle(oReadFirstLine.hEvent);
    if (oReadSecondLine.hEvent) CloseHandle(oReadSecondLine.hEvent);
    if (oReadThirdLine.hEvent) CloseHandle(oReadThirdLine.hEvent);
    if (oReadFourthLine.hEvent) CloseHandle(oReadFourthLine.hEvent);

    CloseHandle(hFile);

    HANDLE hEventsSorting[4];

    Param p1 = { bFirstLineBuf, dwLineSize};
    Param p2 = { bSecondLineBuf, dwLineSize };
    Param p3 = { bThirdLineBuf, dwLineSize };
    Param p4 = { bFourthLineBuf, dwLineSize };
  

    hEventsSorting[0] = CreateThread(NULL, 0, SelectionSort, &p1, 1, 0);
    hEventsSorting[1] = CreateThread(NULL, 0, SelectionSort, &p2, 1, 0);
    hEventsSorting[2] = CreateThread(NULL, 0, SelectionSort, &p3, 1, 0);
    hEventsSorting[3] = CreateThread(NULL, 0, SelectionSort, &p4, 1, 0);

    DWORD results = WaitForMultipleObjects(_countof(hEventsSorting), hEventsSorting, TRUE, INFINITE);

    if (results == WAIT_FAILED) {
        cout << "Error waiting for I/O to finish" << endl;
        CancelIo(hFile);
        return -2;
    }


    if (hEventsSorting[0]) CloseHandle(hEventsSorting[0]);
    if (hEventsSorting[1]) CloseHandle(hEventsSorting[1]);
    if (hEventsSorting[2]) CloseHandle(hEventsSorting[2]);
    if (hEventsSorting[3]) CloseHandle(hEventsSorting[3]);

    char* res = EndSorting(p1, p2, p3, p4);

    HANDLE hFile2 = CreateFile(PATH3,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    oReadFirstLine = { 0 };
    oReadSecondLine = { 0 };
    oReadSecondLine.Offset = dwLineSize;
    oReadThirdLine = { 0 };
    oReadThirdLine.Offset = dwLineSize * 2;
    oReadFourthLine = { 0 };
    oReadFourthLine.Offset = dwLineSize * 3;

    if (!WriteFile(hFile2, &res[0], dwLineSize, NULL, &oReadFirstLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read first line" << endl;
            return -1;
        }
    }

    if (!WriteFile(hFile2, &res[dwLineSize], dwLineSize, &iNumRead2, &oReadSecondLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read second line" << endl;
            return -1;
        }
    }

    if (!WriteFile(hFile2, &res[dwLineSize*2], dwLineSize, &iNumRead3, &oReadThirdLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read third line" << endl;
            return -1;
        }
    }

    if (!WriteFile(hFile2, &res[dwLineSize*3], dwLineSize, &iNumRead4, &oReadFourthLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read forth line" << endl;
            CancelIo(hFile);
            return -1;
        }
    }

    CloseHandle(hFile2);


    unsigned int endTime = clock();

    cout << "Async time: " << endTime - startTime << endl;

    return bResult;
}

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    CreateFileWithValues();
    SyncReadSortWrite();
    ReadFromFileAsync();

    return 0;
}