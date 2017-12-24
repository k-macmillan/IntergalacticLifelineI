#include "GameBoard.h"


GameBoard::GameBoard(const Grubby &bot) {
    BuildBoard(bot);
}


// ! Builds a game board
void GameBoard::BuildBoard(const Grubby &bot) {
    const sc2::Unit* unit = obs_->GetUnits(sc2::Unit::Alliance::Self, IsWorker()).front();

    for ( unsigned int i = 0; i < bot.map_height_; ++i ) {
        std::vector<GameTile> row;
        for ( unsigned int j = 0; j < bot.map_width_; ++j ) {
            GameTile tile(j, i);
            
            // Not too sure about this one
            //if ( bot.gm_info_.pathing_grid.data[tile.loc.x + ( ( bot.gm_info_.height - 1 ) - tile.loc.y ) * bot.gm_info_.width] < 255 )
            //    tile.pathable = true;

            // If a worker can walk from their location to the point the tile is pathable
            if( query_->PathingDistance(unit, sc2::Point2D(float(j), float(i))) != 0.0f)
                tile.pathable = true;
            row.push_back(tile);
        }
        board.push_back(row);
    }
}

// ! Gets the specified GameTile
GameTile* GameBoard::GetTile(const unsigned int &x, const unsigned int &y) {
    return &board[x][y];
}