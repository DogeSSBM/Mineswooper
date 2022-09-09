#include "DogeLib/Includes.h"
#include "Mineswooper.h"

Board boardUpdate(Board board)
{
    static Coord down[2] = {0};

    const Length win = getWindowLen();
    const Coord mid = getWindowMid();
    const uint scale = tileScale(win, board.len);
    const Offset off = tileOffset(win, board.len, scale);

    if(mouseBtnPressed(MOUSE_L))
        down[0] = mouseTilePos(scale, off);
    if(mouseBtnPressed(MOUSE_R))
        down[1] = mouseTilePos(scale, off);

    switch(board.state){
        case BS_NEW:
            const Coord pos = mouseTilePos(scale, off)

            break;
        case BS_PLAY:

            break;
        case BS_LOOSE:

            break;
        default:
            panic("board.state default int: %i", board.state);
    }
}

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
