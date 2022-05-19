#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>

using namespace std;

const char* CREATOR = "creator.exe";
const char* REPORTER = "reporter.exe";

void start_creator(const string& binary_name, const int& record_count);
void start_reported(const string&, const string&, const double&);

int main()
{
    string input_bin_filename;
    long long record_count = 0;

    cout << "Binary_filename:\t";
    cin >> input_bin_filename;
    cout << "Count of records:\t";
    cin >> record_count;

    start_creator(input_bin_filename, record_count);


    string report_filename;
    double hourly_pay = 0.0;
    cout << "\nReport_filename:\t";
    cin >> report_filename;
    cout << "Hourly_pay:\t";
    cin >> hourly_pay;

    start_reported(input_bin_filename, report_filename, hourly_pay);

    return 0;
}

void start_creator(const string& binary_name, const int& record_count)
{
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;

    ZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);

    stringstream sstream;
    sstream << CREATOR << " " << binary_name << " " << record_count;

    if (!CreateProcess(NULL, (char*)sstream.str().c_str(),
                       NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
                       NULL, NULL, &startup_info, &process_info))
    {
        cout << "Error: process not started!\n";
    }
    cout << "Process started!\n";
    WaitForSingleObject(process_info.hProcess, INFINITE);
    CloseHandle(process_info.hProcess);
    cout << "Process stoped!\n";
}


void start_reported(const string& input_bin_filename,
                    const string& report_filename, const double& hourly_pay)
{
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;

    ZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);

    stringstream sstream;
    sstream << REPORTER << " " << input_bin_filename << " " << report_filename <<
            " " << hourly_pay;

    if (!CreateProcess(NULL, (char*)sstream.str().c_str(),
                       NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
                       NULL, NULL, &startup_info, &process_info))
    {
        cout << "Error: process not started!\n";
    }
    else
    {
        cout << "Process started!\n";
        WaitForSingleObject(process_info.hProcess, INFINITE);
        CloseHandle(process_info.hProcess);
        cout << "Process stoped!\n";
    }
}