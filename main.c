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

        board = boardUpdate(board);
        drawBoard(board, true);

        frameEnd(t);
    }
    return 0;
}
