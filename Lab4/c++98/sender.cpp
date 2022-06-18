#include <cstdio>
#include <Windows.h>

const int msg_len = 20;

int main(int argc, char* argv[])
{
    HANDLE writing_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE, "Writing_semaphore");
    HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "Mutex");
    HANDLE reading_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE, "Reading_semaphore");

    char check_eventname[256];
    int len = sprintf_s(check_eventname, "%s Prepared", argv[3]);
    HANDLE flag = OpenEvent(EVENT_ALL_ACCESS,FALSE, check_eventname);
    SetEvent(flag);
    FILE* fptr = fopen(argv[1], "ab");

    int choice = -1;
    char msg[msg_len];

    while(true)
    {
        printf_s("1. Write message\n 2. Exit\n");
        scanf_s("%d", &choice);

        if (choice != 1 && choice != 2)
        {
            printf_s("Sorry! There is no such option!\n");
            continue;
        }
        if (choice == 2){break;}

        printf_s("Enter your message:");
        scanf_s("%s", msg);

        WaitForSingleObject(writing_semaphore, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        fptr = fopen(argv[1], "ab");
        fwrite(msg, msg_len, 1, fptr);
        fclose(fptr);

        printf_s("Message: %s\n", msg);

        ReleaseMutex(mutex);
        ReleaseSemaphore(reading_semaphore, 1, NULL);
        printf_s("Message is written!\n\n");
    }

    fclose(fptr);
    CloseHandle(flag);
    CloseHandle(writing_semaphore);
    CloseHandle(mutex);
    CloseHandle(reading_semaphore);
    free(fptr);
    return 0;
}