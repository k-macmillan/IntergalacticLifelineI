#ifndef GAME_TILE_H
#define GAME_TILE_H

#include <vector>
#include "Grubby.h"
#include "AdditionalFunctionality.h"

class Grubby;

class GameTile {
public:
    GameTile();
    GameTile(unsigned int X, unsigned int Y);

    const sc2::Point2DI loc;
    const float z;                                       // !< Z component of tile
    
    // Tile status information
    sc2::Visibility vis = sc2::Visibility::FullHidden;   // !< Tile's visibility
    bool creep = false;                                  // !< Whether there is creep or not
    bool pathable = false;                               // !< Whether the tile is pathable
    bool placeable = false;                              // !< Whether a tile can have things placed on it 
    unsigned int updated_step_ = 0;                      // !< Last step this tile was updated

    void UpdatePathable(const Grubby &bot);              // !< Updates if the tile is pathable
    void UpdatePlaceable(const Grubby &bot);             // !< Updates if the tile is placeable
    
private:

};

typedef std::vector<GameTile*> Tiles;


#endif