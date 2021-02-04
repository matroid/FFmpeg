#!/bin/bash
make && make build && gcc hls_parser.c -g -o hls_parser \
  -I . \
  -L ./libavformat -L ./libavcodec -L ./libavutil -L libswscale -L libswresample \
  -pthread -lavformat -lavcodec -lavutil -lswscale -lm -lz -llzma -lbz2 -liconv -lswresample \
  -framework VideoToolbox -framework AudioToolbox \
  -framework AVFoundation -framework CoreVideo \
  -framework CoreMedia -framework CoreGraphics \
  -framework OpenGL -framework CoreImage \
  -framework Security -framework CoreServices
