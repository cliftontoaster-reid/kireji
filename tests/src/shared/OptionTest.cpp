#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "shared/Option.hpp"

static Option<int> returnFortyTwo(const int&) { return Option<int>::Some(42); }

static Option<int> returnNoneForAndThen(const int&) {
  return Option<int>::None();
}

static int fallbackFortyThreeValue() { return 43; }

static Option<int> fallbackFortyThreeOption() { return Option<int>::Some(43); }

TEST(OptionTest, NoneStartsEmpty) {
  Option<int> value;

  EXPECT_TRUE(value.is_none());
  EXPECT_FALSE(value.is_some());
}

TEST(OptionTest, SomeStoresValue) {
  Option<std::string> value = Option<std::string>::Some("hello");

  EXPECT_TRUE(value.is_some());
  EXPECT_FALSE(value.is_none());
  EXPECT_EQ("hello", value.unwrap());
}

TEST(OptionTest, UnwrapThrowsOnNone) {
  Option<int> value = Option<int>::None();

  EXPECT_THROW(value.unwrap(), std::runtime_error);
}

TEST(OptionTest, UnwrapOrReturnsFallbackWhenEmpty) {
  Option<int> value = Option<int>::None();

  EXPECT_EQ(7, value.unwrap_or(7));
  EXPECT_EQ(8, value.unwrapOr(8));
}

TEST(OptionTest, UnwrapOrElseUsesFallbackFunction) {
  Option<int> value = Option<int>::None();

  EXPECT_EQ(43, value.unwrap_or_else(fallbackFortyThreeValue));
  EXPECT_EQ(43, value.unwrapOrElse(fallbackFortyThreeValue));
}

TEST(OptionTest, ExpectReturnsValueOrThrowsMessage) {
  Option<std::string> present = Option<std::string>::Some("value");
  Option<std::string> absent = Option<std::string>::None();

  EXPECT_EQ("value", present.expect("should not throw"));
  EXPECT_THROW(absent.expect("missing value"), std::runtime_error);
}

TEST(OptionTest, TakeReturnsPreviousValueAndLeavesNone) {
  Option<int> value = Option<int>::Some(12);

  Option<int> taken = value.take();

  EXPECT_TRUE(value.is_none());
  EXPECT_TRUE(taken.is_some());
  EXPECT_EQ(12, taken.unwrap());
}

TEST(OptionTest, ReplaceReturnsPreviousValueAndStoresNewOne) {
  Option<int> value = Option<int>::Some(1);

  Option<int> previous = value.replace(2);

  EXPECT_TRUE(value.is_some());
  EXPECT_EQ(2, value.unwrap());
  EXPECT_TRUE(previous.is_some());
  EXPECT_EQ(1, previous.unwrap());
}

TEST(OptionTest, AndThenChainsWhenPresent) {
  Option<int> value = Option<int>::Some(3);

  Option<int> result = value.and_then(returnFortyTwo);

  EXPECT_TRUE(result.is_some());
  EXPECT_EQ(42, result.unwrap());
}

TEST(OptionTest, AndThenSkipsWhenEmpty) {
  Option<int> value = Option<int>::None();

  Option<int> result = value.and_then(returnNoneForAndThen);

  EXPECT_TRUE(result.is_none());
}

TEST(OptionTest, OrElseReturnsFallbackWhenEmpty) {
  Option<int> value = Option<int>::None();

  Option<int> result = value.or_else(fallbackFortyThreeOption);

  EXPECT_TRUE(result.is_some());
  EXPECT_EQ(43, result.unwrap());
}

TEST(OptionTest, IsSomeAndIsNoneAliasesMatchCamelCase) {
  Option<int> someValue = Option<int>::Some(9);
  Option<int> noneValue = Option<int>::None();

  EXPECT_TRUE(someValue.isSome());
  EXPECT_FALSE(someValue.isNone());
  EXPECT_TRUE(noneValue.isNone());
  EXPECT_FALSE(noneValue.isSome());
}