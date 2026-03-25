#include "shared/HeaderMap.hpp"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

// MurmurHash2 (32-bit) by Austin Appleby.
// License: Public domain (see https://github.com/aappleby/smhasher).
static uint32_t MurmurHash2(const void* key, std::size_t len, uint32_t seed) {
  if (!seed) seed = 0x9747b28cU;
  const uint32_t m = 0x5bd1e995U;
  const int r = 24;
  uint32_t h = static_cast<uint32_t>(seed ^ len);

  const unsigned char* data = static_cast<const unsigned char*>(key);

  while (len >= 4) {
    uint32_t k;
    memcpy(&k, data, sizeof(k));

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  switch (len) {
    case 3:
      h ^= (uint32_t)data[2] << 16;
      /* fall through */
    case 2:
      h ^= (uint32_t)data[1] << 8;
      /* fall through */
    case 1:
      h ^= (uint32_t)data[0];
      h *= m;
      break;
    default:
      break;
  }

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

static const uint32_t kHeaderMapMurmurSeed = 0x9747b28cU;

HeaderMap::Node::Node(const std::string& n, const std::string& v)
    : name(n), value(v), prev(NULL), next(NULL), hash_next(NULL) {}

HeaderMap::HeaderMap() : head_(NULL), tail_(NULL), size_(0), bucket_count_(0) {
  initializeBuckets(kInitialBucketCount);
}

HeaderMap::HeaderMap(const HeaderMap& other)
    : head_(NULL), tail_(NULL), size_(0), bucket_count_(0) {
  initializeBuckets(kInitialBucketCount);
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
  for (std::size_t i = 0; i < bucket_count_; ++i) {
    buckets_[i] = NULL;
  }
  head_ = NULL;
  tail_ = NULL;
  size_ = 0;
}

const std::string* HeaderMap::get(const std::string& name) const {
  if (!bucket_count_) return NULL;
  std::string lname = toLower(name);
  std::size_t index = bucketIndex(lname);
  const Node* match = findInBucket(lname, index);
  if (match) return &match->value;
  return NULL;
}

void HeaderMap::set(const std::string& name, const std::string& value) {
  std::string lname = toLower(name);

  maybeRehashForInsert();
  std::size_t index = bucketIndex(lname);
  Node* existing = findInBucket(lname, index);
  if (existing) {
    existing->value = value;
    return;
  }

  Node* n = new Node(lname, value);
  if (!tail_) {
    head_ = tail_ = n;
  } else {
    tail_->next = n;
    n->prev = tail_;
    tail_ = n;
  }

  n->hash_next = buckets_[index];
  buckets_[index] = n;
  ++size_;
}

bool HeaderMap::remove(const std::string& name) {
  if (!bucket_count_) return false;

  std::string lname = toLower(name);
  std::size_t index = bucketIndex(lname);
  Node* prevBucket = NULL;
  Node* cur = buckets_[index];

  while (cur) {
    if (cur->name == lname) {
      if (prevBucket)
        prevBucket->hash_next = cur->hash_next;
      else
        buckets_[index] = cur->hash_next;

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
    prevBucket = cur;
    cur = cur->hash_next;
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

void HeaderMap::initializeBuckets(std::size_t count) {
  if (count == 0) count = kInitialBucketCount;

  buckets_.clear();
  buckets_.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    buckets_.push_back(NULL);
  }
  bucket_count_ = count;
}

void HeaderMap::maybeRehashForInsert() {
  if (!bucket_count_) {
    initializeBuckets(kInitialBucketCount);
  }

  if ((size_ + 1) * 100 > bucket_count_ * kMaxLoadPercent) {
    rehash(bucket_count_ * 2);
  }
}

void HeaderMap::rehash(std::size_t newBucketCount) {
  if (newBucketCount < kInitialBucketCount) {
    newBucketCount = kInitialBucketCount;
  }

  Vec<Node*> newBuckets;
  newBuckets.reserve(newBucketCount);
  for (std::size_t i = 0; i < newBucketCount; ++i) {
    newBuckets.push_back(NULL);
  }

  Node* cur = head_;
  while (cur) {
    std::size_t index = hashLowerName(cur->name) % newBucketCount;
    cur->hash_next = newBuckets[index];
    newBuckets[index] = cur;
    cur = cur->next;
  }

  buckets_ = newBuckets;
  bucket_count_ = newBucketCount;
}

std::size_t HeaderMap::bucketIndex(const std::string& lowerName) const {
  if (!bucket_count_) return 0;
  return hashLowerName(lowerName) % bucket_count_;
}

HeaderMap::Node* HeaderMap::findInBucket(const std::string& lowerName,
                                         std::size_t index) {
  Node* cur = buckets_[index];
  while (cur) {
    if (cur->name == lowerName) return cur;
    cur = cur->hash_next;
  }
  return NULL;
}

const HeaderMap::Node* HeaderMap::findInBucket(const std::string& lowerName,
                                               std::size_t index) const {
  const Node* cur = buckets_[index];
  while (cur) {
    if (cur->name == lowerName) return cur;
    cur = cur->hash_next;
  }
  return NULL;
}

std::size_t HeaderMap::hashLowerName(const std::string& lowerName) {
  return static_cast<std::size_t>(
      MurmurHash2(lowerName.data(), lowerName.size(), kHeaderMapMurmurSeed));
}