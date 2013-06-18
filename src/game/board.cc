// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/board.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <map>
#include <utility>
#include <vector>

#include "base/basic_macros.h"
#include "base/log.h"
#include "base/threading/lock.h"
#include "base/threading/scoped_guard.h"
#include "game/board_location.h"
#include "game/game_type.h"
#include "game/piece_color.h"

using std::map;
using std::vector;

namespace game {

namespace {

const int kMaxBoardSize = 7;

int GetBoardSizeFromGameType(GameType type) {
  switch (type) {
    case THREE_MEN_MORRIS:
      return 3;
    case SIX_MEN_MORRIS:
      return 5;
    case NINE_MEN_MORRIS:
      return 7;
  }
  NOTREACHED();
  return -1;
}

bool InternalIsValidLocation(int board_size, const BoardLocation& location) {
  DCHECK_LT(board_size, kMaxBoardSize + 1);
  DCHECK_LT(location.line(), kMaxBoardSize);
  DCHECK_LT(location.column(), kMaxBoardSize);
  DCHECK_GT(location.line(), -1);
  DCHECK_GT(location.column(), -1);

  // Special case for size_ == 3, where all locations are valid
  if (board_size == 3) {
    return true;
  }
  int l = location.line();
  int c = location.column();
  // Only the middle of the board is not allowed for the center line
  if (l == board_size / 2) {
    return (c != board_size / 2);
  }
  // Handle symmetry
  if (l > board_size / 2) {
    l = board_size - l - 1;
  }
  // Location from the middle column are allowed for all lines < size_ / 2
  if (c == board_size / 2) {
    return true;
  }
  // Handle symmetry
  if (c > board_size / 2) {
    c = board_size - c - 1;
  }
  // Check if the location is on the diagonal
  return l == c;
}

}  // anonymous namespace

class Board::BoardImpl {
 public:
  // TODO(game): Might want to make this ctor lighter and add an Init() method
  explicit BoardImpl(GameType type)
      : size_(GetBoardSizeFromGameType(type)),
        valid_(NULL),
        // TODO(board): Check if we can reduce allocation to size_ * size_
        pieces_(kMaxBoardSize * kMaxBoardSize, NO_COLOR),
        white_piece_count_(0),
        black_piece_count_(0) {
    DCHECK_LT(size_, kMaxBoardSize + 1);
    std::map<GameType, std::vector<char> >::iterator it =
        validity_cache.find(type);
    if (it == validity_cache.end()) {
      const int board_size = GetBoardSizeFromGameType(type);
      std::vector<char> valid(kMaxBoardSize * kMaxBoardSize, 0);
      for (int line = 0; line < kMaxBoardSize; ++line) {
        for (int col = 0; col < kMaxBoardSize; ++col) {
          valid[IndexOf(line, col)] =
              InternalIsValidLocation(board_size, BoardLocation(line, col));
        }
      }
      {
        base::threading::ScopedGuard first_lock(&validity_cache_lock);
        it = validity_cache.find(type);
        if (it == validity_cache.end()) {
          it = validity_cache.insert(std::make_pair(type, valid)).first;
        }
      }
    }
    valid_ = &it->second;
  };

  int size() const { return size_; }

  int piece_count() const { return white_piece_count_ + black_piece_count_; }

  int GetPieceCountByColor(PieceColor color) const {
    DCHECK(color != NO_COLOR);
    return (color == WHITE_COLOR) ? white_piece_count_ : black_piece_count_;
  }

  bool IsValidLocation(const BoardLocation& loc) const {
    return loc.line() < size_ && loc.column() < size_ &&
           loc.line() >= 0    && loc.column() >= 0 &&
           (*valid_)[BoardImpl::IndexOf(loc)];
  }

  const vector<BoardLocation>& locations() const {
    map<int, vector<BoardLocation> >::iterator it;
    {
      base::threading::ScopedGuard _(&valid_locations_cache_lock);
      it = valid_locations_cache.find(size_);
    }
    if (it == valid_locations_cache.end()) {
      vector<BoardLocation> result;
      for (int line = 0; line < size_; ++line) {
        for (int col = 0; col < size_; ++col) {
          const BoardLocation loc(line, col);
          if (IsValidLocation(loc)) {
            result.push_back(loc);
          }
        }
      }
      {
        base::threading::ScopedGuard _(&valid_locations_cache_lock);
        it = valid_locations_cache.find(size_);
        if (it == valid_locations_cache.end()) {
          const std::pair<int, vector<BoardLocation> > p(size_, result);
          it = valid_locations_cache.insert(p).first;
        }
      }
    }
    return it->second;
  }

