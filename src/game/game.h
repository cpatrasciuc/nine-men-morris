// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_GAME_H_
#define GAME_GAME_H_

#include <deque>
#include <map>
#include <vector>

#include "base/basic_macros.h"
#include "base/supports_listener.h"
#include "game/board.h"
#include "game/game_export.h"
#include "game/game_options.h"
#include "game/game_type.h"
#include "game/piece_color.h"
#include "game/player_action.h"

namespace game {

class GameListener;

class GAME_EXPORT Game : public base::SupportsListener<GameListener> {
 public:
  explicit Game(const GameOptions& game_options = game::GameOptions());

  const GameOptions& options() const { return game_options_; }

  const Board& board() const { return board_; }

  bool is_game_over() const { return is_game_over_; }

  PieceColor current_player() const { return current_player_; }

  PlayerAction::ActionType next_action_type() const {
    return next_action_type_;
  }

  // Utility method that returns the number of pieces that still have to be
  // placed on the board by the given player.
  int GetPiecesInHand(const PieceColor player_color) const;

  // Returns the color of the winning player. It should only be called after the
  // game is over.
  PieceColor winner() const;

  // This method is responsible for doing the initial setup of the game object
  // before starting the play. It determines the first player, the number of
  // pieces available to each player at the beginning of the game, etc.
  // It must be called only once for each game instance.
  void Initialize();

  // Verifies that the |action| is valid and can be executed given the current
  // state of the game.
  bool CanExecutePlayerAction(const PlayerAction& action) const;

  // Execute the given |action| and update the game state based on its result.
  void ExecutePlayerAction(const PlayerAction& action);

  // Returns |true| if the current player is allowed to perform a jump. This
  // happens when the current player has less then four pieces left on the
  // board, no pieces are to be placed, |game_options.jumps_allowed() is set
  // to |true| and the next type of action is |PlayerAction::MOVE_PIECE|.
  bool CanJump() const;

  // Undo the last executed action and update the game state accordingly.
  void UndoLastAction();

  // Adds the current list of player actions that were executed in this game,
  // to the |actions| vector. The actions are inserted in the order in which
  // they were executed. |actions| is not cleared before appending to it.
  void DumpActionList(std::vector<PlayerAction>* actions) const;

  // Utility method that returns the number of pieces that a player starts with
  // based on the game type.
  static int GetInitialPieceCountByGameType(GameType type);

 private:
  // Update the next player to move and the expected action type, based on the
  // last executed action. It also checks if the game is over and, if it's the
  // case, it updates the corresponding variables.
  void UpdateGameState();

  // Checks if the game reached its end. It is called after each action.
  bool CheckIfGameIsOver() const;

  // Convenience methods used to notify game listeners.
  void FireOnGameInitialized();
  void FireOnPlayerAction(const PlayerAction& action);
  void FireOnUndoAction(const PlayerAction& action);
  void FireOnGameOver(PieceColor winner);

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
  std::map<PieceColor, int> pieces_in_hand_;

  // The color of the next player to move.
  PieceColor current_player_;

  // The expected type of the next player action.
  PlayerAction::ActionType next_action_type_;

  // Store the game state internally. Initially it is set to |false|. Once the
  // game reaches its end state, the variable is switched to |true| and no other
  // player action can be executed.
  bool is_game_over_;

  // Once the game is over, stores the winning color.
  PieceColor winner_;

  DISALLOW_COPY_AND_ASSIGN(Game);
};

}  // namespace game

#endif  // GAME_GAME_H_
