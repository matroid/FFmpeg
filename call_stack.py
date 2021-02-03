""" Matroid Call Stack Analysis """
# pylint: disable=undefined-variable

"""
Things to blame:
- C language has unclear function input/output (all pointers in argument list)
- Pseudo object-oriented programming done via structs and function pointers
- Unclear control flow with return value rather than throw exception
  (sometimes you expect 0, sometimes you expect >0)
- Lots of annoying goto statements
- Separation of implementation and declaration, also still missing documentation
- Not many great resources for learning the API systematically
"""

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
  # using opaque pointer, read_data manages different segments
  # av_probe_input_buffer likely sets in_fmt (which is likely another HLS)
  # avformat_open_input likely sets pls.ctx as in_fmt
mpegts_read_packet(AVFormatContext)
  # ultimately we have mpegts AVFormatContext calling HLSAVIOContext.read_packet
(hls_)read_data(HLSPlayList)
  # this is where segment swapping occurs
  # the weird loop would break after read_from_url
  # we'll have positive ret corresponding to bytes read
read_from_url(HLSPlayList, HLSSegment, Buffer)
  open_input(HLSContext, HLSPlayList, Segment, AVIOContext)
  open_url(AVFormatContext, AVIOContext, ...)
  AVFormatContext.io_open # defines HLSPlayList.input
  io_open_default
avio_read(AVIOContext HLSPlayList.input)

# 6. read one frame
AVFrame = avcodec_decode_video2(AVCodecContext, AVPacket)

# Misc
(decode.c) ff_decode_frame_props # sets f->pkt_pts = pkt->pts
# the add_metadata_from_side_data can be key to our success !!!
# this is another breakthrough

(frame.c) av_frame_new_side_data, av_frame_get_side_data
