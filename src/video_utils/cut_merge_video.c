#include <stdio.h>
#include <stdint.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include "video_utils/video_utils.h"

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt,
                       const char *tag) {
  AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

  printf(
      "%s: pts:%s pts_time:%s dts:%s dts_time:%s duration:%s "
      "duration_time:%s stream_index:%d\n",
      tag, av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
      av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
      av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
      pkt->stream_index);
}

int cut_merge_video(CutNode *node, int node_len, const char *out_filename) {
  AVOutputFormat *ofmt = NULL;
  AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
  int videostream_index = -1;
  AVPacket pkt;
  int ret, i, j;

  if (node == NULL || node_len < 1)
    return -1;

  const char *in_filename = node[0].filename;

  av_register_all();

  if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
    fprintf(stderr, "Could not open input file '%s'", in_filename);
    goto end;
  }

  if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
    fprintf(stderr, "Failed to retrieve input stream information");
    goto end;
  }

  av_dump_format(ifmt_ctx, 0, in_filename, 0);

  avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
  if (!ofmt_ctx) {
    fprintf(stderr, "Could not create output context\n");
    ret = AVERROR_UNKNOWN;
    goto end;
  }

  ofmt = ofmt_ctx->oformat;

  for (i = 0; i < ifmt_ctx->nb_streams; i++) {
    AVStream *in_stream = ifmt_ctx->streams[i];
    AVCodecParameters *in_codecpar = in_stream->codecpar;

    if (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      videostream_index = i;

      AVStream *out_stream = avformat_new_stream(ofmt_ctx, NULL);
      if (!out_stream) {
        fprintf(stderr, "Failed allocating output stream\n");
        ret = AVERROR_UNKNOWN;
        goto end;
      }

      /*
       * since all input files are supposed to be identical (framerate,
       * dimension, color format, ...)
       * we can safely set output codec values from first input file
       */
      ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
      if (ret < 0) {
        fprintf(stderr, "Failed to copy codec parameters\n");
        goto end;
      }
      out_stream->codecpar->codec_tag = 0;
    }

    /* if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) */
    /* out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; */
  }
  if (videostream_index == -1) {
    fprintf(stderr, "Failed to find any video stream\n");
    goto end;
  }
  av_dump_format(ofmt_ctx, 0, out_filename, 1);

  if (!(ofmt->flags & AVFMT_NOFILE)) {
    ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
    if (ret < 0) {
      fprintf(stderr, "Could not open output file '%s'", out_filename);
      goto end;
    }
  }

  /* close the first input file */
  avformat_close_input(&ifmt_ctx);

  ret = avformat_write_header(ofmt_ctx, NULL);
  if (ret < 0) {
    fprintf(stderr, "Error occurred when opening output file\n");
    goto end;
  }

  int64_t last_pts = 0, last_dts = 0, last_duration = 0;

  for (j = 0; j < node_len; j++) {
    double start_seconds = node[j].start_seconds;
    double end_seconds = node[j].end_seconds;
    const char *in_file = node[j].filename;
    node[j].start_seconds_refined = -1;
    node[j].end_seconds_refined = -1;
    printf("----- start: %f\n", start_seconds);
    printf("----- end: %f\n", end_seconds);
    printf("----- file: %s\n", in_file);
    printf("----- last_dts: %lld\n", last_dts);
    printf("----- last_pts: %lld\n", last_pts);

    ifmt_ctx = NULL;
    if ((ret = avformat_open_input(&ifmt_ctx, in_file, 0, 0)) < 0) {
      fprintf(stderr, "Could not open input file '%s'", in_filename);
      break;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
      fprintf(stderr, "Failed to retrieve input stream information");
      break;
    }
    av_dump_format(ifmt_ctx, 0, in_filename, 0);

    /* we only use first video stream of each input file */
    videostream_index = -1;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
      AVStream *in_stream = ifmt_ctx->streams[i];
      AVCodecParameters *in_codecpar = in_stream->codecpar;

      if (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
        videostream_index = i;
        break;
      }
    }
    if (videostream_index == -1) {
      fprintf(stderr, "Failed to find any video stream\n");
      ret = -1;
      break;
    }

    if (start_seconds > 0) {
      ret = av_seek_frame(ifmt_ctx, -1, start_seconds * AV_TIME_BASE,
                          AVSEEK_FLAG_BACKWARD);
      if (ret < 0) {
        fprintf(stderr, "Error seek\n");
        break;
      }
    }

    int64_t dts_start_from = -1, pts_start_from = -1;
    int64_t tmp_dts = 0, tmp_pts = 0;

    while (1) {
      AVStream *in_stream, *out_stream;

      ret = av_read_frame(ifmt_ctx, &pkt);
      if (ret < 0)
        break;

      if (pkt.stream_index == videostream_index) {
        in_stream = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];

        /* log_packet(ifmt_ctx, &pkt, "in"); */

        double tmp_end_seconds = av_q2d(in_stream->time_base) * pkt.pts;
        if (end_seconds >= 0 && tmp_end_seconds > end_seconds) {
          // reach end time
          av_packet_unref(&pkt);
          break;
        }
        node[j].end_seconds_refined = tmp_end_seconds;

        if (dts_start_from == -1) {
          dts_start_from = pkt.dts;
          printf("dts_start_from: %s\n", av_ts2str(dts_start_from));
        }
        if (pts_start_from == -1) {
          pts_start_from = pkt.pts;
          node[j].start_seconds_refined =
              av_q2d(in_stream->time_base) * pts_start_from;
          printf("pts_start_from: %s\n", av_ts2str(pts_start_from));
        }

        /* copy packet */
        pkt.pts = av_rescale_q_rnd(pkt.pts - pts_start_from,
                                   in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.dts = av_rescale_q_rnd(pkt.dts - dts_start_from,
                                   in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        if (pkt.pts < 0) {
          pkt.pts = 0;
        }
        if (pkt.dts < 0) {
          pkt.dts = 0;
        }
        pkt.pts += last_pts;
        pkt.dts += last_dts;
        pkt.duration = (int)av_rescale_q(
            (int64_t)pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        tmp_pts = pkt.pts;
        tmp_dts = pkt.dts;
        last_duration = pkt.duration;
        /* log_packet(ofmt_ctx, &pkt, "out"); */
        /* printf("\n"); */

        ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
        if (ret < 0) {
          fprintf(stderr, "Error muxing packet\n");
          break;
        }
      }
      av_packet_unref(&pkt);
    }
    if (ret < 0 && ret != AVERROR_EOF)
      break;
    last_pts = tmp_pts + last_duration;
    last_dts = tmp_dts + last_duration;
    avformat_close_input(&ifmt_ctx);
  }

  av_write_trailer(ofmt_ctx);
end:

  if (ifmt_ctx) {
    avformat_close_input(&ifmt_ctx);
  }
  /* close output */
  if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
    avio_closep(&ofmt_ctx->pb);
  avformat_free_context(ofmt_ctx);

  if (ret < 0 && ret != AVERROR_EOF) {
    fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
    return -1;
  }

  return 0;
}
