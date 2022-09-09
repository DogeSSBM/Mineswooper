#include "DogeLib/Includes.h"
#include "Mineswooper.h"

int main(int argc, char **argv)
{
    init();
    Board board = boardArgs(argc, argv);
    TileState s = S_NUM;
    while(1){
        const uint t = frameStart();
        if(keyPressed(SDL_SCANCODE_ESCAPE))
            exit(0);

        board = boardUpdate(board);
        boardDraw(board, true);

        frameEnd(t);
    }
    return 0;
}
