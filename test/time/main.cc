#include <string.h>
#include <time.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include "TimeUtils.h"

int main(int argc, char **argv) {
  // if (argc != 2) {
    // return -1;
  // }
  // std::string result(argv[1]);
  int64_t cur_tms = TimeUtils::getEpochTimeMs();
  std::cout << cur_tms<< std::endl;
  std::string result;
  TimeUtils::TimeMstoStringMs(cur_tms, result, "%Y-%m-%d-%H:%M:%S");
  int64_t time_point2 = 0;
  std::cout << result << std::endl;
  TimeUtils::StringMstoTimMs(result, time_point2, "%Y-%m-%d-%H:%M:%S");
  std::cout << time_point2 << std::endl;
  /*
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
*/

  return 0;
}
