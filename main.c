#include "DogeLib/Includes.h"

typedef enum{D_NONE, D_FLAG, D_QUEST, D_N}Decal;
const char DecalChar[D_N] = {'#', '>', '?'};
typedef struct{
    bool isBomb;
    bool clear;
    Decal decal;
    uint num;
}Tile;

typedef enum{B_RNG, B_ADJ, B_SAT, B_N}BoardType;
const char *BoardTypeStr[B_N] = {"B_RNG", "B_ADJ", "B_SAT"};
typedef struct{
    BoardType type;
    Length len;
    uint numBombs;
    uint tilesLeft;
    uint scale;
    bool firstClick;
    Tile **tile;
}Board;

typedef enum{A_LEN, A_MIN, A_TYP, A_ERR, A_N}ArgType;

uint scale(const Length len)
{
    const Length win = getWindowLen();
    return imin(win.x/len.x, win.y/len.y);
}

bool validTilePos(const Coord pos, const Length len)
{
    return pos.x >= 0 && pos.y >= 0 && pos.x < len.x && pos.y < len.y;
}

void resetTiles(Tile **tile, const Length len)
{
    for(int x = 0; x < len.x; x++)
        memset(tile[x], 0, sizeof(Tile)*len.y);
}

bool isReserved(const Coord pos, Coord res[9])
{
    for(int i = 0; i < 9; i++)
        if(coordSame(res[i], pos))
            return true;

    return false;
}

void placeBombOffset(Tile **tile, const Length len, Coord res[9], const uint off)
{
    uint cur = 0;
    for(int y = 0; y < len.y; y++){
        for(int x = 0; x < len.x; x++){
            const Coord pos = {.x = x, .y = y};
            if(cur >= off && !(tile[x][y].isBomb || isReserved(pos, res))){
                tile[x][y].isBomb = true;
                return;
            }
            cur += !tile[x][y].isBomb && !isReserved(pos, res);
        }
    }
}

void placeBombs(Tile **tile, const Length len, const Coord firstClick, const int numBombs)
{
    if(!validTilePos(firstClick, len)){
        fprintf(stderr, "Invalid firstClick: (%2i,%2i)!\n", firstClick.x, firstClick.y);
        exit(EXIT_FAILURE);
    }
    if(len.x*len.y < numBombs+9){
        fprintf(stderr, "Can't fit %u bombs in %ix%i (%i) tiles!\n", numBombs, len.x, len.y, len.x*len.y);
        exit(EXIT_FAILURE);
    }

    Coord pos = {.x = firstClick.x, .y = firstClick.y};
    for(Direction d = 0; d < 4; d++){
        const Coord card = coordShift(pos, d, 1);
        if(!validTilePos(card, len)){
            d = 0;
            pos = coordShift(pos, dirINV(d), 1);
        }
    }

    Coord res[9];
    uint i = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            res[i] = (const Coord){.x = pos.x+xo, .y = pos.y+yo};
            i++;
        }
    }

    uint freeTiles = len.x*len.y - 9;
    for(int i = 0; i < numBombs; i++){
        placeBombOffset(tile, len, res, rand()%freeTiles);
        freeTiles--;
    }
}

void getNums(Tile **tile, const Length len)
{
    for(int y = 0; y < len.y; y++){
        for(int x = 0; x < len.x; x++){
            if(tile[x][y].isBomb)
                continue;
            const Coord pos = {.x = x, .y = y};
            for(int yo = -1; yo <= 1; yo++){
                for(int xo = -1; xo <= 1; xo++){
                    const Coord adj = {.x = x+xo, .y = y+yo};
                    if(coordSame(pos, adj) || !validTilePos(adj, len))
                        continue;
                    tile[x][y].num += tile[adj.x][adj.y].isBomb;
                }
            }
        }
    }
}

