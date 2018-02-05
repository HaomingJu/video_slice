#ifndef INCLUDE_VIDEOUTILS_H_
#define INCLUDE_VIDEOUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CutNode_ {
  double start_seconds;
  double end_seconds;
  double start_seconds_refined;
  double end_seconds_refined;
  char filename[256];
} CutNode;

/**
 * cut and merge video files depending on start_seconds and end_seconds
 *
 * @param node Pointer to user-supplied CutNode struct, cut/merge files
 *        according to filename and start/end_seconds;
 *        after calling, start_seconds_refined will be the I-frame aligned
 *        of start_seconds, end_seconds_refined will be the exact pts of
 *        end_seconds.
 *        start_seconds == -1 means this file will be cut from the beginning;
 *        end_seconds == -1 means this file will be cut until the end.
 *
 * @param node_len num of files to cut/merge
 *
 * @param out_filename Name of the video file to write to.
 *
 * @return 0 on success, a negative value on failure.
 *
 */
int cut_merge_video(CutNode *node, int node_len, const char *out_filename);

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_VIDEOUTILS_H_
