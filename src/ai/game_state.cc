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
#include "base/threading/lock.h"
#include "base/threading/scoped_guard.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game_type.h"
#include "game/piece_color.h"
#include "game/player_action.h"

using game::BoardLocation;

using std::map;

namespace ai {

namespace {

const int kLocationsOffset = 9;

map<game::GameType, map<BoardLocation, int> > g_indices_cache;
base::threading::Lock g_indices_cache_lock;

game::GameType GetGameTypeFromBoardSize(int board_size) {
  switch (board_size) {
    case 3:
      return game::THREE_MEN_MORRIS;
    case 5:
      return game::SIX_MEN_MORRIS;
    case 7:
      return game::NINE_MEN_MORRIS;
    default:
      NOTREACHED();
  }
  NOTREACHED();
  return game::THREE_MEN_MORRIS;
}

const map<BoardLocation, int>& GetLocationIndices(game::GameType type) {
  map<game::GameType, map<BoardLocation, int> >::iterator it =
      g_indices_cache.find(type);
  if (it == g_indices_cache.end()) {
    map<BoardLocation, int> indices;
    game::Board board(type);
    std::queue<BoardLocation> bfs_queue;
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
    {
      base::threading::ScopedGuard _(&g_indices_cache_lock);
      it = g_indices_cache.find(type);
      if (it == g_indices_cache.end()) {
        it = g_indices_cache.insert(std::make_pair(type, indices)).first;
      }
    }
  }
  return it->second;
}

}  // anonymous namespace

GameState::GameState(game::GameType game_type) : game_type_(game_type), s_() {}

GameState::GameState(const GameState& other)
    : game_type_(other.game_type_), s_(other.s_) {}

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
    for (int i = 5; i <= 8; ++i) {
      s_.set(i, false);
    }
  } else {
    for (int i = 1; i <= 4; ++i) {
      s_.set(i, false);
    }
  }
  s_ |= bitmask;
}

void GameState::Encode(const game::Board& board) {
  s_ &= 0x1ffULL;  // Clear existing pieces
  game_type_ = GetGameTypeFromBoardSize(board.size());
  const std::vector<game::BoardLocation>& locations = board.locations();
  for (size_t i = 0; i < locations.size(); ++i) {
    const game::PieceColor color = board.GetPieceAt(locations[i]);
    if (color != game::NO_COLOR) {
      AddPiece(locations[i], color);
    }
  }
}

void GameState::Decode(game::Board* board) const {
  DCHECK(board);
  DCHECK_EQ(game_type_, GetGameTypeFromBoardSize(board->size()));
  const map<BoardLocation, int>& indices = GetLocationIndices(game_type_);
  map<BoardLocation, int>::const_iterator it;
  for (it = indices.begin(); it != indices.end(); ++it) {
    game::PieceColor color = game::NO_COLOR;
    const int pos = it->second + kLocationsOffset;
    if (s_.test(pos)) {
      color = game::WHITE_COLOR;
    }
    if (s_.test(pos + indices.size())) {
      DCHECK_EQ(color, game::NO_COLOR);
      color = game::BLACK_COLOR;
    }
    if (color != game::NO_COLOR) {
      board->AddPiece(it->first, color);
    }
  }
}

GameState& GameState::operator=(const GameState& other) {
  if (this != &other) {
    s_ = other.s_;
  }
  return *this;
}

bool GameState::operator==(const GameState& other) const {
  return s_ == other.s_;
}

bool GameState::operator<(const GameState& other) const {
  return s_.to_ulong() < other.s_.to_ulong();
}

void GameState::AddPiece(const game::BoardLocation& destination,
                         game::PieceColor color) {
  DCHECK(color != game::NO_COLOR);
  const map<BoardLocation, int>& indices = GetLocationIndices(game_type_);
  map<BoardLocation, int>::const_iterator it = indices.find(destination);
  DCHECK(it != indices.end());
  int pos = it->second + kLocationsOffset;
  if (color == game::BLACK_COLOR) {
    pos += indices.size();
  }
  s_.set(pos);
}

