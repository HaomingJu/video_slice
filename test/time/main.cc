#include <string.h>
#include <time.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include "TimeUtils.h"


int main(int argc, char** argv)
{
    if(argc != 2)
    {
        return -1;
    }
    std::stringstream stream;
    int64_t time_point;
    stream << std::string(argv[1]);
    stream >> time_point;


    std::cout << "time point: " << time_point << std::endl;
    std::string result = "";
    int64_t time_point2 = 0;
    TimeUtils::TimeMstoStringMs(time_point, result);
    std::cout << "result: " << result << std::endl;
    TimeUtils::StringMstoTimMs(result, time_point2);
    std::cout << "point: " << time_point2 << std::endl;

    return 0;
}
