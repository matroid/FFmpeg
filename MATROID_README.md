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

Using global timestamp functions exactly the same as frame pts:

```
ffmpeg -i something -f image2 -frame_pts 1 -strftime 1 -global_timestamp 1 'frame-%%%%t-%s.%%06u-%%%%g.png'
```

## Segment Output

Use integer as filename (already supported in regular FFmpeg):

```
ffmpeg -i something -f segment 'clip-%d.mp4'
```

Use integer as filename, but count starting from some other number:

```
ffmpeg -i something -f segment -segment_start_number 13 'clip-%d.mp4'
```

Use clock time as filename, accurate to micro second level:

```
ffmpeg -i something -f segment -strftime 1 'clip-%s.%%06u.mp4'
```

Use frame pts (presentation timestamp), accurate to micro second level:

```
ffmpeg -i something -f segment -frame_pts 1 'clip-%t.mp4'
```

Duration can also be included when `frame_pts` is enabled:

```
ffmpeg -i something -f segment -frame_pts 1 'clip-%t-%l.mp4'
```

Use both clock time and frame pts, both accurate to micro second level (some special escape is needed):

```
ffmpeg -i something -f segment -frame_pts 1 -strftime 1 'clip-%s.%%06u-%%%%t-%%%%l.mp4'
```

Using global timestamp functions exactly the same as frame pts:

```
ffmpeg -i something -f segment -frame_pts 1 -strftime 1 -global_timestamp 1 'clip-%s.%%06u-%%%%t-%%%%l-%%%%g.mp4'
```

## Credits

- https://github.com/InSoundz/ffmpeg-strftime-milliseconds
- https://trac.ffmpeg.org/ticket/1452
- https://trac.ffmpeg.org/attachment/ticket/1452/0001-Added-the-option-to-extract-images-with-timecode-as-.patch
- https://stackoverflow.com/questions/29527882/ffmpeg-copyts-to-preserve-timestamp
