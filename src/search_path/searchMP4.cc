#include "searchMP4.h"
#include <dirent.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <cstdlib>
#include <regex>
#include <vector>
#include "DMSLog.h"
#include "TimeUtils.h"
#include "protobuf_ops.h"

#ifdef MODULE_TAG
#undef MODULE_TAG
#endif
#define MODULE_TAG "SearchMP4"
namespace HobotNebula {
SearchMP4::SearchMP4(){};

SearchMP4::~SearchMP4(){};

int SearchMP4::getMP4Path(const std::string &flag, const int64_t &start_point,
                          const int64_t &mp4_len, std::vector<CutNode> &node) {
  Slice_info info;
  std::vector<std::string> mp4_name_list;
  std::vector<Slice_info> v_slice_info;
  bool result = false;
  for (int i = 0; i < m_search_path.size(); ++i) {
    result |= this->getFiles(m_search_path[i], mp4_name_list);
  }

  for (int i = 0; i < mp4_name_list.size(); i++) {
    info.mp4_path = mp4_name_list[i];
    int find_index = info.mp4_path.find("_Nebula_1_5_");
    std::string result = info.mp4_path.substr(find_index + 12, 19);
    int64_t time_point = 0;
    TimeUtils::StringMstoTimMs(result, time_point);
    info.start_timestamp = time_point;
    v_slice_info.push_back(info);
  }
  std::sort(v_slice_info.begin(), v_slice_info.end(),
            [](const Slice_info &info_1, const Slice_info &info_2) {
              return info_1.start_timestamp < info_2.start_timestamp ? true
                                                                     : false;
            });
  int64_t end_point = start_point + mp4_len;
  int low = 0;
  int high = v_slice_info.size();
  int mid = low;
  while (low < (high - 1)) {
    mid = (low + high) / 2;
    int64_t mid_val = v_slice_info[mid].start_timestamp;
    if (mid_val > start_point) {
      high = mid;
    }
    if (mid_val < start_point) {
      low = mid;
    }
    if (mid_val == start_point) {
      low = mid;
      break;
    }
  }
  int file_1 = low;
  high = v_slice_info.size();
  while (low < (high - 1)) {
    mid = (low + high) / 2;
    int64_t mid_val = v_slice_info[mid].start_timestamp;
    if (mid_val > end_point) {
      high = mid;
    }
    if (mid_val < end_point) {
      low = mid;
    }
    if (mid_val == end_point) {
      low = mid;
      break;
    }
  }
  int file_2 = low;
  int node_len = -1;
  if (end_point >= v_slice_info.back().start_timestamp)
    --file_2;
  if (end_point < v_slice_info.front().start_timestamp ||
      start_point > v_slice_info.back().start_timestamp) {
    node.resize(0);
    return -1;
  } else
    node_len = (file_2 - file_1) + 1;
  CutNode cut_node;

  for (int i = 0; i < node_len; ++i) {
    strcpy(cut_node.filename, v_slice_info[i + file_1].mp4_path.c_str());
    cut_node.start_seconds =
        double(start_point - v_slice_info[i + file_1].start_timestamp) / 1000;
    cut_node.end_seconds =
        double(end_point - v_slice_info[i + file_1].start_timestamp) / 1000;
    if (i < (node_len - 1))
      cut_node.end_seconds = -1;
    if (i >= 1)
      cut_node.start_seconds = -1;
    if (i == 0)
      cut_node.start_seconds =
          cut_node.start_seconds <= 0 ? -1 : cut_node.start_seconds;

    node.push_back(cut_node);
  }
  if (end_point >= v_slice_info.back().start_timestamp) {
    node.back().end_seconds = -1;
  }
  // 添加视频间的during time
  if (node.size() > 1) {
    ProtoReader *reader = nullptr;
    if (flag == "DMS") {
      reader = new DMS_ProtoReader();
    } else if (flag == "ADAS") {
      reader = new ADAS_ProtoReader();
    } else {
      return -1;
    }
    if (!reader)
      return -1;

    std::vector<std::pair<int64_t, int64_t>> tmp_vector_data;
    for (std::vector<CutNode>::iterator iter = node.begin(); iter != node.end();
         ++iter) {
      std::string proto_name = iter->filename;
      proto_name.replace(proto_name.find(".mp4"), 4, ".proto");
      reader->startReader(proto_name);
      std::pair<int64_t, int64_t> tmp_data;
      tmp_data.first = reader->getStartFrameTime();
      tmp_data.second = reader->getEndFrameTime();
      LOGW_T(MODULE_TAG) << "first value: " << tmp_data.first;
      LOGW_T(MODULE_TAG) << "second value: " << tmp_data.second;
      tmp_vector_data.push_back(tmp_data);
      reader->stopReader();
    }
    for (int i = 0; i < tmp_vector_data.size() - 1; ++i) {
      int64_t delta = tmp_vector_data[i + 1].first - tmp_vector_data[i].second;
      node[i].delta_seconds = double(delta) / 1000;
      LOGW_T(MODULE_TAG) << "node[" << i << "] = " << node[i].delta_seconds;
    }
  }
  return 0;
};

int SearchMP4::addSearchPath(std::string &path) {
  std::vector<std::string>::iterator iter =
      find(m_search_path.begin(), m_search_path.end(), path);
  if (iter == m_search_path.end())  // 没有找到
  {
    this->m_search_path.push_back(path);
    return 0;
  } else
    return -1;
};

int SearchMP4::delSearchPath(std::string &path) {
  std::vector<std::string>::iterator iter =
      find(m_search_path.begin(), m_search_path.end(), path);
  if (iter != m_search_path.end())  // 找到
  {
    this->m_search_path.erase(iter, iter + 1);
    return 0;
  } else
    return -1;
};

void SearchMP4::showSearchPath() {
  for (std::vector<std::string>::iterator iter = m_search_path.begin();
       iter != m_search_path.end(); ++iter) {
    LOGD_T(MODULE_TAG) << "path: " << *iter;
  }
}
int SearchMP4::getFiles(std::string &path,
                         std::vector<std::string> &v_mp4_path) {
  // 正则匹配
  std::regex re("_Nebula_1_5_\\d{8}-\\d{6}_\\d{3}.mp4");

  DIR *dp;
  struct dirent *dirp;
  if ((dp = opendir(path.c_str())) == NULL) {
    LOGD_T(MODULE_TAG) << "can not open" << opendir;
    return -1;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_type == 8) {
      std::string s(dirp->d_name);
      std::smatch sm;
      std::regex_search(s, sm, re);
      if (sm.size() == 1) {
        v_mp4_path.push_back(path + "/" + s);
      }
    }
  }
  closedir(dp);
  return 0;
}

int SearchMP4::create_path(const std::string &path) {
  int ret = access(path.c_str(), 0);
  if (ret != 0) {
    ret = mk_multi_dirs(path.c_str());
    if (ret != 0) {
      LOGW_T(MODULE_TAG) << "Path create failed: " << path << "\tret=" << ret;
      return ret;
    } else {
      return ret;
    }

  } else {
    return 0;
  }
}

int SearchMP4::mk_multi_dirs(std::string dst_dirs) {
  std::string subdir;
  int pos = 0;
  int endpos = 0;

  while (std::string::npos != (endpos = dst_dirs.find("/", pos))) {
    if (endpos == 0)
      subdir = dst_dirs.substr(0, 1);
    else
      subdir = dst_dirs.substr(0, endpos);
    if (access(subdir.c_str(), F_OK) != 0) {
      int ret = mkdir(subdir.c_str(), 0777);
      if (ret != 0)
        return -1;
      chmod(subdir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP |
                                S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
    }
    pos = endpos + 1;
  }
  return 0;
}
}
