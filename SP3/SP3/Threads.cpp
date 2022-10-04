#include <windows.h>
#include <strsafe.h>
#include <stdio.h>
#include <ctime>

#define BUF_SIZE 255

int GlobalData = 0;

void DisplayMessage(HANDLE hScreen, char* ThreadName, int iter, int data) {
	TCHAR buf[BUF_SIZE];
	size_t stringSize;
	DWORD dwChars;

	StringCchPrintf(buf, BUF_SIZE, TEXT("Executing iteration %d of %s with data = %d\n"), iter, ThreadName, data);
	StringCchLength(buf, BUF_SIZE, &stringSize);

	WriteConsole(hScreen, buf, stringSize, &dwChars, NULL);
}

int* create_array(size_t size) {
	int* arr = new int[size];
	for (int i = 0; i < size; i++) {
		arr[i] = rand() % 100;
	}
	return arr;
}

void print_array(int* arr, size_t size) {
	for (int i = 0; i < size; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

DWORD WINAPI bubble_sort(LPVOID lpParam) {
	unsigned int start_time = clock();
	printf("\nBubble_sort Start!\n");

	size_t size = 0;
	HANDLE hStdout = NULL;

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == INVALID_HANDLE_VALUE) {
		return 1;
	}

	size = *((int*)lpParam);

	int* arr = create_array(size);
	print_array(arr, size);

	int iter = 0;
	for (int i = 0; i < size; i++, ++iter) {
		DisplayMessage(hStdout, (char*)L"Bubble_sort", iter, i);
		for (int j = 0; j < size - i - 1; j++, ++iter) {
			if (arr[j] > arr[j + 1]) {
				int temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
	unsigned int end_time = clock();
	printf("\nBubble_sort DONE!\nTime: %d\n", end_time - start_time);
	print_array(arr, size);

	return 0;
}

DWORD WINAPI insertion_sort(LPVOID lpParam) {
	unsigned int start_time = clock();
	printf("\nInsertion_sort Start!\n");

	size_t size = 0;
	HANDLE hStdout = NULL;

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == INVALID_HANDLE_VALUE) {
		return 1;
	}

	size = *((int*)lpParam);

	int* arr = create_array(size);
	print_array(arr, size);

	int iter = 0;
	for (int i = 0; i < size; i++, ++iter) {
		DisplayMessage(hStdout, (char*)L"Insertion_sort", iter, i);
		for (int j = i; j < size; j++, ++iter) {
			if (arr[j] < arr[i]) {
				int temp = arr[j];
				arr[j] = arr[i];
				arr[i] = temp;
			}
		}
	}

	unsigned int end_time = clock();

	printf("\nInsertion_sort DONE!\nTime: %d\n", end_time - start_time);
	print_array(arr, size);

	return 0;
}

DWORD WINAPI selection_sort(LPVOID lpParam) {
	unsigned int start_time = clock();
	printf("\selection_sort Start!\n");

	size_t size = 0;
	HANDLE hStdout = NULL;

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == INVALID_HANDLE_VALUE) {
		return 1;
	}

	size = *((int*)lpParam);

	int* arr = create_array(size);
	print_array(arr, size);

	int iter = 0;
	for (int i = 0; i < size; i++, ++iter) {
		int min = i;
		DisplayMessage(hStdout, (char*)L"Selection_sort", iter, i);
		for (int j = i + 1; j < size; j++, ++iter) {
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

	unsigned int end_time = clock();

	printf("\nSelection_sort DONE!\nTime: %d\n", end_time - start_time);
	print_array(arr, size);

	return 0;
}

int main() {
	size_t size = 50;

	HANDLE th_bubble_sort = 0;
	HANDLE th_selection_sort = 0;
	HANDLE th_insertion_sort = 0;

	HANDLE th_array[3];

	th_bubble_sort = CreateThread(NULL, 0, bubble_sort, &size, 1, 0);
	th_selection_sort = CreateThread(NULL, 0, selection_sort, &size, 1, 0);
	th_insertion_sort = CreateThread(NULL, 0, insertion_sort, &size, 1, 0);

	SetPriorityClass(th_bubble_sort, THREAD_PRIORITY_HIGHEST);
	SetPriorityClass(th_selection_sort, THREAD_PRIORITY_LOWEST);
	SetPriorityClass(th_bubble_sort, THREAD_PRIORITY_LOWEST);

	if (th_bubble_sort == NULL) {
		ExitProcess(size);
	}
	if (th_selection_sort == NULL) {
		ExitProcess(size);
	}
	if (th_insertion_sort == NULL) {
		ExitProcess(size);
	}

	th_array[0] = th_bubble_sort;
	th_array[1] = th_selection_sort;
	th_array[2] = th_insertion_sort;

	WaitForMultipleObjects(3, th_array, TRUE, INFINITE);

	printf("\n All threads have been successfully executed\n");

	CloseHandle(th_bubble_sort);
	CloseHandle(th_selection_sort);
	CloseHandle(th_insertion_sort);

	printf("\nAll HANDLES closed! \n");

	system("pause");
}