void GameState::MovePiece(const game::BoardLocation& source,
                          const game::BoardLocation& destination) {
  const map<BoardLocation, int>& indices = GetLocationIndices(game_type_);
  map<BoardLocation, int>::const_iterator source_it = indices.find(source);
  map<BoardLocation, int>::const_iterator dest_it = indices.find(destination);
  int source_pos = source_it->second + kLocationsOffset;
  int dest_pos = dest_it->second + kLocationsOffset;
  DCHECK(!s_.test(dest_pos));
  DCHECK(!s_.test(dest_pos + indices.size()));
  if (s_.test(source_pos)) {
    DCHECK(!s_.test(source_pos + indices.size()));
    s_.reset(source_pos);
    s_.set(dest_pos);
    return;
  }
  source_pos += indices.size();
  dest_pos += indices.size();
  s_.reset(source_pos);
  s_.set(dest_pos);
}

void GameState::RemovePiece(const game::BoardLocation& source) {
  const map<BoardLocation, int>& indices = GetLocationIndices(game_type_);
  map<BoardLocation, int>::const_iterator source_it = indices.find(source);
  int pos = source_it->second + kLocationsOffset;
  if (s_.test(pos)) {
    DCHECK(!s_.test(pos + indices.size()));
    s_.reset(pos);
    return;
  }
  s_.reset(pos + indices.size());
}

// static
std::vector<game::PlayerAction> GameState::GetTransition(
    const GameState& from, const GameState& to) {
  const game::PieceColor player = from.current_player();
  DCHECK_EQ(player, game::GetOpponent(to.current_player()));
  DCHECK_EQ(from.pieces_in_hand(game::GetOpponent(player)),
            to.pieces_in_hand(game::GetOpponent(player)));
  DCHECK_EQ(from.game_type_, to.game_type_);
  const map<BoardLocation, int>& indices = GetLocationIndices(from.game_type_);
  const int offset = (player == game::WHITE_COLOR ?
      kLocationsOffset : kLocationsOffset + indices.size());
  const int opp_offset = (player == game::WHITE_COLOR ?
      kLocationsOffset + indices.size() : kLocationsOffset);
  game::BoardLocation source(-1, -1);
  game::BoardLocation destination(-1, -1);
  game::BoardLocation remove(-1, -1);
  // TODO(game_state): define a typedef for this.
  map<BoardLocation, int>::const_iterator it;
  for (it = indices.begin(); it != indices.end(); ++it) {
    const int pos = it->second + offset;
    const int opp_pos = it->second + opp_offset;
    if (!from.s_.test(pos) && to.s_.test(pos)) {
      DCHECK_EQ(destination.line(), -1);
      destination = it->first;
    } else if (from.s_.test(pos) && !to.s_.test(pos)) {
      DCHECK_EQ(source.line(), -1);
      source = it->first;
    } else if (from.s_.test(opp_pos) && !to.s_.test(opp_pos)) {
      DCHECK_EQ(remove.line(), -1);
      remove = it->first;
    }
  }
  std::vector<game::PlayerAction> result;
  if (source.line() == -1) {
    DCHECK_GT(from.pieces_in_hand(player), 0);
    DCHECK_EQ(from.pieces_in_hand(player), to.pieces_in_hand(player) + 1);
    game::PlayerAction place_action(player, game::PlayerAction::PLACE_PIECE);
    place_action.set_destination(destination);
    result.push_back(place_action);
  } else {
    DCHECK_EQ(from.pieces_in_hand(player), 0);
    DCHECK_EQ(to.pieces_in_hand(player), 0);
    game::PlayerAction move_action(player, game::PlayerAction::MOVE_PIECE);
    move_action.set_source(source);
    move_action.set_destination(destination);
    result.push_back(move_action);
  }
  if (remove.line() != -1) {
    game::PlayerAction remove_action(player, game::PlayerAction::REMOVE_PIECE);
    remove_action.set_source(remove);
    result.push_back(remove_action);
  }
  return result;
}

}  // namespace ai
