#include "slice_video_proto.h"
#include <iostream>
#include <string>
#include "protobuf_ops/protobuf_ops.h"
#include "search_path/searchMP4.h"
#include "time_utils/TimeUtils.h"
#define MODULE_TAG "DMS_Slice"
#include "logging/DMSLog.h"

namespace HobotNebula {

int DMS_Slice::Cut(int64_t start_ms, int64_t dur_ms,
                   const std::string &result_path_name) {
  LOGI_T(MODULE_TAG) << "start: " << start_ms;
  LOGI_T(MODULE_TAG) << "dura : " << dur_ms;
  LOGI_T(MODULE_TAG) << "out  : " << result_path_name;
  if (dur_ms <= 0)
    return -1;
  // 变量定义
  std::string mp4_name = result_path_name + ".mp4";
  std::string proto_name = result_path_name + ".proto";
  SearchMP4 search_mp4;
  std::vector<CutNode> node;
  std::vector<CutNodeProto> node_proto;
  std::string search_path = "/storage/sdcard1/com.hobot.dms.sample/video/";
  int ret = -1;

  // 添加搜索路径
  search_mp4.addSearchPath(search_path);

  // 搜索对应的文件路径
  ret = search_mp4.getMP4Path(start_ms, dur_ms, node);
  if (ret != 0 || node.empty())
    return ret;

  // 切取mp4视频
  ret = cut_merge_video((CutNode *)(&(node[0])), node.size(), mp4_name.c_str());
  if (ret != 0)
    return ret;

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
  }
  // 切取proto文件
  return cut_merge_proto(node_proto, proto_name);
}
}
