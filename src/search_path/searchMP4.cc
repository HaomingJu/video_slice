#include "searchMP4.h"
#include <dirent.h>
#include <stdio.h>
#include <regex>
#include "DMSLog.h"
#include "TimeUtils.h"

#define MODULE_TAG "SearchMP4"
namespace HobotDMS {
SearchMP4::SearchMP4(){};

SearchMP4::~SearchMP4(){};

std::vector<Slice_info> &SearchMP4::getMP4Path(const int64_t &start_point,
                                               const int &mp4_len) {
  Slice_info info;
  std::vector<std::string> mp4_name_list;
  std::vector<Slice_info> v_slice_info;
  bool result = false;
  for (int i = 0; i < m_search_path.size(); ++i) {
    result |= this->getFiles(m_search_path[i], mp4_name_list);
  }

  return v_slice_info;
};

bool SearchMP4::addSearchPath(std::string &path) {
  std::vector<std::string>::iterator iter =
      find(m_search_path.begin(), m_search_path.end(), path);
  if (iter == m_search_path.end())  // 没有找到
  {
    this->m_search_path.push_back(path);
    return true;
  } else
    return false;
};

bool SearchMP4::delSearchPath(std::string &path) {
  std::vector<std::string>::iterator iter =
      find(m_search_path.begin(), m_search_path.end(), path);
  if (iter != m_search_path.end())  // 找到
  {
    this->m_search_path.erase(iter, iter + 1);
    return true;
  } else
    return false;
};

void SearchMP4::showSearchPath() {
  for (std::vector<std::string>::iterator iter = m_search_path.begin();
       iter != m_search_path.end(); ++iter) {
    LOGD_T(MODULE_TAG) << "path: " << *iter;
  }
}
bool SearchMP4::getFiles(std::string &path,
                         std::vector<std::string> &v_mp4_path) {
  // 正则匹配
  std::regex re("DMS_Nebula_1_5_\\d{8}-\\d{6}_\\d{3}.mp4");

  DIR *dp;
  struct dirent *dirp;
  if ((dp = opendir(path.c_str())) == NULL) {
    LOGD_T(MODULE_TAG) << "can not open" << opendir;
    return false;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_type == 8) {
      std::string s(dirp->d_name);
      std::smatch sm;
      std::regex_search(s, sm, re);
      if (sm.size() == 1) {
        v_mp4_path.push_back(s);
      }
    }
  }
  closedir(dp);
  return true;
}
}
