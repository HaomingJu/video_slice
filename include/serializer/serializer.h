//
// Copyright 2016 Horizon Robotics.
//
#ifndef SRC_HOBOT_DMS_UTILS_SERIALIZE_SERIALIZER_H_
#define SRC_HOBOT_DMS_UTILS_SERIALIZE_SERIALIZER_H_

#include <google/protobuf/message.h>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include "DMSFrame.pb.h"
namespace DMSFrameProtocol {
class DMSFrame;
}
namespace AdasOutputProtocol {
class AdasOutput;
}
namespace HobotNebula {
class MetaDeserializer {
 public:
  MetaDeserializer();
  ~MetaDeserializer();

  virtual bool Deserialize(uint8_t *buf, size_t buf_size) = 0;
  virtual bool Init() = 0;
  virtual void Finit() = 0;
  virtual bool getTime(int64_t &frame_id_, int64_t &timestamp_) = 0;
  // protected:
  //::google::protobuf::Message *m_fproto_deserialize = nullptr;
};

class DMS_MetaDeserializer : public MetaDeserializer {
 public:
  DMS_MetaDeserializer();
  ~DMS_MetaDeserializer();
  bool Deserialize(uint8_t *buf, size_t buf_size);
  bool Init();
  void Finit();
  bool getTime(int64_t &frame_id_, int64_t &timestamp_);

 private:
  DMSFrameProtocol::DMSFrame *m_dms_fproto_deserialize;
};

class ADAS_MetaDeserializer : public MetaDeserializer {
 public:
  ADAS_MetaDeserializer();
  ~ADAS_MetaDeserializer();
  bool Deserialize(uint8_t *buf, size_t buf_size);
  bool Init();
  void Finit();
  bool getTime(int64_t &frame_id_, int64_t &timestamp_);

 private:
  AdasOutputProtocol::AdasOutput *m_adas_fproto_deserialize;
};
}  // end of namespace HobotNebula

#endif  // SRC_HOBOT_DMS_UTILS_SERIALIZE_SERIALIZER_H_
