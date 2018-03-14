#ifndef __SLICE_VIDEO_PROTO_H__
#define __SLICE_VIDEO_PROTO_H__
#include <stdint.h>
#include <string>
#include <vector>
namespace HobotNebula {
class SearchMP4;
class DMS_Slice {
 public:
  DMS_Slice();
  ~DMS_Slice();
  int Init(const std::string &json_path_name);
  int64_t Cut(int64_t start_ms, int64_t dur_ms, char *str, int str_size);

 private:
  std::vector<std::string> m_root_path;
  std::string m_search_name;
  std::string m_slice_name;
  std::string m_slice_flag;
  SearchMP4* m_search_mp4;
};

class ADAS_Slice {
 public:
  ADAS_Slice();
  ~ADAS_Slice();
  int Init(const std::string &json_path_name);
  int64_t Cut(int64_t start_ms, int64_t dur_ms, char *str, int str_size);

 private:
  std::vector<std::string> m_root_path;
  std::string m_search_name;
  std::string m_slice_name;
  std::string m_slice_flag;
  SearchMP4* m_search_mp4;
};
}

#endif  // __SLICE_VIDEO_PROTO_H__
