//
// Copyright 2016 Horizon Robotics.
//
#ifndef SRC_HOBOT_DMS_UTILS_SERIALIZE_SERIALIZER_H_
#define SRC_HOBOT_DMS_UTILS_SERIALIZE_SERIALIZER_H_

#include <stdint.h>
#include <fstream>
#include <iostream>
#include "DMSFrame.pb.h"


namespace HobotDMS
{
class MetaDeserializer
{
  public:
    MetaDeserializer();
    ~MetaDeserializer();

    bool Init();
    void Fini();

    bool Deserialize(std::ifstream &ifs, size_t pos, size_t meta_len);
    bool Deserialize(uint8_t *buf, size_t buf_size);
    bool getTime(int64_t &frame_id_, int64_t &timestamp_);

    DMSFrameProtocol::DMSFrame *m_fproto_deserialize = nullptr;

  private:
};

}  // end of namespace HobotDMS

#endif  // SRC_HOBOT_DMS_UTILS_SERIALIZE_SERIALIZER_H_