uint numTilesLeft(Tile **tile, const Length len, const uint numBombs)
{
    if(!tile)
        return 0;
    uint left = len.x*len.y - numBombs;
    for(int y = 0; y < len.y; y++)
        for(int x = 0; x < len.x; x++)
            left -= !tile[x][y].isBomb && tile[x][y].clear;

    return left;
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

Board boardInit(const Length len, const uint bombs, const BoardType type)
{
    Board ret = {
        .type = type,
        .len = len,
        .scale = scale(len),
        .numBombs = bombs,
        .firstClick = true,
        .tile = calloc(len.x, sizeof(Tile*)),
        .tilesLeft = len.x*len.y - bombs
    };
    for(int x = 0; x < len.x; x++)
        ret.tile[x] = calloc(len.y, sizeof(Tile));
    return ret;
}

Board prop(Board, const Coord);
bool solvable(const Board);
Board boardFirstClick(Board board, const Coord firstClick, const uint numBombs)
{
    if(!board.firstClick){
        fprintf(stderr, "Error. Called boardFirstClick when board.firstClick == true\n");
        exit(EXIT_FAILURE);
    }
    ull tries = 0;
    do{
        board = boardFree(board);
        board = boardInit(board.len, numBombs);
        board.firstClick = false;
        placeBombs(board.tile, board.len, firstClick, numBombs);
        getNums(board.tile, board.len);
        board = prop(board, firstClick);
        board.tilesLeft = numTilesLeft(board.tile, board.len, numBombs);
        tries++;
    }while(!solvable(board));
    printf("Generated solvable configuration after %llu tries\n", tries);
    return board;
}

uint parseUint(char *str, const uint len)
{
    char *end;
    const uint ret = strtoul(str, &end, 0);
    if(end-str > len){
        fprintf(stderr, "junk arg: \"%s\"\n", str);
        exit(EXIT_FAILURE);
    }
    return ret;
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
            const Coord pos = coordMul((const Coord){.x = x, .y = y}, board.scale);
            setColor(BLACK);
            fillSquareCoord(pos, board.scale);
            if(board.tile[x][y].clear){
                setColor(GREY3);
                fillSquareCoordResize(pos, board.scale, -1);
                if(board.tile[x][y].num > 0 && board.tile[x][y].num < 9){
                    setTextColor(numColor[board.tile[x][y].num-1]);
                    char txt[2] = "0";
                    txt[0] = '0'+board.tile[x][y].num;
                    drawTextCenteredCoord(txt,coordOffset(pos, (const Coord){.x=board.scale/2,.y=board.scale/2}));
                }
            }else{
                setColor(GREY1);
                fillSquareCoordResize(pos, board.scale, -1);
                setColor(GREY2);
                fillSquareCoordResize(pos, board.scale, -1 -(board.scale/8));
                if(board.tile[x][y].decal == D_FLAG){
                    const uint t1 = board.scale/3;
                    const uint t2 = t1*2;
                    const uint n1 = t1/3;
                    const uint n2 = n1*2;

                    setColor(BLACK);
                    const Coord p1 = coordShift(coordShift(pos, DIR_R, t1+n1), DIR_D, n1);
                    const Coord p2 = coordShift(coordShift(pos, DIR_R, t1+n2), DIR_D, t2+n1);
                    fillRectCoords(p1, p2);
                    const Coord b1 = coordShift(coordShift(pos, DIR_R, t1), DIR_D, t2+n1);
                    const Coord b2 = coordShift(coordShift(pos, DIR_R, t2), DIR_D, t2+n2);
                    fillRectCoords(b1, b2);
                    setColor(RED);
                    Coord f[3] = {
                        coordShift(p1, DIR_R, n1),
                        coordShift(coordShift(p1, DIR_R, t1+n1), DIR_D, n2),
                        coordShift(coordShift(p1, DIR_R, n1), DIR_D, t1+n1)
                    };
                    fillPoly(f, 3);
                }else if(board.tile[x][y].decal == D_QUEST){
                    setTextColor(BLACK);
                    drawTextCenteredCoord("?",coordOffset(pos, (const Coord){.x=board.scale/2,.y=board.scale/2}));
                }
            }
        }
    }
}

void printBoard(const Board board)
{
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
    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            if(board.tile[x][y].clear)
                putchar(board.tile[x][y].num ? '0'+board.tile[x][y].num : '-');
            else
                putchar(DecalChar[board.tile[x][y].decal]);
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}

