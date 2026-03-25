#pragma once

#include <cctype>
#include <cstddef>
#include <string>

/**
 * @brief Insertion-ordered, case-insensitive map for HTTP headers.
 *
 * Header names are normalized to lowercase when inserted. Lookups and removals
 * accept any case, but each header is stored once and keeps its original
 * position in insertion order.
 *
 * @code
 * HeaderMap headers;
 * headers.set("Host", "example.com");
 * headers.set("Content-Length", "12");
 * const std::string* host = headers.get("host");
 * @endcode
 */
class HeaderMap {
 public:
  /**
   * @brief Link node used to preserve insertion order.
   *
   * The name is stored in lowercase so that lookups remain case-insensitive.
   */
  struct Node {
    std::string name;   //!< Header name stored in lowercase.
    std::string value;  //!< Header value as provided by the caller.
    Node* prev;  //!< Previous node in insertion order, or NULL for the first
                 //!< node.
    Node* next;  //!< Next node in insertion order, or NULL for the last node.

    /**
     * @brief Creates a detached node containing @p n and @p v.
     */
    Node(const std::string& n, const std::string& v);
  };

  /**
   * @brief Constructs an empty header map.
   */
  HeaderMap();

  /**
   * @brief Copies all headers from @p other.
   *
   * The new map owns its own nodes.
   */
  HeaderMap(const HeaderMap& other);

  /**
   * @brief Destroys all stored headers.
   */
  ~HeaderMap();

  /**
   * @brief Replaces the current headers with a copy of @p other.
   */
  HeaderMap& operator=(const HeaderMap& other);

  /**
   * @brief Removes every header from the map.
   */
  void clear();

  /**
   * @brief Returns the value for @p name, or NULL when the header is missing.
   *
   * The returned pointer refers to storage owned by the map and remains valid
   * until the map is mutated or destroyed.
   */
  const std::string* get(const std::string& name) const;

  /**
   * @brief Inserts or replaces a header value.
   *
   * If @p name already exists, only the value is updated and the insertion
   * order is preserved. Otherwise a new node is appended to the end of the
   * list.
   */
  void set(const std::string& name, const std::string& value);

  /**
   * @brief Removes the header named @p name.
   *
   * @return true when a header was removed; false when no matching header was
   * found.
   */
  bool remove(const std::string& name);

  /**
   * @brief Returns the number of stored headers.
   */
  std::size_t size() const { return size_; }

  /**
   * @brief Returns the first node in insertion order, or NULL when empty.
   */
  Node* head() const { return head_; }

 private:
  Node* head_;
  Node* tail_;
  std::size_t size_;

  void copyFrom(const HeaderMap& other);

  static std::string toLower(const std::string& s);
};
