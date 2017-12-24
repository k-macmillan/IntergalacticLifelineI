#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "AdditionalFunctionality.h"
#include "Grubby.h"
#include "GameTile.h"

class GameTile;
class Grubby;

class GameBoard {
public:
    GameBoard(const Grubby &bot);

    GameTile* GetTile(const unsigned int &x, const unsigned int &y);

private:
    void BuildBoard(const Grubby &bot);

    std::vector< std::vector<GameTile> > board;
};

#endif