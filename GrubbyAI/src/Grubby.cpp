#include "Grubby.h"


Grubby::Grubby(){}

void Grubby::OnGameStart() {
    obs_ = Observation();
    query_ = Query();
    gm_info_ = obs_->GetGameInfo();
    map_height_ = gm_info_.height;
    map_width_ = gm_info_.width;

    starting_base_ = obs_->GetUnits(sc2::Unit::Self, IsTownHall()).front();
    game_board_ = new GameBoard(*this);


}

void Grubby::OnStep() {
    sc2::SleepFor(30);   // Human speed
    obs_ = Observation();
}

void Grubby::OnGameEnd() {
    delete game_board_;
}