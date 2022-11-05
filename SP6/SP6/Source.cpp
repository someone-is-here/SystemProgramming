#include <iostream>
#include <mutex>
#include <thread>
#include <Windows.h>

#define P 5

HANDLE hThreads[P]; //my philosophers
HANDLE mtx[P]; //forks
CRITICAL_SECTION CriticalSection; //might be changed to mutex


bool pickUp(int left, int right) {
    if (WaitForSingleObject(mtx[left], 1) == WAIT_OBJECT_0) {
        if (WaitForSingleObject(mtx[right], 1) == WAIT_OBJECT_0) {
            return true;
        }
        else {
            ReleaseMutex(mtx[left]);
        }
    }
    return false;
}

void putDown(int left, int right) {
    ReleaseMutex(mtx[left]);
    ReleaseMutex(mtx[right]);
}

DWORD WINAPI Run(CONST LPVOID philID) {
    int leftIndex = (int)philID - 1;
    int rightIndex = ((int)philID) % (P - 1);

    while (true) {
        if (pickUp(leftIndex, rightIndex)) {
            TryEnterCriticalSection(&CriticalSection);
            std::cout << "Philosopher " << philID << " eats.\n";
            LeaveCriticalSection(&CriticalSection);

            Sleep(rand() % 200);
            putDown(leftIndex, rightIndex);
        } else {
            TryEnterCriticalSection(&CriticalSection);
            std::cout << "Philosopher " << philID << " thinks.\n";
            LeaveCriticalSection(&CriticalSection);
            Sleep(rand() % 200);
        }
    }
}
int main() {
    for (int i = 1; i <= P; i++) {
        mtx[i - 1] = CreateMutex(NULL, FALSE, NULL);
    }

    for (int i = 1; i <= P; i++) {
        hThreads[i - 1] = CreateThread(NULL, 0, &Run, (LPVOID)i, 0, NULL);
    }

    WaitForMultipleObjects(P, hThreads, TRUE, INFINITE);

    for (int i = 1; i <= P; i++) {
        CloseHandle(hThreads[i - 1]);
    }

    CloseHandle(mtx);
    ExitProcess(0);
}