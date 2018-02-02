#include <iostream>
#include "hobotlog/hobotlog.hpp"
#include "protobuf_ops/protobuf_ops.h"

using namespace std;

int main(int argc, char **argv) {
  SetLogLevel(HOBOT_LOG_DEBUG);
  if (argc != 2) {
    std::cout << "./unit_test ./data.proto" << std::endl;
    return -1;
  }

  std::string data_path(argv[1]);
  HobotDMS::DMSProtoReader *proto_reader_ptr = new HobotDMS::DMSProtoReader();
  if (proto_reader_ptr) {
    proto_reader_ptr->startReader(data_path);
    int index = 0;
    int64_t frame_id = 0;
    int64_t timestamp = 0;
    std::cout << "frame Count: " << proto_reader_ptr->getFrameCnt()
              << std::endl;
    while (true) {
      std::cout << "get a index" << std::endl;
      std::cin >> index;
      proto_reader_ptr->seekTo(index);
      proto_reader_ptr->readOne(frame_id, timestamp);
      std::cout << "frame_id: " << frame_id << std::endl
                << "timestamp: " << timestamp << std::endl;
    }
  }

  return 0;
}
