/**
 * HLS frame splitter with PDT parsing functionality
 * Reference:
 * - https://blog.csdn.net/leixiaohua1020/article/details/38868499
 * - https://gist.github.com/tyraeltong/2309baf41e9546b7d757a6477c236418
 * 
 * Author: tak-matroid@github.com
 */
 
#include <stdio.h>
#include <inttypes.h>
#include <jpeglib.h> 

#define __STDC_CONSTANT_MACROS

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libavutil/frame.h"
#include "libavutil/rational.h"
#include "libswscale/swscale.h"

#define ASSERT(cond, ...) do { \
	if (!(cond)) { \
		printf(__VA_ARGS__); \
		return -1; \
	} \
} while(0)

int process(
	AVStream *stream,
	AVCodecContext *inputCodecCtx,
	AVCodecContext *outputCodecCtx,
	struct SwsContext *img_convert_ctx,
	AVPacket *packet, AVFrame *frame, AVFrame *frameYUV)
{
	sws_scale(
		img_convert_ctx,
		(const unsigned char* const*)frame->data,
		frame->linesize, 0, inputCodecCtx->height, 
		frameYUV->data, frameYUV->linesize
	);
	frameYUV->format = AV_PIX_FMT_YUV420P;
	frameYUV->width  = frame->width;
	frameYUV->height = frame->height;

	double global_timestamp;
	AVFrameSideData *side_data = av_frame_get_side_data(frame, AV_FRAME_DATA_GLOBAL_TIMESTAMP);
	if (side_data != NULL) {
		double *global_timestamp_data = (double *) side_data->data;
		if (global_timestamp_data != NULL) {
			global_timestamp = global_timestamp_data[0];
		}
	}
    
	AVPacket opkt = {.data = NULL, .size = 0};
    av_init_packet(&opkt);
    int gotFrame;
    ASSERT(
		avcodec_encode_video2(outputCodecCtx, &opkt, frameYUV, &gotFrame) >= 0,
		"jpeg encoding failed"
	);

	char jpg_name[32];
    sprintf(jpg_name, "%017.06f.jpg", global_timestamp);
    FILE *jpg_file = fopen(jpg_name, "wb");
    fwrite(opkt.data, 1, opkt.size, jpg_file);
    fclose(jpg_file);
	av_frame_unref(frame);

	return 0;
}

int main(int argc, char* argv[])
{
	ASSERT(
		argc == 2,
		"Usage: ./matroid [path/to/media]\n"
	);
	char *filepath = argv[1];

	printf("#########################\n");
	printf("##### MATROID BEGIN #####\n");
	printf("#########################\n");

	av_register_all();
	avformat_network_init();
	AVFormatContext	*fmtCtx = avformat_alloc_context();
 
	ASSERT(
		avformat_open_input(&fmtCtx, filepath, NULL, NULL) == 0,
		"Couldn't open input stream '%s'.\n", filepath
	);

	ASSERT(
		avformat_find_stream_info(fmtCtx, NULL) == 0,
		"Couldn't find stream information.\n"
	);

	int videoindex = -1;
	for (int i = 0; i < fmtCtx->nb_streams; i++) {
		if (fmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i; break; } }

	ASSERT(videoindex != -1, "Didn't find a video stream.\n");
 
	AVStream 		*stream        = fmtCtx->streams[videoindex];
	AVCodecContext	*inputCodecCtx = stream->codec;
	AVCodec			*inputCodec    = avcodec_find_decoder(inputCodecCtx->codec_id);
	ASSERT(inputCodec != NULL, "Codec not found.\n");
	ASSERT(avcodec_open2(inputCodecCtx, inputCodec, NULL) >= 0, "Could not open codec.\n");

	AVCodec        *outputCodec    = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
	ASSERT(outputCodec != NULL, "Codec not found.\n");
	AVCodecContext *outputCodecCtx = avcodec_alloc_context3(outputCodec);
	ASSERT(outputCodecCtx != NULL, "Could not open codec.\n");
	outputCodecCtx->time_base = AV_TIME_BASE_Q;
	outputCodecCtx->pix_fmt   = AV_PIX_FMT_YUVJ420P;
	outputCodecCtx->width     = inputCodecCtx->width;
	outputCodecCtx->height    = inputCodecCtx->height; 
	outputCodecCtx->flags    |= AV_CODEC_FLAG_QSCALE;
    outputCodecCtx->global_quality = FF_QP2LAMBDA * 1;
	ASSERT(avcodec_open2(outputCodecCtx, outputCodec, NULL) >= 0, "Could not open codec.\n");
	
	AVPacket *packet   = av_packet_alloc();
	AVFrame  *frame    = av_frame_alloc();
	AVFrame  *frameYUV = av_frame_alloc();
	unsigned char *out_buffer = (unsigned char *) av_malloc(
		av_image_get_buffer_size(AV_PIX_FMT_YUV420P, inputCodecCtx->width, inputCodecCtx->height, 1)
	);
	av_image_fill_arrays(frameYUV->data, frameYUV->linesize, out_buffer,
		AV_PIX_FMT_YUV420P, inputCodecCtx->width, inputCodecCtx->height, 1);
	struct SwsContext *img_convert_ctx = sws_getContext(inputCodecCtx->width, inputCodecCtx->height, inputCodecCtx->pix_fmt, 
		inputCodecCtx->width, inputCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 

	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(fmtCtx, 0, filepath, 0);
	printf("-------------------------------------------------\n");

	int got_picture = 0;
	while (av_read_frame(fmtCtx, packet) >= 0) {
		if(packet->stream_index == videoindex){
			ASSERT(
				avcodec_decode_video2(inputCodecCtx, frame, &got_picture, packet),
				"Decode Error.\n"
			);
			if (got_picture) {
				process(stream, inputCodecCtx, outputCodecCtx, img_convert_ctx, packet, frame, frameYUV);
			}
		}
		av_free_packet(packet);
	}
	// Flush last frames remained in codec
	while (avcodec_decode_video2(inputCodecCtx, frame, &got_picture, packet) >= 0 && got_picture) {
		process(stream, inputCodecCtx, outputCodecCtx, img_convert_ctx, packet, frame, frameYUV);
	}
 
	av_frame_free(&frame);
	avcodec_close(inputCodecCtx);
	avcodec_close(outputCodecCtx);
	avformat_close_input(&fmtCtx);
 
	return 0;
}
