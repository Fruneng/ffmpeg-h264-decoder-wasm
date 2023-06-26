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
    AVPacket packet;

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
        uint8_t inbuf[1024*1024];
        int bytesRead = fread(inbuf, 1, 1024*1024, file);
        if (bytesRead <= 0) {
            break;
        }

        // 设置数据包数据
        packet.data = inbuf;
        packet.size = bytesRead;

        // 发送数据包到解码器
        int ret = avcodec_send_packet(codecContext, &packet);
        if (ret < 0) {
            fprintf(stderr, "%s\n", av_err2str(ret));
            // fprintf(stderr, "Error sending a packet for decoding\n");
            // exit(1);make
        }

        // 获取解码后的帧
        while (avcodec_receive_frame(codecContext, frame) == 0) {
            printf("Frame %3d\n", codecContext->frame_number);
            // 在这里，你可以访问 frame->data 来获取图像数据
            char frame_name[256];
            sprintf(frame_name, "frame_%04d.jpeg", codecContext->frame_number);
            save_jpeg(frame, frame_name);
        }

        fclose(file);
    }

    // 清理和关闭
    av_frame_free(&frame);
    avcodec_close(codecContext);
    avcodec_free_context(&codecContext);

    return 0;
}