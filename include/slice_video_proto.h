#ifndef __SLICE_VIDEO_PROTO_H__
#define __SLICE_VIDEO_PROTO_H__
#include <stdint.h>
#include <string>
namespace HobotNebula {

class Slice {
 public:
  virtual int Cut(int64_t start_ms, int64_t dur_ms, char *str,
                  int str_size) = 0;
};

class DMS_Slice : public Slice {
 public:
  int Cut(int64_t start_ms, int64_t dur_ms, char *str, int str_size) override;
};

class ADAS_Slice : public Slice {
 public:
  int Cut(int64_t start_ms, int64_t dur_ms, char *str, int str_size) override;
};
}

#endif  // __SLICE_VIDEO_PROTO_H__
