#include <iostream>
#include <sstream>
#include <string>
#include "logging/DMSLog.h"
#include "slice_video_proto.h"

int main(int argc, char **argv) {
  std::cout << "argc = " << argc << std::endl;
  SetLogLevel(HOBOT_LOG_DEBUG);
  if (argc != 4)
    return -1;
  std::string flag(argv[1]);
  int64_t start_ms, dura_ms;
  std::istringstream iss((std::string(argv[2])));
  iss >> start_ms;
  std::istringstream iss2((std::string(argv[3])));
  iss2 >> dura_ms;
  char result_file[1024];
  int64_t ret_time = -1;
  int ret = 0;

  std::cout << "begin." << std::endl;
  HobotNebula::Slice *slice = nullptr;
  if (flag == "DMS") {
    slice = new HobotNebula::DMS_Slice();
    ret = slice->Init("DMS", "../etc/dataslice.json");
    std::cout << "ret = " << std::endl;
    ret_time = slice->Cut(start_ms, dura_ms, result_file, 1024);
  } else if (flag == "ADAS") {
    slice = new HobotNebula::ADAS_Slice();
    ret = slice->Init("ADAS", "../etc/dataslice.json");
    std::cout << "ret = " << std::endl;
    ret_time = slice->Cut(start_ms, dura_ms, result_file, 1024);
  } else {
    return -1;
  }
  std::cout << "result: " << result_file << std::endl;
  std::cout << "ret_time: " << ret_time << std::endl;
  std::cout << "end." << std::endl;

  return 0;
}
