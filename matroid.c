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
	AVPacket *packet, AVFrame *frame)
{
	double global_timestamp;
	AVFrameSideData *side_data = av_frame_get_side_data(frame, AV_FRAME_DATA_GLOBAL_TIMESTAMP);
	if (side_data != NULL) {
		double *global_timestamp_data = (double *) side_data->data;
		if (global_timestamp_data != NULL) {
			global_timestamp = global_timestamp_data[0];
		}
	}
	/*printf(
		"Frame pkt.pts=%f pkt.dts=%f frame.pts=%f frame.global_timestamp=%f\n",
		packet->pts * av_q2d(stream->time_base),
		packet->dts * av_q2d(stream->time_base),
		frame->pts * av_q2d(stream->time_base),
		global_timestamp
	);*/

    
	AVPacket opkt = {.data = NULL, .size = 0};
    av_init_packet(&opkt);
    int gotFrame;
    ASSERT(
		avcodec_encode_video2(outputCodecCtx, &opkt, frame, &gotFrame) >= 0,
		"jpeg encoding failed"
	);

	char jpg_name[32];
    sprintf(jpg_name, "%017.06f.jpg", global_timestamp);
    FILE *jpg_file = fopen(jpg_name, "wb");
	// printf("writing to file %s\n", jpg_name);
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
	outputCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
	outputCodecCtx->width  = inputCodecCtx->width;
	outputCodecCtx->height = inputCodecCtx->height; 
	ASSERT(avcodec_open2(outputCodecCtx, outputCodec, NULL) >= 0, "Could not open codec.\n");
	
	AVFrame  *frame  = av_frame_alloc();
	AVPacket *packet = av_packet_alloc();

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
				process(stream, inputCodecCtx, outputCodecCtx, packet, frame);
			}
		}
		av_free_packet(packet);
	}
	// Flush last frames remained in codec
	while (avcodec_decode_video2(inputCodecCtx, frame, &got_picture, packet) >= 0 && got_picture) {
		process(stream, inputCodecCtx, outputCodecCtx, packet, frame);
	}
 
	av_frame_free(&frame);
	avcodec_close(inputCodecCtx);
	avcodec_close(outputCodecCtx);
	avformat_close_input(&fmtCtx);
 
	return 0;
}
