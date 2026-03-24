#include "shared/Option.hpp"

#include <stdexcept>

template <typename T>
Option<T>::Option() : has_value_(false), value_() {}

template <typename T>
Option<T>::Option(const T& value) : has_value_(true), value_(value) {}

template <typename T>
Option<T> Option<T>::Some(const T& value) {
  return Option<T>(value);
}

template <typename T>
Option<T> Option<T>::None() {
  return Option<T>();
}

template <typename T>
bool Option<T>::is_some() const {
  return has_value_;
}

template <typename T>
bool Option<T>::is_none() const {
  return !has_value_;
}

template <typename T>
bool Option<T>::isSome() const {
  return is_some();
}

template <typename T>
bool Option<T>::isNone() const {
  return is_none();
}

template <typename T>
bool Option<T>::is_some_and(const T& value) const {
  return has_value_ && value_ == value;
}

template <typename T>
bool Option<T>::is_none_or(const T& value) const {
  return !has_value_ || value_ == value;
}

template <typename T>
const T& Option<T>::unwrap() const {
  if (!has_value_) {
    throw std::runtime_error("Called unwrap on None");
  }
  return value_;
}

template <typename T>
T Option<T>::unwrap_or(const T& defaultValue) const {
  return has_value_ ? value_ : defaultValue;
}

template <typename T>
T Option<T>::unwrapOr(const T& defaultValue) const {
  return unwrap_or(defaultValue);
}

template <typename T>
T Option<T>::unwrap_or_else(T (*defaultFunc)()) const {
  return has_value_ ? value_ : defaultFunc();
}

template <typename T>
T Option<T>::unwrapOrElse(T (*defaultFunc)()) const {
  return unwrap_or_else(defaultFunc);
}

template <typename T>
T Option<T>::unwrap_or_default() const {
  return has_value_ ? value_ : T();
}

template <typename T>
T Option<T>::unwrapOrDefault() const {
  return unwrap_or_default();
}

template <typename T>
const T& Option<T>::expect(const std::string& message) const {
  if (!has_value_) {
    throw std::runtime_error(message);
  }
  return value_;
}

template <typename T>
Option<T> Option<T>::take() {
  Option<T> current(*this);
  has_value_ = false;
  return current;
}

template <typename T>
Option<T> Option<T>::replace(const T& value) {
  Option<T> current(*this);
  has_value_ = true;
  value_ = value;
  return current;
}

template <typename T>
Option<T> Option<T>::and_then(Option<T> (*next)(const T& value)) const {
  return has_value_ ? next(value_) : Option<T>();
}

template <typename T>
Option<T> Option<T>::or_else(Option<T> (*fallback)()) const {
  return has_value_ ? *this : fallback();
}

template class Option<std::string>;
template class Option<int>;
template class Option<bool>;
template class Option<char>;
template class Option<long>;
template class Option<unsigned long>;
template class Option<float>;
template class Option<double>;