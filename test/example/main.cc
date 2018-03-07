#include <iostream>
#include <sstream>
#include <string>
#include "logging/DMSLog.h"
#include "slice_video_proto.h"

// ./example DMS 1123234 100000
int main(int argc, char **argv) {
  SetLogLevel(HOBOT_LOG_DEBUG);
  if (argc != 4)
    return -1;
  HobotNebula::Slice *slice = nullptr;
  std::string flag(argv[1]);
  if (flag == "DMS") {
    slice = new HobotNebula::DMS_Slice();
  } else if (flag == "ADAS") {
    slice = new HobotNebula::ADAS_Slice();
  } else {
    return -1;
  }
  int64_t start_ms, dura_ms;
  std::istringstream iss((std::string(argv[2])));
  iss >> start_ms;
  std::istringstream iss2((std::string(argv[3])));
  iss2 >> dura_ms;

  std::string result_file;
  slice->Cut(start_ms, dura_ms, result_file);
  std::cout << "result: " << result_file << std::endl;
  return 0;
}
