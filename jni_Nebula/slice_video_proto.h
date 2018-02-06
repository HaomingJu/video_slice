#ifndef __SLICE_VIDEO_PROTO_H__
#define __SLICE_VIDEO_PROTO_H__

namespace HobotNebula {
class ProtoOps;
};

class Slice {
 public:
  static int DMSProto_CutOneFile(const std::string &fname_src, int64_t start_id,
                                 int64_t end_id, const std::string &fname_dst);
  static int DMSProto_MergeTwoFile(const std::string &fname_src1,
                                   const std::string &fname_src2,
                                   const std::string &fname_dst);

  static int DMSVideo_CutOneFile(const std::string &fname_src, int64_t start_id,
                                 int64_t end_id, const std::string &fname_dst);
  static int DMSVideo_MergeTwoFile(const std::string &fname_src1,
                                   const std::string &fname_src2,
                                   const std::string &fname_dst);

  static int ADASProto_CutOneFile(const std::string &fname_src,
                                  int64_t start_id, int64_t end_id,
                                  const std::string &fname_dst);
  static int ADASProto_MergeTwoFile(const std::string &fname_src1,
                                    const std::string &fname_src2,
                                    const std::string &fname_dst);

  static int ADASVideo_CutOneFile(const std::string &fname_src,
                                  int64_t start_id, int64_t end_id,
                                  const std::string &fname_dst);
  static int ADASVideo_MergeTwoFile(const std::string &fname_src1,
                                    const std::string &fname_src2,
                                    const std::string &fname_dst);
};

#endif  // __SLICE_VIDEO_PROTO_H__
