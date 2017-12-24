#include "GameTile.h"

GameTile::GameTile() : loc(0,0), z(0.0f) {}

// ! GameTile constructor
GameTile::GameTile(const unsigned int X, const unsigned int Y) : loc(X,Y), z(obs_->TerrainHeight(sc2::Point2D(float(X), float(Y)))) {
    creep = obs_->HasCreep(sc2::Point2D(float(X), float(Y)));
}

// ! Updates whether a tile is pathable. 
// Can change when something is walled off or if it is unpathable terrain.
void GameTile::UpdatePathable(const Grubby &bot) {


    updated_step_ = bot.step_;
}

// ! Updates whether a tile is placeable.
// Can change when a building or unit is in the way.
void GameTile::UpdatePlaceable(const Grubby &bot) {



    updated_step_ = bot.step_;
}

