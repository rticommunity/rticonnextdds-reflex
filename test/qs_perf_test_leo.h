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

class Stats
{
  std::map<int, int> histogram;
  double total;
  std::vector<int> keys;

  public:

  Stats(int size = 10000)
  {
    total = 0;
  }

  void insert(int val)
  {
    if(keys.empty())
    {
      histogram[val]++;
      total++;
    }
    else
      throw std::runtime_error("Stats::insert Can't change the histogram anymore");
  }

  void sort_keys()
  {
    keys.reserve(histogram.size());

    for(std::map<int, int>::iterator iter = histogram.begin();
        iter != histogram.end();
        ++iter)
    {
      keys.push_back(iter->first);  
    }
    std::sort(keys.begin(), keys.end());
  }

  void print_histogram() 
  {
    if(keys.empty())
      sort_keys();

    std::cout << "printing histogram\n";

    for(std::vector<int>::const_iterator iter = keys.begin();
        iter != keys.end();
        ++iter)
    {
      printf("[%d] = %d\n", *iter, histogram[*iter]);
    }

  }

  int percentile(double ptile)
  {
    if(ptile <= 0.0 || ptile > 1.0)
      return -1;

    if(keys.empty())
      sort_keys();

    long count = 0;
    for(std::vector<int>::reverse_iterator iter = keys.rbegin();
        iter != keys.rend();
        ++iter)
    {
      count += histogram[*iter];
      if(count/total >= (1.0-ptile))
        return *iter;
    }
    return -1;
  }

  double average()
  {
    if(keys.empty())
      sort_keys();

    double sum = 0;
    int count = 0;
    for(std::vector<int>::iterator iter = keys.begin();
        iter != keys.end();
        ++iter)
    {
      sum += (*iter * histogram[*iter]);
      count += histogram[*iter];
    }
    return sum/count;
  }

};

double sleep_period(int hertz) 
{
  return 1.0/hertz;
}

#endif // QS_PERF_TEST_LEO_H

