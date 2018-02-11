//
// CopyRight 2017 Horizon Robotics
//
#define MODULE_TAG "_ProtobufOps"
#include "protobuf_ops/protobuf_ops.h"
#include "logging/DMSLog.h"

namespace HobotNebula {

ProtoReader::ProtoReader() : mVersion(0), mInput_cnt(0) {}

DMS_ProtoReader::DMS_ProtoReader()
    : ProtoReader(), m_dms_deserializer(new DMS_MetaDeserializer) {
  m_dms_deserializer->Init();
};
ADAS_ProtoReader::ADAS_ProtoReader()
    : ProtoReader(), m_adas_deserializer(new ADAS_MetaDeserializer) {
  m_adas_deserializer->Init();
};

bool DMS_ProtoReader::getTime(uint8_t *data_, int len_, int64_t &frame_id_,
                              int64_t &timestamp_) {
  return m_dms_deserializer->Deserialize(data_, len_) &&
         m_dms_deserializer->getTime(frame_id_, timestamp_);
};

bool ADAS_ProtoReader::getTime(uint8_t *data_, int len_, int64_t &frame_id_,
                               int64_t &timestamp_) {
  return m_adas_deserializer->Deserialize(data_, len_) &&
         m_adas_deserializer->getTime(frame_id_, timestamp_);
};

void DMS_ProtoReader::bin2dec(uint8_t *bin_, int &value) {
  value = (bin_[0] << 24) & 0xFF000000;
  value |= (bin_[1] << 16) & 0x00FF0000;
  value |= (bin_[2] << 8) & 0x0000FF00;
  value |= (bin_[3]) & 0x000000FF;
};
void ADAS_ProtoReader::bin2dec(uint8_t *bin_, int &value) {
  value = (bin_[3] << 24) & 0xFF000000;
  value |= (bin_[2] << 16) & 0x00FF0000;
  value |= (bin_[1] << 8) & 0x0000FF00;
  value |= (bin_[0]) & 0x000000FF;
};

ProtoReader::~ProtoReader() { this->stopReader(); }

int ProtoReader::startReader(const std::string &filename) {
  mFileName = filename;
  mInput_cnt = 0;
  /// 1. Open File and Get file size
  mIfs.open(mFileName.c_str(), std::ofstream::binary);
  if (mIfs.is_open() == false)
    return -1;
  size_t fileSize = GetFileSize(mIfs);
  size_t max_proto_len = 0;

  /// 2. Read Version
  uint8_t tran_ver[4];
  mIfs.read((char *)tran_ver, 4);
  bin2dec(tran_ver, mVersion);

  size_t cur_pos = 4;  // current position in ifstream
  ProtoInfo_t protoInfo;
  mProtoInfoVec.clear();
  mProtoBufVec.clear();
  LOGI_T(MODULE_TAG) << "proto's VERSION: " << mVersion;

  // [version, [ProtoLen, ProtoData], [ProtoLen, ProtoData], ...]
  while (cur_pos < fileSize) {
    int proto_len = 0;
    uint8_t temp_len[4];
    mIfs.read((char *)temp_len, 4);
    bin2dec(temp_len, proto_len);
    mIfs.seekg(proto_len, mIfs.cur);

    protoInfo.prtLen = proto_len;
    protoInfo.prtPos = cur_pos + 4;
    // TODO read timestamp into a RB-tree to speed up

    mProtoInfoVec.push_back(protoInfo);
    cur_pos += 4 + proto_len;
    if (proto_len > max_proto_len)
      max_proto_len = proto_len;
  }
  LOGI_T(MODULE_TAG) << "max_proto_len = " << max_proto_len;
  if (mProtoBufVec.capacity() < max_proto_len) {
    mProtoBufVec.reserve(max_proto_len + 8);
  }

  return 0;
}

uint32_t ProtoReader::getVersion() { return mVersion; }

int ProtoReader::readOne(int64_t &frame_id_, int64_t &timestamp_) {
  if (mInput_cnt < 0 ||
      static_cast<unsigned int>(mInput_cnt) >= mProtoInfoVec.size()) {
    LOGI_T(MODULE_TAG) << "mInput_cnt = " << mInput_cnt;
    LOGI_T(MODULE_TAG) << "mProtoInfoVec.size = " << mProtoInfoVec.size();
    return -1;
  }
  LOGV_T(MODULE_TAG) << "read from input_cnt = " << mInput_cnt;
  ProtoInfo_t &frame_info = mProtoInfoVec[mInput_cnt];
  mIfs.seekg((int)frame_info.prtPos);
  mIfs.read(&mProtoBufVec[0], frame_info.prtLen);
  LOGV_T(MODULE_TAG) << "read from file frame_info.prtLen = "
                     << frame_info.prtLen;
  if (this->getTime((uint8_t *)(&mProtoBufVec[0]), frame_info.prtLen, frame_id_,
                    timestamp_)) {
    mInput_cnt++;
    return 0;
  }
  return -1;
}

int ProtoReader::readOneRaw(std::vector<char> &proto_raw, int64_t &timestamp_) {
  if (mInput_cnt < 0 ||
      static_cast<unsigned int>(mInput_cnt) >= mProtoInfoVec.size()) {
    LOGI_T(MODULE_TAG) << "mInput_cnt = " << mInput_cnt;
    LOGI_T(MODULE_TAG) << "mProtoInfoVec.size = " << mProtoInfoVec.size();
    return -2;
  }
  ProtoInfo_t &frame_info = mProtoInfoVec[mInput_cnt];
  proto_raw.resize(frame_info.prtLen);
  mIfs.seekg((int)frame_info.prtPos);
  mIfs.read(&proto_raw[0], frame_info.prtLen);
  LOGV_T(MODULE_TAG) << "read from file frame_info.prtLen = "
                     << frame_info.prtLen;
  int64_t frame_id;
  if (this->getTime((uint8_t *)(&proto_raw[0]), frame_info.prtLen, frame_id,
                    timestamp_)) {
    mInput_cnt++;
    return 0;
  }
  return -1;
}

int ProtoReader::seekByPos(int pos) {
  // LOGI_T(MODULE_TAG) << "do seek:  " << pos;
  mInput_cnt = pos;
  return 0;
}

int ProtoReader::seekByTime(int64_t timestamp_) {
  // TODO use RB-tree to do seek
  this->seekByPos(0);
  int64_t frame_id = 0, timestamp = 0;
  while (mInput_cnt < mProtoInfoVec.size()) {
    this->readOne(frame_id, timestamp);
    LOGV_T(MODULE_TAG) << "seek timestamp = " << timestamp;
    LOGV_T(MODULE_TAG) << "seek frame_id= " << frame_id;
    if (timestamp >= timestamp_) {
      mInput_cnt--;
      return 0;
    }
  }
  return -1;
}

int ProtoReader::getFrameCnt(void) { return mProtoInfoVec.size(); }

void ProtoReader::stopReader() {
  mProtoInfoVec.clear();
  if (mIfs.is_open())
    mIfs.close();
}

///////////////////////////////////////////////////////////////////////////////

ProtoWriter::ProtoWriter() {}

ProtoWriter::~ProtoWriter() {}

int ProtoWriter::startWriter(const std::string &filename) {
  mFileName = filename;
  mOfs.open(mFileName.c_str(), std::ofstream::binary);
  if (!mOfs.is_open())
    return -1;
  return 0;
}

int ProtoWriter::writeVersion(uint32_t version) {
  /// Write Version info
  if (mOfs.is_open()) {
    uint8_t tran_ver[4];
    int tmp_version = static_cast<int>(version);
    dec2bin(tmp_version, tran_ver);
    mOfs.write((char *)tran_ver, 4);
    return 0;
  }
  return -1;
};
void DMS_ProtoWriter::dec2bin(int value_, uint8_t *bin_) {
  bin_[3] = value_ & 0x00FF;
  bin_[2] = (value_ >> 8) & 0x00FF;
  bin_[1] = (value_ >> 16) & 0x00FF;
  bin_[0] = (value_ >> 24) & 0x00FF;
};
void ADAS_ProtoWriter::dec2bin(int value_, uint8_t *bin_) {
  bin_[0] = value_ & 0x00FF;
  bin_[1] = (value_ >> 8) & 0x00FF;
  bin_[2] = (value_ >> 16) & 0x00FF;
  bin_[3] = (value_ >> 24) & 0x00FF;
};

int ProtoWriter::writeRaw(const std::vector<char> &proto_raw) {
  uint8_t temp_len[4];
  int buf_size = proto_raw.size();
  dec2bin(buf_size, temp_len);
  mOfs.write((char *)temp_len, 4);
  if (!mOfs.good())
    return -1;
  mOfs.write(&proto_raw[0], proto_raw.size());
  if (!mOfs.good())
    return -1;
  return 0;
};

void ProtoWriter::stopWriter() {
  if (mOfs.is_open())
    mOfs.close();
};

///////////////////////////////////////////////////////////////////////////////

int cut_merge_proto(const std::vector<CutNodeProto> &nodes,
                    std::unique_ptr<ProtoReader> &reader_,
                    std::unique_ptr<ProtoWriter> &writer_,
                    const std::string &out_file_) {
  int ret = reader_->startReader(nodes[0].filename);
  if (ret)
    return ret;
  uint32_t ver = reader_->getVersion();
  LOGI_T(MODULE_TAG) << "version = " << ver;
  reader_->stopReader();
  ret = writer_->startWriter(out_file_);
  if (ret)
    return ret;
  ret = writer_->writeVersion(ver);
  if (ret) {
    writer_->stopWriter();
    return ret;
  }

  std::vector<char> proto_raw;
  int64_t timestamp = 0;
  for (auto iter = nodes.cbegin(); iter != nodes.cend(); iter++) {
    LOGI_T(MODULE_TAG) << "  filename = " << iter->filename;
    LOGI_T(MODULE_TAG) << "  start_ts = " << iter->start_ts;
    ret = reader_->startReader(iter->filename);
    if (ret)
      return ret;
    ret = reader_->seekByTime(iter->start_ts);
    if (ret)
      return ret;
    while (1) {
      ret = reader_->readOneRaw(proto_raw, timestamp);
      if (ret == -2)
        break;
      else if (ret)
        return ret;
      if (timestamp > iter->end_ts)
        break;
      LOGI_T(MODULE_TAG) << "  cur_ts = " << timestamp;
      ret = writer_->writeRaw(proto_raw);
      if (ret)
        return ret;
    }
    reader_->stopReader();
  }
  // TODO err handle
  return 0;
};

int ADAS_cut_merge_proto(std::vector<CutNodeProto> nodes,
                         const std::string out_file) {
  if (nodes.empty())
    return -1;
  std::unique_ptr<ProtoReader> proto_reader_(new ADAS_ProtoReader());
  std::unique_ptr<ProtoWriter> proto_writer_(new ADAS_ProtoWriter());
  return cut_merge_proto(nodes, proto_reader_, proto_writer_, out_file);
};

int DMS_cut_merge_proto(std::vector<CutNodeProto> nodes,
                        const std::string out_file) {
  if (nodes.empty())
    return -1;
  std::unique_ptr<ProtoReader> proto_reader_(new DMS_ProtoReader());
  std::unique_ptr<ProtoWriter> proto_writer_(new DMS_ProtoWriter());
  return cut_merge_proto(nodes, proto_reader_, proto_writer_, out_file);
}
}
