#include <iostream>
#include <sstream>
#include <string>
#include "logging/DMSLog.h"
#include "slice_video_proto.h"

int main(int argc, char **argv) {
  SetLogLevel(HOBOT_LOG_DEBUG);
  if (argc != 3)
    return -1;
  HobotNebula::DMS_Slice slice;
  int64_t start_ms, dura_ms;
  std::istringstream iss((std::string(argv[1])));
  iss >> start_ms;
  std::istringstream iss2((std::string(argv[2])));
  iss2 >> dura_ms;

  std::string result_file;
  slice.Cut(start_ms, dura_ms, result_file);
  std::cout << "result: " << result_file << std::endl;
  return 0;
}
