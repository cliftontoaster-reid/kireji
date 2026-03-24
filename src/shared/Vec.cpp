#include "shared/Vec.hpp"

class HttpRequest;

#include <string>

template <typename T>
Vec<T>::Vec() : data_(NULL), size_(0), capacity_(0) {}

template <typename T>
Vec<T>::Vec(const Vec& other) : data_(NULL), size_(0), capacity_(0) {
  reserve(other.size_);
  for (std::size_t i = 0; i < other.size_; ++i) {
    push_back(other.data_[i]);
  }
}

template <typename T>
Vec<T>::~Vec() {
  clear();
  ::operator delete(data_);
}

template <typename T>
Vec<T>& Vec<T>::operator=(const Vec& other) {
  if (this != &other) {
    clear();
    reserve(other.size_);
    for (std::size_t i = 0; i < other.size_; ++i) {
      push_back(other.data_[i]);
    }
  }
  return *this;
}

template <typename T>
void Vec<T>::reserve(std::size_t newCapacity) {
  if (newCapacity <= capacity_) return;

  T* newData = static_cast<T*>(::operator new(sizeof(T) * newCapacity));
  std::size_t constructed = 0;
  std::size_t oldSize = size_;

  try {
    for (; constructed < size_; ++constructed) {
      new (newData + constructed) T(data_[constructed]);
    }
  } catch (...) {
    for (std::size_t i = 0; i < constructed; ++i) {
      newData[i].~T();
    }
    ::operator delete(newData);
    throw;
  }

  clearStorage();
  data_ = newData;
  size_ = oldSize;
  capacity_ = newCapacity;
}

template <typename T>
void Vec<T>::push_back(const T& value) {
  if (size_ == capacity_) {
    reserve(capacity_ == 0 ? 4 : capacity_ * 2);
  }

  new (data_ + size_) T(value);
  ++size_;
}

template <typename T>
T Vec<T>::pop_front() {
  if (size_ == 0) {
    throw std::runtime_error("Vec::pop_front on empty vector");
  }

  T value = data_[0];

  for (std::size_t i = 1; i < size_; ++i) {
    data_[i - 1] = data_[i];
  }

  data_[size_ - 1].~T();
  --size_;
  return value;
}

template <typename T>
bool Vec<T>::empty() const {
  return size_ == 0;
}

template <typename T>
std::size_t Vec<T>::size() const {
  return size_;
}

template <typename T>
T& Vec<T>::front() {
  if (size_ == 0) {
    throw std::runtime_error("Vec::front on empty vector");
  }
  return data_[0];
}

template <typename T>
const T& Vec<T>::front() const {
  if (size_ == 0) {
    throw std::runtime_error("Vec::front on empty vector");
  }
  return data_[0];
}

template <typename T>
T& Vec<T>::operator[](std::size_t index) {
  return data_[index];
}

template <typename T>
const T& Vec<T>::operator[](std::size_t index) const {
  return data_[index];
}

template <typename T>
void Vec<T>::clear() {
  for (std::size_t i = 0; i < size_; ++i) {
    data_[i].~T();
  }
  size_ = 0;
}

template <typename T>
void Vec<T>::clearStorage() {
  clear();
  ::operator delete(data_);
  data_ = NULL;
  capacity_ = 0;
}

template class Vec<int>;
template class Vec<std::string>;
template class Vec<HttpRequest*>;