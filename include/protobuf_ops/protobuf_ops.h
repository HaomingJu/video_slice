#ifndef SRC_MODULES_INCLUDE_HOBOT_DMS_MODULES_UTILS_PROTOBUF_OPS_H_
#define SRC_MODULES_INCLUDE_HOBOT_DMS_MODULES_UTILS_PROTOBUF_OPS_H_

#include <stdint.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "serializer/serializer.h"

namespace HobotDMS {
#define VERSION 2017121901
typedef struct ProtoInfo {
  size_t prtPos;
  size_t prtLen;
} ProtoInfo_t;

class ProtoReader {
 public:
  int startReader(const std::string &filename);
  int getPos() { return mInput_cnt; }
  int getFrameCnt(void);
  void stopReader();

  uint32_t getVersion();
  int readOne(int64_t &frame_id_, int64_t &timestamp_);
  int readOneRaw(std::vector<char> &proto_raw, int64_t &timestamp);
  int seekByPos(int pos);
  int seekByTime(int64_t timestamp);

  inline size_t GetFileSize(std::ifstream &ifs) {
    // 保留原始位置
    size_t cur_pos = ifs.tellg();
    ifs.seekg(0, ifs.end);
    size_t size = ifs.tellg();
    ifs.seekg(cur_pos);
    return size;
  }

 private:
  MetaDeserializer *m_deserializer;
  std::string mFileName;
  std::ifstream mIfs;
  std::vector<char> mProtoBufVec;
  std::vector<ProtoInfo_t> mProtoInfoVec;
  int mInput_cnt;
  uint32_t mVersion;
};

class ProtoWriter {
 public:
  int startWriter(const std::string &filename);
  int writeVersion(uint32_t version);
  int writeRaw(const std::vector<char> &proto_raw);
  void stopWriter();

 private:
  std::string mFileName;
  std::ofstream mOfs;
};

typedef struct CutNodeProto_ {
  int64_t start_ts;
  int64_t end_ts;
  std::string filename;
} CutNodeProto;

class ProtoOps {
 public:
  ProtoOps();
  ~ProtoOps();
  int cut_merge_proto(std::vector<CutNodeProto> nodes,
                      const std::string out_file);

 private:
  std::unique_ptr<ProtoReader> proto_reader_;
  std::unique_ptr<ProtoWriter> proto_writer_;
};
}  // namespace HobotDMS

#endif  // SRC_MODULES_INCLUDE_HOBOT_DMS_MODULES_UTILS_PROTOBUF_OPS_H_
