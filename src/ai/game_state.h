// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_GAME_STATE_H_
#define AI_GAME_STATE_H_

#include <bitset>
#include <vector>

#include "ai/ai_export.h"
#include "game/game_type.h"
#include "game/piece_color.h"
#include "game/player_action.h"

namespace game {
class Board;
class BoardLocation;
}

namespace ai {

class AI_EXPORT GameState {
 public:
  // TODO(game_state): Enhance the behavior of the default constructor.
  explicit GameState(game::GameType game_type = game::NINE_MEN_MORRIS);
  GameState(const GameState& other);
  ~GameState();

  game::GameType game_type() const { return game_type_; }

  game::PieceColor current_player() const;
  void set_current_player(const game::PieceColor player_color);

  int pieces_in_hand(const game::PieceColor player_color) const;
  void set_pieces_in_hand(const game::PieceColor player_color, const int count);

  void Encode(const game::Board& board);
  void Decode(game::Board* board) const;

  GameState& operator=(const GameState& other);
  bool operator==(const GameState& other) const;

  // This must be declared so one can use GameState objects as std::map keys.
  bool operator<(const GameState& other) const;

  // Methods that make changes to the current board configuration.
  void AddPiece(const game::BoardLocation& destination, game::PieceColor color);
  void MovePiece(const game::BoardLocation& source,
                 const game::BoardLocation& destination);
  void RemovePiece(const game::BoardLocation& source);

  // Utility method that determines the actions that must be played in order to
  // reach the game state encoded by |to| from the game state encoded by |from|.
  // The first element of the result is the MOVE or PLACE action that the player
  // must perform in the state encoded by |from|. If this action closes a mill,
  // the result will contain a second element representing the REMOVE action
  // that must be performed in order to reach the state encoded by |to|.
  // If |to| is not a direct successor of |from| the behavior is undefined.
  static std::vector<game::PlayerAction> GetTransition(const GameState& from,
                                                       const GameState& to);

  // Utility function used to store GameState instances in hash maps.
  static size_t Hash(const GameState& state) { return state.s_.to_ulong(); }

 private:
  game::GameType game_type_;
  std::bitset<64> s_;
};

// Convenience class used to declare hash maps that are able to store GameState
// instances as keys.
class GameStateHasher {
 public:
  size_t operator()(const GameState& state) const {
    return GameState::Hash(state);
  };
};

}  // namespace ai

#endif  // AI_GAME_STATE_H_

