#ifndef SOLVE_H
#define SOLVE_H

uint flagAdj(Board *board, const Coord pos)
{
    const uint unflagged = adjTileState(*board, pos, S_TILE);
    if(unflagged != board->tile[pos.x][pos.y].num - adjTileState(*board, pos, S_FLAG))
        return 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = iC(pos.x+xo, pos.y+yo);
            if(coordSame(pos, adj) || !validTilePos(adj, board->len))
                continue;
            board->tile[adj.x][adj.y].state = S_FLAG;
        }
    }
    return unflagged;
}

Board numTilesLeft(Board board)
{
    board.tilesLeft = board.len.x * board.len.y - board.numBombs;
    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            board.tilesLeft -= board.tile[x][y].state == S_NUM;
        }
    }
    return board;
}

uint clearAdj(Board *board, const Coord pos)
{
    if(adjTileState(*board, pos, S_FLAG) != board->tile[pos.x][pos.y].num)
        return 0;
    *board = numTilesLeft(*board);
    const uint before = board->tilesLeft;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = iC(pos.x+xo, pos.y+yo);
            if(coordSame(pos, adj) || !validTilePos(adj, board->len))
                continue;
            if(
                board->tile[adj.x][adj.y].state == S_NUM &&
                board->tile[adj.x][adj.y].num == 0 &&
                board->tile[adj.x][adj.y].state == S_TILE
            )
                *board = prop(*board, adj);
        }
    }
    *board = numTilesLeft(*board);
    return before - board->tilesLeft;
}

bool solvableAdj(const Board original, const Coord firstClick)
{
    Board board = original;
    board.tile = calloc(board.len.x, sizeof(Tile*));
    for(int x = 0; x < board.len.x; x++){
        board.tile[x] = calloc(board.len.y, sizeof(Tile));
        memcpy(board.tile[x], original.tile[x], sizeof(Tile)*board.len.y);
    }
    board = prop(board, firstClick);

    uint tries = 3;
    do{
        bool progress = false;
        for(int y = 0; y < board.len.y; y++){
            for(int x = 0; x < board.len.x; x++){
                if(board.tile[x][y].state == S_NUM){
                    const Coord pos = iC(x,y);
                    progress |= flagAdj(&board, pos);
                    const uint cleared = clearAdj(&board, pos);
                    progress |= cleared;
                    board.tilesLeft -= cleared;
                }
            }
        }
        tries = progress ? 3 : tries-1;
    }while(tries);

    boardFree(board);
    return !board.tilesLeft;
}

#endif /* end of include guard: SOLVE_H */
