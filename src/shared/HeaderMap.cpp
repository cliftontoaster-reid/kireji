#include "shared/HeaderMap.hpp"

HeaderMap::Node::Node(const std::string& n, const std::string& v)
    : name(n), value(v), prev(NULL), next(NULL) {}

HeaderMap::HeaderMap() : head_(NULL), tail_(NULL), size_(0) {}

HeaderMap::HeaderMap(const HeaderMap& other)
    : head_(NULL), tail_(NULL), size_(0) {
  copyFrom(other);
}

HeaderMap::~HeaderMap() { clear(); }

HeaderMap& HeaderMap::operator=(const HeaderMap& other) {
  if (this != &other) {
    clear();
    copyFrom(other);
  }
  return *this;
}

void HeaderMap::clear() {
  Node* cur = head_;
  while (cur) {
    Node* nxt = cur->next;
    delete cur;
    cur = nxt;
  }
  head_ = tail_ = NULL;
  size_ = 0;
}

const std::string* HeaderMap::get(const std::string& name) const {
  std::string lname = toLower(name);
  Node* cur = head_;
  while (cur) {
    if (cur->name == lname) return &cur->value;
    cur = cur->next;
  }
  return NULL;
}

void HeaderMap::set(const std::string& name, const std::string& value) {
  std::string lname = toLower(name);
  Node* cur = head_;
  while (cur) {
    if (cur->name == lname) {
      cur->value = value;
      return;
    }
    cur = cur->next;
  }
  Node* n = new Node(lname, value);
  if (!tail_) {
    head_ = tail_ = n;
  } else {
    tail_->next = n;
    n->prev = tail_;
    tail_ = n;
  }
  ++size_;
}

bool HeaderMap::remove(const std::string& name) {
  std::string lname = toLower(name);
  Node* cur = head_;
  while (cur) {
    if (cur->name == lname) {
      if (cur->prev)
        cur->prev->next = cur->next;
      else
        head_ = cur->next;
      if (cur->next)
        cur->next->prev = cur->prev;
      else
        tail_ = cur->prev;
      delete cur;
      --size_;
      return true;
    }
    cur = cur->next;
  }
  return false;
}

void HeaderMap::copyFrom(const HeaderMap& other) {
  Node* cur = other.head_;
  while (cur) {
    set(cur->name, cur->value);
    cur = cur->next;
  }
}

std::string HeaderMap::toLower(const std::string& s) {
  std::string r(s);
  for (std::size_t i = 0; i < r.size(); ++i)
    r[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(r[i])));
  return r;
}