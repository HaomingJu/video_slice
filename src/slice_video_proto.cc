#include "slice_video_proto.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include <string>
#include "logging/DMSLog.h"
#include "protobuf_ops/protobuf_ops.h"
#include "search_path/searchMP4.h"
#include "time_utils/TimeUtils.h"

#define MODULE_TAG "Slice"

namespace HobotNebula {

int DMS_Slice::Cut(int64_t start_ms, int64_t dur_ms, char *str, int str_size) {
  LOGI_T(MODULE_TAG) << "start: " << start_ms;
  LOGI_T(MODULE_TAG) << "dura : " << dur_ms;

  if (dur_ms <= 0)
    return -1;
  // 变量定义
  memset(str, 0x00, str_size);
  SearchMP4 search_mp4;
  std::vector<CutNode> node;
  std::vector<CutNodeProto> node_proto;
  std::string search_path = "/storage/sdcard1/com.hobot.dms.sample/video/";
  std::string slice_path = "/storage/sdcard1/com.hobot.dms.sample/slice/";
  std::string mp4_name = slice_path + "tmp.mp4";
  std::string new_mp4_name;
  int ret = -1;

  // 尝试创建切片路径
  if (search_mp4.create_path(slice_path))
    return -1;
  // 添加搜索路径
  search_mp4.addSearchPath(search_path);
  // 搜索对应的文件路径
  ret = search_mp4.getMP4Path("DMS", start_ms, dur_ms, node);
  if (ret != 0 || node.empty()) {
    LOGD_T(MODULE_TAG) << "node size = " << node.size();
    return ret;
  }

  // 切取mp4视频
  ret = cut_merge_video((CutNode *)(&(node[0])), node.size(), mp4_name.c_str());
  if (ret != 0) {
    LOGD_T(MODULE_TAG) << "cut_merge_video fail";
    return ret;
  }

  // 时间戳转换
  for (std::vector<CutNode>::iterator iter = node.begin(); iter != node.end();
       ++iter) {
    CutNodeProto tmp_node_proto;
    tmp_node_proto.filename = std::string(iter->filename);
    int find_index = tmp_node_proto.filename.find("_Nebula_1_5_");
    if (find_index == std::string::npos)
      return -1;
    std::string result = tmp_node_proto.filename.substr(find_index + 12, 19);
    find_index = tmp_node_proto.filename.find(".mp4");
    if (find_index == std::string::npos)
      return -1;
    tmp_node_proto.filename.replace(find_index, 4, ".proto");
    int64_t time_base = 0;
    TimeUtils::StringMstoTimMs(result, time_base);
    if (time_base == -1)
      return -1;
    tmp_node_proto.start_ts = iter->start_seconds_refined * 1000 + time_base;
    tmp_node_proto.end_ts = iter->end_seconds_refined * 1000 + time_base;
    node_proto.push_back(tmp_node_proto);
    // 将生成的mp4结果重命名, 伟少那个JB真TM会提需求啊
    if (iter == node.begin()) {
      std::string mv_mp4_name;
      TimeUtils::TimeMstoStringMs(tmp_node_proto.start_ts, mv_mp4_name);

      std::string result_path_name =
          slice_path + "/Slice_DMS_Nebula_1_5_" + mv_mp4_name;
      if (str_size <= result_path_name.size()) {
        LOGD_T(MODULE_TAG) << "size of char is not enough";
        return -1;
      }
      strncpy(str, result_path_name.c_str(), result_path_name.size());
      new_mp4_name = result_path_name + ".mp4";
      LOGD_T(MODULE_TAG) << "mv_mp4_name: " << mv_mp4_name;
      if (rename(mp4_name.c_str(), new_mp4_name.c_str()) != 0) {
        LOGD_T(MODULE_TAG) << "rename fail";
        return -1;
      }
    }
  }
  // 切取proto文件
  int index = new_mp4_name.find(".mp4");
  std::string proto_name = new_mp4_name.replace(index, 4, ".proto");
  return DMS_cut_merge_proto(node_proto, proto_name);
}

int ADAS_Slice::Cut(int64_t start_ms, int64_t dur_ms, char *str, int str_size) {
  LOGI_T(MODULE_TAG) << "start: " << start_ms;
  LOGI_T(MODULE_TAG) << "dura : " << dur_ms;
  if (dur_ms <= 0)
    return -1;
  // 变量定义

  memset(str, 0x00, str_size);
  SearchMP4 search_mp4;
  std::vector<CutNode> node;
  std::vector<CutNodeProto> node_proto;
  std::string search_path =
      "/storage/sdcard1/Android/data/com.hobot.adas/video/";
  std::string slice_path =
      "/storage/sdcard1/Android/data/com.hobot.adas/slice/";

  std::string mp4_name = slice_path + "tmp.mp4";
  std::string new_mp4_name;

  int ret = -1;

  // 尝试创建切片路径
  if (search_mp4.create_path(slice_path))
    return -1;
  // 添加搜索路径
  search_mp4.addSearchPath(search_path);

  // 搜索对应的文件路径
  ret = search_mp4.getMP4Path("ADAS", start_ms, dur_ms, node);
  if (ret != 0 || node.empty()) {
    LOGD_T(MODULE_TAG) << "node size = " << node.size();
    return ret;
  }

  // 切取mp4视频
  ret = cut_merge_video((CutNode *)(&(node[0])), node.size(), mp4_name.c_str());
  if (ret != 0) {
    return ret;
  }

  // 时间戳转换
  for (std::vector<CutNode>::iterator iter = node.begin(); iter != node.end();
       ++iter) {
    CutNodeProto tmp_node_proto;
    tmp_node_proto.filename = std::string(iter->filename);
    int find_index = tmp_node_proto.filename.find("_Nebula_1_5_");
    if (find_index == std::string::npos)
      return -1;
    std::string result = tmp_node_proto.filename.substr(find_index + 12, 19);
    find_index = tmp_node_proto.filename.find(".mp4");
    if (find_index == std::string::npos)
      return -1;
    tmp_node_proto.filename.replace(find_index, 4, ".proto");
    int64_t time_base = 0;
    TimeUtils::StringMstoTimMs(result, time_base);
    if (time_base == -1)
      return -1;
    tmp_node_proto.start_ts = iter->start_seconds_refined * 1000 + time_base;
    tmp_node_proto.end_ts = iter->end_seconds_refined * 1000 + time_base;
    node_proto.push_back(tmp_node_proto);
    if (iter == node.begin()) {
      std::string mv_mp4_name;
      TimeUtils::TimeMstoStringMs(tmp_node_proto.start_ts, mv_mp4_name);
      std::string result_path_name =
          slice_path + "/Slice_ADAS_Nebula_1_5_" + mv_mp4_name;
      if (str_size <= result_path_name.size()) {
        LOGD_T(MODULE_TAG) << "size of char is not enough";
        return -1;
      }
      strncpy(str, result_path_name.c_str(), result_path_name.size());
      new_mp4_name = result_path_name + ".mp4";
      LOGD_T(MODULE_TAG) << "mv_mp4_name: " << mv_mp4_name;
      if (rename(mp4_name.c_str(), new_mp4_name.c_str()) != 0) {
        LOGD_T(MODULE_TAG) << "rename fail";
        return -1;
      }
    }
  }
  // 切取proto文件
  int index = new_mp4_name.find(".mp4");
  std::string proto_name = new_mp4_name.replace(index, 4, ".proto");
  return ADAS_cut_merge_proto(node_proto, proto_name);
};
}
