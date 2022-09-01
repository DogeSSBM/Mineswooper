#include "DogeLib/Includes.h"
#include "Mineswooper.h"

int main(int argc, char **argv)
{
    init();
    Board board = boardArgs(argc, argv);
    while(1){
        const uint t = frameStart();
        if(keyPressed(SDL_SCANCODE_ESCAPE))
            exit(0);

        const Coord tpos = mouseTilePos(board);
        switch(clickInput(board))
        {
            case C_LOOSE:
                board.gameOver = true;
                break;
            case C_START:
                board.gameOver = false;
                board = boardAlloc(board);
                break;
            case C_CLEAR:
                board = prop(board, tpos);
                break;
            case C_DECAL:
                if(++board.tile[tpos.x][tpos.y].state == S_N)
                    board.tile[tpos.x][tpos.y].state = S_TILE;
                break;
            default:
                panic("Unknown click thingy!??\n");
                break;
        }

        board = drawBoard(board);
        frameEnd(t);
    }
    return 0;
}
