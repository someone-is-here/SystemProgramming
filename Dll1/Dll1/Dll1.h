#pragma once
#include <Windows.h>


#ifdef Dll1DLL_EXPORTS  
#define Dll1_API __declspec(dllexport)   
#else  
#define Dll1_API __declspec(dllimport)   
#endif  
