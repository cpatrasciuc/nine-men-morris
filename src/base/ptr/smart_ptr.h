// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_SMART_PTR_H_
#define BASE_PTR_SMART_PTR_H_

#include "base/ptr/default_storage_policy.h"

namespace base {
namespace ptr {

template <
  typename T,
  template <class> class OwnershipPolicy = DefaultStoragePolicy  // NOLINT
>
class SmartPtr : public OwnershipPolicy<T> {
 public:
  typedef typename OwnershipPolicy<T>::PointerType PointerType;
  typedef typename OwnershipPolicy<T>::StoredType StoredType;
  typedef typename OwnershipPolicy<T>::ReferenceType ReferenceType;

  SmartPtr() : OwnershipPolicy<T>() {}
  explicit SmartPtr(const StoredType& t) : OwnershipPolicy<T>(t) {}

  SmartPtr(const SmartPtr<T>& other) {
    // TODO(smart_pointer): Implement the copy constructor
  }

  ~SmartPtr() {
    // OwnershipPolicy<T>::Destroy();
  }

  // This enables statements like: if (!smart_ptr) { ... }
  bool operator!() const {
    return Get(*this) == 0;
  }

  // Equality and inequality operators (non-template versions)
  inline friend bool operator==(const SmartPtr& lhs, const PointerType rhs) {
    return Get(lhs) == rhs;
  }

  inline friend bool operator==(const PointerType lhs, const SmartPtr& rhs) {
    return lhs == Get(rhs);
  }

  inline friend bool operator!=(const SmartPtr& lhs, const PointerType rhs) {
    return Get(lhs) != rhs;
  }

  inline friend bool operator!=(const PointerType lhs, const SmartPtr& rhs) {
    return lhs != Get(rhs);
  }

  // Equality and inequality operators (template versions)
  template <class U>
  inline friend bool operator==(const SmartPtr& lhs, const U* rhs) {
    return Get(lhs) == rhs;
  }

  template <class U>
  inline friend bool operator==(const U* lhs, const SmartPtr& rhs) {
    return lhs == Get(rhs);
  }

  template <class U>
  inline friend bool operator!=(const SmartPtr& lhs, const U* rhs) {
    return Get(lhs) != rhs;
  }

  template <class U>
  inline friend bool operator!=(const U* lhs, const SmartPtr& rhs) {
    return lhs != Get(rhs);
  }

  // Equality and inequality operators for SmartPtr itself;
  // delegates to a comparison between T* and U*.
  template <class U>
  bool operator==(const SmartPtr<U>& other) const {
    return Get(*this) == Get(other);
  }

  template <class U>
  bool operator!=(const SmartPtr<U>& other) const {
    return Get(*this) != Get(other);
  }

  // This trick enables statements like: if (smart_ptr) { ... }
 private:
  class Dummy {
    void operator delete(void* not_used);
  };

 public:
  operator Dummy*() const {
    if (!Get(*this)) {
      return 0;
    }
    static Dummy test;
    return &test;
  }
};

// Ordering operators:
//
// The first two operators are left undefined; its client responsibility
// to define them if it wants to allow ordering between smart pointers,
// for specific types.
//
// All the other ordering operator are defined based on the first two.
// The client won't have to implement them.
template <class T, class U> bool operator<(const SmartPtr<T>&, const U&);
template <class T, class U> bool operator<(const T&, const SmartPtr<U>&);

template <class T, class U>
bool operator<(const SmartPtr<T>& lhs, const SmartPtr<U>& rhs) {
  return lhs < Get(rhs);
}

// operator >

template <class T, class U>
bool operator>(const SmartPtr<T>& lhs, const U& rhs) {
  return rhs < lhs;
}
template <class T, class U>
bool operator>(const SmartPtr<T>& lhs, U& rhs) {
  return rhs < lhs;
}
template <class T, class U>
bool operator>(const U& lhs, const SmartPtr<T>& rhs) {
  return rhs < lhs;
}
template <class T, class U>
bool operator>(const SmartPtr<T>& lhs, const SmartPtr<U>& rhs) {
  return Get(lhs) > rhs;
}

// operator >=

template <class T, class U>
bool operator>=(const SmartPtr<T>& lhs, const U& rhs) {
  return (rhs < lhs) || (rhs == lhs);
}
template <class T, class U>
bool operator>=(const U& lhs, const SmartPtr<T>& rhs) {
  return (rhs < lhs) || (rhs == lhs);
}
template <class T, class U>
bool operator>=(const SmartPtr<T>& lhs, const SmartPtr<U>& rhs) {
  return (Get(lhs) > rhs) || (lhs == rhs);
}

// operator <=

template <class T, class U>
bool operator<=(const SmartPtr<T>& lhs, const U& rhs) {
  return (lhs < rhs) || (lhs == rhs);
}
template <class T, class U>
bool operator<=(const U& lhs, const SmartPtr<T>& rhs) {
  return (lhs < rhs) || (lhs == rhs);
}
template <class T, class U>
bool operator<=(const SmartPtr<T>& lhs, const SmartPtr<U>& rhs) {
  return (lhs < rhs) || (lhs == rhs);
}

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_SMART_PTR_H_

