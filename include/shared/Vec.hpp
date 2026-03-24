#pragma once

#include <cstddef>
#include <new>
#include <stdexcept>

template <typename T>
class Vec {
 public:
  Vec();

  Vec(const Vec& other);

  ~Vec();

  Vec& operator=(const Vec& other);

  void reserve(std::size_t newCapacity);

  void push_back(const T& value);

  T pop_front();

  bool empty() const;

  std::size_t size() const;

  T& front();

  const T& front() const;

  T& operator[](std::size_t index);

  const T& operator[](std::size_t index) const;

  void clear();

 private:
  T* data_;
  std::size_t size_;
  std::size_t capacity_;

  void clearStorage();
};