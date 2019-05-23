#include "strftime_micro.h"

size_t strftime_micro(char *buf, size_t size, const char *format, const struct timeval *tv)
{
  struct tm *tm;
  char temp_name[size];

  if ((tm = localtime(&(tv->tv_sec))) == NULL)
    return 0;

  strftime(temp_name, size, format, tm);
  return snprintf(buf, size, temp_name, tv->tv_usec);
}
