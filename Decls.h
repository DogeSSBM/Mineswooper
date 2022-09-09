#ifndef DECLS_H
#define DECLS_H

// Args.h
void usage(void);
uint parseUint(char *, const uint);
ArgType parseArgType(char *);
Length parseLen(char *);
uint parseBombs(char *);

// Board.h
void printBoard(const Board);
void printDecals(const Board);
uint lin(const Coord, const Length);
Coord delin(const uint, const Length);
uint tileScale(const Length, const Length);
Offset tileOffset(const Length, const Length, const uint);
Board boardFit(Board);
Board boardFree(Board);
Board boardAlloc(Board);
void drawBoom(const Board, const Coord);
void drawBoard(const Board, const MetaInf);
bool validTilePos(const Coord, const Coord);
bool inArr(const uint, uint *, const uint);
uint adjTileState(const Board, const Coord, const TileState);
uint adjBombs(const Board, const Coord);
Board boardRng(Board, Coord);
Board boardInit(Board, const Coord);
Board boardArgs(int, char **);
Board prop(Board, const Coord);

// Click.h
Coord mouseTilePos(const uint, const Offset);
Coord tileMousePos(const uint, const Offset, const Coord);
ClickType clickInput(const Board);

// Solve.h
uint adjDecal(Board *, const Coord, const TileState);
uint flagAdj(Board *, const Coord);
uint clearAdj(Board *, const Coord);
bool solvableAdj(const Board, const Coord);

#endif /* end of include guard: DECLS_H */
