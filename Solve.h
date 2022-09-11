#ifndef SOLVE_H
#define SOLVE_H

uint flagAdj(Board *board, const Coord pos)
{
    uint flagged = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = iC(pos.x+xo, pos.y+yo);
            if(
                coordSame(pos, adj) ||
                !validTilePos(adj, board->len) ||
                board->tile[adj.x][adj.y].state != S_TILE
            )
                continue;

            board->tile[adj.x][adj.y].state = S_FLAG;
            flagged++;
        }
    }
    return flagged;
}

uint clearAdj(Board *board, const Coord pos)
{
    // printDecalsPos(*board, pos);
    // printf("adj flags: %u tiles:%u\n\n",
    //     adjTileState(*board, pos, S_FLAG),
    //     adjTileState(*board, pos, S_TILE)
    // );
    uint cleared = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = iC(pos.x+xo, pos.y+yo);
            if(
                !coordSame(pos, adj) &&
                validTilePos(adj, board->len) &&
                board->tile[adj.x][adj.y].state == S_TILE
            ){
                if(board->tile[adj.x][adj.y].isBomb){
                    printDecalsPos(*board, adj);
                    panic("Cleared adj bomb at: (%i,%i)", adj.x, adj.y);
                }
                cleared += prop(board, adj);
            }
        }
    }
    return cleared;
}

/*
while progress
    trivial flags
    trivial clears

moar complicated shit

*/

bool solvableAdj(Board *board)
{
    uint tries = 3;
    do{
        bool progress = false;
        for(int y = 0; y < board->len.y; y++){
            for(int x = 0; x < board->len.x; x++){
                const Coord pos = iC(x,y);
                if(board->tile[x][y].num && board->tile[x][y].state == S_NUM){
                    if(adjTileState(*board, pos, S_TILE) <=
                        board->tile[x][y].num - adjTileState(*board, pos, S_FLAG)
                    )
                        progress |= flagAdj(board, pos);

                    if(adjTileState(*board, pos, S_FLAG) == board->tile[x][y].num)
                        progress |= clearAdj(board, pos);
                }
            }
        }
        tries = progress ? 3 : tries-1;
    }while(tries);

    return !boardRemaining(*board);
}

bool solvable(Board *board)
{
    if(board->type == B_RNG)
        return true;

    if(board->type == B_ADJ)
        return solvableAdj(board);

    return true;
}

#endif /* end of include guard: SOLVE_H */
