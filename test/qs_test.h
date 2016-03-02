#ifndef LEO_TEST_H
#define LEO_TEST_H 

#include "reflex.h"

struct single_member
{
    int m1;
};

REFLEX_ADAPT_STRUCT( single_member, 
	(int, m1))
#endif
