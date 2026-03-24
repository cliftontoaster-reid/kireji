#pragma once

#include <cctype>
#include <cstddef>
#include <string>

class HeaderMap {
 public:
  struct Node {
    std::string name;
    std::string value;
    Node* prev;
    Node* next;
    Node(const std::string& n, const std::string& v);
  };

  HeaderMap();
  HeaderMap(const HeaderMap& other);
  ~HeaderMap();

  HeaderMap& operator=(const HeaderMap& other);

  void clear();

  const std::string* get(const std::string& name) const;

  void set(const std::string& name, const std::string& value);

  bool remove(const std::string& name);

  std::size_t size() const { return size_; }

  Node* head() const { return head_; }

 private:
  Node* head_;
  Node* tail_;
  std::size_t size_;

  void copyFrom(const HeaderMap& other);

  static std::string toLower(const std::string& s);
};
