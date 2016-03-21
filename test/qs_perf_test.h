#ifndef QS_PERF_TEST_H
#define QS_PERF_TEST_H 

#include <vector>
#include "reflex.h"

struct PerfHelloWorld {
  unsigned int messageId;
  std::vector<reflex::match::octet_t> payload;
};

REFLEX_ADAPT_STRUCT(PerfHelloWorld,
    (unsigned int, messageId)
    (std::vector<reflex::match::octet_t>,payload));

#endif // QS_PERF_TEST_H

