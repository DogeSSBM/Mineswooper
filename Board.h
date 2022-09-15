#ifndef BOARD_H
#define BOARD_H

Board boardFree(Board board)
{
    if(board.tile == NULL)
        panic("board.tile == NULL");
    for(int x = 0; x < board.len.x; x++)
        free(board.tile[x]);
    free(board.tile);
    board.tile = NULL;
    printf("Free'd board: {%i,%i}\n", board.len.x, board.len.y);
    return board;
}

Board boardAlloc(Board board)
{
    if(board.tile)
        panic("Tried to boardAlloc when board.tile was not NULL");
    if(coordMin(board.len) < 4)
        panic("Cannot boardAlloc anything < {4,4}, board.len: {%i,%i}\n", board.len.x, board.len.y);
    board.tile = calloc(board.len.x, sizeof(Tile*));
    for(int x = 0; x < board.len.x; x++){
        board.tile[x] = calloc(board.len.y, sizeof(Tile));
        for(int y = 0; y < board.len.y; y++)
            board.tile[x][y].state = S_TILE;
    }
    printf("Allocating new board: {%i,%i}\n", board.len.x, board.len.y);

    return board;
}

bool validTilePos(const Coord pos, const Length bounds)
{
    return inBound(pos.x, 0, bounds.x) && inBound(pos.y, 0, bounds.y);
}

uint adjBombs(const Board board, const Coord pos)
{
    uint count = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
            count += validTilePos(adj, board.len) && !coordSame(pos, adj) && board.tile[adj.x][adj.y].isBomb;
        }
    }
    return count;
}

Board* boardCalcNums(Board *board)
{
    for(int y = 0; y < board->len.y; y++){
        for(int x = 0; x < board->len.x; x++){
            board->tile[x][y].num = adjBombs(*board, iC(x,y));
        }
    }
    return board;
}

Board boardReset(Board board)
{
    if(!board.tile)
        return boardAlloc(board);
    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            board.tile[x][y].state = S_TILE;
            board.tile[x][y].isBomb = false;
            board.tile[x][y].num = 0;
        }
    }
    board.state = BS_NEW;
    return board;
}

Board* boardRestart(Board *board)
{
    if(!board->tile)
        panic("Cant boardRestart when board->tiles == NULL");

    printf("Restarting board\n");
    for(int y = 0; y < board->len.y; y++)
        for(int x = 0; x < board->len.x; x++)
            board->tile[x][y].state = S_TILE;

    board->state = BS_PLAY;
    return board;
}

Board* boardRngBombs(Board *board)
{
    *board = boardReset(*board);
    for(uint i = 0; i < board->numBombs; i++){
        Coord pos;
        do{
            pos.x = rand()%board->len.x;
            pos.y = rand()%board->len.y;
        }while(
            board->tile[pos.x][pos.y].isBomb || (
                inBound(pos.x, board->lastClick.x-1,board->lastClick.x+2) &&
                inBound(pos.y, board->lastClick.y-1,board->lastClick.y+2)
            )
        );
        board->tile[pos.x][pos.y].isBomb = true;
    }
    return board;
}

Board* boardPlaceBombs(Board *board, const Coord firstClick)
{
    if(board->state != BS_NEW)
        panic("can only boardPlaceBombs when board->state == BS_NEW, board->state: %s", BoardStateStr[board->state]);
    *board = boardReset(*board);
    board->lastClick = firstClick;
    for(Direction d = 0; d < 4; d++)
        if(!validTilePos(coordShift(board->lastClick, d, 1), board->len))
            board->lastClick = coordShift(board->lastClick, dirINV(d), 1);

    uint i = 0;
    do{
        if(i > 50000)
            panic(">50000");
        i++;
        printf("try: %u\n", i);
        floodFill(boardCalcNums(boardRngBombs(board)), board->lastClick);
    }while(board->type != B_RNG && !solvable(board));
    printf("%u tries\n", i);
    printBoard(*board);
    printDecals(*board);
    board = boardRestart(board);
    printf("cleared %u tiles\n", floodFill(board, board->lastClick));
    const uint left = boardRemaining(*board);
    printf("Tiles remaining: %u\n", left);
    return board;
}

uint adjTileState(const Board board, const Coord pos, const TileState state)
{
    uint count = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
            count += (
                !coordSame(pos, adj) &&
                validTilePos(adj, board.len) &&
                board.tile[adj.x][adj.y].state == state
            );
        }
    }
    return count;
}

uint floodFill(Board *board, const Coord pos)
{
    if(board->state == BS_NEW)
        board->state = BS_PLAY;

    if(board->state != BS_PLAY)
        panic(
            "can only prop when board->state == BS_PLAY, board->state: %s",
            BoardStateStr[board->state]
        );

    if(!validTilePos(pos, board->len) || board->tile[pos.x][pos.y].state != S_TILE)
        return 0;
    if(board->tile[pos.x][pos.y].isBomb){
        printBoard(*board);
        printf("\n");
        printDecals(*board);
        printf("\n");
        printDecalsPos(*board, pos);
        panic("tried to prop bomb at %i,%i\n", pos.x, pos.y);
    }
    uint cleared = board->tile[pos.x][pos.y].state == S_TILE;
    board->tile[pos.x][pos.y].state = S_NUM;
    if(board->tile[pos.x][pos.y].num > 0)
        return cleared;

    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
            if(
                coordSame(pos, adj) ||
                !validTilePos(adj, board->len) ||
                board->tile[adj.x][adj.y].state != S_TILE
            )
                continue;
            cleared += floodFill(board, adj);
        }
    }

    return cleared;
}

uint boardRemaining(const Board board)
{
    uint total = 0;
    for(int y = 0; y < board.len.y; y++)
        for(int x = 0; x < board.len.x; x++)
            total += board.tile[x][y].state != S_NUM;
    return total - board.numBombs;
}

uint boardNumState(const Board board, const TileState state)
{
    uint total = 0;
    for(int y = 0; y < board.len.y; y++)
        for(int x = 0; x < board.len.x; x++)
            total += board.tile[x][y].state == state;
    return total;
}

#endif /* end of include guard: BOARD_H */
