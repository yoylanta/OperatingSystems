#include <cstdio>
#include <Windows.h>
#include "thread.h"

CRITICAL_SECTION CriticalSection;

int* create_array(const int& n)
{
    int *arr = new int[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = 0;
    }
    return arr;
}

void print(int* const& arr, const int& n)
{
    EnterCriticalSection(&CriticalSection);
    for (int i = 0; i < n; i++)
    {
        printf("%d\t", arr[i]);
    }
    printf("\n");
    LeaveCriticalSection(&CriticalSection);
    return;
}

DWORD WINAPI marker(LPVOID params)
{
    thread* info = (thread*)params;
    srand(info->num);
    bool mark_thread = false;
    int numberOfMarkedElements = 0;

    WaitForSingleObject(info->start, INFINITE);
    int index = -1;
    while (!mark_thread)
    {
        index = rand() % info->size;
        EnterCriticalSection(&CriticalSection);
        if (info->arr[index] == 0)
        {
            printf("%d thread: index: %d, changed %d to %d\n", info->num, index,
                   info->arr[index], info->num);
            Sleep(5);
            info->arr[index] = info->num;
            numberOfMarkedElements++;
            LeaveCriticalSection(&CriticalSection);
            Sleep(5);
        }
        else
        {
            printf("%d thread: Number of marked elements: %d. Cant mark element %d\n",
                   info->num, numberOfMarkedElements, index);
            fflush(stdout);
            LeaveCriticalSection(&CriticalSection);

            SetEvent(info->stop);
            int mainsResponse = WaitForMultipleObjects(2, info->terminate,
                                                       FALSE, INFINITE) - WAIT_OBJECT_0;
            if (mainsResponse == 0)
            {
                mark_thread = true;
            }
        }
    }

    for (int i = 0; i < info->size; i++)
    {
        if (info->arr[i] == info->num)
        {
            info->arr[i] = 0;
        }
    }
    return 0;
}

int main()
{
    int n = 0;
    do
    {
        printf("Enter size of array:\n");
        scanf("%d", &n);
    }
    while (n <= 0);

    int *arr = create_array(n);

    int numo_markers = 0;
    do
    {
        printf("Enter number of marker threads:\n");
        scanf("%d", &numo_markers);
    }
    while (numo_markers <= 0);

    InitializeCriticalSection(&CriticalSection);

    HANDLE* hThread = new HANDLE[numo_markers];
    thread* markerparams = new thread[numo_markers];

    bool* terminated_threads = new bool[numo_markers];
    HANDLE* marker_threads = new HANDLE[numo_markers];

    HANDLE start = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (start == NULL)
    {
        printf("Error: Event didn't start!");
        return GetLastError();
    }

    for (int i = 0; i < numo_markers; i++)
    {
        HANDLE stop = CreateEvent(NULL, TRUE, FALSE, NULL);
        HANDLE *state = new HANDLE[2];
        state[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
        state[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
        markerparams[i] = thread(arr, n, i + 1, start,
                                   stop, state);
        marker_threads[i] = markerparams[i].stop = CreateEvent(NULL, TRUE,
                                                               FALSE, NULL);
        hThread[i] = CreateThread(NULL, 0, marker,
                                  (LPVOID)(&markerparams[i]), NULL, NULL);

        terminated_threads[i] = false;
    }

    if (!SetEvent(start))
    {
        printf("Error: Event didnt start!\n");
        return GetLastError();
    }
    int numo_markerthreads = 0;
    while (numo_markerthreads != numo_markers)
    {
        WaitForMultipleObjects(numo_markers, marker_threads,
                               TRUE, INFINITE);
        fflush(stdout);
        printf("Threads are ready:\n");
        print(arr, n);

        int id = -1;

        while (id <= 0 || id > numo_markers)
        {
            printf("Enter id:\n");
            scanf("%d", &id);
            if (id > numo_markers || id <= 0)
            {
                printf("Error: incorrect input format\n");
                continue;
            }
            else
            {
                SetEvent(markerparams[id-1].terminate[0]);
                WaitForSingleObject(hThread[id-1], INFINITE);
                print(arr, n);
                terminated_threads[id-1] = true;
                numo_markerthreads++;
                break;
            }
        }

        for (int i = 0; i < numo_markers; ++i) {
            if (!terminated_threads[i]) {
                ResetEvent(markerparams[i].stop);
                SetEvent(markerparams[i].terminate[1]);
            }
        }
    }
    printf("Marker threads have been terminated!\n");


    if (!CloseHandle(start))
    {
        printf("Error: Handle didnt close!\n");
        return GetLastError();
    }
    for (int i = 0; i < numo_markers; i++)
    {
        bool closed = CloseHandle(hThread[i]) &&  CloseHandle(marker_threads[i]) &&
                             CloseHandle(markerparams[i].terminate[0])
                             && CloseHandle(markerparams[i].terminate[1]);
        if (!closed)
        {
            printf("Error: Handle didnt close!\n");
            return GetLastError();
        }
    }
    delete[] arr;
    delete[] hThread;
    delete[] terminated_threads;
    delete[] markerparams;
    delete[] marker_threads;

    DeleteCriticalSection(&CriticalSection);
    return 0;
}