Board prop(Board board, const Coord pos)
{
    if(!validTilePos(pos, board.len) || board.tile[pos.x][pos.y].isBomb)
        return board;

    board.tile[pos.x][pos.y].clear = true;
    if(board.tile[pos.x][pos.y].num)
        return board;

    // printf("(%i,%i) -\n", pos.x, pos.y);
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
            if(coordSame(pos, adj) || !validTilePos(adj, board.len) || board.tile[adj.x][adj.y].clear)
                continue;
            // printf("\t(%i,%i)\n", adj.x, adj.y);
            board = prop(board, adj);
        }
    }

    return board;
}

void endGame(Board board, const Coord bombClicked, const bool win)
{
    const char *text = win ? " You win! " : " You loose! ";
    const Color textColor = win ? BLUE : RED;
    Length window = getWindowLen();
    while(1){
        const uint t = frameStart();

        const Length newWindow = getWindowLen();
        if(!coordSame(newWindow, window)){
            board.scale = scale(board.len);
        }

        drawBoard(board);
        if(!win){
            const Coord pos = coordMul((const Coord){.x=bombClicked.x, .y=bombClicked.y}, board.scale);
            setColor((const Color){.r = 255, .g = 128, .b = 0, .a = 255});
            fillSquareCoordResize(pos, board.scale, -1);
        }

        setTextSize(board.scale);
        setColor(WHITE);
        const Length tlen = getTextLength(text);
        fillRectCoordLength(coordOffset(getWindowMid(), coordDiv(tlen, -2)), tlen);
        setColor(BLACK);
        fillBorderCoords(coordOffset(getWindowMid(), coordDiv(tlen, -2)), tlen, board.scale/4);

        setTextColor(textColor);
        drawTextCenteredCoord(text, getWindowMid());
        if(keyPressed(SDL_SCANCODE_ESCAPE)){
            boardFree(board);
            exit(0);
        }
        frameEnd(t);
    }
}

uint adjDecal(Board *board, const Coord pos, const Decal decal)
{
    uint ret = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = iC(pos.x+xo, pos.y+yo);
            if(coordSame(pos, adj) || !validTilePos(adj, board->len))
                continue;
            ret += !board->tile[adj.x][adj.y].clear && board->tile[adj.x][adj.y].decal == decal;
        }
    }
    return ret;
}

uint flagAdj(Board *board, const Coord pos)
{
    const uint unflagged = adjDecal(board, pos, D_NONE);
    if(unflagged != board->tile[pos.x][pos.y].num - adjDecal(board, pos, D_FLAG))
        return 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = iC(pos.x+xo, pos.y+yo);
            if(coordSame(pos, adj) || !validTilePos(adj, board->len))
                continue;
            board->tile[adj.x][adj.y].decal = D_FLAG;
        }
    }
    return unflagged;
}

uint clearAdj(Board *board, const Coord pos)
{
    if(adjDecal(board, pos, D_FLAG) != board->tile[pos.x][pos.y].num)
        return 0;
    board->tilesLeft = numTilesLeft(board->tile, board->len, board->numBombs);
    const uint before = board->tilesLeft;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = iC(pos.x+xo, pos.y+yo);
            if(coordSame(pos, adj) || !validTilePos(adj, board->len))
                continue;
            if(!board->tile[adj.x][adj.y].clear && board->tile[adj.x][adj.y].decal == D_NONE)
                *board = prop(*board, adj);
        }
    }
    board->tilesLeft = numTilesLeft(board->tile, board->len, board->numBombs);
    return before - board->tilesLeft;
}

