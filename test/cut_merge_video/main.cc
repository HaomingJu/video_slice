#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "video_utils/video_utils.h"

int main(int argc, char **argv) {
  if (argc < 6 || ((argc - 2) % 4 != 0))
    return -1;
  int node_len = (argc - 2) / 4;
  CutNode *node = new CutNode[node_len];

  int i;
  for (i = 0; i < node_len; i++) {
    sscanf(argv[1 + i * 4], "%lf", &node[i].start_seconds);
    sscanf(argv[2 + i * 4], "%lf", &node[i].end_seconds);
    sscanf(argv[3 + i * 4], "%lf", &node[i].delta_seconds);
    strcpy(node[i].filename, argv[4 + i * 4]);
  }
  const char *out_filename = argv[argc - 1];
  cut_merge_video(node, node_len, out_filename);
  for (i = 0; i < node_len; i++) {
    printf("%d: start_: %f\n", i, node[i].start_seconds_refined);
    printf("%d: end_: %f\n", i, node[i].end_seconds_refined);
  }
  delete[] node;
  return 0;
}
