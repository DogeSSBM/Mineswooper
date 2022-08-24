#ifndef BOARD_H
#define BOARD_H

void printBoard(const Board board)
{
    if(!board.tile){
        printf("board.tile == NULL\n");
        return;
    }
    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            if(board.tile[x][y].isBomb)
                putchar('B');
            else
                putchar('0'+board.tile[x][y].num);
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}

void printDecals(const Board board)
{
    if(!board.tile){
        printf("board.tile == NULL\n");
        return;
    }    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            if(board.tile[x][y].state == S_NUM)
                putchar(board.tile[x][y].num ? '0'+board.tile[x][y].num : '-');
            else
                putchar(board.tile[x][y].state == S_QEST ? '?' : '>');
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}

uint lin(const Coord pos, const Length len)
{
    return pos.y*len.x+pos.x;
}

Coord delin(const uint pos, const Length len)
{
    return iC(pos%len.x, pos/len.x);
}

uint tileScale(const Length win, const Length len)
{

    return imin(win.x/len.x, win.y/len.y);
}

Offset tileOffset(const Length win, const Length len, const uint scale)
{
    return coordDiv(coordSub(win, coordMul(len, scale)), 2);
}

Board boardFit(Board board)
{
    const Length win = getWindowLen();
    board.scale = tileScale(win, board.len);
    board.off = tileOffset(win, board.len, board.scale);
    return board;
}

Board boardFree(Board board)
{
    if(board.tile){
        for(int x = 0; x < board.len.x; x++)
            free(board.tile[x]);
        free(board.tile);
        board.tile = NULL;
    }
    return board;
}

Board boardAlloc(Board board)
{
    if(board.tile)
        panic("Ya gotta boardFree() before you boardAlloc()!!!1!");
    board.tile = calloc(board.len.x, sizeof(Tile*));
    for(int x = 0; x < board.len.x; x++){
        board.tile[x] = calloc(board.len.y, sizeof(Tile));
        for(int y = 0; y < board.len.y; y++)
            board.tile[x][y].state = S_TILE;
    }

    return board;
}

void drawBoard(const Board board)
{
    static const Color numColor[] = {
        {0x00, 0x00, 0xAA, 0xFF},
        {0x00, 0x52, 0x00, 0xFF},
        {0xAA, 0x00, 0x00, 0xFF},
        {0x00, 0x00, 0x52, 0xFF},
        {0x52, 0x00, 0x00, 0xFF},
        {0x00, 0x52, 0x52, 0xFF},
        {0x00, 0x00, 0x00, 0xFF},
        {0x52, 0x52, 0x52, 0xFF},
    };
    setTextSize(board.scale);

    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            const Coord pos = coordOffset(coordMul(iC(x, y), board.scale), board.off);
            if(board.tile[x][y].state == S_NUM){
                setColor(GREY3);
                fillSquareCoordResize(pos, board.scale, -1);
                if(board.tile[x][y].num > 0){
                    setTextSize(board.scale);
                    setTextColor(numColor[board.tile[x][y].num]);
                    char txt[2] = "0";
                    txt[0] = '0'+board.tile[x][y].num;
                    drawTextCenteredCoord(txt, coordOffset(pos, iC(board.scale/2, board.scale/2)));
                }
            }else{
                setColor(GREY1);
                fillSquareCoordResize(pos, board.scale, -1);
                setColor(GREY2);
                fillSquareCoordResize(pos, board.scale, -1 -(board.scale/8));
            }
        }
    }
}

bool validTilePos(const Coord pos, const Coord bounds)
{
    return inBound(pos.x, 0, bounds.x) && inBound(pos.y, 0, bounds.y);
}

bool inArr(const uint n, uint *arr, const uint len)
{
    for(uint i = 0; i < len; i++)
        if(arr[i] == n)
            return true;
    return false;
}

uint adjTileState(const Board board, const Coord pos, const TileState state)
{
    uint count = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
            count += validTilePos(adj, board.len) && !coordSame(pos, adj) && board.tile[adj.x][adj.y].state == state;
        }
    }
    return count;
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

Board boardRng(Board board, Coord firstClick)
{
    for(Direction d = 0; d < 4; d++){
        if(!validTilePos(coordShift(firstClick, d, 1), board.len)){
            d = 0;
            firstClick = coordShift(firstClick, dirINV(d), 1);
        }
    }

    uint res[9];
    for(int yo = -1, i = 0; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            res[i] = lin(iC(firstClick.x+xo, firstClick.y+yo), board.len);
            i++;
        }
    }

    const uint total = board.len.x * board.len.y;
    uint i = 0;
    while(i < board.numBombs){
        const uint n = rand()%total;
        const Coord pos = delin(n, board.len);
        if(!inArr(n, res, 9) && !board.tile[pos.x][pos.y].isBomb){
            board.tile[pos.x][pos.y].isBomb = true;
            i++;
        }
    }

    return board;
}

Board boardInit(Board board, const Coord firstClick)
{
    board = boardFree(board);
    if(!validTilePos(firstClick, board.len)){
        fprintf(stderr, "Firstclick invalid\n");
        fprintf(stderr, "firstclick: %i,%i\n", firstClick.x, firstClick.y);
        fprintf(stderr, "board.len: %i,%i\n", board.len.x, board.len.y);
        exit(EXIT_FAILURE);
    }
    const uint numTiles = board.len.x*board.len.y;
    if(numTiles < board.numBombs+9){
        fprintf(stderr, "Need at least 3x3 non bomb tiles\n");
        fprintf(
            stderr, "Can't fit %u bombs in %ix%i (%i) tiles!\n",
            board.numBombs, board.len.x, board.len.y, board.len.x*board.len.y
        );
        usage();
    }
    board = boardAlloc(board);
    switch(board.type){
        case B_RNG:
            board = boardRng(board, firstClick);
            break;
        case B_ADJ:

            break;
        case B_SAT:
        default:
            usage();
            break;
    }
    board.bombsPlaced = true;

    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            board.tile[x][y].num = adjBombs(board, iC(x,y));
        }
    }

    return board;
}

Board boardArgs(int argc, char **argv)
{
    Board board = {
        .len = iC(30, 16),
        .numBombs = 99
    };

    bool lenDone = false;
    bool bombsDone = false;
    // bool typeDone = false;
    for(int i = 1; i < argc; i++){
        switch(parseArgType(argv[i])){
            case A_LEN:
                if(lenDone)
                    usage();
                board.len = parseLen(argv[i]);
                lenDone = true;
                break;
            case A_MIN:
                if(bombsDone)
                    usage();
                board.numBombs = parseBombs(argv[i]);
                bombsDone = true;
                break;
            // case A_TYP:
            //     if(typeDone)
            //         usage();
            //     board.type = parseType(argv[i]);
            //     typeDone = true;
            //     break;
            default:
                usage();
                break;
        }
    }

    return boardAlloc(board);
}

Board prop(Board board, const Coord pos)
{
    if(!board.bombsPlaced){
        board = boardInit(board, pos);
        board.bombsPlaced = true;
        printBoard(board);
    }

    if(!validTilePos(pos, board.len) || board.tile[pos.x][pos.y].state != S_TILE)
        return board;

    board.tile[pos.x][pos.y].state = S_NUM;
    if(board.tile[pos.x][pos.y].num)
        return board;

    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
            if(
                coordSame(pos, adj) ||
                !validTilePos(adj, board.len) ||
                board.tile[pos.x][pos.y].state != S_TILE
            )
                continue;
            board = prop(board, adj);
        }
    }

    return board;
}

#endif /* end of include guard: BOARD_H */
