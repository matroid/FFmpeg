# pylint: disable=undefined-variable
""" Call Stack """

# 1. opens input and gets context
AVFormatContext = avformat_open_input(path)

# 2. obtain streams information
avformat_find_stream_info()

# 3. obtain decoder
AVCodec = avcodec_find_decoder(AVCodecContext)

# 4. initialize decoder
avcodec_open(AVCodecContext, AVCodec)

# 5. read one packet
AVPacket = av_read_frame(AVFormatContext)
read_frame_internal(AVFormatContext)
ff_read_packet(AVFormatContext)
AVFormatContext.AVInputFormat.read_packet(AVFormatContext)
hls_read_packet(AVFormatContext)
av_read_frame(AVFormatContext.HLSContext.PlayList.AVFormatContext)
  # it would go through the same loop and land at AVInputFormat.read_packet
  # so we need to figure out what that one is calling and where segment comes in
  # initialized in hls_read_header, called by AVInputFormat.read_header
  # ffio_init_context hints that AVFormatContext.AVIOContext.read_packet is (hls_)read_data
  # av_probe_input_buffer likely sets in_fmt (which is likely another HLS)
  # avformat_open_input likely sets pls.ctx as in_fmt


# 6. read one frame
AVFrame = avcodec_decode_video2(AVCodecContext, AVPacket)
