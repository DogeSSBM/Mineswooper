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

    if(
        mouseBtnReleased(MOUSE_L) &&
        coordSame(down[0], mouseTilePos(board)) &&
        !mouseBtnState(MOUSE_R)
    )
        return C_CLEAR;

    if(
        mouseBtnReleased(MOUSE_R) &&
        coordSame(down[1], mouseTilePos(board)) &&
        !mouseBtnState(MOUSE_L)
    )
        return C_FLAG;

    return C_NONE;
}

#endif /* end of include guard: CLICK_H */
