#include <SmoothingFilter.hpp>

SmoothingFilter::SmoothingFilter(size_t size){
  this->size = size;
  clear();
}

void SmoothingFilter::clear(){
  sum = 0;

  std::queue<int> emptyQ;
  std::swap(buffer, emptyQ);
}

long long SmoothingFilter::smoothen(long long val){
  // If buffer is full, remove oldest value
  if (buffer.size() == size) {
    sum -= buffer.front();
    buffer.pop();
  }
  // Add new value
  buffer.push(val);
  sum += buffer.back();

  return sum/buffer.size();
}