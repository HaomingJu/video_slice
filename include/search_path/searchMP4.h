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
  bool create_path(const std::string &path);  // 创建路径

 private:
  std::vector<std::string> m_search_path;  // 搜索路径集合
  int mk_multi_dirs(std::string dst_dirs);
};
}

#endif  //  __SEARCH_PATH_SEARCH_H__
