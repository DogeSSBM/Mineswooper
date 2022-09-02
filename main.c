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
            case C_NONE:
                break;
            default:
                panic("Unknown click thingy!??\n");
                break;
        }
        board = drawBoard(board);

        s += mouseScrolled(MW_D);
        s -= mouseScrolled(MW_U);
        s = s<0?S_N-1:s%S_N;

        if(validTilePos(tpos, board.len)){
            char buf[32] = {0};
            sprintf(buf, "%s:%u", TileStateStr[s], adjTileState(board, tpos, s));
            setTextSize(board.scale);
            setColor(CYAN);
            const Coord mpos = tileMousePos(board, tpos);
            fillBorderCoordSquare(mpos, board.scale, 2);
            setTextColor(MAGENTA);
            drawTextCoord(buf, mpos);
        }

        frameEnd(t);
    }
    return 0;
}
