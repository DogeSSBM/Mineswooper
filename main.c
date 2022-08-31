#include "DogeLib/Includes.h"
#include "Mineswooper.h"

Board restart(Board board, const Coord tpos)
{
    while(1){
        const uint t = frameStart();
        if(keyPressed(SDL_SCANCODE_ESCAPE))
            exit(0);

        const Length newLen = iC(
            imax(1, board.len.x+keyPressed(SDL_SCANCODE_RIGHT)-keyPressed(SDL_SCANCODE_LEFT)),
            imax(1, board.len.y+keyPressed(SDL_SCANCODE_DOWN)-keyPressed(SDL_SCANCODE_UP))
        );
        if(windowResized())
            board = boardFit(board);
        if(!coordSame(newLen, board.len)){
            board = boardFree(board);
            board.len = newLen;
            board = boardAlloc(board);
            board = boardFit(board);
        }
        drawBoard(board);
        if(validTilePos(tpos, board.len))
            drawBoom(board, tpos);

        setColor(GREY);
        setTextColor(WHITE);
        setTextSize(board.scale);
        const Rect rect = rectify(
            coordOffset(getWindowMid(), coordDiv(getTextLength("New Game"), -2)),
            getTextLength("New Game")
        );
        fillRectRect(rect);
        drawTextCenteredCoord("New Game", getWindowMid());

        if(mouseBtnReleased(MOUSE_L) && coordInRect(mouse.pos, rect))
            return boardAlloc(board);

        frameEnd(t);
    }
    return board;
}

int main(int argc, char **argv)
{
    init();
    // maximizeWindow();
    Board board = boardArgs(argc, argv);
    board = restart(board, iC(-1,-1));
    while(1){
        const uint t = frameStart();
        if(keyPressed(SDL_SCANCODE_ESCAPE))
            exit(0);

        if(windowResized())
            board = boardFit(board);

        const Coord tpos = mouseTilePos(board);
        switch(clickInput(board))
        {
            case C_LOOSE:
                board = restart(board, tpos);
                break;
            case C_CLEAR:
                board = prop(board, tpos);
                break;
            case C_DECAL:
                if(++board.tile[tpos.x][tpos.y].state == S_N)
                    board.tile[tpos.x][tpos.y].state = S_TILE;
                break;
            default:
                break;
        }

        drawBoard(board);
        frameEnd(t);
    }
    return 0;
}
