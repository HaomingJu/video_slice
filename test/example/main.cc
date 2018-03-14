#include <iostream>
#include <sstream>
#include <string>
#include "logging/DMSLog.h"
#include "slice_video_proto.h"

int main(int argc, char **argv) {
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

  int ret_time = -1;
  if (flag == "DMS") {
    HobotNebula::DMS_Slice *slice = new HobotNebula::DMS_Slice();
    slice->Init("../etc/dataslice.json");
    ret_time = slice->Cut(start_ms, dura_ms, result_file, 1024);
  } else if (flag == "ADAS") {
    HobotNebula::ADAS_Slice *slice = new HobotNebula::ADAS_Slice();
    slice->Init("../etc/dataslice.json");
    ret_time = slice->Cut(start_ms, dura_ms, result_file, 1024);
  } else {
    return -1;
  }
  std::cout << "result: " << result_file << std::endl;
  std::cout << "ret_time: " << ret_time << std::endl;

  return 0;
}
