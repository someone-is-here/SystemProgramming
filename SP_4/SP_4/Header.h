#pragma once
#include <Windows.h>
#include <string.h>
#include <ctime>
#include <iostream>
#include <vector>

#define SIZE_BUFFER 10000

#define PATH L"NotSorted.txt"
#define PATH2 L"SortedSync.txt"
#define PATH3 L"SortedAsync.txt"

using namespace std;

struct Param {
    char* arr;
    int size;
};