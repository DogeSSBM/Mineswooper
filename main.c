#include "Includes.h"

typedef enum{D_NONE, D_FLAG, D_QUEST, D_N}Decal;
const char DecalChar[D_N] = {'N', 'F', 'Q'};
typedef struct{
    bool isBomb;
    bool clear;
    Decal decal;
    uint num;
}Tile;

typedef struct{
    Length len;
    uint numBombs;
    uint tilesLeft;
    uint scale;
    Tile **tile;
}Board;

int imin(const int a, const int b)
{
    return a < b ? a : b;
}

int imax(const int a, const int b)
{
    return a > b ? a : b;
}

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
    for(uint x = 0; x < len.x; x++)
        memset(tile[x], 0, sizeof(Tile)*len.y);
}

bool isReserved(const Coord pos, Coord res[9])
{
    for(uint i = 0; i < 9; i++)
        if(coordSame(res[i], pos))
            return true;

    return false;
}

uint bombsBefore(Tile **tile, const Length len, const Coord pos)
{
    uint total = 0;
    for(uint y = 0; y < len.y; y++)
        for(uint x = 0; x < len.x; x++)
            total+=tile[x][y].isBomb;

    return total;
}

void placeBombOffset(Tile **tile, const Length len, Coord res[9], const uint off)
{
    uint cur = 0;
    for(uint y = 0; y < len.y; y++){
        for(uint x = 0; x < len.x; x++){
            const Coord pos = {.x = x, .y = y};
            if(cur >= off && !(tile[x][y].isBomb || isReserved(pos, res))){
                tile[x][y].isBomb = true;
                return;
            }
            cur += !tile[x][y].isBomb && !isReserved(pos, res);
        }
    }
}

void placeBombs(Tile **tile, const Length len, const Coord firstClick, const uint numBombs)
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
    for(uint i = 0; i < numBombs; i++){
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
        for(uint x = 0; x < board.len.x; x++)
            free(board.tile[x]);
        free(board.tile);
        board.tile = NULL;
    }
    return board;
}

Board boardInit(const Length len)
{
    Board ret = {
        .len = len,
        .scale = scale(len),
        .numBombs = 0,
        .tile = calloc(len.x, sizeof(Tile*))
    };
    for(uint x = 0; x < len.x; x++)
        ret.tile[x] = calloc(len.y, sizeof(Tile));
    return ret;
}

Board boardFirstClick(Board board, const Coord firstClick, const uint numBombs)
{
    board = boardFree(board);
    board = boardInit(board.len);
    board.numBombs = numBombs;
    placeBombs(board.tile, board.len, firstClick, numBombs);
    board.tilesLeft = numTilesLeft(board.tile, board.len, numBombs);
    getNums(board.tile, board.len);
    return board;
}

uint parseUint(char *str)
{
    char *end;
    const uint ret = strtoul(str, &end, 0);
    if(end != str+strlen(str)){
        fprintf(stderr, "junk arg: \"%s\"\n", str);
        exit(EXIT_FAILURE);
    }
    return ret;
}

void drawBoard(const Board board)
{
    const static Color numColor[] = {
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

    for(uint y = 0; y < board.len.y; y++){
        for(uint x = 0; x < board.len.x; x++){
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
    for(uint y = 0; y < board.len.y; y++){
        for(uint x = 0; x < board.len.x; x++){
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

Board prop(Board board, const Coord pos)
{
    if(!validTilePos(pos, board.len) || board.tile[pos.x][pos.y].isBomb)
        return board;

    board.tile[pos.x][pos.y].clear = true;
    if(board.tile[pos.x][pos.y].num)
        return board;

    printf("(%i,%i) -\n", pos.x, pos.y);
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
            if(coordSame(pos, adj) || !validTilePos(adj, board.len) || board.tile[adj.x][adj.y].clear)
                continue;
            printf("\t(%i,%i)\n", adj.x, adj.y);
            board = prop(board, adj);
        }
    }

    return board;
}

void loose(Board board, const Coord bombClicked)
{
    Length window = getWindowLen();
    while(1){
        Ticks t = frameStart();

        const Length newWindow = getWindowLen();
        if(!coordSame(newWindow, window)){
            board.scale = scale(board.len);
        }

        drawBoard(board);
        const Coord pos = coordMul((const Coord){.x=bombClicked.x, .y=bombClicked.y}, board.scale);
        setColor((const Color){.r = 255, .g = 128, .b = 0, .a = 255});
        fillSquareCoordResize(pos, board.scale, -1);

        setTextSize(board.scale);
        setColor(WHITE);
        const Length tlen = getTextLength(" You loose! ");
        fillRectCoordLength(coordOffset(getWindowMid(), coordDiv(tlen, -2)), tlen);
        setColor(BLACK);
        fillBorderCoords(coordOffset(getWindowMid(), coordDiv(tlen, -2)), tlen, board.scale/4);

        setTextColor(RED);
        drawTextCenteredCoord(" You loose! ", getWindowMid());
        if(keyPressed(SDL_SCANCODE_ESCAPE)){
            boardFree(board);
            exit(0);
        }
        frameEnd(t);
    }
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
    Length window = {800, 600};
    init();
    setWindowLen(window);
    window = maximizeWindow();
    bool firstClick = true;
    Board board = boardInit(len);

    Coord down[2] = {0};

    while(1){
        Ticks t = frameStart();

        if(mouseBtnPressed(MOUSE_L))
            down[0] = coordDiv(mouse.pos, board.scale);

        if(mouseBtnPressed(MOUSE_R))
            down[1] = coordDiv(mouse.pos, board.scale);

        Coord tilePos;
        if(mouseBtnReleased(MOUSE_L) && coordSame(down[0], (tilePos = coordDiv(mouse.pos, board.scale)))){
            printf("M_L - (%3i,%3i)[%2i,%2i]\n", mouse.pos.x, mouse.pos.y, tilePos.x, tilePos.y);
            if(validTilePos(tilePos, board.len)){
                if(firstClick){
                    board = boardFirstClick(board, tilePos, bombs);
                    // printBoard(board);
                    firstClick = false;
                    board = prop(board, tilePos);
                }else{
                    if(board.tile[tilePos.x][tilePos.y].isBomb)
                        loose(board, tilePos);
                    if(!board.tile[tilePos.x][tilePos.y].clear)
                        board = prop(board, tilePos);
                }
            }
        }
        if(mouseBtnReleased(MOUSE_R)){
            const Coord tilePos = coordDiv(mouse.pos, board.scale);
            if(validTilePos(tilePos, board.len)){
                board.tile[tilePos.x][tilePos.y].decal++;
                board.tile[tilePos.x][tilePos.y].decal %= D_N;
                printf(
                    "M_R - (%3i,%3i)[%2i,%2i]: %c\n",
                    mouse.pos.x,
                    mouse.pos.y,
                    tilePos.x,
                    tilePos.y,
                    DecalChar[board.tile[tilePos.x][tilePos.y].decal]
                );
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
