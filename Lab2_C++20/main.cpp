#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

int main()
{
    vector<double> arr;
    int size = 0;
    cout << "Enter array size:"<<endl;
    cin >> size;
    if (size <= 0)
    {
        throw runtime_error("Size smaller than zero.");
        return -1;
    }
    int temp = 0;
    cout<<"Enter array:"<<endl;
    for (int i = 0; i < size; i++)
    {
        cin >> temp;
        arr.push_back(temp);
    }

    int min = arr[0];
    int max = arr[0];
    thread Min_Max([&min, &max](vector<double> arr){

        for (int i = 1; i < arr.size(); i++)
        {
            if (min > arr[i])
            {
                min = arr[i];
            }
            this_thread::sleep_for(chrono::milliseconds(7));
            if (max < arr[i])
            {
                max = arr[i];
            }
            this_thread::sleep_for(chrono::milliseconds(7));
        }


    }, arr);

    double average = 0;
    thread Average([&average](vector<double> arr){
        for (const double& item : arr)
        {
            average += item;
            this_thread::sleep_for(chrono::milliseconds(12));
        }
        average /= static_cast<double>(arr.size());
    }, arr);
    Min_Max.join();
    Average.join();

    cout << "Max element is:" << max << endl;
    cout << "Min element is:" << min << endl;
    cout << "Average element is:" << average << endl;
    cout << "Final array is: "<<endl;
    for (double& item : arr)
    {
        if (item == min || item == max)
            item = average;
        cout << item << " ";
    }
    cout << endl;
    return 0;
}