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

  int getMP4Path(const std::string &flag, const int64_t &start_point,
                 const int64_t &len, std::vector<CutNode> &node);
  int addSearchPath(const std::string &path);  // 增加搜索路径
  int delSearchPath(const std::string &path);  // 删除搜索路径
  void showSearchPath();
  int getFiles(std::string &path, std::vector<std::string> &v_mp4_path);
  int create_path(const std::string &path);  // 创建路径
  void setReg(const std::string &reg_);      // 设置正则表达式

 private:
  std::vector<std::string> m_search_path;  // 搜索路径集合
  int mk_multi_dirs(std::string dst_dirs);
  std::string m_reg;
};
}

#endif  //  __SEARCH_PATH_SEARCH_H__
