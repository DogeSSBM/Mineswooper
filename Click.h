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
    if(
        validTilePos(tpos, board.len) &&
        mouseBtnReleased(MOUSE_L) &&
        coordSame(down[0], tpos) &&
        !mouseBtnState(MOUSE_R)
    ){
        printf("MOUSE_L -\n\ttpos: (%i,%i)\n\tmpos: (%i,%i)\n", tpos.x, tpos.y, mouse.pos.x, mouse.pos.y);
        return C_CLEAR;
    }
    if(
        validTilePos(tpos, board.len) &&
        mouseBtnReleased(MOUSE_R) &&
        coordSame(down[1], tpos) &&
        !mouseBtnState(MOUSE_L)
    ){
        printf("MOUSE_R -\n\ttpos: (%i,%i)\n\tmpos: (%i,%i)\n", tpos.x, tpos.y, mouse.pos.x, mouse.pos.y);
        return C_DECAL;
    }
    return C_NONE;
}

#endif /* end of include guard: CLICK_H */
