// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_ALPHABETA_EVALUATORS_H_
#define AI_ALPHABETA_EVALUATORS_H_

#include "ai/ai_export.h"
#include "base/callable.h"
#include "game/piece_color.h"

namespace game {
class Board;
}

namespace ai {
namespace alphabeta {

// Handy definitions for the heuristics that evaluate a game state.
typedef int(EvaluatorSignature)(const game::Board&, game::PieceColor);
typedef base::Callable<EvaluatorSignature> Evaluator;

// Heuristic that counts the number of actions available to the |player|.
AI_EXPORT int Mobility(const game::Board& board, game::PieceColor player);

// Heuristic that counts the difference between the number of remaining pieces.
AI_EXPORT int Material(const game::Board& board, game::PieceColor player);

// Heuristic that counts the difference between the number of closed mills.
AI_EXPORT int Mills(const game::Board& board, game::PieceColor player);

// Utility heuristic that evaluates the board using the given |evaluator|, but
// from the opponent perspective.
AI_EXPORT int OpponentEval(Evaluator* evaluator,
                           const game::Board& board,
                           game::PieceColor player);

}  // namespace alphabeta
}  // namespace ai

#endif  // AI_ALPHABETA_EVALUATORS_H_

