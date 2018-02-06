//
// CopyRight 2017 Horizon Robotics
//
#define MODULE_TAG "_ProtobufOps"
#include "protobuf_ops/protobuf_ops.h"
#include "logging/DMSLog.h"

namespace HobotDMS {

ProtoReader::ProtoReader()
    : mVersion(0), mInput_cnt(0), m_deserializer(new MetaDeserializer()) {}

ProtoReader::~ProtoReader() { this->stopReader(); }

int ProtoReader::startReader(const std::string &filename) {
  if (m_deserializer == nullptr)
    return -1;
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
  mVersion = (tran_ver[0] << 24) & 0xFF000000;
  mVersion |= (tran_ver[1] << 16) & 0x00FF0000;
  mVersion |= (tran_ver[2] << 8) & 0x0000FF00;
  mVersion |= (tran_ver[3]) & 0x000000FF;

  size_t cur_pos = 4;  // current position in ifstream
  ProtoInfo_t protoInfo;
  mProtoInfoVec.clear();
  mProtoBufVec.clear();
  LOGI_T(MODULE_TAG) << "project's VERSION: " << VERSION;
  LOGI_T(MODULE_TAG) << "proto's VERSION: " << mVersion;
  switch (mVersion) {
    // [version, [ProtoLen, ProtoData], [ProtoLen, ProtoData], ...]
    case VERSION:
      while (cur_pos < fileSize) {
        size_t proto_len = 0;
        char temp_len[4];
        mIfs.read(temp_len, 4);
        proto_len = (temp_len[0] << 24) & 0xFF000000;
        proto_len |= (temp_len[1] << 16) & 0x00FF0000;
        proto_len |= (temp_len[2] << 8) & 0x0000FF00;
        proto_len |= (temp_len[3]) & 0x000000FF;
        mIfs.seekg(proto_len, mIfs.cur);

        protoInfo.prtLen = proto_len;
        protoInfo.prtPos = cur_pos + 4;
        // TODO read timestamp into a RB-tree to speed up

        mProtoInfoVec.push_back(protoInfo);
        cur_pos += 4 + proto_len;
        if (proto_len > max_proto_len)
          max_proto_len = proto_len;
      }
      break;
    default: {
      LOGI_T(MODULE_TAG) << "VERSION ERROR";
      return -1;
      break;
    }
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
  if (m_deserializer->Deserialize((uint8_t *)(&mProtoBufVec[0]),
                                  frame_info.prtLen) &&
      m_deserializer->getTime(frame_id_, timestamp_)) {
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
  if (m_deserializer->Deserialize((uint8_t *)(&proto_raw[0]),
                                  frame_info.prtLen) &&
      m_deserializer->getTime(frame_id, timestamp_)) {
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
    unsigned char tran_ver[4];
    tran_ver[0] = version >> 24 & 0x00FF;
    tran_ver[1] = version >> 16 & 0x00FF;
    tran_ver[2] = version >> 8 & 0x00FF;
    tran_ver[3] = version >> 0 & 0x00FF;
    mOfs.write((char *)tran_ver, 4);
    return 0;
  }
  return -1;
};

int ProtoWriter::writeRaw(const std::vector<char> &proto_raw) {
  char temp_len[4];
  size_t buf_size = proto_raw.size();
  temp_len[3] = buf_size & 0x00FF;
  temp_len[2] = (buf_size >> 8) & 0x00FF;
  temp_len[1] = (buf_size >> 16) & 0x00FF;
  temp_len[0] = (buf_size >> 24) & 0x00FF;
  mOfs.write(temp_len, 4);
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
#if 0
int ProtoReader::cutOneFile(int64_t start_id, int64_t end_id,
                            const std::string &fname) {
  if ((start_id > end_id) || (start_id < 0) ||
      (end_id >= mProtoInfoVec.size())) {
    return -1;
  }

  std::ofstream Ofs;
  Ofs.open(fname.c_str(), std::ofstream::binary);
  if (!Ofs.is_open()) {
    return -1;
  }
  // Write VERSION
  mIfs.seekg(0);
  mIfs.read(&mProtoBufVec[0], 4);
  Ofs.write(&mProtoBufVec[0], 4);

  for (int i = start_id; i <= end_id; ++i) {
    ProtoInfo_t &frame_info = mProtoInfoVec[i];
    mIfs.seekg((int)frame_info.prtPos - 4);
    mIfs.read(&mProtoBufVec[0], frame_info.prtLen + 4);
    Ofs.write(&mProtoBufVec[0], frame_info.prtLen + 4);
  }

  if (Ofs.is_open())
    Ofs.close();

  return 0;
}
#endif

///////////////////////////////////////////////////////////////////////////////

int cut_merge_proto(std::vector<CutNodeProto> nodes,
                    const std::string out_file) {
  if (nodes.empty())
    return -1;
  std::unique_ptr<ProtoReader> proto_reader_(new ProtoReader());
  std::unique_ptr<ProtoWriter> proto_writer_(new ProtoWriter());
  // use first node's version
  int ret = proto_reader_->startReader(nodes[0].filename);
  if (ret)
    return ret;
  uint32_t ver = proto_reader_->getVersion();
  LOGI_T(MODULE_TAG) << "version = " << ver;
  proto_reader_->stopReader();
  ret = proto_writer_->startWriter(out_file);
  if (ret)
    return ret;
  ret = proto_writer_->writeVersion(ver);
  if (ret) {
    proto_writer_->stopWriter();
    return ret;
  }

  std::vector<char> proto_raw;
  int64_t timestamp = 0;
  for (auto iter = nodes.cbegin(); iter != nodes.cend(); iter++) {
    LOGI_T(MODULE_TAG) << "  filename = " << iter->filename;
    LOGI_T(MODULE_TAG) << "  start_ts = " << iter->start_ts;
    ret = proto_reader_->startReader(iter->filename);
    if (ret)
      return ret;
    ret = proto_reader_->seekByTime(iter->start_ts);
    if (ret)
      return ret;
    while (1) {
      ret = proto_reader_->readOneRaw(proto_raw, timestamp);
      if (ret == -2)
        break;
      else if (ret)
        return ret;
      if (timestamp > iter->end_ts)
        break;
      LOGI_T(MODULE_TAG) << "  cur_ts = " << timestamp;
      ret = proto_writer_->writeRaw(proto_raw);
      if (ret)
        return ret;
    }
    proto_reader_->stopReader();
  }
  // TODO err handle
  return 0;
}
}

#if 0
int ProtoOps::cutOneFile(const std::string &fname_src, int64_t start_id,
                         int64_t end_id, const std::string &fname_dst) {
  ProtoReader *proto_reader_ptr = new ProtoReader();
  proto_reader_ptr->startReader(fname_src);
  return proto_reader_ptr->cutOneFile(start_id, end_id, fname_dst);
  proto_reader_ptr->stopReader();
  delete proto_reader_ptr;
  proto_reader_ptr = nullptr;
}

int ProtoOps::mergeTwoFile(const std::string &fname_src1,
                           const std::string &fname_src2,
                           const std::string &fname_dst) {
  ProtoReader *proto_reader_ptr_1 = new ProtoReader();
  ProtoReader *proto_reader_ptr_2 = new ProtoReader();
  proto_reader_ptr_1->startReader(fname_src1);
  proto_reader_ptr_2->startReader(fname_src2);

  std::ofstream Ofs;
  Ofs.open(fname_dst.c_str(), std::ofstream::binary);
  if (!Ofs.is_open()) {
    return -1;
  }
  // Write VERSION
  proto_reader_ptr_1->mIfs.seekg(0);
  proto_reader_ptr_1->mIfs.read(&(proto_reader_ptr_1->mProtoBufVec[0]), 4);
  Ofs.write(&(proto_reader_ptr_1->mProtoBufVec[0]), 4);
  int count_1 = proto_reader_ptr_1->getFrameCnt();
  int count_2 = proto_reader_ptr_2->getFrameCnt();

  for (int i = 0; i < count_1; ++i) {
    ProtoInfo_t &frame_info = proto_reader_ptr_1->mProtoInfoVec[i];
    proto_reader_ptr_1->mIfs.seekg((int)frame_info.prtPos - 4);
    proto_reader_ptr_1->mIfs.read(&(proto_reader_ptr_1->mProtoBufVec[0]),
                                  frame_info.prtLen + 4);
    Ofs.write(&(proto_reader_ptr_1->mProtoBufVec[0]), frame_info.prtLen + 4);
  }
  for (int i = 0; i < count_2; ++i) {
    ProtoInfo_t &frame_info = proto_reader_ptr_2->mProtoInfoVec[i];
    proto_reader_ptr_2->mIfs.seekg((int)frame_info.prtPos - 4);
    proto_reader_ptr_2->mIfs.read(&(proto_reader_ptr_2->mProtoBufVec[0]),
                                  frame_info.prtLen + 4);
    Ofs.write(&(proto_reader_ptr_2->mProtoBufVec[0]), frame_info.prtLen + 4);
  }
  proto_reader_ptr_1->stopReader();
  proto_reader_ptr_2->stopReader();
  delete proto_reader_ptr_1;
  proto_reader_ptr_1 = nullptr;
  delete proto_reader_ptr_2;
  proto_reader_ptr_2 = nullptr;
}
#endif
