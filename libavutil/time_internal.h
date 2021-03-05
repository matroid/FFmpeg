/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVUTIL_TIME_INTERNAL_H
#define AVUTIL_TIME_INTERNAL_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#if !HAVE_GMTIME_R && !defined(gmtime_r)
static inline struct tm *ff_gmtime_r(const time_t* clock, struct tm *result)
{
    struct tm *ptr = gmtime(clock);
    if (!ptr)
        return NULL;
    *result = *ptr;
    return result;
}
#define gmtime_r ff_gmtime_r
#endif

#if !HAVE_LOCALTIME_R && !defined(localtime_r)
static inline struct tm *ff_localtime_r(const time_t* clock, struct tm *result)
{
    struct tm *ptr = localtime(clock);
    if (!ptr)
        return NULL;
    *result = *ptr;
    return result;
}
#define localtime_r ff_localtime_r
#endif

static inline time_t ff_timegm(struct tm *tm)
{
  time_t ret;
  char *tz;

  tz = getenv("TZ");
  if (tz)
      tz = strdup(tz);
  setenv("TZ", "", 1);
  tzset();
  ret = mktime(tm);
  if (tz) {
      setenv("TZ", tz, 1);
      free(tz);
  } else
      unsetenv("TZ");
  tzset();
  return ret;
}

#endif /* AVUTIL_TIME_INTERNAL_H */
