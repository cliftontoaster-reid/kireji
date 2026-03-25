#pragma once

#include <cctype>
#include <cstddef>
#include <string>

#include "shared/Vec.hpp"

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
    Node* hash_next;  //!< Next node in the same hash bucket chain.

    /**
     * @brief Creates a detached node containing @p n and @p v.
     *
     * @param n Header name to store in lowercase.
     * @param v Header value to store.
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
   * @param other Header map to copy from.
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
   *
   * @param other Header map to copy from.
   * @return Reference to this header map.
   */
  HeaderMap& operator=(const HeaderMap& other);

  /**
   * @brief Removes every header from the map.
   */
  void clear();

  /**
   * @brief Returns the value for @p name, or NULL when the header is missing.
   *
   * @param name Header name to look up.
   *
   * The returned pointer refers to storage owned by the map and remains valid
   * until the map is mutated or destroyed.
   *
   * @return Pointer to the stored value, or NULL if not found.
   */
  const std::string* get(const std::string& name) const;

  /**
   * @brief Inserts or replaces a header value.
   *
   * @param name Header name to insert or replace.
   * @param value Header value to store.
   *
   * If @p name already exists, only the value is updated and the insertion
   * order is preserved. Otherwise a new node is appended to the end of the
   * list.
   */
  void set(const std::string& name, const std::string& value);

  /**
   * @brief Removes the header named @p name.
   *
   * @param name Header name to remove.
   *
   * @return true when a header was removed; false when no matching header was
   * found.
   */
  bool remove(const std::string& name);

  /**
   * @brief Returns the number of stored headers.
   *
   * @return Number of stored headers.
   */
  std::size_t size() const { return size_; }

  /**
   * @brief Returns the first node in insertion order, or NULL when empty.
   *
   * @return Pointer to the first node, or NULL when empty.
   */
  Node* head() const { return head_; }

 private:
  enum {
    kInitialBucketCount = 16,
    kMaxLoadPercent = 70,
  };

  Node* head_;
  Node* tail_;
  std::size_t size_;
  Vec<Node*> buckets_;
  std::size_t bucket_count_;

  /**
   * @brief Copies all nodes from @p other into this map.
   *
   * @param other Header map to copy from.
   */
  void copyFrom(const HeaderMap& other);

  void initializeBuckets(std::size_t count);
  void maybeRehashForInsert();
  void rehash(std::size_t newBucketCount);
  std::size_t bucketIndex(const std::string& lowerName) const;
  Node* findInBucket(const std::string& lowerName, std::size_t index);
  const Node* findInBucket(const std::string& lowerName,
                           std::size_t index) const;
  static std::size_t hashLowerName(const std::string& lowerName);

  /**
   * @brief Returns a lowercase copy of @p s.
   *
   * @param s String to convert.
   * @return Lowercase copy of @p s.
   */
  static std::string toLower(const std::string& s);
};
