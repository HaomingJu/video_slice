#include <iostream>
#include <string>
#include "hobotlog/hobotlog.hpp"
#include "searchMP4.h"
#include "video_utils/video_utils.h"

using namespace HobotDMS;
using namespace std;

int main(int argc, char **argv) {
  SetLogLevel(HOBOT_LOG_DEBUG);
  SearchMP4 search;
  std::string search_path = "/home/haoming/Desktop";

  search.addSearchPath(search_path);
  std::vector<CutNode> node;
  int len;
  int64_t s = 1262312750319;
  int64_t s_len = 30;
  std::cout << "input timestamp_1 = " << s << std::endl
            << "input len = " << s_len << std::endl
            << "input timestamp_2 = " << s + s_len << std::endl;

  search.getMP4Path(s, s_len, node);

  std::cout << "len = " << len << std::endl;
  std::cout << "filename = " << node[0].filename << std::endl;
  std::cout << "start_timestamp = " << node[0].start_seconds << std::endl;
  std::cout << "end_timestamp = " << node[0].end_seconds << std::endl;

  return 0;
}
