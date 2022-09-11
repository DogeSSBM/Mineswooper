// #ifndef SOLVE_H
// #define SOLVE_H
//
// uint flagAdj(Board *board, const Coord pos)
// {
//     uint flagged = 0;
//     for(int yo = -1; yo <= 1; yo++){
//         for(int xo = -1; xo <= 1; xo++){
//             const Coord adj = iC(pos.x+xo, pos.y+yo);
//             if(coordSame(pos, adj) || !validTilePos(adj, board->len) || board->tile[adj.x][adj.y].state != S_TILE)
//                 continue;
//             board->tile[adj.x][adj.y].state = S_FLAG;
//             flagged++;
//         }
//     }
//     return flagged;
// }
//
// uint clearAdj(Board *board, const Coord pos)
// {
//     uint cleared = 0;
//     for(int yo = -1; yo <= 1; yo++){
//         for(int xo = -1; xo <= 1; xo++){
//             const Coord adj = iC(pos.x+xo, pos.y+yo);
//             if(coordSame(pos, adj) || !validTilePos(adj, board->len) || board->tile[adj.x][adj.y].state != S_TILE)
//                 continue;
//             if(board->tile[adj.x][adj.y].isBomb)
//                 panic("Cleared adj bomb at: (%i,%i)\n", adj.x, adj.y);
//             board->tile[adj.x][adj.y].state = S_NUM;
//             cleared++;
//         }
//     }
//     return cleared;
// }
//
// /*
// while progress
//     trivial flags
//     trivial clears
//
// moar complicated shit
//
// */
//
// bool solvableAdj(const Board original, const Coord firstClick)
// {
//     Board board = original;
//     board.tile = calloc(board.len.x, sizeof(Tile*));
//     for(int x = 0; x < board.len.x; x++){
//         board.tile[x] = calloc(board.len.y, sizeof(Tile));
//         memcpy(board.tile[x], original.tile[x], sizeof(Tile)*board.len.y);
//     }
//     board = prop(board, firstClick);
//
//     uint tries = 3;
//     do{
//         bool progress = false;
//         for(int y = 0; y < board.len.y; y++){
//             for(int x = 0; x < board.len.x; x++){
//                 const Coord pos = iC(x,y);
//                 if(board.tile[x][y].state == S_NUM){
//                     const uint tiles = adjTileState(board, pos, S_TILE);
//                     if(board.tile[x][y].num && tiles <= board.tile[x][y].num)
//                         progress |= flagAdj(&board, pos);
//                     const uint flags = adjTileState(board, pos, S_FLAG);
//                     if(flags>=board.tile[x][y].num)
//                         progress |= clearAdj(&board, pos);
//                 }
//             }
//         }
//         tries = progress ? 3 : tries-1;
//         printDecals(board);
//         printf("tries: %u\n", tries);
//         // int c;
//         // do{
//         //     c = getchar();
//         //     if(c == 'q')
//         //         exit(0);
//         // }while(c != '\n');
//
//     }while(tries);
//
//     boardFree(board);
//     return !board.tilesLeft;
// }
//
//
//
// bool solvable(Board board)
// {
//     switch(original.type){
//         case B_RNG:
//             return true;
//         case B_ADJ:
//             do{
//                 Board board = original;
//
//             }
//     }
// }
//
// #endif /* end of include guard: SOLVE_H */
