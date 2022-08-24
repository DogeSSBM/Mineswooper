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
        down[0] = coordDiv(mouse.pos, board.scale);
    if(mouseBtnPressed(MOUSE_R))
        down[1] = coordDiv(mouse.pos, board.scale);

    (void)down;
    return C_FLAG;
}

#endif /* end of include guard: CLICK_H */
