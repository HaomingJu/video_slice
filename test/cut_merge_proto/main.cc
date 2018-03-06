#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "logging/DMSLog.h"
#include "protobuf_ops/protobuf_ops.h"

int main(int argc, char **argv) {
  if (argc < 5 || ((argc - 2) % 3 != 0))
    return -1;
  SetLogLevel(HOBOT_LOG_DEBUG);
  int node_len = (argc - 2) / 3;
  std::vector<HobotNebula::CutNodeProto> nodes;
  int i;
  for (i = 0; i < node_len; i++) {
    HobotNebula::CutNodeProto node;
    std::string str_start_ms(argv[1 + i * 3]);
    std::istringstream iss(str_start_ms);
    iss >> node.start_ts;

    std::string str_end_ms(argv[2 + i * 3]);
    std::istringstream iss2(str_end_ms);
    iss2 >> node.end_ts;

    node.filename = std::string(argv[3 + i * 3]);
    nodes.push_back(node);
  }
  const std::string out_filename(argv[argc - 1]);
  return HobotNebula::DMS_cut_merge_proto(nodes, out_filename);
  // return HobotNebula::ADAS_cut_merge_proto( nodes, out_filename);
}
