//#include <iostream>
//#include <thread>
//#include <chrono>
//#include <ctime>
//
//using namespace std;
//
//void print_array(int* arr, size_t size) {
//	for (int i = 0; i < size; i++) {
//		cout << arr[i] << " ";
//	}
//	cout << endl;
//}
//
//void init_array(int* arr, size_t size) {
//	for (int i = 0; i < size; i++) {
//		arr[i] = rand() % 100 + 1;
//	}
//}
//
//void insertion_sort(int* arr, size_t size) {
//	cout << "Insertion_sort Start!" << endl;
//	for (int i = 0; i < size; i++) {
//		cout << "Insertion_sort iteration!" << i << "; Thread_id: " << this_thread::get_id() << endl;
//		for (int j = i; j < size; j++) {
//			if (arr[j] < arr[i]) {
//				int temp = arr[j];
//				arr[j] = arr[i];
//				arr[i] = temp;
//			}
//		}
//	}
//	print_array(arr, size);
//	cout << "Insertion_sort DONE!" << endl;
//}
//
//void selection_sort(int* arr, size_t size) {
//	cout << "Selection_sort Start!" << endl;
//	for (int i = 0; i < size; i++) {
//		int min = i;
//		cout << "Selection_sort iteration!" << i << "; Thread_id: " << this_thread::get_id() << endl;
//		for (int j = i + 1; j < size; j++) {
//			if (arr[j] < arr[min]) {
//				min = j;
//			}
//		}
//		if (min != i) {
//			int temp = arr[i];
//			arr[i] = arr[min];
//			arr[min] = temp;
//		}
//	}
//	print_array(arr, size);
//	cout << "Selection_sort DONE!" << endl;
//}
//
//void bubble_sort(int* arr, size_t size) {
//	cout << "Bubble_sort Start!" << endl;
//	for (int i = 0; i < size; i++) {
//		cout << "Bubble_sort iteration!" << i << "; Thread_id: " << this_thread::get_id() << endl;
//		for (int j = 0; j < size - i - 1; j++) {
//			if (arr[j] > arr[j + 1]) {
//				int temp = arr[j];
//				arr[j] = arr[j + 1];
//				arr[j + 1] = temp;
//			}
//		}
//	}
//	cout << "Bubble_sort DONE!" << endl;
//	print_array(arr, size);
//}
//
//int* copy_array(int* arr, size_t size) {
//	int* newArr = new int[size];
//
//	for (int i = 0; i < size; i++) {
//		newArr[i] = arr[i];
//	}
//
//	return newArr;
//}
//
//int main() {
//	srand(time(NULL));
//
//	size_t size = 10;
//	int* arr = new int[size];
//
//	init_array(arr, size);
//
//	int* cpArr = copy_array(arr, size);
//	int* cpArr2 = copy_array(arr, size);
//
//	print_array(arr, size);
//	print_array(cpArr, size);
//	print_array(cpArr2, size);
//
//	cout << "MAIN_THREAD " << this_thread::get_id() << endl;
//
//	thread th_selection_sort(selection_sort, arr, size);
//	thread th_bubble_sort(bubble_sort, cpArr, size);
//	thread th_insertion_sort(insertion_sort, cpArr, size);
//
//	th_selection_sort.join();
//	th_bubble_sort.join();
//	th_insertion_sort.join();
//
//	return 0;
//}