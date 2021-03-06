#include "Profiler.h"

#ifdef _WIN32
// need to undef ERROR to avoid redefinition between R and Windows gdi.h
#undef ERROR
#endif
#include <R.h>

#include <vector>
#include <algorithm>

#include "SimpleTimer.h"

std::map<std::string, Profiler::Metric> Profiler::data;

void Profiler::addTimer(const char* func) {
  Metric& m = data[func];
  m.nHits++;
  m.timer.start();
}

void Profiler::deleteTimer(const char* func) {
  Metric& m = data[func];
  m.timer.stop();
}

struct FlatMetric{
  FlatMetric(const std::string& func, int nHits, double elapsed) {
    this->func = func;
    this->nHits = nHits;
    this->avgElapsed = elapsed / nHits;
    this->totalElapsed = elapsed;
  }
  std::string func;
  int nHits;
  double avgElapsed;
  double totalElapsed;
};

struct FlatMetricCompare{
  bool operator()(const FlatMetric& a, const FlatMetric& b) {
    return a.avgElapsed > b.avgElapsed;
  }
} flatMetricCompare;


void Profiler::dump() {
  std::vector< FlatMetric > v;
  for (std::map<std::string, Metric>::iterator it = data.begin();
       it != data.end();
       ++it) {
    v.push_back(FlatMetric(it->first, it->second.nHits, it->second.timer.getSeconds()));
  }
  std::sort(v.begin(), v.end(), flatMetricCompare);
  for (size_t i = 0; i != v.size(); ++i) {
    const FlatMetric& x = v[i];
    REprintf(
        "Function [ %s ] hit [ %d ] times, total elapsed time [ %g ] seconds, avg elapsed time [ %g ] seconds\n",
        x.func.c_str(), x.nHits, x.totalElapsed, x.avgElapsed);
  }
}
