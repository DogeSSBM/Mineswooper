#ifndef CLICK_H
#define CLICK_H

Coord mouseTilePos(const Board board)
{
    return coordDiv(coordSub(mouse.pos, board.off), board.scale);
}

Coord tileMousePos(const Board board, const Coord tpos)
{
    return coordOffset(coordMul(tpos, board.scale), board.off);
}

ClickType clickInput(const Board board)
{
    static Coord down[2] = {0};

    if(mouseBtnPressed(MOUSE_L))
        down[0] = mouseTilePos(board);
    if(mouseBtnPressed(MOUSE_R))
        down[1] = mouseTilePos(board);

    const Coord tpos = mouseTilePos(board);
    if(board.gameOver){
        setTextSize(board.scale);
        const Length txtlen = getTextLength("New Game");
        const Rect rect = rectify(coordOffset(getWindowMid(), coordDiv(txtlen, -2)), txtlen);
        return mouseBtnReleased(MOUSE_L) &&
            coordInRect(mouse.pos, rect) ?
            C_START : C_NONE;
    }

    if(
        validTilePos(tpos, board.len) &&
        mouseBtnReleased(MOUSE_L) &&
        coordSame(down[0], tpos) &&
        !mouseBtnState(MOUSE_R)
    ){
        printf("MOUSE_L -\n\ttpos: (%i,%i)\n\tmpos: (%i,%i)\n", tpos.x, tpos.y, mouse.pos.x, mouse.pos.y);
        return board.tile[tpos.x][tpos.y].isBomb ? C_LOOSE : C_CLEAR;
    }
    if(
        validTilePos(tpos, board.len) &&
        mouseBtnReleased(MOUSE_R) &&
        coordSame(down[1], tpos) &&
        !mouseBtnState(MOUSE_L) &&
        board.tile[tpos.x][tpos.y].state != S_NUM
    ){
        printf("MOUSE_R -\n\ttpos: (%i,%i)\n\tmpos: (%i,%i)\n", tpos.x, tpos.y, mouse.pos.x, mouse.pos.y);
        return C_DECAL;
    }
    return C_NONE;
}

Board boardResize(Board board)
{
    if(board.gameOver){
        const Length newLen = iC(
            imax(1, board.len.x+keyPressed(SDL_SCANCODE_RIGHT)-keyPressed(SDL_SCANCODE_LEFT)),
            imax(1, board.len.y+keyPressed(SDL_SCANCODE_DOWN)-keyPressed(SDL_SCANCODE_UP))
        );
        if(!coordSame(newLen, board.len)){
            board = boardFree(board);
            board.len = newLen;
            board = boardAlloc(board);
            board = boardFit(board);
        }
    }
    return board;
}

#endif /* end of include guard: CLICK_H */
