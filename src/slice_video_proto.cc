#include "slice_video_proto.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "jsoncpp/json.h"
#include "logging/DMSLog.h"
#include "protobuf_ops/protobuf_ops.h"
#include "search_path/searchMP4.h"
#include "time_utils/TimeUtils.h"

#ifdef MODULE_TAG
#undef MODULE_TAG
#endif
#define MODULE_TAG "Slice"

namespace HobotNebula {
// Slice::Slice() : m_slice_name(""), m_slice_flag(""), m_search_mp4(nullptr) {}
// Slice::~Slice() {}
DMS_Slice::DMS_Slice() {}
DMS_Slice::~DMS_Slice() {}
ADAS_Slice::ADAS_Slice() {}
ADAS_Slice::~ADAS_Slice() {}

int Slice::Init(const std::string &app_flag,
                const std::string &config_path_name) {
  m_search_mp4 = new SearchMP4();
  if (!m_search_mp4)
    return -1;
  // 读取json文件流
  Json::Reader reader;
  Json::Value value;
  std::ifstream ifs;
  ifs.open(config_path_name.c_str());
  if (ifs.is_open() == false)
    return -1;
  if (!reader.parse(ifs, value)) {
    LOGD_T(MODULE_TAG) << "[ERROR]: parse json fail";
    return -1;
  }
  ifs.close();

  Json::Value conf_app = value[app_flag];
  if (conf_app.empty()) {
    return -1;
  }

  // 读取搜索路径和文件名
  this->m_search_name = conf_app.get("search_path_name", "").asString();
  if (this->m_search_name.empty()) {
    LOGE_T(MODULE_TAG) << "json parse error slice_path_name";
    return -1;
  }
  LOGD_T(MODULE_TAG) << "read search_path_name: " << this->m_search_name;

  // 读取切片存放路径和名称
  this->m_slice_name = conf_app.get("slice_path_name", "").asString();
  if (this->m_slice_name.empty()) {
    LOGE_T(MODULE_TAG) << "json parse error slice_path_name";
    return -1;
  }
  LOGD_T(MODULE_TAG) << "read slice_path_name: " << this->m_slice_name;

  // 读取切片标志名称
  this->m_slice_flag = conf_app.get("slice_data_flag", "").asString();
  if (this->m_slice_flag.empty()) {
    LOGE_T(MODULE_TAG) << "json parse error slice_data_flag";
    return -1;
  }
  LOGD_T(MODULE_TAG) << "read slice_flag: " << this->m_slice_flag;

  // 读取匹配正则表达式
  std::string tmp_re_str = conf_app.get("re_str", "").asString();
  if (tmp_re_str.empty()) {
    LOGE_T(MODULE_TAG) << "json parse error re_str";
    return -1;
  }
  this->m_search_mp4->setReg(tmp_re_str);
  LOGD_T(MODULE_TAG) << "read re_str: " << tmp_re_str;

  // 读取RootPath
  Json::Value SearchRootPath = value["SearchRootPath"];
  if (SearchRootPath.size() <= 0) {
    LOGE_T(MODULE_TAG) << "json parse error: SearchRootPath";
    return -1;
  }

  int ret = -1;
  for (int i = 0; i < SearchRootPath.size(); ++i) {
    std::string tmp_root_path = SearchRootPath[i].asString();
    ret &= this->m_search_mp4->create_path(tmp_root_path + "/" +
                                           this->m_slice_name);
    if (!ret) {
      this->m_search_mp4->addSearchPath(tmp_root_path + "/" +
                                        this->m_search_name);
      this->m_root_path.push_back(tmp_root_path);
      LOGD_T(MODULE_TAG) << "read SearchRootPath " << i << " " << tmp_root_path;
    }
  }
  return ret;
}

int64_t DMS_Slice::Cut(int64_t start_ms, int64_t dur_ms, char *str,
                       int str_size, const std::string &user_flag) {
  LOGI_T(MODULE_TAG) << "start: " << start_ms;
  LOGI_T(MODULE_TAG) << "dura : " << dur_ms;

  if (dur_ms <= 0)
    return -1;
  // 变量定义
  memset(str, 0x00, str_size);
  std::vector<CutNode> node;
  std::vector<CutNodeProto> node_proto;
  std::string new_mp4_name;
  int64_t ret_timestamp = -1;
  // 搜索对应的文件路径
  int ret = -1;
  ret = this->m_search_mp4->getMP4Path("DMS", start_ms, dur_ms, node);
  if (ret != 0 || node.empty()) {
    LOGD_T(MODULE_TAG) << "node size = " << node.size();
    return ret;
  }

  // 切取mp4视频
  std::string mp4_path = "";
  std::string mp4_path_tmp_name = "";
  mp4_path = std::string(node[0].filename);
  int rfind_index = mp4_path.rfind(this->m_search_name);
  if (rfind_index == std::string::npos)
    return -1;
  mp4_path.replace(rfind_index, mp4_path.size() - rfind_index,
                   this->m_slice_name);
  mp4_path_tmp_name = mp4_path + "/tmp.mp4";
  LOGD_T(MODULE_TAG) << "tmp mp4 path name : " << mp4_path_tmp_name;
  ret = cut_merge_video((CutNode *)(&(node[0])), node.size(),
                        mp4_path_tmp_name.c_str());
  if (ret != 0) {
    LOGD_T(MODULE_TAG) << "cut_merge_video fail";
    return ret;
  }

  // 时间戳转换
  for (std::vector<CutNode>::iterator iter = node.begin(); iter != node.end();
       ++iter) {
    CutNodeProto tmp_node_proto;
    tmp_node_proto.filename = std::string(iter->filename);
    int find_index = tmp_node_proto.filename.rfind(".mp4");
    if (find_index == std::string::npos)
      return -1;
    std::string result = tmp_node_proto.filename.substr(find_index - 19, 19);
    LOGD_T(MODULE_TAG) << "the data: " << result;
    tmp_node_proto.filename.replace(
        find_index, tmp_node_proto.filename.size() - find_index, ".proto");
    int64_t time_base = 0;
    TimeUtils::StringMstoTimMs(result, time_base);
    if (time_base == -1)
      return -1;
    tmp_node_proto.start_ts = iter->start_seconds_refined * 1000 + time_base;
    tmp_node_proto.end_ts = iter->end_seconds_refined * 1000 + time_base;
    node_proto.push_back(tmp_node_proto);
    // 将生成的mp4结果重命名.
    if (iter == node.begin()) {
      std::string mv_mp4_name;
      ret_timestamp = tmp_node_proto.start_ts;
      TimeUtils::TimeMstoStringMs(ret_timestamp, mv_mp4_name);

      std::string result_path_name;
      result_path_name.append(mp4_path);
      result_path_name.append("/");
      result_path_name.append(this->m_slice_flag);

      if (user_flag.size()) {
        result_path_name.append(user_flag);
        result_path_name.append("_");
      }
      result_path_name.append(mv_mp4_name);

      if (str_size <= result_path_name.size()) {
        LOGD_T(MODULE_TAG) << "size of char is not enough";
        return -1;
      }
      strncpy(str, result_path_name.c_str(), result_path_name.size());
      new_mp4_name = result_path_name + ".mp4";
      LOGD_T(MODULE_TAG) << "mv_mp4_name: " << mv_mp4_name;
      if (rename(mp4_path_tmp_name.c_str(), new_mp4_name.c_str()) != 0) {
        LOGD_T(MODULE_TAG) << "rename fail";
        return -1;
      }
    }
  }
  // 切取proto文件
  int index = new_mp4_name.rfind(".mp4");
  std::string proto_name =
      new_mp4_name.replace(index, new_mp4_name.size() - index, ".proto");
  ret = DMS_cut_merge_proto(node_proto, proto_name);
  if (ret)
    return ret;
  return ret_timestamp;
}

int64_t ADAS_Slice::Cut(int64_t start_ms, int64_t dur_ms, char *str,
                        int str_size, const std::string &user_flag) {
  LOGI_T(MODULE_TAG) << "start: " << start_ms;
  LOGI_T(MODULE_TAG) << "dura : " << dur_ms;

  if (dur_ms <= 0)
    return -1;
  // 变量定义
  memset(str, 0x00, str_size);
  std::vector<CutNode> node;
  std::vector<CutNodeProto> node_proto;
  std::string new_mp4_name;
  int64_t ret_timestamp = -1;
  int ret = -1;

  // 搜索对应的文件路径
  ret = this->m_search_mp4->getMP4Path("ADAS", start_ms, dur_ms, node);
  if (ret != 0 || node.empty()) {
    LOGD_T(MODULE_TAG) << "node size = " << node.size();
    return ret;
  }

  // 切取mp4视频
  std::string mp4_path = "";
  std::string mp4_path_tmp_name = "";
  mp4_path = std::string(node[0].filename);
  int rfind_index = mp4_path.rfind(this->m_search_name);
  if (rfind_index == std::string::npos)
    return -1;
  mp4_path.replace(rfind_index, mp4_path.size() - rfind_index,
                   this->m_slice_name);
  mp4_path_tmp_name = mp4_path + "/tmp.mp4";
  LOGD_T(MODULE_TAG) << "tmp mp4 path name : " << mp4_path_tmp_name;
  ret = cut_merge_video((CutNode *)(&(node[0])), node.size(),
                        mp4_path_tmp_name.c_str());
  if (ret != 0) {
    LOGD_T(MODULE_TAG) << "cut_merge_video fail";
    return ret;
  }

  // 时间戳转换
  for (std::vector<CutNode>::iterator iter = node.begin(); iter != node.end();
       ++iter) {
    CutNodeProto tmp_node_proto;
    tmp_node_proto.filename = std::string(iter->filename);
    int find_index = tmp_node_proto.filename.rfind(".mp4");
    if (find_index == std::string::npos)
      return -1;
    std::string result = tmp_node_proto.filename.substr(find_index - 19, 19);
    LOGD_T(MODULE_TAG) << "the data: " << result;
    tmp_node_proto.filename.replace(
        find_index, tmp_node_proto.filename.size() - find_index, ".proto");
    int64_t time_base = 0;
    TimeUtils::StringMstoTimMs(result, time_base);
    if (time_base == -1)
      return -1;
    tmp_node_proto.start_ts = iter->start_seconds_refined * 1000 + time_base;
    tmp_node_proto.end_ts = iter->end_seconds_refined * 1000 + time_base;
    node_proto.push_back(tmp_node_proto);
    // 将生成的mp4结果重命名.
    if (iter == node.begin()) {
      std::string mv_mp4_name;
      ret_timestamp = tmp_node_proto.start_ts;
      TimeUtils::TimeMstoStringMs(ret_timestamp, mv_mp4_name);

      std::string result_path_name;
      result_path_name.append(mp4_path);
      result_path_name.append("/");
      result_path_name.append(this->m_slice_flag);

      if (user_flag.size()) {
        result_path_name.append(user_flag);
        result_path_name.append("_");
      }
      result_path_name.append(mv_mp4_name);

      if (str_size <= result_path_name.size()) {
        LOGD_T(MODULE_TAG) << "size of char is not enough";
        return -1;
      }
      strncpy(str, result_path_name.c_str(), result_path_name.size());
      new_mp4_name = result_path_name + ".mp4";
      LOGD_T(MODULE_TAG) << "mv_mp4_name: " << mv_mp4_name;
      if (rename(mp4_path_tmp_name.c_str(), new_mp4_name.c_str()) != 0) {
        LOGD_T(MODULE_TAG) << "rename fail";
        return -1;
      }
    }
  }
  // 切取proto文件
  int index = new_mp4_name.rfind(".mp4");
  std::string proto_name =
      new_mp4_name.replace(index, new_mp4_name.size() - index, ".proto");
  ret = ADAS_cut_merge_proto(node_proto, proto_name);
  if (ret)
    return ret;
  return ret_timestamp;
};
}
