//#include <iostream>
#include <stdio.h> /* puts, printf */
#include <time.h>  /* time_t, struct tm, time, localtime */
#include <string>
#include <iostream>
#include <functional>
//#include "MyAlarm.hpp"

using namespace std;

int main()
{
    int i = 3;
    switch (i)
    {
    case 1:
        std::cout << "1";
    case 2:
        std::cout << "2"; //execution starts at this case label
    case 3:
        std::cout << "3";
    case 4:
    case 5:
        std::cout << "45";
        break; //execution of subsequent statements is terminated
    case 6:
        std::cout << "6";
    }
    return 0;
}