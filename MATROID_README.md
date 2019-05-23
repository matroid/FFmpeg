FFmpeg is modified in order to extract as much information of timestamps as possible from videos and live streams.

## Image2 Output

Use integer as filename (already supported in regular FFmpeg):

```
ffmpeg -i something -f image2 'frame-%d.jpg'
```

Use integer as filename, but count starting from some other number:

```
ffmpeg -i something -f image2 -start_number 13 'frame-%d.jpg'
```

Use clock time as filename, accurate to micro second level:

```
ffmpeg -i something -f image2 -strftime 1 'frame-%s.%%06u.png'
```

Use frame pts (presentation timestamp), accurate to micro second level:

```
ffmpeg -i something -f image2 -frame_pts 1 'frame-%t'.png
```

Use both clock time and frame pts, both accurate to micro second level (some special escape is needed):

```
ffmpeg -i something -f image2 -frame_pts 1 -strftime 1 'frame-%%%%t-%s.%%06u.png'
```
