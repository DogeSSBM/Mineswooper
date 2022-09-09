#include "DogeLib/Includes.h"
#include "Mineswooper.h"

Board boardUpdate(Board board)
{
    static Coord down[2] = {0};

    const Length win = getWindowLen();
    const Coord mid = getWindowMid();
    const uint scale = tileScale(win, board.len);
    const Offset off = tileOffset(win, board.len, scale);

    const Coord btnOrig = coordSub(mid, iC(scale*4,scale+scale/2));
    Coord btn[2][3];
    for(uint i = 0; i < 3; i++){
        btn[]
    }
    for(uint i = 0)
    if(mouseBtnPressed(MOUSE_L))
        down[0] = mouseTilePos(scale, off);
    if(mouseBtnPressed(MOUSE_R))
        down[1] = mouseTilePos(scale, off);

    switch(board.state){
        case BS_NEW:

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
                if(!board.bombsPlaced)
                    board = boardPlaceBombs(board, tpos);
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
