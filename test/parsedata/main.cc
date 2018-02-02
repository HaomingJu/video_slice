#include <iostream>
#include "hobotlog/hobotlog.hpp"
#include "protobuf_ops/protobuf_ops.h"

using namespace std;

int main(int argc, char **argv) {
  SetLogLevel(HOBOT_LOG_DEBUG);
  HobotDMS::ProtoOps::cutOneFile("./test.proto", 0, 10, "./result1.proto");
  HobotDMS::ProtoOps::cutOneFile("./test.proto", 0, 10, "./result2.proto");
  HobotDMS::ProtoOps::mergeTwoFile("./result1.proto", "./result2.proto",
                                   "result3.proto");

  HobotDMS::DMSProtoReader *proto_reader_ptr1 = new HobotDMS::DMSProtoReader();
  HobotDMS::DMSProtoReader *proto_reader_ptr2 = new HobotDMS::DMSProtoReader();
  proto_reader_ptr1->startReader("./result2.proto");
  proto_reader_ptr2->startReader("./result3.proto");

  std::cout << "jhm 2 frame count: " << proto_reader_ptr1->getFrameCnt()
            << std::endl;
  std::cout << "jhm 3 frame count: " << proto_reader_ptr2->getFrameCnt()
            << std::endl;
  ;
  int64_t frame_id, frame_id2, timestamp, timestamp2;
  for (int i = 0; i < proto_reader_ptr2->getFrameCnt(); ++i) {
    proto_reader_ptr1->seekTo(i);
    proto_reader_ptr2->seekTo(i);
    proto_reader_ptr1->readOne(frame_id, timestamp);
    proto_reader_ptr2->readOne(frame_id2, timestamp2);
    std::cout << "jhm frame_id: " << frame_id << std::endl
              << "jhm timestamp: " << timestamp << std::endl;
    std::cout << "jhm frame_id2: " << frame_id2 << std::endl
              << "jhm timestamp2: " << timestamp2 << std::endl;
  }

  return 0;
}
