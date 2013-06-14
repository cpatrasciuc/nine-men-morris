// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basic_macros.h"
#include "base/supports_listener.h"
#include "gtest/gtest.h"

namespace base {
namespace {

class Listener {
 public:
  Listener() : notified_(false) {}
  virtual ~Listener() {}

  virtual void OnTestEvent() { notified_ = true; }

  void clear_notification_flag() { notified_ = false; }

  bool notified() const { return notified_; }

 private:
  bool notified_;

  DISALLOW_COPY_AND_ASSIGN(Listener);
};

// Listener that removes itself from the listeners list when the event occurs.
class DeleteWhenNotifiedListener : public Listener {
 public:
  explicit DeleteWhenNotifiedListener(SupportsListener<Listener>* observable)
      : notified_(false), observable_(observable) {
    observable_->AddListener(this);
  }

  virtual void OnTestEvent() {
    notified_ = true;
    observable_->RemoveListener(this);
  }

  void clear_notification_flag() { notified_ = false; }

  bool notified() const { return notified_; }

 private:
  bool notified_;
  SupportsListener<Listener>* observable_;

  DISALLOW_COPY_AND_ASSIGN(DeleteWhenNotifiedListener);
};

class Observable : public SupportsListener<Listener> {
 public:
  Observable() {}

  void FireTestEvent() {
    typedef SupportsListener<Listener>::ListenerList ListenerList;
    typedef SupportsListener<Listener>::ListenerIter ListenerIter;
    const ListenerList list(listeners());
    for (ListenerIter it = list.begin(); it != list.end(); ++it) {
      (*it)->OnTestEvent();
    }
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Observable);
};

TEST(SupportsListener, Basic) {
  Observable observable;
  Listener listener;
  EXPECT_FALSE(listener.notified());
  observable.FireTestEvent();
  EXPECT_FALSE(listener.notified());
  observable.AddListener(&listener);
  observable.FireTestEvent();
  EXPECT_TRUE(listener.notified());
  listener.clear_notification_flag();
  observable.RemoveListener(&listener);
  EXPECT_FALSE(listener.notified());
  observable.FireTestEvent();
  EXPECT_FALSE(listener.notified());
}

TEST(SupportsListener, RemoveWhenNotified) {
  Observable observable;
  DeleteWhenNotifiedListener listener(&observable);
  EXPECT_FALSE(listener.notified());
  observable.FireTestEvent();
  EXPECT_TRUE(listener.notified());
  listener.clear_notification_flag();
  observable.FireTestEvent();
  EXPECT_FALSE(listener.notified());
}

TEST(SupportsListenerDeathTest, DEBUG_ONLY_TEST(RemoveInvalidListener)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Observable observable;
  Listener listener;
  ASSERT_DEATH(observable.RemoveListener(&listener), "");
}

}  // anonymous namespace
}  // namespace base
