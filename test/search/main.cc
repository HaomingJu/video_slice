#include <iostream>
#include <string>
#include "hobotlog/hobotlog.hpp"
#include "searchMP4.h"
#include "video_utils/video_utils.h"

using namespace HobotNebula;
using namespace std;

int main(int argc, char **argv) {
  HobotNebula::SearchMP4 search_mp4;
  std::vector<CutNode> node;
  std::string search_path = "/home/haoming/Desktop";
  search_mp4.addSearchPath(search_path);

  search_mp4.getMP4Path(1262312750319, 10000, node);

  for (int i = 0; i < node.size(); ++i) {
    std::cout << "name: " << node[i].filename << std::endl
              << "start_timestamp: " << node[i].start_seconds << std::endl
              << "end_timestamp: " << node[i].end_seconds << std::endl;
  }

  cut_merge_video((CutNode *)(&(node[0])), node.size(),
                  "/home/haoming/Desktop/result.mp4");

  /*
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
  */

  return 0;
}