bool solvable(const Board original)
{
    Board board = original;
    board.tile = calloc(board.len.x, sizeof(Tile*));
    for(int x = 0; x < board.len.x; x++){
        board.tile[x] = calloc(board.len.y, sizeof(Tile));
        memcpy(board.tile[x], original.tile[x], sizeof(Tile)*board.len.y);
    }

    uint tries = 3;
    do{
        bool progress = false;
        for(int y = 0; y < board.len.y; y++){
            for(int x = 0; x < board.len.x; x++){
                if(board.tile[x][y].clear){
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

void usage(const char *prog)
{
    printf("Usage -\n");
    printf("\t%s [Width,Height] [Num Bombs] [RNG|ADJ|SAT]\n\n", prog);
    printf("Default options are: 30,16 99 RNG\n");
    exit(EXIT_FAILURE);
}

ArgType parseType(const char *arg)
{
    if(!arg || strlen(arg) == 0)
        return A_ERR;
    if(isdigit(*arg)){
        if(strchr(arg, ','))
            return A_LEN;
        return A_MIN;
    }
    return A_TYP;
}

Length parseLen(char *arg)
{
    const uint slen = strlen(arg);
    const char *comma = strchr(arg, ',');
    return iC(
        parseUint(arg, comma-arg),
        parseUint(comma+1, strlen(comma+1))
    );
}

uint parseBombs(const char *arg)
{
    return parseUint(arg, strlen(arg));
}

BoardType parseType(const char *arg)
{
    if(strlen(arg) != 3)
        return A_ERR;
}


Board parseArgs(const int argc, char **argv)
{
    if(argc > 4)
        usage(argv[0]);

    BoardType type = B_RNG;
    Length len = iC(30, 16);
    uint bombs = 99;

    bool lenDone = false;
    bool bombsDone = false;
    bool typeDone = false;
    for(uint i = 1; i < argc; i++){
        switch(parseType(argv[i])){
            case A_LEN:
                if(lenDone)
                    usage(argv[0]);
                len = parseLen(argv[i]);
                lenDone = true;
                break;
            case A_MIN:
                if(bombsDone)
                    usage(argv[0]);
                bombs = parseBombs(argv[i]);
                bombsDone = true;
                break;
            case A_TYP:
                if(typeDone)
                    usage(argv[0]);
                type = parseType(argv[i]);
                typeDone = true;
                break;
            case A_ERR:
            default:
                usage(argv[0]);
                break;
        }
    }

    return boardInit(len, bombs, type);
}

int main(int argc, char **argv)
{
    Length len = {.x = 30, .y = 16};
    uint bombs = 99;
    switch(argc){
        case 4:
            len.x = parseUint(argv[1]);
            len.y = parseUint(argv[2]);
            bombs = parseUint(argv[3]);
            break;
        case 3:
            len.x = parseUint(argv[1]);
            len.y = parseUint(argv[2]);
            break;
        case 2:
            bombs = parseUint(argv[1]);
            break;
        default:
            break;
    }

    printf("%ux%u - %u\n", len.x, len.y, bombs);
    init();
    Length window = maximizeWindow();
    Board board = boardInit(len, bombs, type);

    Coord down[2] = {0};

    while(1){
        const uint t = frameStart();

        if(mouseBtnPressed(MOUSE_L))
            down[0] = coordDiv(mouse.pos, board.scale);

        if(mouseBtnPressed(MOUSE_R))
            down[1] = coordDiv(mouse.pos, board.scale);

        Coord tilePos;
        if(mouseBtnReleased(MOUSE_L) && coordSame(down[0], (tilePos = coordDiv(mouse.pos, board.scale)))){
            // printf("M_L - (%3i,%3i)[%2i,%2i]\n", mouse.pos.x, mouse.pos.y, tilePos.x, tilePos.y);
            if(validTilePos(tilePos, board.len)){
                if(board.firstClick){
                    board = boardFirstClick(board, tilePos, bombs);
                }else{
                    if(board.tile[tilePos.x][tilePos.y].isBomb)
                        endGame(board, tilePos, false);
                    if(!board.tile[tilePos.x][tilePos.y].clear){
                        const uint before = board.tilesLeft;
                        board = prop(board, tilePos);
                        board.tilesLeft = numTilesLeft(board.tile, board.len, board.numBombs);
                        printf(
                            "Before: %u, After: %u, Cleared: %u\n",
                            before, board.tilesLeft, before - board.tilesLeft
                        );
                        if(board.tilesLeft == 0)
                            endGame(board, tilePos, true);
                    }
                }
            }
        }
        if(mouseBtnReleased(MOUSE_R)){
            const Coord tilePos = coordDiv(mouse.pos, board.scale);
            if(validTilePos(tilePos, board.len)){
                board.tile[tilePos.x][tilePos.y].decal++;
                board.tile[tilePos.x][tilePos.y].decal %= D_N;
            }
        }

        const Length newWindow = getWindowLen();
        if(!coordSame(newWindow, window)){
            window = newWindow;
            board.scale = scale(board.len);
        }

        if(keyPressed(SDL_SCANCODE_ESCAPE)){
            boardFree(board);
            return 0;
        }

        drawBoard(board);

        frameEnd(t);
    }
    return 0;
}
