#ifndef DECLS_H
#define DECLS_H

// Args.h
void usage(void);
uint parseUint(char *, const uint);
ArgType parseArgType(char *);
Length parseLen(char *);
uint parseBombs(char *);

// Board.h
uint lin(const Coord, const Length);
Coord delin(const uint, const Length);
uint tileScale(const Length, const Length);
Offset tileOffset(const Length, const Length, const uint);
Board boardFit(Board);
Board boardFree(Board);
Board boardAlloc(Board);
void drawBoard(const Board);
bool validTilePos(const Coord, const Coord);
bool inArr(const uint, uint *, const uint);
Board boardRng(Board, Coord);
Board boardInit(Board, const Coord);
Board boardArgs(int, char **);

// Solve.h
uint adjDecal(Board *, const Coord, const TileState);
uint flagAdj(Board *, const Coord);
uint clearAdj(Board *, const Coord);
bool solvableAdj(const Board);

#endif /* end of include guard: DECLS_H */
