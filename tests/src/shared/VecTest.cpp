#include <gtest/gtest.h>

#include <string>

#include "shared/Vec.hpp"

TEST(VecTest, StartsEmpty) {
  Vec<int> values;

  EXPECT_TRUE(values.empty());
  EXPECT_EQ(0u, values.size());
}

TEST(VecTest, PushBackStoresValuesInOrder) {
  Vec<int> values;

  values.push_back(1);
  values.push_back(2);
  values.push_back(3);

  EXPECT_FALSE(values.empty());
  EXPECT_EQ(3u, values.size());
  EXPECT_EQ(1, values.front());
  EXPECT_EQ(2, values[1]);
  EXPECT_EQ(3, values[2]);
}

TEST(VecTest, PopFrontReturnsValuesInOrder) {
  Vec<int> values;

  values.push_back(10);
  values.push_back(20);
  values.push_back(30);

  EXPECT_EQ(10, values.pop_front());
  EXPECT_EQ(20, values.pop_front());
  EXPECT_EQ(30, values.pop_front());
  EXPECT_TRUE(values.empty());
}

TEST(VecTest, ReserveKeepsExistingValues) {
  Vec<int> values;

  values.push_back(5);
  values.push_back(6);
  values.reserve(16);

  EXPECT_EQ(2u, values.size());
  EXPECT_EQ(5, values.front());
  EXPECT_EQ(6, values[1]);
}

TEST(VecTest, CopyConstructorCopiesContents) {
  Vec<std::string> values;
  values.push_back("alpha");
  values.push_back("beta");

  Vec<std::string> copy(values);

  EXPECT_EQ(2u, copy.size());
  EXPECT_EQ("alpha", copy[0]);
  EXPECT_EQ("beta", copy[1]);
}

TEST(VecTest, AssignmentOperatorCopiesContents) {
  Vec<std::string> values;
  values.push_back("left");

  Vec<std::string> other;
  other.push_back("right");
  other.push_back("side");

  values = other;

  EXPECT_EQ(2u, values.size());
  EXPECT_EQ("right", values[0]);
  EXPECT_EQ("side", values[1]);
}

TEST(VecTest, ClearEmptiesVector) {
  Vec<std::string> values;
  values.push_back("one");
  values.push_back("two");

  values.clear();

  EXPECT_TRUE(values.empty());
  EXPECT_EQ(0u, values.size());
}

TEST(VecTest, SupportsStringValues) {
  Vec<std::string> values;

  values.push_back("hello");
  values.push_back("world");

  EXPECT_EQ("hello", values.front());
  EXPECT_EQ("world", values[1]);
}