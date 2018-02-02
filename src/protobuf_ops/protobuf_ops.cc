//
// CopyRight 2017 Horizon Robotics
//
#define MODULE_TAG "_ProtobufOps"
#include "protobuf_ops/protobuf_ops.h"
#include "logging/DMSLog.h"

using namespace HobotDMS;

int DMSProtoReader::startReader(const std::string &filename) {
  m_deserializer = new MetaDeserializer();
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
  uint32_t version = 0;
  uint8_t tran_ver[4];
  mIfs.read((char *)tran_ver, 4);
  version = (tran_ver[0] << 24) & 0xFF000000;
  version |= (tran_ver[1] << 16) & 0x00FF0000;
  version |= (tran_ver[2] << 8) & 0x0000FF00;
  version |= (tran_ver[3]) & 0x000000FF;

  size_t cur_pos = 4;  // current position in ifstream
  ProtoInfo_t protoInfo;
  mProtoInfoVec.clear();
  mProtoBufVec.clear();
  LOGI_T(MODULE_TAG) << "project's VERSION: " << VERSION;
  LOGI_T(MODULE_TAG) << "proto's VERSION: " << version;
  switch (version) {
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
};

int DMSProtoReader::readOne(int64_t &frame_id_, int64_t &timestamp_) {
  if (mInput_cnt < 0 ||
      static_cast<unsigned int>(mInput_cnt) >= mProtoInfoVec.size()) {
    LOGI_T(MODULE_TAG) << "mInput_cnt = " << mInput_cnt;
    LOGI_T(MODULE_TAG) << "mProtoInfoVec.size = " << mProtoInfoVec.size();
    return -1;
  }
  ProtoInfo_t &frame_info = mProtoInfoVec[mInput_cnt];
  mIfs.seekg((int)frame_info.prtPos);
  mIfs.read(&mProtoBufVec[0], frame_info.prtLen);
  LOGI_T(MODULE_TAG) << "read from file frame_info.prtLen = "
                     << frame_info.prtLen;
  if (m_deserializer->Deserialize((uint8_t *)(&mProtoBufVec[0]),
                                  frame_info.prtLen) &&
      m_deserializer->getTime(frame_id_, timestamp_)) {
    mInput_cnt++;
    return 0;
  }
  return -1;
};

int DMSProtoReader::seekTo(int pos) {
  // LOGI_T(MODULE_TAG) << "do seek:  " << pos;
  mInput_cnt = pos;
  return 0;
};

int DMSProtoReader::getFrameCnt(void) { return mProtoInfoVec.size(); };
void DMSProtoReader::stopReader() {
  mProtoInfoVec.clear();
  if (m_deserializer != nullptr)
    delete m_deserializer;
  if (mIfs.is_open())
    mIfs.close();
};

int DMSProtoReader::cutOneFile(int64_t start_id, int64_t end_id,
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

int ProtoOps::cutOneFile(const std::string &fname_src, int64_t start_id,
                         int64_t end_id, const std::string &fname_dst) {
  DMSProtoReader *proto_reader_ptr = new DMSProtoReader();
  proto_reader_ptr->startReader(fname_src);
  return proto_reader_ptr->cutOneFile(start_id, end_id, fname_dst);
  proto_reader_ptr->stopReader();
  delete proto_reader_ptr;
  proto_reader_ptr = nullptr;
}

int ProtoOps::mergeTwoFile(const std::string &fname_src1,
                           const std::string &fname_src2,
                           const std::string &&fname_dst) {
  DMSProtoReader *proto_reader_ptr_1 = new DMSProtoReader();
  DMSProtoReader *proto_reader_ptr_2 = new DMSProtoReader();
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
