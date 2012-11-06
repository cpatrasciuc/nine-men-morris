// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_GAME_H_
#define GAME_GAME_H_

#include <map>
#include <vector>

#include "base/basic_macros.h"
#include "game/board.h"
#include "game/game_export.h"
#include "game/game_options.h"
#include "game/player_action.h"

namespace game {

class GAME_EXPORT Game {
 public:
  explicit Game(const GameOptions& game_options);

  const GameOptions& options() const { return game_options_; }

  const Board& board() const { return board_; }

  // This method is responsible for doing the initial setup of the game object
  // before starting the play. It determines the first player, the number of
  // pieces available to each player at the beginning of the game, etc.
  void Initialize();

  // Checks if the game reached its end. It must be called after each action.
  bool IsGameOver() const;

  // Execute the given |action| and update the game state based on its result.
  void ExecutePlayerAction(const PlayerAction& action);

  // Undo the last executed action and update the game state accordingly.
  void UndoLastAction();

  // Adds the current list of player actions that were executed in this game,
  // to the |actions| vector. The actions are inserted in the order in which
  // they were executed. |actions| is not cleared before appending to it.
  void DumpActionList(std::vector<PlayerAction>* actions) const;

  // Utility method that returns the number of pieces that a player starts with
  // based on the game type.
  static int GetInitialPieceCountByGameType(GameOptions::GameType type);

 private:
  // Update the next player to move and the expected action type, based on the
  // last executed action. It also checks if the game is over and, if it's the
  // case, it updates the corresponding variables.
  void UpdateGameState();

  // The options for this game instance.
  GameOptions game_options_;

  // The board used by this game instance.
  Board board_;

  // The list of player actions that were executed from the beginning of the
  // game. It allows the user to undo the recent actions and also to save the
  // the game to a file so it can be resumed later.
  std::vector<PlayerAction> moves_;

  // A map that stores the number of pieces that still have to be placed on the
  // board by each player.
  std::map<Board::PieceColor, int> pieces_in_hand_;

  // The color of the next player to move.
  Board::PieceColor current_player_;

  // The expected type of the next player action.
  PlayerAction::ActionType next_action_type_;

  DISALLOW_COPY_AND_ASSIGN(Game);
};

}  // namespace game

#endif  // GAME_GAME_H_
