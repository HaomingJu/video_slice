#ifndef SRC_MODULES_INCLUDE_HOBOT_DMS_MODULES_UTILS_PROTOBUF_OPS_H_
#define SRC_MODULES_INCLUDE_HOBOT_DMS_MODULES_UTILS_PROTOBUF_OPS_H_

#include <stdint.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "serializer/serializer.h"

namespace HobotDMS
{
#define VERSION 2017121901


/// class DMSProtoReader
class DMSProtoReader
{
    typedef struct ProtoInfo
    {
        size_t prtPos;
        size_t prtLen;
    } ProtoInfo_t;

  public:
    int startReader(const std::string &filename);
    int getPos()
    {
        return mInput_cnt;
    }
    int readOne(int64_t& frame_id_, int64_t& timestamp_);
    int seekTo(int pos);
    int getFrameCnt(void);
    void stopReader();

  private:
    MetaDeserializer *m_deserializer = nullptr;
    inline size_t GetFileSize(std::ifstream &ifs)
    {
        // 保留原始位置
        size_t cur_pos = ifs.tellg();
        ifs.seekg(0, ifs.end);
        size_t size = ifs.tellg();
        ifs.seekg(cur_pos);
        return size;
    }
    std::string mFileName;
    std::ifstream mIfs;
    std::vector<char> mProtoBufVec;
    std::vector<ProtoInfo_t> mProtoInfoVec;
    int mInput_cnt;
};

}  // namespace HobotDMS

#endif  // SRC_MODULES_INCLUDE_HOBOT_DMS_MODULES_UTILS_PROTOBUF_OPS_H_
