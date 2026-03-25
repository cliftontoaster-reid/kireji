#pragma once

#include <cstddef>
#include <new>
#include <stdexcept>

/**
 * @brief Small C++98-compatible dynamic array with FIFO-style front removal.
 *
 * Vec stores elements contiguously, grows automatically when needed, and keeps
 * values in insertion order. It is intentionally minimal: it provides
 * appending, indexed access, reserving capacity, and removing the oldest
 * element from the front.
 *
 * @code
 * Vec<int> values;
 * values.push_back(10);
 * values.push_back(20);
 * int first = values.pop_front(); // first == 10
 * @endcode
 *
 * @tparam T Stored element type.
 */
template <typename T>
class Vec {
 public:
  /**
   * @brief Constructs an empty vector.
   */
  Vec();

  /**
   * @brief Copies all elements from @p other.
   *
   * @param other Vector to copy from.
   *
   * The new vector owns its own storage and elements.
   */
  Vec(const Vec& other);

  /**
   * @brief Destroys all stored elements and releases the allocated storage.
   */
  ~Vec();

  /**
   * @brief Replaces the current contents with a copy of @p other.
   *
   * @param other Vector to copy from.
   * @return Reference to this vector.
   *
   * The new vector owns its own storage and elements.
   */
  Vec& operator=(const Vec& other);

  /**
   * @brief Ensures the vector can store at least @p newCapacity elements.
   *
   * @param newCapacity Minimum capacity to reserve.
   *
   * Existing elements are preserved. This function never shrinks the allocated
   * storage.
   */
  void reserve(std::size_t newCapacity);

  /**
   * @brief Appends @p value to the end of the vector.
   *
   * @param value Element to append.
   *
   * The vector grows automatically when it runs out of capacity.
   */
  void push_back(const T& value);

  /**
   * @brief Removes and returns the first element in the vector.
   *
   * Remaining elements are shifted left by one position.
   *
   * @return The element that was previously at the front of the vector.
   * @throws std::runtime_error if the vector is empty.
   */
  T pop_front();

  /**
   * @brief Returns true when the vector contains no elements.
   *
   * @return true when the vector contains no elements; false otherwise.
   */
  bool empty() const;

  /**
   * @brief Returns the number of stored elements.
   *
   * @return Number of stored elements.
   */
  std::size_t size() const;

  /**
   * @brief Returns a reference to the first element.
   *
   * @return Reference to the first element.
   * @throws std::runtime_error if the vector is empty.
   */
  T& front();

  /**
   * @brief Returns a const reference to the first element.
   *
   * @return Const reference to the first element.
   * @throws std::runtime_error if the vector is empty.
   */
  const T& front() const;

  /**
   * @brief Returns a reference to the element at @p index.
   *
   * @param index Zero-based element index.
   * @return Reference to the element at @p index.
   *
   * This accessor does not perform bounds checking.
   */
  T& operator[](std::size_t index);

  /**
   * @brief Returns a const reference to the element at @p index.
   *
   * @param index Zero-based element index.
   * @return Const reference to the element at @p index.
   *
   * This accessor does not perform bounds checking.
   */
  const T& operator[](std::size_t index) const;

  /**
   * @brief Removes every element from the vector.
   *
   * The allocated storage is kept for reuse.
   */
  void clear();

 private:
  T* data_;
  std::size_t size_;
  std::size_t capacity_;
  std::size_t offset_;

  void clearStorage();
};
