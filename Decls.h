#ifndef DECLS_H
#define DECLS_H

// ARGS_H
void usage(void);
uint parseUint(char *str, const uint len);
ArgType parseArgType(char *arg);
Length parseLen(char *arg);
uint parseBombs(char *arg);
BoardType parseType(char *arg);
Board boardArgs(int argc, char **argv);

// BOARD_H
Board boardFree(Board board);
Board boardAlloc(Board board);
bool validTilePos(const Coord pos, const Length bounds);
uint adjBombs(const Board board, const Coord pos);
Board* boardCalcNums(Board *board);
Board boardReset(Board board);
Board* boardPlaceBombs(Board *board, const Coord firstClick);
uint adjTileState(const Board board, const Coord pos, const TileState state);
uint prop(Board *board, const Coord pos);

// SOLVE_H
uint flagAdj(Board *board, const Coord pos);
uint clearAdj(Board *board, const Coord pos);
bool solvableAdj(Board *board);
bool solvable(Board *board);

// UPDATE_H
Coord mouseTilePos(const uint scale, const Offset boardOff);
Coord tileMousePos(const uint scale, const Offset boardOff, const Coord tpos);
MetaInf boardUpdate(Board *board, MetaInf inf);

// PRINT_H
void printBoard(const Board board);
void printDecals(const Board board);
void printDecalsPos(const Board board, const Coord pos);

// DRAW_H
uint tileScale(const Length win, const Length len);
Offset tileOffset(const Length win, const Length len, const uint scale);
void drawBoardBlank(const Length len, const uint scale, const Offset off);
void drawBoard(const Board board, const MetaInf inf);

// INF_H
MetaInf updateInf(MetaInf inf);
MetaInf infBoard(const Board board);

#endif /* end of include guard: DECLS_H */
