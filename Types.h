#ifndef TYPES_H
#define TYPES_H

typedef enum{
    S_NUM,
    S_TILE,
    S_FLAG,
    S_QEST,
    S_N
}TileState;
const char *TileStateStr[S_N] = {
    " NUM",
    "TILE",
    "FLAG",
    "QEST"
};
const char TileStateChar[S_N] = {
    ' ',
    '#',
    '>',
    '?'
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
    BS_LOOSE,
    BS_PLAY,
    BS_N
}BoardState;
const char *BoardStateStr[BS_N] = {
    "BS_NEW",
    "BS_LOOSE",
    "BS_PLAY"
};

typedef struct{
    Length len;
    uint numBombs;
    bool cheat;
    Coord lastClick;
    BoardState state;
    BoardType type;
    Tile **tile;
}Board;

typedef enum{
    A_LEN,
    A_MIN,
    A_TYP,
    A_CHEAT,
    A_N
}ArgType;

typedef enum{
    C_NONE,
    C_CLEAR,
    C_DECAL,
    C_CHORD,
    C_LOOSE,
    C_START,
    C_N
}ClickType;
const char *ClickTypeStr[C_N] = {
    "C_NONE",
    "C_CLEAR",
    "C_DECAL",
    "C_CHORD",
    "C_LOOSE",
    "C_START"
};

#endif /* end of include guard: TYPES_H */
