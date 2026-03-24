#include <gtest/gtest.h>

#include <string>

#include "shared/HeaderMap.hpp"

TEST(HeaderMapTest, StartsEmpty) {
  HeaderMap headers;

  EXPECT_EQ(0u, headers.size());
  EXPECT_EQ(NULL, headers.get("Host"));
  EXPECT_EQ(NULL, headers.head());
}

TEST(HeaderMapTest, StoresAndRetrievesHeadersCaseInsensitively) {
  HeaderMap headers;

  headers.set("Host", "example.com");
  headers.set("Content-Length", "12");

  ASSERT_TRUE(headers.get("host") != NULL);
  ASSERT_TRUE(headers.get("CONTENT-LENGTH") != NULL);
  EXPECT_EQ("example.com", *headers.get("host"));
  EXPECT_EQ("12", *headers.get("CONTENT-LENGTH"));
  EXPECT_EQ(2u, headers.size());
}

TEST(HeaderMapTest, ReplacesExistingHeaderWithoutGrowing) {
  HeaderMap headers;

  headers.set("X-Test", "one");
  headers.set("x-test", "two");

  EXPECT_EQ(1u, headers.size());
  ASSERT_TRUE(headers.get("X-TEST") != NULL);
  EXPECT_EQ("two", *headers.get("X-TEST"));
}

TEST(HeaderMapTest, RemovesExistingHeader) {
  HeaderMap headers;

  headers.set("A", "1");
  headers.set("B", "2");

  EXPECT_TRUE(headers.remove("a"));
  EXPECT_EQ(1u, headers.size());
  EXPECT_EQ(NULL, headers.get("A"));
  ASSERT_TRUE(headers.get("b") != NULL);
  EXPECT_EQ("2", *headers.get("b"));
}

TEST(HeaderMapTest, RemoveMissingHeaderReturnsFalse) {
  HeaderMap headers;

  headers.set("A", "1");

  EXPECT_FALSE(headers.remove("missing"));
  EXPECT_EQ(1u, headers.size());
}

TEST(HeaderMapTest, CopyConstructorMakesIndependentCopy) {
  HeaderMap original;
  original.set("A", "1");
  original.set("B", "2");

  HeaderMap copy(original);
  copy.set("A", "updated");
  copy.remove("B");

  ASSERT_TRUE(original.get("A") != NULL);
  ASSERT_TRUE(original.get("B") != NULL);
  EXPECT_EQ("1", *original.get("A"));
  EXPECT_EQ("2", *original.get("B"));

  EXPECT_EQ(2u, original.size());
  EXPECT_EQ(1u, copy.size());
}

TEST(HeaderMapTest, AssignmentOperatorMakesIndependentCopy) {
  HeaderMap left;
  left.set("A", "1");

  HeaderMap right;
  right.set("B", "2");
  right.set("C", "3");

  left = right;
  left.set("B", "updated");

  ASSERT_TRUE(right.get("B") != NULL);
  EXPECT_EQ("2", *right.get("B"));
  EXPECT_EQ(2u, left.size());
  EXPECT_EQ(2u, right.size());
}

TEST(HeaderMapTest, ClearEmptiesMap) {
  HeaderMap headers;

  headers.set("A", "1");
  headers.set("B", "2");
  headers.clear();

  EXPECT_EQ(0u, headers.size());
  EXPECT_EQ(NULL, headers.get("A"));
  EXPECT_EQ(NULL, headers.head());
}

TEST(HeaderMapTest, PreservesInsertionOrderInLinkedList) {
  HeaderMap headers;

  headers.set("First", "1");
  headers.set("Second", "2");
  headers.set("Third", "3");

  ASSERT_TRUE(headers.head() != NULL);
  EXPECT_EQ("first", headers.head()->name);
  ASSERT_TRUE(headers.head()->next != NULL);
  EXPECT_EQ("second", headers.head()->next->name);
  ASSERT_TRUE(headers.head()->next->next != NULL);
  EXPECT_EQ("third", headers.head()->next->next->name);
}