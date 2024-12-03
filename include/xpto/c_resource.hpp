#pragma once

#include <type_traits>
#include <utility>

namespace xpto {

  /**
   *
   * Stolen and adapted from
   * https://github.com/DanielaE/CppInAction/blob/7961360303c51b254f7c9718381968e234f835a8/Demo-App/c_resource.hpp
   *
   * - Removed constexpr (are C-resources ver going to be compile-time?)
   *
   * - Removed what seemed like overkill const-correctness antics (for
   *   C++23.)
   *
   * - Can be default-constructed, and I put the "Invalid" as a NTTP
   *   instead of separate template customization point object.
   *
   * - Can be constructed with 0 arguments via 'noargs_construct' tag
   *   found via
   *
   * - Removed the 'guard' subclass template.  Don't really see that
   *   much use.  See also Arthur O'Dwyer "C++11 in the Wild:
   *   Techniques from a Real Codebase" on youtube.
   *
   * - Removed som "standard" ops like non-member 'have'.
   *
   * - Use std::swap 

   *
   *
   * ADL.  Or with more arguments as usual.   */
  template <typename T, auto* Construct, auto* Destruct, T* Invalid = {}>
  class c_resource {
    using Constructor = decltype(Construct);
    using Destructor = decltype(Destruct);
    T* payload_ = Invalid;

    struct noargs_construct_s {};

    void destruct() {
      if (payload_ != Invalid)
        Destruct(payload_);
    }

   public:
    static constexpr noargs_construct_s noargs_construct;

    c_resource() noexcept = default;

    explicit c_resource(noargs_construct_s) : payload_{Construct()} {}

    template <typename... Args>
      requires(sizeof...(Args) > 0 &&
               std::is_invocable_r_v<T*, decltype(Construct), Args...>)
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
    operator T*() noexcept { return payload_; }
    operator const T*() const noexcept { return payload_; }
    T* operator->() noexcept { return payload_; }
    const T* operator->() const noexcept { return payload_; }

    void reset(T* ptr = Invalid) {
      Destruct(payload_);
      payload_ = ptr;
    }

    T* release() {
      auto ptr = payload_;
      payload_ = Invalid;
      return ptr;
    }
  };

}  // namespace xpto
