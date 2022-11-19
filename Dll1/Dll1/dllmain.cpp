
#include "pch.h"
#include <windows.h>
#include "../../SP_4/SP_4/Header.h"
#include "Dll1.h"

#define EOF (-1)

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

    __declspec(dllexport) int __cdecl CreateFileWithValues(){
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

    __declspec(dllexport) DWORD __cdecl SelectionSort(LPVOID lpParam) {
        HANDLE hStdout = NULL;

        hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hStdout == INVALID_HANDLE_VALUE) {
            return 1;
        }
        Param* p = (Param*)lpParam;

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

#ifdef __cplusplus
}
#endif
