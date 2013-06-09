// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>
#include <memory>
#include <set>

#include "ai/alphabeta/alphabeta.h"
#include "base/basic_macros.h"
#include "gtest/gtest.h"

namespace ai {
namespace alphabeta {

// Specialize the hashing function for int's.
template <> size_t Hash<int>(const int& x) { return x; }

namespace {

// This test simulates an abstract game whose game tree expansion would be
// equal to the one from the following image:
// http://upload.wikimedia.org/wikipedia/commons/9/91/AB_pruning.svg.
// The state encoding is an integer representing the 0-based index of the node
// in the BFS traversal of the tree in the picture. The root will have index 0.
// The children of one node are visited in left-to-right order.
// The score is a |double| only for testing purposes. The values are only given
// for the leaf nodes in the picture.
class TestDelegate : public AlphaBeta<int>::Delegate {
 public:
  TestDelegate() {
    children_[0].push_back(1);
    children_[0].push_back(2);
    children_[0].push_back(3);
    children_[1].push_back(4);
    children_[1].push_back(5);
    children_[2].push_back(6);
    children_[2].push_back(7);
    children_[3].push_back(8);
    children_[3].push_back(9);
    children_[4].push_back(10);
    children_[4].push_back(11);
    children_[5].push_back(12);
    children_[6].push_back(13);
    children_[6].push_back(14);
    children_[7].push_back(15);
    children_[8].push_back(16);
    children_[9].push_back(17);
    children_[9].push_back(18);
    children_[10].push_back(19);
    children_[10].push_back(20);
    children_[11].push_back(21);
    children_[11].push_back(22);
    children_[11].push_back(23);
    children_[12].push_back(24);
    children_[13].push_back(25);
    children_[14].push_back(26);
    children_[14].push_back(27);
    children_[15].push_back(28);
    children_[16].push_back(29);
    children_[17].push_back(30);
    children_[17].push_back(31);
    children_[18].push_back(32);

    scores_[19] = 5;
    scores_[20] = 6;
    scores_[21] = 7;
    scores_[22] = 4;
    scores_[23] = 5;
    scores_[24] = 3;
    scores_[25] = 6;
    scores_[26] = 6;
    scores_[27] = 9;
    scores_[28] = 7;
    scores_[29] = 5;
    scores_[30] = 9;
    scores_[31] = 8;
    scores_[32] = 6;
  }

  const std::set<int>& visited_states() const {
    return visited_states_;
  }

 private:
  virtual bool IsTerminal(const int& state) {
    visited_states_.insert(state);
    return scores_.count(state) > 0;
  };

  virtual int Evaluate(const int& state) {
    visited_states_.insert(state);
    if (scores_.count(state)) {
      return scores_[state];
    }
    return 0;
  }

  virtual void GetSuccessors(const int& state, std::vector<int>* successors) {
    visited_states_.insert(state);
    ASSERT_GT(children_.count(state), 0U);
    const std::vector<int>& children = children_[state];
    successors->insert(successors->end(), children.begin(), children.end());
  }

  std::map<int, std::vector<int> > children_;
  std::map<int, int> scores_;
  std::set<int> visited_states_;
};

TEST(AlphaBeta, AbstractGame) {
  const int v[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14, 15, 16, 19,
                    20, 21, 22, 24, 25, 26, 28, 29 };
  TestDelegate* delegate = new TestDelegate();
  AlphaBeta<int> alpha_beta(
      (std::auto_ptr<AlphaBeta<int>::Delegate>(delegate)));
  alpha_beta.set_shuffling_enabled(false);
  EXPECT_EQ(2, alpha_beta.GetBestSuccessor(0));
  const std::set<int> expected_visited_states(v, v + arraysize(v));
  EXPECT_EQ(expected_visited_states, delegate->visited_states());
}

}  // anonymous namespace
}  // namespace alphabeta
}  // namespace ai
