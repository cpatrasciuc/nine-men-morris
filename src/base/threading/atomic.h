// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_ATOMIC_H_
#define BASE_THREADING_ATOMIC_H_

namespace base {
namespace threading {

#if defined(__GNUC__) || defined(__clang__)

// This class wraps a simple variable and offers atomic operations on it.
// The functions used to implement this are only supported on GCC/Clang.
// The meaning of each method is equivalent with the comment above it, just that
// these builtins are considered a full barrier. That is, no memory operand will
// be moved across the operation, either forward or backward.
// Further, instructions will be issued as necessary to prevent the processor
// from speculating loads across the operation and from queuing stores after the
// operation.
//
// The template argument T should be one of the following types:
//     int
//     unsigned int
//     long
//     unsigned long
//     long long
//     unsigned long long
//
template <typename T>
class Atomic {
 public:
  Atomic() : var_() {}
  explicit Atomic(const T& t) : var_(t) {}
  Atomic(const Atomic& other) : var_(other.var_) {}

  T Get() const { return var_; }

  // return var_ += value
  T Add(const T& value) {
    return __sync_add_and_fetch(&var_, value);
  }

  // return ++var_
  T Increment() {
    return __sync_add_and_fetch(&var_, 1);
  }

  // return var_ -= value
  T Subtract(const T& value) {
    return __sync_sub_and_fetch(&var_, value);
  }

  // return --var_
  T Decrement() {
    return __sync_sub_and_fetch(&var_, 1);
  }

  // return var_ |= value
  T BitwiseOr(const T& value) {
    return __sync_or_and_fetch(&var_, value);
  }

  // return var_ &= value
  T BitwiseAnd(const T& value) {
    return __sync_and_and_fetch(&var_, value);
  }

  // return var_ ^= value
  T BitwiseXor(const T& value) {
    return __sync_xor_and_fetch(&var_, value);
  }

  // TODO(clang): Enable this when clang will support atomic nand
  // return var_ = ~var_ & value
  // T BitwiseNand(const T& value) {
  // return __sync_nand_and_fetch(&var_, value);
  // }

  // if (var_ == old_value) {
  //     var_ = new_value;
  //     return var_ = true;
  // }
  // return false;
  bool CompareAndSwap(const T& old_value, const T& new_value) {
    return __sync_bool_compare_and_swap(&var_, old_value, new_value);
  }

 private:
  T var_;
};

#else
#error Your compiler does not support atomic operations
#endif

}  // namespace threading
}  // namespace base

#endif  // BASE_THREADING_ATOMIC_H_

