#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

int ErrorCode_FFmpeg_Parsing = -1;
int ErrorCode_FFmpeg_Send_Packet = -2;
int ErrorCode_FFmpeg_Receive_Frame = -3;

typedef void(*frame_callback)(unsigned char* data_y, unsigned char* data_u, unsigned char* data_v, int line1, int line2, int line3, int width, int height, long pts);

typedef struct {
  AVCodec *codec;
  AVCodecParserContext *parser;
  AVCodecContext *codec_ctx;
  AVFrame *frame;
  AVPacket *pkt;
  frame_callback callback;
}decoder_context;

decoder_context* decoder_context_new(long callback) {
  AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
  if (!codec) {
    fprintf(stderr, "AVCodec not found\n");
    goto FAIL;
  } 

  AVCodecParserContext *parser = av_parser_init(codec->id);
  if (!parser) {
    fprintf(stderr, "CodecParser not found\n");
    return NULL;
  }

  AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
  if (!codec_ctx) {
    fprintf(stderr, "Could not allocate video codec context\n");
    return NULL;
  }

  if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
    fprintf(stderr, "Could not open codec\n");
    return NULL;
  }

  AVFrame *frame = av_frame_alloc();
  if (!frame) {
    fprintf(stderr, "Could not allocate frame context\n");
    return NULL;
  }

  AVPacket *pkt = av_packet_alloc();
  if (!pkt) {
    fprintf(stderr, "Could not allocate packet context\n");
    return NULL;
  }

  decoder_context *ctx = malloc(sizeof(decoder_context));

  ctx->codec = codec;
  ctx->parser = parser;
  ctx->codec_ctx = codec_ctx;
  ctx->frame = frame;
  ctx->pkt = pkt;
  ctx->callback = (frame_callback)callback;
  return ctx;

FAIL:
  if (parser) {
    av_parser_close(parser);
  }
  if (codec_ctx) {
    avcodec_close(codec_ctx);
    avcodec_free_context(&codec_ctx);
  }
  if (frame) {
    av_frame_free(&frame);
  }
  if (pkt) {
    av_packet_free(&pkt);
  }

  return NULL;
}

void decoder_context_free(decoder_context* ctx) {
  if (!ctx) {
    return;
  }

  if (ctx->parser) {
    av_parser_close(ctx->parser);
  }
  if (ctx->codec_ctx) {
    avcodec_close(ctx->codec_ctx);
    avcodec_free_context(&(ctx->codec_ctx));
  }
  if (ctx->frame) {
    av_frame_free(&(ctx->frame));
  }
  if (ctx->pkt) {
    av_packet_free(&(ctx->pkt));
  }
  free(ctx);
}

static int _decode(AVCodecContext* dec_ctx, AVFrame* frame, AVPacket* pkt, frame_callback callback) {
  char buf[1024];
  int ret;

  ret = avcodec_send_packet(dec_ctx, pkt);
  if (ret < 0) {
    printf("send packet error \n");
    return ErrorCode_FFmpeg_Send_Packet;
  }
  else {
    while (ret >= 0) {
      ret = avcodec_receive_frame(dec_ctx, frame);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        ret = 0;
        break;
      } else if (ret < 0) {
        printf("receive frame error \n");
        ret = ErrorCode_FFmpeg_Receive_Frame;
        break;
      }
      if (callback) {
        callback(frame->data[0], frame->data[1], frame->data[2], frame->linesize[0], frame->linesize[1], frame->linesize[2], frame->width, frame->height, frame->pts);
      }
    }
  }
  return ret;
}

int decode(decoder_context* ctx, unsigned char* data, size_t data_size) {
  int ret = 0;
  while (data_size > 0) {
    int size = av_parser_parse2(ctx->parser, ctx->codec_ctx, &(ctx->pkt->data), &(ctx->pkt->size),
      data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
    if (size < 0) {
      ret = ErrorCode_FFmpeg_Parsing;
    }
    data += size;
    data_size -= size;

    if (ctx->pkt->size) {
      ret = _decode(ctx->codec_ctx, ctx->frame, ctx->pkt, ctx->callback);
      if (ret < 0) {
        break; 
      }
    }
  }
  return ret;
}

void demo_callback(unsigned char* data_y, unsigned char* data_u, unsigned char* data_v, int line1, int line2, int line3, int width, int height, long pts) {
  printf("got a frame, width: %d, height: %d\n", width, height);
}

// nal test
// int main(int argc, char **argv) {
//   decoder_context *ctx = decoder_context_new((long int)demo_callback);
  
//   for (int i=0; i<=689; i++) {
//     char filename[255];
//     sprintf(filename, "../data/dump.%04d", i);
//     FILE *file = fopen(filename, "rb");
//     if (!file) {
//         fprintf(stderr, "Could not open file\n");
//         exit(1);
//     }

//     // 读取NAL
//     char* buf = malloc(1024*1024);
//     char* data = buf;
//     int data_size = fread(buf, 1, 1024*1024, file);
//     if (data_size <= 0) {
//         break;
//     }

//     int ret = decode(ctx, data, data_size);
//     if (ret < 0) {
//       printf("error_code %d\n", ret);
//     }

//     fclose(file);
//   }

//   decoder_context_free(ctx);
// }

// block test
int main(int argc, char **argv) {
  decoder_context *ctx = decoder_context_new((long int)demo_callback);
  
  FILE *file = fopen("../dump", "rb");
  if (!file) {
    fprintf(stderr, "Could not open file\n");
    exit(1);
  }

  int buf_size = 1024 * 1024;
  char* buf = malloc(buf_size);
  int data_size;

  for (;;) {
    char* data = buf;
    int data_size = fread(buf, 1, buf_size, file);
    if (data_size <= 0) {
        break;
    }

    int ret = decode(ctx, data, data_size);
    if (ret < 0) {
      printf("error_code %d\n", ret);
    } 
  }
  fclose(file);
  free(buf);

  decoder_context_free(ctx);
}