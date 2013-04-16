// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/game_state.h"

#include <stdint.h>

#include <bitset>
#include <map>
#include <queue>
#include <utility>
#include <vector>

#include "base/log.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game_options.h"
#include "game/piece_color.h"

namespace ai {

namespace {

using game::BoardLocation;
using game::GameOptions;

using std::queue;
using std::map;

map<GameOptions::GameType, map<BoardLocation, int> > kIndicesCache;

GameOptions::GameType GetGameTypeFromBoardSize(int board_size) {
  switch (board_size) {
    case 3:
      return GameOptions::THREE_MEN_MORRIS;
    case 5:
      return GameOptions::SIX_MEN_MORRIS;
    case 7:
      return GameOptions::NINE_MEN_MORRIS;
    default:
      NOTREACHED();
  }
  NOTREACHED();
  return GameOptions::THREE_MEN_MORRIS;
}

const map<BoardLocation, int>& GetLocationIndices(GameOptions::GameType type) {
  map<BoardLocation, int>& indices = kIndicesCache[type];
  if (indices.empty()) {
    game::Board board(type);
    queue<BoardLocation> bfs_queue;
    const BoardLocation start(0, 0);
    int index = 0;
    indices.insert(std::make_pair(start, index++));
    bfs_queue.push(start);
    while (!bfs_queue.empty()) {
      const BoardLocation current = bfs_queue.front();
      bfs_queue.pop();
      std::vector<BoardLocation> adjacent_locations;
      board.GetAdjacentLocations(current, &adjacent_locations);
      for (size_t i = 0; i < adjacent_locations.size(); ++i) {
        if (indices.count(adjacent_locations[i]) == 0) {
          indices.insert(std::make_pair(adjacent_locations[i], index++));
          bfs_queue.push(adjacent_locations[i]);
        }
      }
    }
  }
  return indices;
}

}  // anonymous namespace

GameState::GameState() : s_() {}

GameState::GameState(const GameState& other) : s_(other.s_) {}

GameState::~GameState() {}

game::PieceColor GameState::current_player() const {
  return s_.test(0) ? game::BLACK_COLOR : game::WHITE_COLOR;
}

void GameState::set_current_player(const game::PieceColor player_color) {
  s_.set(0, player_color == game::BLACK_COLOR);
}

int GameState::pieces_in_hand(const game::PieceColor player_color) const {
  DCHECK(player_color != game::NO_COLOR);
  const uint64_t state = s_.to_ulong();
  if (player_color == game::WHITE_COLOR) {
    return ((state & 0x1eULL) >> 1);
  }
  return ((state & 0x1e0ULL) >> 5);
}

void GameState::set_pieces_in_hand(const game::PieceColor player_color,
                                   const int count) {
  DCHECK(player_color != game::NO_COLOR);
  DCHECK_LT(count, 1 << 5);
  uint64_t bitmask = static_cast<uint64_t>(count) << 1;
  if (player_color == game::BLACK_COLOR) {
    bitmask <<= 4;
  }
  s_ |= bitmask;
}

void GameState::Encode(const game::Board& board) {
  s_ &= 0x1ffULL;  // Clear existing pieces
  const map<BoardLocation, int>& indices =
      GetLocationIndices(GetGameTypeFromBoardSize(board.size()));
  map<BoardLocation, int>::const_iterator it;
  for (it = indices.begin(); it != indices.end(); ++it) {
    const game::PieceColor color = board.GetPieceAt(it->first);
    if (color != game::NO_COLOR) {
      int pos = it->second + 9;
      if (color == game::BLACK_COLOR) {
        pos += indices.size();
      }
      s_.set(pos);
    }
  }
}

void GameState::Decode(game::Board* board) const {}

}  // namespace ai
