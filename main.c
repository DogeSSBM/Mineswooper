#include "DogeLib/Includes.h"
#include "Mineswooper.h"

int main(int argc, char **argv)
{
    init();
    Board board = boardArgs(argc, argv);
    MetaInf inf = infBoard(board);
    while(1){
        const uint t = frameStart();

        inf = boardUpdate(&board, inf);
        drawBoard(board, inf);

        frameEnd(t);
    }
    return 0;
}
