#pragma once

#include <type_traits>
#include <utility>

namespace xpto {

template <typename F>
struct fret;
template <typename R, typename ... Args>
struct fret<R(*)(Args...) noexcept> : std::type_identity<R> {};
template <typename R, typename ... Args>
struct fret<R(*)(Args...)> : std::type_identity<R> {};
template <typename R, typename ... Args>
struct fret<R(*)(Args...,...) noexcept> : std::type_identity<R> {};
template <typename R, typename ... Args>
struct fret<R(*)(Args...,...)> : std::type_identity<R> {};

template <auto F>
using fret_t = fret<decltype(F)>::type;

/** @brief a wrapper for C-style system stuff
 *
 * Heavily adapted from
 * https://github.com/DanielaE/CppInAction/blob/7961360303c51b254f7c9718381968e234f835a8/Demo-App/c_resource.hpp
 *
 * - Works with any handle type, not just pointer.
 *
 * - Instead of working with two schemas, works only with the more
 *   common one where the constructor return a handle (usually a
 *   pointer).
 *
 * - Removed constexpr (are C-resources ver going to be compile-time?)
 *
 * - Removed what seemed like overkill const-correctness antics (for
 *   C++23.)
 *
 * - Can be default-constructed, and I put
 *   the "Invalid" as a NTTP instead of separate template
 *   customization point object..
 *
 * - Like DanielaE's version, can be constructed with 0 arguments via
 *   'noargs_construct' tag found via ADL.
 *
 * - Removed the 'guard' subclass template.  Don't really see that
 *   much use.  See also Arthur O'Dwyer "C++11 in the Wild: Techniques
 *   from a Real Codebase" on youtube, could be better.
 *
 * - Removed some "standard" ops like non-member 'have'.
 *
 */
template <auto* Construct, auto* Destruct, fret_t<Construct> Invalid = {}>
class c_resource {
  using value_type = fret_t<Construct>;
  using Constructor = decltype(Construct);
  using Destructor = decltype(Destruct);
  value_type payload_ = Invalid;

  struct construct_t {};

  void destruct() {
    if (payload_ != Invalid)
      Destruct(payload_);
  }

public:
  static constexpr construct_t noargs_construct{};

  c_resource() noexcept = default;

  explicit c_resource(construct_t) noexcept : payload_{Construct()} {}

  template <typename... Args>
  requires(sizeof...(Args) > 0 &&
           std::is_invocable_r_v<value_type, decltype(Construct), Args...>)
  explicit(sizeof...(Args) == 1) c_resource(Args&&... args) noexcept
  : payload_{Construct(std::forward<Args>(args)...)} {}

  c_resource(const c_resource&) = delete;
  c_resource& operator=(const c_resource&) = delete;

  c_resource(c_resource&& o) noexcept : payload_{o.payload_} {
    o.payload_ = Invalid;
  };
  c_resource& operator=(c_resource&& rhs) noexcept {
    swap(this, rhs); // see https://quick-bench.com/q/GDbjgEXAzWV5Md17mBC-2oINdv0 and
    // and https://godbolt.org/z/K9eGThMxK
    return *this;
  };
  friend void swap(c_resource& lhs, c_resource& rhs) noexcept {
    std::swap(lhs.payload_, rhs.payload_);
  }

  ~c_resource() { destruct(); }
  void clear() {
    destruct();
    payload_ = Invalid;
  }
  c_resource& operator=(std::nullptr_t) noexcept {
    clear();
    return *this;
  }

  explicit operator bool() const noexcept { return payload_ != Invalid; }
  bool empty() const noexcept { return payload_ == Invalid; }
  operator value_type() noexcept { return payload_; }
  operator const value_type() const noexcept { return payload_; }

  void reset(value_type ptr = Invalid) {
    Destruct(payload_);
    payload_ = ptr;
  }

  value_type release() {
    auto ptr = payload_;
    payload_ = Invalid;
    return ptr;
  }
};

}  // namespace xpto
