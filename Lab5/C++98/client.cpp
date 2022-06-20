#include <iostream>
#include <conio.h>
#include <windows.h>
#include "employee.h"

const std::string pipename = "\\\\.\\pipe\\name";
void connect(HANDLE hPipe){
    while(true){
        char command[10];
        std::cout << "Enter: \n";
        std::cout << "r - read, w - write \n";
        std::cout << "And enter employee id: \n";
        std::cin.getline(command, 10, '\n');
        if(std::cin.eof()) {
            return;
        }
        DWORD wBytes;

        if(!WriteFile(hPipe, command, 10, &wBytes, NULL)){
            std::cerr << "Error! Failed to send message!" << std::endl;
            getch();
            return;
        }
        employee employee;
        DWORD rBytes;

        if(!ReadFile(hPipe, &employee, sizeof(employee), &rBytes, NULL))
            std::cerr << "Error! Failed to read answer!" << std::endl;

        else{
            if(employee.id < 0) {
                std::cerr << "Error! Incorrect employee id!" << std::endl;
                continue;
            }
            else {
                employee.print(std::cout);
                if('w' == command[0]) {
                    std::cout << "Enter employee data (id, name, hours):\n" << std::flush;
                    std::cin >> employee.id >> employee.name >> employee.hours;
                    std::cin.ignore(2, '\n');
                    if (WriteFile(hPipe, &employee, sizeof (employee), &wBytes, NULL))
                        std::cout << "Message has been sent!" << std::endl;

                    else {
                        std::cerr << "Error! Failed to send data!" << std::endl;
                        getch();
                        break;
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    HANDLE hReadyEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, argv[1]);
    HANDLE hStartEvent = OpenEvent(SYNCHRONIZE, FALSE, "START_ALL");

    if(NULL == hStartEvent || NULL == hReadyEvent){
        std::cerr << "Error! Failed to start events!"  << std::endl;
        getch();
        return GetLastError();
    }
    SetEvent(hReadyEvent);
    WaitForSingleObject(hStartEvent, INFINITE);
    HANDLE hPipe;
    while (true)
    {
        hPipe = CreateFile(pipename.c_str(),
                           GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
                           NULL,OPEN_EXISTING, 0,
                           NULL);
        if (INVALID_HANDLE_VALUE != hPipe) {
            break;
        }
        if(!WaitNamedPipe(pipename.c_str(), 2000)){
            std::cout << "Runtime error!" << std::endl;
            getch();
            return 0;
        }
    }
    std::cout << "Successfully connected to pipe!" << std::endl;
    connect(hPipe);
    return 0;
}
