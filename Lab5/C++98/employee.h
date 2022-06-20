
#ifndef LABABABABA5_EMPLOYEE_H
#define LABABABABA5_EMPLOYEE_H

#include <ostream>
struct employee {
    int id;
    char name[10];
    double hours;
    void print(std::ostream &out){
        out << "id: " << id
            << "\nname: " << name
            << "\nhours: " << hours << std::endl;
    }
};

int comparator(const void* emp1, const void* emp2){
    return ((employee*)emp1)->id - ((employee*)emp2)->id;
}

#endif //LABABABABA5_EMPLOYEE_H
