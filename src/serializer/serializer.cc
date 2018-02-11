//
// Copyright 2016 Horizon Robotics.
//

#define MODULE_TAG "_serializer"
#include <algorithm>
#include <string>
#include <vector>

#include "adas_frame/adas_output.pb.h"
#include "dms_frame/DMSFrame.pb.h"
#include "logging/DMSLog.h"
#include "serializer/serializer.h"
namespace HobotNebula {
// de-serialize
MetaDeserializer::MetaDeserializer() {}
MetaDeserializer::~MetaDeserializer() {}
DMS_MetaDeserializer::DMS_MetaDeserializer()
    : MetaDeserializer(), m_dms_fproto_deserialize(nullptr) {}
DMS_MetaDeserializer::~DMS_MetaDeserializer() { Finit(); }
ADAS_MetaDeserializer::ADAS_MetaDeserializer()
    : MetaDeserializer(), m_adas_fproto_deserialize(nullptr) {}
ADAS_MetaDeserializer::~ADAS_MetaDeserializer() { Finit(); }

bool DMS_MetaDeserializer::Deserialize(uint8_t *buf, size_t buf_size) {
  if (!m_dms_fproto_deserialize)
    return false;
  LOGV_T(MODULE_TAG) << "MetaDeserializer::Deserialize get one frame before";
  m_dms_fproto_deserialize->ParseFromArray(buf, buf_size);
  LOGV_T(MODULE_TAG) << "MetaDeserializer::Deserialize get one frame after";
  return true;
};

bool DMS_MetaDeserializer::Init() {
  if (this->m_dms_fproto_deserialize == nullptr) {
    this->m_dms_fproto_deserialize = new DMSFrameProtocol::DMSFrame();
  }
  return true;
};

void DMS_MetaDeserializer::Finit() {
  if (!m_dms_fproto_deserialize) {
    delete m_dms_fproto_deserialize;
    m_dms_fproto_deserialize = nullptr;
  }
};
bool DMS_MetaDeserializer::getTime(int64_t &frame_id_, int64_t &timestamp_) {
  if (m_dms_fproto_deserialize == nullptr) {
    LOGI_T(MODULE_TAG)
        << "m_dms_fproto_deserizlize == nullptr || frame == nullptr";
    frame_id_ = -1;
    timestamp_ = -1;
    return false;
  }

  if (m_dms_fproto_deserialize->has_frame_id()) {
    frame_id_ = m_dms_fproto_deserialize->frame_id();
    // frame_id_ = m_fproto_deserialize->frame_id();
    LOGV_T(MODULE_TAG) << "get frame_id_: " << frame_id_;
  }
  // write img info
  LOGV_T(MODULE_TAG) << "Write img infor";
  if (m_dms_fproto_deserialize->has_image()) {
    timestamp_ = m_dms_fproto_deserialize->image().time();
  }

  return true;
};
bool ADAS_MetaDeserializer::Deserialize(uint8_t *buf, size_t buf_size) {
  if (!m_adas_fproto_deserialize)
    return false;
  LOGV_T(MODULE_TAG) << "MetaDeserializer::Deserialize get one frame before";
  m_adas_fproto_deserialize->ParseFromArray(buf, buf_size);
  LOGV_T(MODULE_TAG) << "MetaDeserializer::Deserialize get one frame after";
  return true;
};

bool ADAS_MetaDeserializer::Init() {
  if (this->m_adas_fproto_deserialize == nullptr) {
    this->m_adas_fproto_deserialize = new AdasOutputProtocol::AdasOutput();
  }
  return true;
};
void ADAS_MetaDeserializer::Finit() {
  if (!m_adas_fproto_deserialize) {
    delete m_adas_fproto_deserialize;
    m_adas_fproto_deserialize = nullptr;
  }
};

bool ADAS_MetaDeserializer::getTime(int64_t &frame_id_, int64_t &timestamp_) {
  if (m_adas_fproto_deserialize == nullptr) {
    LOGI_T(MODULE_TAG)
        << "m_adas_fproto_deserizlize == nullptr || frame == nullptr";
    frame_id_ = -1;
    timestamp_ = -1;
    return false;
  }
  frame_id_ = m_adas_fproto_deserialize->frame_id();
  LOGV_T(MODULE_TAG) << "get frame_id_: " << frame_id_;

  LOGV_T(MODULE_TAG) << "Write img infor";
  if (m_adas_fproto_deserialize->has_image()) {
    timestamp_ = m_adas_fproto_deserialize->image().time_stamp();
  }

  return true;
};
}  // namespace HobotNebula
