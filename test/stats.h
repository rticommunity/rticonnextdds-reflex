#ifndef REFLEX_STATS_H
#define REFLEX_STATS_H

#include <vector>
#include <map>

class Stats
{
  std::map<int, int> histogram;
  std::vector<int> keys;
  int total;
  int max;
  double avg;
  double standard_dev;
  bool avg_done;
  bool stddev_done;

public:

  Stats();

  void insert(int val);
  void sort_keys();
  void print_histogram(const char *filename);
  int percentile(double ptile);
  double average();
  double stddev();
  int get_max() const;
};



#endif // REFLEX_STATS_H
