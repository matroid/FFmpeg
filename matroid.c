
/**
 * Modified from "Simple FFmpeg Player 2"
 * Source: https://blog.csdn.net/leixiaohua1020/article/details/38868499
 * Original Author: 雷霄骅 Lei Xiaohua
 * Author: Jiahang (Tak) Li
 */
 
#include <stdio.h>
#include <inttypes.h>
 
#define __STDC_CONSTANT_MACROS

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/frame.h"

#define ASSERT(cond, ...) do { \
	if (!(cond)) { \
		printf(__VA_ARGS__); \
		return -1; \
	} \
} while(0)

void process(
	struct SwsContext *img_convert_ctx,
	AVStream *pStream, AVCodecContext *pCodecCtx,
	AVPacket *packet, AVFrame *pFrame, AVFrame *pFrameYUV)
{
	sws_scale(
		img_convert_ctx,
		(const unsigned char* const*)pFrame->data,
		pFrame->linesize, 0, pCodecCtx->height, 
		pFrameYUV->data, pFrameYUV->linesize
	);

	uint8_t global_timestamp = 0;
	AVFrameSideData *side_data = av_frame_get_side_data(pFrame, AV_FRAME_DATA_GLOBAL_TIMESTAMP);
	if (side_data != NULL) {
		uint8_t *data = side_data->data;
		if (data != NULL && side_data->size > 0) {
			global_timestamp = data[0];
		}
	}
	printf(
		"Frame pkt.pts=%f pkt.dts=%f frame.pts=%f frame.side_data.global_timestamp=%"PRIu8"\n",
		packet->pts * av_q2d(pStream->time_base),
		packet->dts * av_q2d(pStream->time_base),
		pFrame->pts * av_q2d(pStream->time_base),
		global_timestamp
	);
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

	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame,*pFrameYUV;
	AVStream *pStream;
	unsigned char *out_buffer;
	AVPacket *packet;
	int y_size;
	int got_picture;
	struct SwsContext *img_convert_ctx;
 
	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
 
	ASSERT(
		avformat_open_input(&pFormatCtx, filepath, NULL, NULL) == 0,
		"Couldn't open input stream '%s'.\n", filepath
	);

	ASSERT(
		avformat_find_stream_info(pFormatCtx, NULL) == 0,
		"Couldn't find stream information.\n"
	);

	videoindex = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i; break; } }

	ASSERT(
		videoindex != -1,
		"Didn't find a video stream.\n"
	);
 
	pStream   = pFormatCtx->streams[videoindex];
	pCodecCtx = pStream->codec;
	pCodec	= avcodec_find_decoder(pCodecCtx->codec_id);
	ASSERT(
		pCodec != NULL,
		"Codec not found.\n"
	);
	ASSERT(
		avcodec_open2(pCodecCtx, pCodec,NULL) >= 0,
		"Could not open codec.\n"
	);
	
	pFrame	 = av_frame_alloc();
	pFrameYUV  = av_frame_alloc();
	out_buffer = (unsigned char *) av_malloc(
		av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1)
	);
	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
		AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx,0,filepath,0);
	printf("-------------------------------------------------\n");
	
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 

	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if(packet->stream_index == videoindex){
			ASSERT(
				avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet),
				"Decode Error.\n"
			);
			if (got_picture) {
				process(img_convert_ctx, pStream, pCodecCtx, packet, pFrame, pFrameYUV);
			}
		}
		av_free_packet(packet);
	}
	// Flush last frames remained in codec
	while (avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet) >= 0 && got_picture) {
		process(img_convert_ctx, pStream, pCodecCtx, packet, pFrame, pFrameYUV);
	}
 
	sws_freeContext(img_convert_ctx);
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
 
	return 0;
}
