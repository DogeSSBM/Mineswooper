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
uint floodFill(Board *board, const Coord pos);
uint boardRemaining(const Board board);
uint boardNumState(const Board board, const TileState state);

// SAVE_H
Length boardFileLength(File *file);
Tile tileUnpack(char c);
char tilePack(const Tile tile);
bool boardSave(Board *board);
bool boardLoad(Board *board);

// SOLVE_H
bool clear121(Board *board, const Coord pos);
uint flagAdj(Board *board, const Coord pos);
uint setAdj(Board *board, const Coord pos, const TileState state);
uint clearAdj(Board *board, const Coord pos);
Board* resetQuests(Board *board);
bool isAbsurd(Board *board);
bool reductioAdAbsurdum(Board *board);
bool satSolve(Board *board);
bool solve(Board *board, const bool patterns);
bool solvable(Board *board);

// UPDATE_H
Coord mouseTilePos(const uint scale, const Offset boardOff);
Coord tileMousePos(const uint scale, const Offset boardOff, const Coord tpos);
void checkQuit(Board *board);
bool checkCheat(const bool cheat);
bool checkLen(Board* board);
Coord checkNewGame(Board *board, const Coord point, const uint scale);
bool checkPlaceBombs(Board *board, const Coord pos, const Coord downPos);
bool checkRestart(Board *board);
bool checkClick(Board *board, const Coord pos, const Coord downPos);
bool checkRight(Board *board, const Coord pos, const Coord downPos);
bool checkCtrlKey(const Scancode key);
uint boardUpdate(Board *board);
bool checkBombNum(Board *board);
bool checkSave(Board *board);
uint boardUpdate(Board *board);

// PRINT_H
void printBoard(const Board board, const bool decals);
void printDecalsPos(const Board board, const Coord pos);
uint printCleared(const uint cleared);
void printTilePacked(const char c);
void printTile(const Tile tile);

// DRAW_H
uint tileScale(const Length win, const Length len);
Offset tileOffset(const Length win, const Length len, const uint scale);
void drawBoardBlank(const Length len, const uint scale, const Offset off);
void drawBoard(const Board board, const uint scale);

#endif /* end of include guard: DECLS_H */
