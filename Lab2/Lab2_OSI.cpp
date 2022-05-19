#include <iostream>
#include <Windows.h>
#include <process.h>
#include <limits>
using namespace std;

int arr[10000]; //массив, принимающий значения, введённые пользователем
int arr_size;

struct Min_Max {
    int min = INT_MAX; //начальное значение для нахождения минимального числа в массиве
    int max = INT_MIN; //начальное значение для нахождения максимлаьного числа в массиве
    int max_pos = 0;
    int min_pos = 0;
};

unsigned int __stdcall min_max_thread(void* data)
{

    Min_Max *x = static_cast<Min_Max*>(data);
    int max = x->max;
    int min = x->min;
    int max_pos = 0;
    int min_pos = 0;
    for (int i = 0; i < arr_size; i++) {
        if (arr[i] > max) {
            max = arr[i];
            max_pos = i;
        }
        Sleep(7);
        if (arr[i] < min) {
            min = arr[i];
            min_pos = i;
        }
        Sleep(7);
    }

    x->max = max;
    x->min = min;
    x->max_pos = max_pos;
    x->min_pos = min_pos;
    DWORD thread_id = GetCurrentThreadId();
    printf("Thread Min_Max with %d reports: Max element is %d \n", thread_id, max);
    printf("Thread Min_Max with %d reports: Min element is %d \n", thread_id, min);
    return 0;
}

unsigned int __stdcall average_thread(void* data)
{
    int *average = static_cast<int*>(data);
    for (int i = 0; i < arr_size; i++) {
        (*average) += arr[i];
        Sleep(12);
    }
    (*average) = (*average) / arr_size;

    DWORD thread_id = GetCurrentThreadId();
    printf("Thread Average with id = %d reports: Average number is %d \n", thread_id, (*average));
    return 0;
}

int main()
{
    int x, n;
    cout << "Enter the number of items:" << "\n";
    cin >> n;
    arr_size = n;
    cout << "Enter " << n << " items" << endl;
    for (x = 0; x < n; x++) {
        cin >> arr[x];
    }
    Min_Max elements;
    int average = 0;
    HANDLE myMinMaxHandle = (HANDLE)_beginthreadex(0, 0, &min_max_thread, &elements, 0, 0);
    HANDLE myAverageHandle = (HANDLE)_beginthreadex(0, 0, &average_thread, &average, 0, 0);

    WaitForSingleObject(myMinMaxHandle, INFINITE); // ждём завершения потока min_max
    WaitForSingleObject(myAverageHandle, INFINITE); // ждём завершения потока average

    cout << "Max elem is " << elements.max << " at position " << elements.max_pos << endl;
    cout << "Min elem is " << elements.min << " at position " << elements.min_pos << endl;
    cout << "Average is " << average << endl;

    arr[elements.max_pos] = average;
    arr[elements.min_pos] = average;

    cout << "Final results: " << endl;
    for (x = 0; x < n; x++) {
        cout << arr[x] << " ";
    }
    CloseHandle(myMinMaxHandle);
    CloseHandle(myAverageHandle);
    return 0;
}