  bool IsAdjacent(const BoardLocation& b1, const BoardLocation& b2) const {
    DCHECK(IsValidLocation(b1));
    DCHECK(IsValidLocation(b2));
    int x = 0;
    int y = 0;
    int common_coordinate = 0;
    if (b1.line() == b2.line()) {
      x = b1.column();
      y = b2.column();
      common_coordinate = b1.line();
    } else if (b1.column() == b2.column()) {
      x = b1.line();
      y = b2.line();
      common_coordinate = b1.column();
    }
    if (common_coordinate > size_ / 2) {
      common_coordinate = size_ - common_coordinate - 1;
    }
    if (abs(x - y) == GetStep(common_coordinate)) {
      return true;
    }
    return false;
  }

  void GetAdjacentLocations(const BoardLocation& loc,
                            vector<BoardLocation>* adjacent_locations) const {
    DCHECK(IsValidLocation(loc));
    const int horizontal_step = GetStep(loc.column());
    const int vertical_step = GetStep(loc.line());
    const int dx[] = { horizontal_step, -horizontal_step, 0, 0 };
    const int dy[] = { 0, 0, vertical_step, -vertical_step };
    for (size_t i = 0; i < arraysize(dx); ++i) {
      const BoardLocation new_loc(loc.line() + dx[i], loc.column() + dy[i]);
      if (IsValidLocation(new_loc)) {
        adjacent_locations->push_back(new_loc);
      }
    }
  }

  bool AddPiece(const BoardLocation& location, PieceColor color) {
    DCHECK(color != NO_COLOR);
    if (!IsValidLocation(location)) {
      return false;
    }
    const int index = IndexOf(location);
    if (pieces_[index] != NO_COLOR) {
      return false;
    }
    if (color == WHITE_COLOR) {
      ++white_piece_count_;
    } else {
      ++black_piece_count_;
    }
    pieces_[index] = color;
    return true;
  }

  bool RemovePiece(const BoardLocation& location) {
    DCHECK(IsValidLocation(location));
    const int index = IndexOf(location);
    const PieceColor color = pieces_[index];
    if (color == NO_COLOR) {
      return false;
    } else if (color == WHITE_COLOR) {
      --white_piece_count_;
    } else {
      --black_piece_count_;
    }
    pieces_[index] = NO_COLOR;
    return true;
  }

  PieceColor GetPieceAt(const BoardLocation& location) const {
    DCHECK(IsValidLocation(location));
    return pieces_[IndexOf(location)];
  }

  void MovePiece(const BoardLocation& old_loc,
                        const BoardLocation& new_loc) {
    DCHECK(IsValidLocation(old_loc));
    DCHECK(IsValidLocation(new_loc));
    PieceColor color = GetPieceAt(old_loc);
    DCHECK(color != NO_COLOR);
    DCHECK_EQ(NO_COLOR, GetPieceAt(new_loc));
    RemovePiece(old_loc);
    AddPiece(new_loc, color);
  }

  bool IsPartOfMill(const BoardLocation& location) const {
    PieceColor color = GetPieceAt(location);
    if (color == NO_COLOR) {
      return false;
    }
    int step = GetStep(location.column());
    int dx[] = { -2 * step, -step, 0, step, 2 * step };
    bool horizontal_pieces[arraysize(dx)] = { false };
    for (size_t i = 0; i < arraysize(dx); ++i) {
      BoardLocation loc(location.line() + dx[i], location.column());
      if (IsValidLocation(loc)) {
        if (GetPieceAt(loc) == color) {
          horizontal_pieces[i] = true;
          if (i >= 2) {
            if (horizontal_pieces[i - 1] && horizontal_pieces[i - 2]) {
              return true;
            }
          }
        }
      }
    }
    step = GetStep(location.line());
    int dy[] = { -2 * step, -step, 0, step, 2 * step };
    bool vertical_pieces[arraysize(dy)] = { false };
    for (size_t i = 0; i < arraysize(dy); ++i) {
      BoardLocation loc(location.line(), location.column() + dy[i]);
      if (IsValidLocation(loc)) {
        if (GetPieceAt(loc) == color) {
          vertical_pieces[i] = true;
          if (i >= 2) {
            if (vertical_pieces[i - 1] && vertical_pieces[i - 2]) {
              return true;
            }
          }
        }
      }
    }
    return false;
  }

