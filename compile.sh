#!/bin/bash
gcc matroid.c -g -o matroid.out \
  -I /usr/local/include -L /usr/local/lib \
  -pthread -lavformat -lavcodec -lavutil -lswscale \
  -lm -lz -llzma -lbz2 -liconv -lswresample \
  -framework VideoToolbox -framework AudioToolbox \
  -framework AVFoundation -framework CoreVideo \
  -framework CoreMedia -framework CoreGraphics \
  -framework OpenGL -framework CoreImage \
  -framework Security -framework CoreServices

# printf "CC\t%s\n" fftools/ffmpeg.o; gcc -I. -I./ -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -I./compat/dispatch_semaphore -DPIC -DZLIB_CONST -std=c11 -Werror=partial-availability -fomit-frame-pointer -fPIC -pthread  -g -Wdeclaration-after-statement -Wall -Wdisabled-optimization -Wpointer-arith -Wredundant-decls -Wwrite-strings -Wtype-limits -Wundef -Wmissing-prototypes -Wno-pointer-to-int-cast -Wstrict-prototypes -Wempty-body -Wno-parentheses -Wno-switch -Wno-format-zero-length -Wno-pointer-sign -Wno-unused-const-variable -O3 -fno-math-errno -fno-signed-zeros -mstack-alignment=16 -Qunused-arguments -Werror=implicit-function-declaration -Werror=missing-prototypes -Werror=return-type -D_THREAD_SAFE -I/usr/local/include/SDL2   -MMD -MF fftools/ffmpeg.d -MT fftools/ffmpeg.o -c -o fftools/ffmpeg.o fftools/ffmpeg.c
# printf "LD\t%s\n" ffmpeg_g; gcc -Llibavcodec -Llibavdevice -Llibavfilter -Llibavformat -Llibavresample -Llibavutil -Llibpostproc -Llibswscale -Llibswresample -Wl,-dynamic,-search_paths_first -Qunused-arguments   -o ffmpeg_g fftools/ffmpeg_opt.o fftools/ffmpeg_filter.o fftools/ffmpeg_hw.o fftools/cmdutils.o fftools/ffmpeg.o fftools/ffmpeg_videotoolbox.o -lavdevice -lavfilter -lavformat -lavcodec -lswresample -lswscale -lavutil  -framework Foundation -lm -framework AVFoundation -framework CoreVideo -framework CoreMedia -pthread -framework CoreGraphics -L/usr/local/lib -lSDL2 -framework OpenGL -framework OpenGL -pthread -lm -framework CoreImage -framework AppKit -lm -lbz2 -lz -Wl,-framework,CoreFoundation -Wl,-framework,Security -liconv -lm -llzma -lz -framework AudioToolbox -pthread -framework VideoToolbox -framework CoreFoundation -framework CoreMedia -framework CoreVideo -framework CoreServices -lm -lm -pthread -lm -framework VideoToolbox -framework CoreFoundation -framework CoreMedia -framework CoreVideo -framework CoreServices
# printf "STRIP\t%s\n" ffmpeg; strip -x -o ffmpeg ffmpeg_g
