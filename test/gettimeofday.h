#ifndef GET_TIME_OF_DAY_H
#define GET_TIME_OF_DAY_H

struct timeval;
struct timezone
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif 