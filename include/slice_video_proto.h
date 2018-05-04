#ifndef __SLICE_VIDEO_PROTO_H__
#define __SLICE_VIDEO_PROTO_H__
#include <stdint.h>
#include <string>
#include <vector>
namespace HobotNebula {
class SearchMP4;

class Slice {
 public:
  // Slice();
  // virtual ~Slice();
  int Init(const std::string &, const std::string &);
  virtual int64_t Cut(int64_t start_ms, int64_t dur_ms, char *str, int str_size,
                      const std::string & = "") = 0;

 protected:
  std::vector<std::string> m_root_path;
  std::string m_search_name;
  std::string m_slice_name;
  std::string m_slice_flag;
  SearchMP4 *m_search_mp4;
};

class DMS_Slice : public Slice {
 public:
  DMS_Slice();
  ~DMS_Slice();
  int64_t Cut(int64_t start_ms, int64_t dur_ms, char *str, int str_size,
              const std::string & = "");
};

class ADAS_Slice : public Slice {
 public:
  ADAS_Slice();
  ~ADAS_Slice();
  int64_t Cut(int64_t start_ms, int64_t dur_ms, char *str, int str_size,
              const std::string & = "");
};
}

#endif  // __SLICE_VIDEO_PROTO_H__
