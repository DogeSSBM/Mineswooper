#include "DogeLib/Includes.h"
#include "Mineswooper.h"

int main(int argc, char **argv)
{
    init();
    Board board = boardArgs(argc, argv);
    while(1){
        const uint t = frameStart();

        board = boardUpdate(board);
        drawBoard(board);

        frameEnd(t);
    }
    return 0;
}
