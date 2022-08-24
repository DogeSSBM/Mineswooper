#ifndef TYPES_H
#define TYPES_H

typedef enum{S_NUM, S_TILE, S_FLAG, S_QEST, S_N}TileState;
typedef struct{
    bool isBomb;
    uint num;
    TileState state;
}Tile;

typedef enum{B_RNG, B_ADJ, B_SAT, B_N}BoardType;
typedef struct{
    Length len;
    Offset off;
    uint scale;
    uint numBombs;
    uint tilesLeft;
    bool bombsPlaced;
    BoardType type;
    Tile **tile;
}Board;

typedef enum{A_LEN, A_MIN, A_TYP, A_N}ArgType;

typedef enum{C_NONE, C_CLEAR, C_FLAG, C_CHORD, C_N}ClickType;

#endif /* end of include guard: TYPES_H */
