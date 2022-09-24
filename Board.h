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
    printf("Bombs: %u\n", board.numBombs);

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
        if(i > 10000000)
            panic(">10000000");
        ++i;
        if((i-1)/100 != i/100)
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

Length boardFileLength(File *file)
{
    if(file == NULL)
        panic("Cant get length of NULL file");
    Length len = {0};
    int c;
    while((c = fgetc(file)) != EOF && c != '\n')
        len.x++;
    printf("x: %i\n", len.x);
    rewind(file);

    while((c = fgetc(file)) != EOF){
        len.y += c == '\n';
    }

    rewind(file);
    printf("y: %i\n", len.y);

    for(int y = 0; y < len.y; y++){
        for(int x = 0; x < len.x; x++){
            if((c = fgetc(file))=='\n'){
                fclose(file);
                panic("unexpected '\\n'. char pos: %i,%i", x, len.y);
            }else if(c == EOF){
                fclose(file);
                panic("unexpected EOF. char pos: %i,%i", x, len.y);
            }
        }
        if((c = fgetc(file)) != '\n'){
            fclose(file);
            panic("expected '\\n' at char pos: %i,%i", len.x, y);
        }
    }
    if((c = fgetc(file)) != EOF){
        fclose(file);
        panic("Expected EOF at pos: %i,%i", len.x, len.y);
    }

    rewind(file);
    printf("Board file len: %i,%i\n", len.x, len.y);
    return len;
}

Tile tileUnpack(char c)
{
    Tile tile = {0};
    tile.isBomb = 1 & (c >> 7);
    tile.num = 15 & (c >> 3);
    tile.state = 7 & c;
    return tile;
}

char tilePack(const Tile tile)
{
    return tile.state | (tile.num << 3) | (tile.isBomb << 7);
}

bool boardSave(Board *board)
{

    for(uint i = 0; i < ~0u; i++){
        char path[64] = {0};
        sprintf(path, "./Saves/%u.swoop", i);
        File *file = fopen(path, "r");
        if(file){
            fclose(file);
        }else{
            printf("Saving board in file \"%s\"\n", path);
            file = fopen(path, "w");
            for(int y = 0; y < board->len.y; y++){
                for(int x = 0; x < board->len.x; x++)
                    fputc(tilePack(board->tile[x][y]), file);
                if(fputc('\n', file) == EOF){
                    fclose(file);
                    return false;
                }
            }
            fclose(file);
            return true;
        }
    }
    return false;
}

bool boardLoad(Board *board)
{
    char buf[64] = {0};
    sprintf(buf, "./Saves/%i.swoop", board->lvl);
    if((board->file = fopen(buf, "r")) == NULL){
        printf("Board at \"%s\" does not exitst\n", buf);
        return false;
    }
    board->len = boardFileLength(board->file);
    printf("len.x: %i\nlen.y: %i\n", board->len.x, board->len.y);
    if(board->tile)
        *board = boardFree(*board);

    *board = boardAlloc(*board);
    int c;
    for(int y = 0; y < board->len.y; y++){
        for(int x = 0; x < board->len.x; x++){
            if((c = fgetc(board->file))=='\n'){
                fclose(board->file);
                panic("unexpected '\\n'. char pos: %i,%i", x, y);
                printf("Board file len: %i,%i\n", board->len.x, board->len.y);
            }else if(c == EOF){
                fclose(board->file);
                panic("unexpected EOF. char pos: %i,%i", x, y);
                printf("Board file len: %i,%i\n", board->len.x, board->len.y);
            }
            board->tile[x][y] = tileUnpack(c);
        }
        if((c = fgetc(board->file)) != '\n'){
            fclose(board->file);
            panic("Expected newline at end of line %i\n", y);
        }
    }
    if((c = fgetc(board->file)) != EOF){
        fclose(board->file);
        panic("Expected EOF at end of line %i\n", board->len.y);
    }

    return true;
}

#endif /* end of include guard: BOARD_H */
