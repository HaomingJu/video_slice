#ifndef SRC_MODULES_INCLUDE_HOBOT_DMS_MODULES_UTILS_PROTOBUF_OPS_H_
#define SRC_MODULES_INCLUDE_HOBOT_DMS_MODULES_UTILS_PROTOBUF_OPS_H_

#include <stdint.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "serializer/serializer.h"

namespace HobotNebula {
//#define DMS_VERSION 2017121901
typedef struct ProtoInfo {
  size_t prtPos;
  size_t prtLen;
} ProtoInfo_t;

class ProtoReader {
 public:
  ProtoReader();
  ~ProtoReader();
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

 protected:
  std::string mFileName;
  std::ifstream mIfs;
  std::vector<char> mProtoBufVec;
  std::vector<ProtoInfo_t> mProtoInfoVec;
  int mInput_cnt;
  int mVersion;
  virtual void bin2dec(uint8_t *, int &) = 0;
  virtual bool getTime(uint8_t *data_, int len_, int64_t &frame_id_,
                       int64_t &timestamp_) = 0;
};

class DMS_ProtoReader : public ProtoReader {
 public:
  DMS_ProtoReader();

 protected:
  void bin2dec(uint8_t *, int &);
  std::unique_ptr<DMS_MetaDeserializer> m_dms_deserializer;
  bool getTime(uint8_t *data_, int len_, int64_t &frame_id_,
               int64_t &timestamp_);
};

class ADAS_ProtoReader : public ProtoReader {
 public:
  ADAS_ProtoReader();

 protected:
  void bin2dec(uint8_t *, int &);
  std::unique_ptr<ADAS_MetaDeserializer> m_adas_deserializer;
  bool getTime(uint8_t *data_, int len_, int64_t &frame_id_,
               int64_t &timestamp_);
};

class ProtoWriter {
 public:
  ProtoWriter();
  ~ProtoWriter();
  int startWriter(const std::string &filename);
  int writeVersion(uint32_t version);
  int writeRaw(const std::vector<char> &proto_raw);
  void stopWriter();

 protected:
  std::string mFileName;
  std::ofstream mOfs;
  virtual void dec2bin(int, uint8_t *) = 0;
};

class DMS_ProtoWriter : public ProtoWriter {
 protected:
  void dec2bin(int, uint8_t *);
};
class ADAS_ProtoWriter : public ProtoWriter {
 protected:
  void dec2bin(int, uint8_t *);
};

typedef struct CutNodeProto_ {
  int64_t start_ts;
  int64_t end_ts;
  std::string filename;
} CutNodeProto;

int DMS_cut_merge_proto(std::vector<CutNodeProto> nodes,
                        const std::string out_file);
int ADAS_cut_merge_proto(std::vector<CutNodeProto> nodes,
                         const std::string out_file);

}  // namespace HobotNebula

#endif  // SRC_MODULES_INCLUDE_HOBOT_DMS_MODULES_UTILS_PROTOBUF_OPS_H_
