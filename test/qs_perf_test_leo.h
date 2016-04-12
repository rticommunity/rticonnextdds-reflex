#ifndef QS_PERF_TEST_LEO_H
#define QS_PERF_TEST_LEO_H 

#include "reflex.h"

struct PerfHelloWorld {
  unsigned int messageId;
  long timestamp;

  PerfHelloWorld()
  {
    timestamp = 0;
    messageId = 0;
  }
};

REFLEX_ADAPT_STRUCT(PerfHelloWorld,
    (unsigned int, messageId) 
    (long ,timestamp));

#endif // QS_PERF_TEST_LEO_H

