#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>


static void save_jpeg(AVFrame *frame, const char *filename) {
    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    codec_ctx->width = frame->width;
    codec_ctx->height = frame->height;
    codec_ctx->pix_fmt = AV_PIX_FMT_YUVJ420P;
    codec_ctx->time_base = (AVRational){1, 25};

    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Could not open output file\n");
        exit(1);
    }

    AVPacket pkt = {0};
    av_init_packet(&pkt);
    
    int ret = avcodec_send_frame(codec_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending frame to codec context\n");
        exit(1);
    }

    ret = avcodec_receive_packet(codec_ctx, &pkt);
    if (ret < 0) {
        fprintf(stderr, "Error receiving packet from codec context\n");
        exit(1);
    }

    fwrite(pkt.data, 1, pkt.size, file);
    fclose(file);

    av_packet_unref(&pkt);
    avcodec_close(codec_ctx);
    avcodec_free_context(&codec_ctx);
}

static int decode(AVCodecContext* dec_ctx, AVFrame* frame, AVPacket* pkt)
{
	char buf[1024];
	int ret;

	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0) {
    printf("send packet error \n");
		return ret;
	}
	else {
		while (ret >= 0) {
			ret = avcodec_receive_frame(dec_ctx, frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        ret = 0;
				break;
			}
			else if (ret < 0) {
        printf("receive frame error \n");
				break;
			}

      // save frame as jpeg file
      char filename[256];
      sprintf(filename, "frames/frame_%d.jpeg", dec_ctx->frame_number);
      save_jpeg(frame, filename);
		}
	}
	return ret;
}


int main(int argc, char **argv) {
    // ffmpeg 3.1 deprecated
    // av_register_all();
    // avcodec_register_all();

    // 找到 H264 解码器
    AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    AVCodecParserContext *parser = av_parser_init(codec->id);
		if (!parser) {
        fprintf(stderr, "CodecParser not found\n");
        exit(1);
		}

    // 创建编解码器上下文
    AVCodecContext *codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    // 打开编解码器
    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    // 创建帧和数据包
    AVFrame *frame = av_frame_alloc();
    AVPacket pkt;

    // 循环读取和解码数据包
    for (int i=0; i<=689; i++) {
        char filename[255];
        sprintf(filename, "./data/dump.%04d", i);
        FILE *file = fopen(filename, "rb");
        if (!file) {
            fprintf(stderr, "Could not open file\n");
            exit(1);
        }

        // 读取NAL
        char* buf = malloc(1024*1024);
        char* data = buf;
        int data_size = fread(buf, 1, 1024*1024, file);
        if (data_size <= 0) {
            break;
        }

        while (data_size > 0) {
          int size = av_parser_parse2(parser, codecContext, &(pkt.data), &(pkt.size),
            data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
          if (size < 0) {
            fprintf(stderr, "Error while parsing\n");
            exit(1);
          }
          data += size;
          data_size -= size;

          if (pkt.size) {
            int ret = decode(codecContext, frame, &pkt);
            if (ret < 0) {
              fprintf(stderr, "Error while decode %s\n", av_err2str(ret));
            }
          }
        }

        fclose(file);
    }

    // 清理和关闭
    av_frame_free(&frame);
    avcodec_close(codecContext);
    avcodec_free_context(&codecContext);

    return 0;
}