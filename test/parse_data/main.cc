#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include "protobuf_ops.h"

// ./parse_data DMS ./xxxx.proto
int main(int argc, char **argv) {
  if (argc != 3)
    return -1;
  std::string type_name(argv[1]);
  std::string pack_name(argv[2]);
  std::fstream result_file;
  std::string pack_name_ = pack_name;
  int pos = pack_name.find(".proto");
  std::string result_file_name = pack_name_.replace(pos, 6, ".txt");
  result_file.open(result_file_name.c_str(), std::ios::out);

  HobotNebula::ProtoReader *reader = nullptr;
  if (type_name == "DMS") {
    reader = new HobotNebula::DMS_ProtoReader;
  } else if (type_name == "ADAS") {
    reader = new HobotNebula::ADAS_ProtoReader;
  }
  reader->startReader(pack_name);
  std::cout << type_name << "proto frame count = " << reader->getFrameCnt()
            << std::endl;
  std::cout << type_name << "proto version = " << reader->getVersion()
            << std::endl;
  result_file << "proto frame count = " << reader->getFrameCnt() << std::endl;
  result_file << "proto version = " << reader->getVersion() << std::endl;

  int64_t frame_id_ = -1, timestamp_ = -1;
  reader->seekByPos(0);
  for (int i = 0; i < reader->getFrameCnt(); ++i) {
    reader->readOne(frame_id_, timestamp_);
    std::cout << i << " : " << timestamp_ << std::endl;
    result_file << i << " : " << timestamp_ << std::endl;
  }
  reader->stopReader();
  result_file.close();
  return 0;
}