 private:
  // Utility method that returns the distance between valid locations on the
  // line or column specified by |index|.
  int GetStep(int index) const {
    if (index == size_ / 2) {
      return 1;
    }
    return (size_ - 1 - 2 * index) / 2;
  };

  // Utility method that computes the index of a given board location inside the
  // vectors used by BoardImpl to store various information regarding the board.
  static int IndexOf(const BoardLocation& loc) {
    return IndexOf(loc.line(), loc.column());
  }

  static int IndexOf(const int line, const int column) {
    DCHECK_LT(line, kMaxBoardSize);
    DCHECK_LT(column, kMaxBoardSize);
    DCHECK_GT(line, -1);
    DCHECK_GT(column, -1);
    return line * kMaxBoardSize + column;
  }

  static std::map<GameType, std::vector<char> > validity_cache;
  static base::threading::Lock validity_cache_lock;

  static map<int, vector<BoardLocation> > valid_locations_cache;
  static base::threading::Lock valid_locations_cache_lock;

  int size_;

  // This is a kMaxBoardSize * kMaxBoardSize matrix stored in vector form that
  // specifies if a location (i, j) is valid or not.
  std::vector<char>* valid_;

  // Matrix representing the board (and a bit more for smaller games, but that
  // should not be a problem).
  std::vector<PieceColor> pieces_;

  // These variable store the piece count for each player to avoid traversing
  // the |pieces_| vector.
  int white_piece_count_;
  int black_piece_count_;

  DISALLOW_COPY_AND_ASSIGN(BoardImpl);
};

// static
std::map<GameType, std::vector<char> > Board::BoardImpl::validity_cache;

// static
base::threading::Lock Board::BoardImpl::validity_cache_lock;

// static
map<int, vector<BoardLocation> > Board::BoardImpl::valid_locations_cache;

// static
base::threading::Lock Board::BoardImpl::valid_locations_cache_lock;

Board::Board(GameType type) : impl_(new BoardImpl(type)) {}

// Explicit empty destructor required to correctly deallocate |impl_|, since it
// is a smart pointer that uses an incomplete type. An empty destructor must
// be defined here so it can see the definition of the BoardImpl destructor.
// See: will-an-empty-destructor-do-the-same-thing-as-the-generated-destructor
// on StackOverflow.
Board::~Board() {}

int Board::size() const { return impl_->size(); }

int Board::piece_count() const { return impl_->piece_count(); }

int Board::GetPieceCountByColor(PieceColor color) const {
  return impl_->GetPieceCountByColor(color);
}

bool Board::IsValidLocation(const BoardLocation& loc) const {
  return impl_->IsValidLocation(loc);
}

const std::vector<BoardLocation>& Board::locations() const {
  return impl_->locations();
}

bool Board::IsAdjacent(const BoardLocation& b1, const BoardLocation& b2) const {
  return impl_->IsAdjacent(b1, b2);
}

void Board::GetAdjacentLocations(const BoardLocation& loc,
    vector<BoardLocation>* adjacent_locations) const {
  impl_->GetAdjacentLocations(loc, adjacent_locations);
}

bool Board::AddPiece(const BoardLocation& location, PieceColor color) {
  return impl_->AddPiece(location, color);
}

bool Board::RemovePiece(const BoardLocation& location) {
  return impl_->RemovePiece(location);
}

PieceColor Board::GetPieceAt(const BoardLocation& location) const {
  return impl_->GetPieceAt(location);
}

void Board::MovePiece(const BoardLocation& old_loc,
                      const BoardLocation& new_loc) {
  impl_->MovePiece(old_loc, new_loc);
}

bool Board::IsPartOfMill(const BoardLocation& location) const {
  return impl_->IsPartOfMill(location);
}

}  // namespace game
