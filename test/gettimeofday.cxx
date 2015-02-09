#include "gettimeofday.h"

#include <time.h>

//#include "soccer.h"
//#include "soccerSupport.h"
#include "ndds/ndds_cpp.h"

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;
  long tzdiff = 0;
  int daylight = 0;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /*converting file time to unix epoch*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tmpres /= 10;  /*convert into microseconds*/
    tv->tv_sec = (long) (tmpres / 1000000UL);
    tv->tv_usec = (long) (tmpres % 1000000UL);
  }

  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    
    //tz->tz_minuteswest = _timezone / 60; // unsafe
    //tz->tz_dsttime = _daylight;          // unsafe
    
    _get_timezone(&tzdiff);
    _get_daylight(&daylight);
    tz->tz_minuteswest = tzdiff / 60;
    tz->tz_dsttime = daylight;
  }

  return 0;
}
