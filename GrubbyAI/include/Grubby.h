#ifndef GRUBBY_H
#define GRUBBY_H

#include "AdditionalFunctionality.h"
#include "GameBoard.h"
#include "sc2utils/sc2_manage_process.h"

class GameBoard;

// ! GrubbyAI Bot
class Grubby : public sc2::Agent 
{
public:

    Grubby();

    void OnGameStart() override;
    void OnStep() override;
    void OnGameEnd() override;

    unsigned int map_width_;       // !< Map width
    unsigned int map_height_;      // !< Map height
    unsigned int step_;            // !< Current Step
    GameBoard *game_board_ = nullptr;
    sc2::GameInfo gm_info_;
    const sc2::Unit* starting_base_ = nullptr;
};


#endif