#ifndef PRINT_H
#define PRINT_H

void printBoard(const Board board)
{
    if(!board.tile)
        panic("board.tile == NULL\n");

    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            if(board.tile[x][y].isBomb)
                putchar('B');
            else
                putchar(board.tile[x][y].num ? '0' + board.tile[x][y].num : ' ');
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}

void printDecals(const Board board)
{
    if(!board.tile)
        panic("board.tile == NULL\n");

    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            if(board.tile[x][y].state == S_NUM)
                putchar(board.tile[x][y].num ? '0' + board.tile[x][y].num : ' ');
            else
                putchar(TileStateChar[board.tile[x][y].state]);
            putchar(' ');
        }
        putchar('\n');
    }
}

void printDecalsPos(const Board board, const Coord pos)
{
    if(!board.tile)
        panic("board.tile == NULL\n");

    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            if(x == pos.x && y == pos.y)
                putchar('X');
            else if(board.tile[x][y].state == S_NUM)
                putchar(board.tile[x][y].num ? '0'+board.tile[x][y].num : ' ');
            else
                putchar(TileStateChar[board.tile[x][y].state]);
            putchar(' ');
        }
        putchar('\n');
    }
}

uint printCleared(const uint cleared)
{
    if(cleared)
        printf("Cleared %u tiles.\n", cleared);
    return cleared;
}

#endif /* end of include guard: PRINT_H */
