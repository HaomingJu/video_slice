#ifndef __SEARCH_PATH_SEARCH_H__
#define __SEARCH_PATH_SEARCH_H__
#include <iostream>
#include <vector>
#include "video_utils/video_utils.h"

namespace HobotNebula {
typedef struct Slice_info_ {
  std::string mp4_path = "";
  int64_t start_timestamp = -1;
} Slice_info;

class SearchMP4 {
 public:
  SearchMP4();
  ~SearchMP4();

  int getMP4Path(const int64_t &start_point, const int64_t &len,
                 std::vector<CutNode> &node);
  bool addSearchPath(std::string &path);  // 增加搜索路径
  bool delSearchPath(std::string &path);  // 删除搜索路径
  void showSearchPath();
  bool getFiles(std::string &path, std::vector<std::string> &v_mp4_path);

 private:
  std::vector<std::string> m_search_path;  // 搜索路径集合
};
}

#endif  //  __SEARCH_PATH_SEARCH_H__
