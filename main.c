#include "DogeLib/Includes.h"
#include "Mineswooper.h"

int main(int argc, char **argv)
{
    init();
    Length win = maximizeWindow();
    Board board = boardArgs(argc, argv);

    while(1){
        const uint t = frameStart();
        if(keyPressed(SDL_SCANCODE_ESCAPE))
            exit(0);

        bool refit = false;
        const Length newLen = iC(
            imax(1, board.len.x+keyPressed(SDL_SCANCODE_RIGHT)-keyPressed(SDL_SCANCODE_LEFT)),
            imax(1, board.len.y+keyPressed(SDL_SCANCODE_DOWN)-keyPressed(SDL_SCANCODE_UP))
        );
        if((refit = !coordSame(newLen, board.len))){
            board = boardFree(board);
            board.len = newLen;
            board = boardAlloc(board);
            printf("Len: %i,%i\n", board.len.x, board.len.y);
        }

        const Length newWin = getWindowLen();
        if((refit |= !coordSame(newWin, win))){
            win = newWin;
            printf("Window: %i,%i\n", win.x, win.y);
        }
        if(refit){
            board = boardFit(board);
            printf("Scale: %u\nOffset: %i,%i\n", board.scale, board.off.x, board.off.y);
        }

        drawBoard(board);

        const Coord tpos = mouseTilePos(board);
        setColor(CYAN);
        drawSquareCoord(tileMousePos(board, tpos), board.scale);
        setTextSize(board.scale);
        setTextColor(MAGENTA);
        char buf[32] = {0};
        sprintf(buf, "mouse: (%i, %i)", mouse.pos.x, mouse.pos.y);
        drawTextCoord(buf, mouse.pos);
        sprintf(buf, "tpos: (%i, %i)", tpos.x, tpos.y);
        drawTextCoord(buf, coordShift(mouse.pos, DIR_D, board.scale));

        frameEnd(t);
    }
    return 0;
}
