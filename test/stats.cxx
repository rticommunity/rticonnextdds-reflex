#include "stats.h"
#include <algorithm>
#include <fstream>
#include <stdexcept>

Stats::Stats()
{
  total = 0;
  max = -1;
  avg = -1;
  standard_dev = -1;
  avg_done = false;
  stddev_done = false;
}

void Stats::insert(int val)
{
  if (keys.empty())
  {
    histogram[val]++;
    total++;
    if (max < val)
      max = val;
  }
  else
    throw std::runtime_error("Stats::insert Can't change the histogram anymore");
}

void Stats::sort_keys()
{
  if (keys.empty())
  {
    keys.reserve(histogram.size());

    for (std::map<int, int>::iterator iter = histogram.begin();
    iter != histogram.end();
      ++iter)
    {
      keys.push_back(iter->first);
    }
    std::sort(keys.begin(), keys.end());
  }
}

void Stats::print_histogram(const char *filename)
{
  std::ofstream file(filename);
  sort_keys();

  for (std::vector<int>::const_iterator iter = keys.begin();
  iter != keys.end();
    ++iter)
  {
    file << "[" << *iter << "] = " << histogram[*iter] << "\n";
  }
}

int Stats::percentile(double ptile)
{
  if (ptile <= 0.0 || ptile > 1.0)
    return -1;

  sort_keys();

  long count = 0;
  for (std::vector<int>::reverse_iterator iter = keys.rbegin();
  iter != keys.rend();
    ++iter)
  {
    count += histogram[*iter];
    if ((double) count / total >= (1.0 - ptile))
      return *iter;
  }
  return -1;
}

double Stats::average()
{
  if (!avg_done)
  {
    sort_keys();

    double sum = 0;
    int count = 0;
    for (std::vector<int>::iterator iter = keys.begin();
    iter != keys.end();
      ++iter)
    {
      sum += (*iter * histogram[*iter]);
      count += histogram[*iter];
    }
    avg = sum / total;
    avg_done = true;
  }
  return avg;
}

double Stats::stddev()
{
  if (!stddev_done)
  {
    double avg = average();
    double sum_of_sqr = 0;
    for (std::vector<int>::iterator iter = keys.begin();
    iter != keys.end();
      ++iter)
    {
      double diff = *iter - avg;
      sum_of_sqr += diff*diff*histogram[*iter];
    }
    standard_dev = sqrt(sum_of_sqr / total);
    stddev_done = true;
  }
  return standard_dev;
}

int Stats::get_max() const
{
  return max;
}

