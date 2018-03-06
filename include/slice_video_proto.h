#ifndef __SLICE_VIDEO_PROTO_H__
#define __SLICE_VIDEO_PROTO_H__
#include <stdint.h>
#include <string>
namespace HobotNebula {

class Slice {
 public:
  virtual int Cut(int64_t start_ms, int64_t dur_ms,
                  std::string &result_path_name) = 0;
};

class DMS_Slice : public Slice {
 public:
  int Cut(int64_t start_ms, int64_t dur_ms,
          std::string &result_path_name) override;
};

class ADAS_Slice : public Slice {
 public:
  int Cut(int64_t start_ms, int64_t dur_ms,
          std::string &result_path_name) override;
};
}

#endif  // __SLICE_VIDEO_PROTO_H__
