#ifndef QS_HELLO_TEST_H
#define QS_HELLO_TEST_H 

#include <vector>
#include "reflex.h"

struct HelloWorld{
  unsigned int messageId;
  std::vector<reflex::match::octet_t> payload;
};

REFLEX_ADAPT_STRUCT(HelloWorld,
    (unsigned int, messageId)
    (std::vector<reflex::match::octet_t>,payload));

#endif // QS_HELLO_TEST_H

