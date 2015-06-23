/*
 Class: Smoothing Filter

 Author: Daniel R. Carvalho

 Last Updated: 05/27/2015

 Function: Parses a value through a buffer of previous values to estimate correct value.

 Notice that it doesn't handle overflow, as input values are assumed to be small for this project.
*/

#ifndef SMOOTHING_FILTER
#define SMOOTHING_FILTER

#include <cstdlib>
#include <queue>

class SmoothingFilter{
private:
  // FIFO buffer
  std::queue<int> buffer;

  // Sum of all elements on buffer
  long long sum;

  // Buffer Size
  size_t size;

public:
  // Initialize buffer with given size
  SmoothingFilter(size_t size = 50);

  // Destructor
  ~SmoothingFilter();

  // Clear all buffer data
  void clear();

  // Insert a new value on the buffer, removing the oldest one if needed
  // Returns the smoothened value
  long long smoothen(long long val);
};

#endif