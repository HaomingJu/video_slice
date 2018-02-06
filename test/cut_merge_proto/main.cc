#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "protobuf_ops/protobuf_ops.h"
#include "logging/DMSLog.h"

int main(int argc, char **argv) {
  if (argc < 5 || ((argc - 2) % 3 != 0))
    return -1;
  SetLogLevel(HOBOT_LOG_DEBUG);
  int node_len = (argc - 2) / 3;
  std::vector<HobotNebula::CutNodeProto> nodes;
  int i;
  for (i = 0; i < node_len; i++) {
    HobotNebula::CutNodeProto node;
    std::istringstream iss(std::string(argv[1 + i * 3]));
    iss >> node.start_ts;
    std::istringstream iss2(std::string(argv[2 + i * 3]));
    iss2 >> node.end_ts;

    node.filename = std::string(argv[3 + i * 3]);
    nodes.push_back(node);
  }
  const std::string out_filename(argv[argc - 1]);
  return HobotNebula::cut_merge_proto(nodes, out_filename);
}
