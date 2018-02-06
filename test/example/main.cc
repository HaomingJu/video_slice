#include <iostream>
#include <string>
#include "hobotlog/hobotlog.hpp"
#include "protobuf_ops/protobuf_ops.h"
#include "searchMP4.h"
#include "time_utils/TimeUtils.h"
#include "video_utils/video_utils.h"

#include <sstream>
int64_t stoi_(const std::string &s) {
  std::stringstream stream;
  int64_t i;
  stream << s;
  stream >> i;
  return i;
}

using namespace HobotNebula;
using namespace std;

int main(int argc, char **argv) {
  int64_t start_ms = 0;
  int64_t dur_ms = 0;
  std::string search_path = "";

  if (argc != 4) {
    start_ms = 1262312750319;
    dur_ms = 10000;
    search_path = "/data/data/haoming/";
  } else {
    start_ms = stoi_(std::string(argv[1]));
    dur_ms = stoi_(std::string(argv[2]));
    search_path = std::string(argv[3]);
  }

  std::string mp4_path_name = search_path + "result.mp4";
  std::string proto_path_name = search_path + "result.proto";

  std::cout << "start_ms = " << start_ms << std::endl
      << "dur_ms = " << dur_ms << std::endl
      << "search_path = " << search_path << std::endl
      << "mp4_path_name = " << mp4_path_name << std::endl
      << "proto_path_name = " << proto_path_name << std::endl;


  SetLogLevel(HOBOT_LOG_DEBUG);
  HobotNebula::SearchMP4 search_mp4;
  std::vector<CutNode> node;
  search_mp4.addSearchPath(search_path);
  search_mp4.getMP4Path(start_ms, dur_ms, node);
  std::cout << "node size = " << node.size() << std::endl;
  for (int i = 0; i < node.size(); ++i)
  {
      std::cout << i << " : " << node[i].start_seconds << "\t" << node[i].end_seconds << "\t" <<node[i].filename << std::endl;
  }

  cut_merge_video((CutNode *)(&(node[0])), node.size(),
                  mp4_path_name.c_str());

  std::vector<CutNodeProto> node_proto;

  for (std::vector<CutNode>::iterator iter = node.begin(); iter != node.end();
       ++iter) {
    CutNodeProto tmp_node_proto;
    tmp_node_proto.filename = std::string(iter->filename);

    int find_index = tmp_node_proto.filename.find("_Nebula_1_5_");
    std::string result = tmp_node_proto.filename.substr(find_index + 12, 19);
    find_index = tmp_node_proto.filename.find(".mp4");
    tmp_node_proto.filename.replace(find_index, 4, ".proto");
    std::cout << "filename: " << tmp_node_proto.filename << std::endl;
    std::cout << "result: " << result << std::endl;
    int64_t time_base = 0;
    TimeUtils::StringMstoTimMs(result, time_base);

    std::cout << "time_base: " << time_base << std::endl;
    tmp_node_proto.start_ts = iter->start_seconds_refined * 1000 + time_base;
    tmp_node_proto.end_ts = iter->end_seconds_refined * 1000 + time_base;
    std::cout << "start_ts: " << tmp_node_proto.start_ts << std::endl;
    std::cout << "end_ts: " << tmp_node_proto.end_ts << std::endl;
    node_proto.push_back(tmp_node_proto);
  }
  cut_merge_proto(node_proto, proto_path_name);

  return 0;
}
