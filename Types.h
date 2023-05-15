#ifndef TYPES_H
#define TYPES_H

typedef enum{
    S_NUM,
    S_TILE,
    S_FLAG,
    S_QEST,
    S_QEST_SAFE,
    S_N
}TileState;
const char *TileStateStr[S_N] = {
    " NUM",
    "TILE",
    "FLAG",
    "QEST",
    "S_QEST_SAFE",
};
const char TileStateChar[S_N] = {
    ' ',
    '#',
    '>',
    '?',
    'S'
};

typedef struct{
    bool isBomb;
    uint num;
    TileState state;
}Tile;

typedef enum{
    B_RNG,
    B_ADJ,
    B_SAT,
    B_N
}BoardType;
const char *BoardTypeStr[B_N] = {
    "B_RNG",
    "B_ADJ",
    "B_SAT"
};

typedef enum{
    BS_NEW,
    BS_WIN,
    BS_MENU,
    BS_OPEN,
    BS_PLAY,
    BS_N
}BoardState;
const char *BoardStateStr[BS_N] = {
    "BS_NEW",
    "BS_WIN",
    "BS_MENU",
    "BS_OPEN",
    "BS_PLAY"
};

typedef struct{
    Length len;
    uint numBombs;
    bool cheat;
    uint lastClickTick;
    Coord lastClick;
    BoardState state;
    BoardType type;
    Tile **tile;
    int lvl;
    File *file;
}Board;

typedef enum{
    A_LEN,
    A_MIN,
    A_TYP,
    A_CHEAT,
    A_N
}ArgType;

typedef enum{E_TEXT, E_CHOICE, E_VALUE, E_N}EntryType;

typedef struct{
    EntryType type;
    uint val;
    char *text;
    Texture *texture;
}Entry;

#endif /* end of include guard: TYPES_H */
