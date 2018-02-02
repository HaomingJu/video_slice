#include <iostream>
#include <string>
#include "protobuf_ops/protobuf_ops.h"
#include "slice_video_proto.h"

int Slice::DMSProto_CutOneFile(const std::string &fname_src, int64_t start_id,
                               int64_t end_id, const std::string &fname_dst) {
  return HobotDMS::ProtoOps::cutOneFile(fname_src, start_id, end_id, fname_dst);
}

int Slice::DMSProto_MergeTwoFile(const std::string &fname_src1,
                                 const std::string &fname_src2,
                                 const std::string &fname_dst) {
  return HobotDMS::ProtoOps::mergeTwoFile(fname_src1, fname_src2, fname_dst);
}

int Slice::DMSVideo_CutOneFile(const std::string &fname_src, int64_t start_id,
                               int64_t end_id, const std::string &fname_dst) 
{
    return 0;
}

int Slice::DMSVideo_MergeTwoFile(const std::string &fname_src1,
                                 const std::string &fname_src2,
                                 const std::string &fname_dst)
{
    return 0;
}

int Slice::ADASProto_CutOneFile(const std::string &fname_src, int64_t start_id,
                                int64_t end_id, const std::string &fname_dst) 
{
    return 0;
}

int Slice::ADASProto_MergeTwoFile(const std::string &fname_src1,
                                  const std::string &fname_src2,
                                  const std::string &fname_dst) 
{
    return 0;
}

int Slice::ADASVideo_CutOneFile(const std::string &fname_src, int64_t start_id,
                                int64_t end_id, const std::string &fname_dst) 
{
    return 0;
}

int Slice::ADASVideo_MergeTwoFile(const std::string &fname_src1,
                                  const std::string &fname_src2,
                                  const std::string &fname_dst) 
{
    return 0;
}

/*
int main(int argc, char **argv) {
  HobotDMS::SearchMP4 search_mp4;
  std::string s1 = "/home/haoming/";
  std::string s2 = "/home/wente";

  search_mp4.showSearchPath();
  search_mp4.addSearchPath(s1);
  search_mp4.addSearchPath(s2);
  search_mp4.showSearchPath();
  std::cout << "1111111" << std::endl;

  std::cout << "erase: " << search_mp4.delSearchPath(s2) << std::endl;
  // std::cout << "erase: " << search_mp4.delSearchPath(s1) << std::endl;
  search_mp4.showSearchPath();

  search_mp4.getMP4Path(1, 2);
  std::cout << "hello world" << std::endl;
  return 0;
}
*/
