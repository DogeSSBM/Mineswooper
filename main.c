#include "Includes.h"

typedef enum{D_NONE, D_FLAG, D_QUEST, D_N}Decal;
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
    return imin(win.x/len.x, win.x/len.x);
}

void resetTiles(Tile **tile, const Length len)
{
    for(uint x = 0; x < len.x; x++)
        memset(tile[x], 0, sizeof(Tile)*len.y);
}

void placeBombOffset(Tile **tile, const Length len, const Coord firstClick, const uint off)
{
    uint cur = 0;
    for(uint y = 0; y < len.y; y++){
        for(uint x = 0; x < len.x; x++){
            if(cur == off){
                if(tile[x][y].isBomb || (x == firstClick.x && y == firstClick.y))
                    continue;
                tile[x][y].isBomb = true;
            }
            cur += !tile[x][y].isBomb;
        }
    }

}

void placeBombs(Tile **tile, const Length len, const Coord firstClick, const uint numBombs)
{
    if(len.x*len.y < numBombs+1){
        fprintf(stderr, "Can't fit %u bombs in %ix%i (%i) tiles!\n", numBombs, len.x, len.y, len.x*len.y);
        exit(EXIT_FAILURE);
    }

    uint freeTiles = len.x*len.y;
    for(uint i = 0; i < numBombs; i++){
        placeBombOffset(tile, len, firstClick, rand()%freeTiles);
        freeTiles--;
    }
}

Board boardInit(const Length len, const Coord firstClick, const uint numBombs)
{
    Board ret = {
        .len = len,
        .scale = scale(len),
        .numBombs = numBombs,
        .tile = calloc(len.x, sizeof(Tile*))
    };
    for(uint x = 0; x < len.x; x++)
        ret.tile[x] = calloc(len.y, sizeof(Tile));

    placeBombs(ret.tile, len, firstClick, numBombs);
    return ret;
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

bool validTilePos(const Coord pos, const Length len)
{
    return pos.x >= 0 && pos.y >= 0 && pos.x < len.x && pos.y < len.y;
}

void drawBoard(const Board board)
{
    for(uint y = 0; y < board.len.y; y++){
        for(uint x = 0; x < board.len.x; x++){
            const Coord pos = coordMul((const Coord){.x = x, .y = y}, board.scale);
            setColor(BLACK);
            fillSquareCoord(pos, board.scale);
            if(board.tile[x][y].clear){
                setColor(GREY3);
                fillSquareCoordResize(pos, board.scale, -1);
            }else{
                setColor(GREY1);
                fillSquareCoordResize(pos, board.scale, -1);
            }
        }
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

    Length window = {800, 600};
    init();
    setWindowLen(window);

    bool firstClick = true;
    Board board = {0};
    board.scale = scale(len);
    while(1){
        Ticks t = frameStart();

        if(mouseBtnPressed(MOUSE_L)){
            const Coord tilePos = coordDiv(mouse.pos, board.scale);
            if(firstClick){
                if(validTilePos(tilePos, len)){
                    board = boardInit(len, tilePos, bombs);
                }
            }else{

            }
        }
        if(mouseBtnPressed(MOUSE_R)){

        }

        drawBoard(board);

        frameEnd(t);
    }
    return 0;
}
