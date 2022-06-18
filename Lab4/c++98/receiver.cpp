#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <Windows.h>

const int msg_len = 20;
const char* sender_exe =  "sender.exe";
const char* binary_ext= ".bin";

HANDLE Start_Process(char* args)
{
    STARTUPINFO startupinfo;
    PROCESS_INFORMATION processinfo;

    ZeroMemory(&startupinfo, sizeof(STARTUPINFO));
    startupinfo.cb = sizeof(startupinfo);

    if (!CreateProcess(NULL, args, NULL, NULL, FALSE,
                       CREATE_NEW_CONSOLE, NULL, NULL, &startupinfo, &processinfo))
    {
        return NULL;
    }

    CloseHandle(processinfo.hThread);
    return processinfo.hProcess;
}

int main()
{
    char filename[100];
    int n = 0;

    printf_s("Enter bin filename:");
    scanf_s("%[^\n]%*c", filename);

    do
    {
        printf_s("\nEnter number of messages:");
        scanf_s("%d",  &n);

    } while (n <= 0);

    char path[150];
    int len = sprintf_s(path, "%s%s", filename, binary_ext);
    FILE *fptr = fopen(path, "wb");
    fclose(fptr);

    int sender_process_num = 0;

    do
    {
        printf_s("\nEnter number of sender_processes:");
        scanf_s("%d", &sender_process_num);

        if (sender_process_num <=  0)
        {
            printf_s("Incorrect input. Please enter a natural number.");
        }
    } while (sender_process_num <= 0);

    HANDLE* senderHandle = (HANDLE*)malloc(sizeof(HANDLE) * sender_process_num);

    HANDLE writing_semaphore = CreateSemaphore(NULL, n, n, "Writing_semaphore");
    HANDLE mutex = CreateMutex(NULL, FALSE, "Mutex");
    HANDLE reading_semaphore = CreateSemaphore(NULL, 0, n, "Reading_semaphore");

    if ((mutex && writing_semaphore && reading_semaphore) == NULL)
    {
        perror("Error! Failed creating mutex, writing_semaphore and reading_semaphore!\n");
        return GetLastError();
    }
    HANDLE* flag = (HANDLE*)malloc(sizeof(HANDLE) * sender_process_num);
    char pathname[256];
    len = sprintf_s(pathname, "%s", path);
    for (int i = 0; i < sender_process_num; i++)
    {
        char args[256];
        len = sprintf_s(args, "%s %s %d %d", sender_exe, pathname, n, i);
        printf_s("%i: %s\n", i, args);
        char check_eventname[256];
        len = sprintf_s(check_eventname, "%d Prepared", i);
        HANDLE ready = CreateEvent(NULL, FALSE, FALSE, check_eventname);
        if (ready == NULL)
        {
            perror("Create event error! \n");
            return GetLastError();
        }
        flag[i] = ready;

        HANDLE hProcess = Start_Process(args);
        if (!hProcess)
        {
            perror("Starting process error!\n");
            return GetLastError();
        }
        senderHandle[i] = hProcess;
    }
    WaitForMultipleObjects(sender_process_num, flag, TRUE, INFINITE);

    char messageBuffer[msg_len];
    int choice = -1;
    while (true)
    {
        memset(messageBuffer, 0, msg_len);
        printf_s("1. Read message\n 2. Exit\n ");
        scanf_s("%d", &choice);

        if (choice != 1 && choice != 2)
        {
            printf_s("Sorry! There is no such option! \n");
            continue;
        }
        if (choice == 2)
        {
            break;
        }
        WaitForSingleObject(reading_semaphore, INFINITE);
        WaitForSingleObject(mutex, INFINITE);


        fptr = fopen(path, "rb");
        fseek(fptr, 0, SEEK_SET);
        fread(messageBuffer, msg_len, 1, fptr);
        printf_s("\nNew message: %s\n", messageBuffer);
        fseek(fptr, 0, SEEK_END);
        int k = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);
        char *fileBuffer = (char*)malloc(sizeof(char) * k);
        fread(fileBuffer, k, 1, fptr);
        fclose(fptr);
        fptr = fopen(path, "wb");
        fwrite(fileBuffer + msg_len, k - msg_len, 1, fptr);
        fclose(fptr);
        free(fileBuffer);
        ReleaseMutex(mutex);
        ReleaseSemaphore(writing_semaphore, 1, NULL);
    }

    for (int i = 0; i < sender_process_num; i++)
    {
        CloseHandle(senderHandle[i]);
        CloseHandle(flag[i]);
    }
    free(flag);
    CloseHandle(writing_semaphore);
    CloseHandle(mutex);
    CloseHandle(reading_semaphore);
    free(senderHandle);
    free(fptr);
    return 0;
}
