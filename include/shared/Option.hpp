#pragma once

#include <string>

/**
 * @brief Represents an optional value: either present (Some) or absent (None).
 *
 * Option stores at most one value of type T and makes absence explicit in the
 * type system. This helps avoid sentinel values and makes error-prone "maybe"
 * states explicit at call sites.
 *
 * Typical usage starts with `Some(value)` or `None()`, then checks state with
 * `is_some()` / `is_none()`, and finally extracts with `unwrap_or(...)` or
 * `expect(...)` depending on how strict extraction should be.
 *
 * @tparam T Stored value type.
 *
 * ```c++
 * Option<int> parsed = Option<int>::Some(42);
 * if (parsed.is_some()) {
 *   int value = parsed.unwrap();
 * }
 *
 * Option<int> missing = Option<int>::None();
 * int fallback = missing.unwrap_or(0); // fallback == 0
 * ```
 */
template <typename T>
class Option {
 public:
  /**
   * @brief Constructs a None value.
   *
   * The resulting option has no contained value.
   */
  Option();

  /**
   * @brief Constructs a Some value from @p value.
   *
   * @param value Value to store.
   */
  Option(const T& value);

  /**
   * @brief Creates a Some value from @p value.
   *
   * This is equivalent to calling `Option(value)` and is provided for explicit,
   * self-documenting construction.
   *
   * @param value Value to store.
   * @return An option containing @p value.
   */
  static Option Some(const T& value);

  /**
   * @brief Creates a None value.
   *
   * @return An empty option.
   */
  static Option None();

  /**
   * @brief Returns true when this option contains a value.
   *
   * @return true if the option is Some; false otherwise.
   */
  bool is_some() const;

  /**
   * @brief Returns true when this option is empty.
   *
   * @return true if the option is None; false otherwise.
   */
  bool is_none() const;

  /**
   * @brief CamelCase alias of is_some().
   *
   * @return true if the option is Some; false otherwise.
   */
  bool isSome() const;

  /**
   * @brief CamelCase alias of is_none().
   *
   * @return true if the option is None; false otherwise.
   */
  bool isNone() const;

  /**
   * @brief Returns true if this is Some and the contained value equals @p
   * value.
   *
   * This is a convenience predicate for exact-value checks.
   *
   * @param value Value to compare against the contained value.
   * @return true when Some and equal to @p value; false otherwise.
   */
  bool is_some_and(const T& value) const;

  /**
   * @brief Returns true if this is None or the contained value equals @p value.
   *
   * @param value Value to compare against when this option is Some.
   * @return true when None, or when Some and equal to @p value.
   */
  bool is_none_or(const T& value) const;

  /**
   * @brief Returns a reference to the contained value.
   *
   * Use this only when a value is guaranteed to be present. Prefer
   * `unwrap_or(...)` or `expect(...)` when handling absence explicitly.
   *
   * @return Reference to the contained value.
   * @throws std::runtime_error if called on None.
   *
   * ```c++
   * Option<std::string> name = Option<std::string>::Some("kireji");
   * const std::string& value = name.unwrap();
   * ```
   */
  const T& unwrap() const;

  /**
   * @brief Returns the contained value or @p defaultValue if None.
   *
   * @param defaultValue Value returned when this option is None.
   * @return Contained value when Some; otherwise @p defaultValue.
   *
   * ```c++
   * Option<int> maybePort = Option<int>::None();
   * int port = maybePort.unwrap_or(8080); // port == 8080
   * ```
   */
  T unwrap_or(const T& defaultValue) const;

  /**
   * @brief CamelCase alias of unwrap_or().
   *
   * @param defaultValue Value returned when this option is None.
   * @return Contained value when Some; otherwise @p defaultValue.
   */
  T unwrapOr(const T& defaultValue) const;

  /**
   * @brief Returns the contained value or computes a fallback.
   *
   * The fallback function is evaluated only when this option is None.
   *
   * @param defaultFunc Function called to produce a default value for None.
   * @return Contained value when Some; otherwise `defaultFunc()`.
   *
   * ```c++
   * int defaultPort() { return 8080; }
   * Option<int> maybePort = Option<int>::None();
   * int port = maybePort.unwrap_or_else(defaultPort); // port == 8080
   * ```
   */
  T unwrap_or_else(T (*defaultFunc)()) const;

  /**
   * @brief CamelCase alias of unwrap_or_else().
   *
   * @param defaultFunc Function called to produce a default value for None.
   * @return Contained value when Some; otherwise `defaultFunc()`.
   */
  T unwrapOrElse(T (*defaultFunc)()) const;

  /**
   * @brief Returns the contained value or a default-constructed T.
   *
   * Equivalent to `unwrap_or(T())`.
   *
   * @return Contained value when Some; otherwise `T()`.
   */
  T unwrap_or_default() const;

  /**
   * @brief CamelCase alias of unwrap_or_default().
   *
   * @return Contained value when Some; otherwise `T()`.
   */
  T unwrapOrDefault() const;

  /**
   * @brief Returns a reference to the contained value or throws with message.
   *
   * This method is useful when failure should include domain-specific context.
   *
   * @param message Error message used when this option is None.
   * @return Reference to the contained value.
   * @throws std::runtime_error if called on None.
   *
   * ```c++
   * Option<std::string> token = Option<std::string>::None();
   * // Throws std::runtime_error("Token must be present")
   * token.expect("Token must be present");
   * ```
   */
  const T& expect(const std::string& message) const;

  /**
   * @brief Takes the current value out, leaving this option as None.
   *
   * After this call, the returned option contains the previous state and this
   * instance becomes None.
   *
   * @return Previous state of this option.
   */
  Option take();

  /**
   * @brief Replaces the current value with @p value.
   *
   * The current option state is returned, and this instance becomes
   * Some(value).
   *
   * @param value New value to store.
   * @return Previous state of this option.
   */
  Option replace(const T& value);

  /**
   * @brief Chains another option-producing operation.
   *
   * If this is Some(v), returns `next(v)`. If this is None, returns None
   * without calling @p next.
   *
   * @param next Function called only when this option is Some.
   * @return Result of @p next for Some, or None for None.
   *
   * ```c++
   * Option<int> plusOneIfPositive(const int& v) {
   *   return v > 0 ? Option<int>::Some(v + 1) : Option<int>::None();
   * }
   *
   * Option<int> a = Option<int>::Some(10);
   * Option<int> b = a.and_then(plusOneIfPositive); // Some(11)
   * ```
   */
  Option and_then(Option (*next)(const T& value)) const;

  /**
   * @brief Returns this option when Some, otherwise computes a fallback option.
   *
   * The fallback function is evaluated only when this option is None.
   *
   * @param fallback Function called to produce a fallback option.
   * @return This option when Some; otherwise `fallback()`.
   */
  Option or_else(Option (*fallback)()) const;

 private:
  bool has_value_;
  T value_;
};
