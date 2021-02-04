#!/bin/bash
./ffmpeg -i /Users/tak/Developer/example_hls_video/pdt.m3u8 \
  -f image2  -frame_pts 1 -strftime 1 -global_timestamp 1 'test/frame_%%%%t_%s.%%06u_%%%%g.png' \
  -f segment -frame_pts 1 -strftime 1 -global_timestamp 1 'test/clip_%s.%%06u_%%%%t_%%%%l_%%%%g.mp4'
