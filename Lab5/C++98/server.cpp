#include <iostream>
#include <conio.h>
#include <fstream>
#include <time.h>
#include <algorithm>
#include <process.h>
#include <Windows.h>
#include "employee.h"

CRITICAL_SECTION cs;
int count;
employee* emp;
HANDLE* hReady;
bool *mod;
const char pipename[30] = "\\\\.\\pipe\\name";

void sort(){
    qsort(emp, count, sizeof(employee), comparator);
}

void write(char filename[50]){
    std::fstream fin(filename, std::ios::binary | std::ios::out);
    fin.write(reinterpret_cast<char*>(emp), sizeof(employee) * count);
    fin.close();
}

void read(){
    emp = new employee[count];
    std::cout << "Enter employee data (id, name, hours):" << std::endl;
    for(int i = 1; i <= count; ++i){
        std::cout  << i << ")";
        std::cin >> emp[i - 1].id >> emp[i - 1].name >> emp[i - 1].hours;
    }
}

employee* find_employee(int id){
    employee key;
    key.id = id;
    return (employee*) bsearch((const char *) (&key), (const char *) (emp),
                               count, sizeof(employee),comparator);
}

void START_PROCESS(int count){
    char buffer[10];
    for(int i = 0; i < count; i++) {
        char args[80] = "..\\cmake-build-debug\\Client.exe ";
        char eventname[50] = "READY_EVENT_";
        itoa(i + 1, buffer, 10);
        strcat(eventname, buffer);
        strcat(args, eventname);
        STARTUPINFO startupinfo;
        PROCESS_INFORMATION processinfo;
        ZeroMemory(&startupinfo, sizeof(STARTUPINFO));
        startupinfo.cb = sizeof(STARTUPINFO);
        hReady[i] = CreateEvent(NULL, TRUE, FALSE, eventname);
        if (!CreateProcess(NULL, args, NULL, NULL,
                           FALSE, CREATE_NEW_CONSOLE,NULL,
                           NULL, &startupinfo, &processinfo)) {
            printf("Error! Failed process creation! \n");
            CloseHandle(processinfo.hProcess);
            CloseHandle(processinfo.hThread);
        }
    }
}

DWORD WINAPI connect(LPVOID handle){
    HANDLE hPipe = (HANDLE)hPipe;
    employee* error = new employee;
    error->id = -1;
    while(true){
        DWORD rBytes;
        char message[10];
        bool isRead = ReadFile(handle, message, 10, &rBytes, NULL);
        if(!isRead){
            if(ERROR_BROKEN_PIPE == GetLastError()){
                std::cout << "Error! Failed client connection!" << std::endl;
                break;
            }
            else {
                std::cerr << "Error! Failed reading message!" << std::endl;
                break;
            }
        }

        if(strlen(message) > 0) {
            char command = message[0];
            message[0] = ' ';
            int id = atoi(message);
            DWORD wBytes;
            EnterCriticalSection(&cs);
            employee* send_error = find_employee(id);
            LeaveCriticalSection(&cs);
            if(NULL == send_error){
                send_error = error;
            }
            else{
                int ind = send_error - emp;
                if(mod[ind])
                    send_error = error;
                else{
                    switch (command) {
                        case 'w':
                            printf("Request: Modify id %d", id);
                            mod[ind] = true;
                            break;
                        case 'r':
                            printf("Request: Read id %d", id);
                            break;
                        default:
                            std::cout << "Request does not exist!";
                            send_error = error;
                    }
                }
            }
            bool isSent = WriteFile(handle, send_error, sizeof(employee), &wBytes, NULL);
            if(isSent) std::cout << "Success! Answer has been sent!" << std::endl;
            else std::cout << "Error! Faild to send the answer!" << std::endl;

            if('w' == command && send_error != error){
                isRead = ReadFile(handle, send_error, sizeof(employee), &rBytes, NULL);
                if(isRead){
                    std::cout << "Record has been modified!" << std::endl;
                    mod[send_error - emp] = false;
                    EnterCriticalSection(&cs);
                    sort();
                    LeaveCriticalSection(&cs);
                }
                else{
                    std::cerr << "Error! Failed to read message!" << std::endl;
                    break;
                }
            }
        }
    }
    FlushFileBuffers(handle);
    DisconnectNamedPipe(handle);
    CloseHandle(handle);
    delete error;
}

void openPipes(int clientCount){
    HANDLE hPipe;
    HANDLE* hThreads = new HANDLE[clientCount];
    for(int i = 0; i < clientCount; i++){
        hPipe = CreateNamedPipe(pipename, PIPE_ACCESS_DUPLEX,
                                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                PIPE_UNLIMITED_INSTANCES,0, 0,INFINITE, NULL);
        if (INVALID_HANDLE_VALUE == hPipe){
            std::cerr << "Error! Failed to create pipe!" << std::endl;
            getch();
            return;
        }
        if(!ConnectNamedPipe(hPipe, NULL)){
            std::cout << "Error! Client is not connected!" << std::endl;
            break;
        }
        hThreads[i] = CreateThread(NULL, 0, connect,
                                   (LPVOID) hPipe, 0, NULL);
    }
    std::cout << "Success! Clients have been connected!\n" << std::endl;
    WaitForMultipleObjects(clientCount, hThreads, TRUE, INFINITE);
    std::cout << "Clients have been disconnected!\n" << std::endl;
    delete[] hThreads;
}

int main() {
    char filename[50];
    std::cout << "Enter filename and number of employee records: \n>";
    std::cin >> filename >> count;
    read();
    write(filename);
    sort();
    InitializeCriticalSection(&cs);
    srand(time(0));
    int clientCount = 2 + rand() % 3;
    HANDLE hstartALL = CreateEvent(NULL, TRUE, FALSE, "START_ALL");
    mod = new bool[count];
    for(int i = 0; i < count; ++i)
        mod[i] = false;
    hReady = new HANDLE[clientCount];
    START_PROCESS(clientCount);
    WaitForMultipleObjects(clientCount, hReady, TRUE, INFINITE);
    std::cout << "Processes are prepared!" << std::endl;
    SetEvent(hstartALL);

    openPipes(clientCount);
    for(int i = 0; i < count; i++)
        emp[i].print(std::cout);
    getch();
    DeleteCriticalSection(&cs);
    delete[] mod;
    delete[] hReady;
    delete[] emp;
    return 0;
}

