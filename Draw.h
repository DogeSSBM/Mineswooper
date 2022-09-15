#ifndef DRAW_H
#define DRAW_H

uint tileScale(const Length win, const Length len)
{

    return imin(win.x/len.x, win.y/len.y);
}

Offset tileOffset(const Length win, const Length len, const uint scale)
{
    return coordDiv(coordSub(win, coordMul(len, scale)), 2);
}

void drawBoardBlank(const Length len, const uint scale, const Offset off)
{
    for(int y = 0; y < len.y; y++){
        for(int x = 0; x < len.x; x++){
            const Coord pos = tileMousePos(scale, off, iC(x,y));
            setColor(GREY1);
            fillSquareCoordResize(pos, scale, -1);
            setColor(GREY2);
            fillSquareCoordResize(pos, scale, -1 -(scale/8));
        }
    }
}

void drawBoard(const Board board, const MetaInf inf)
{
    static const Color numColor[] = {
        {0x00, 0x00, 0xAA, 0xFF},
        {0x00, 0x52, 0x00, 0xFF},
        {0xAA, 0x00, 0x00, 0xFF},
        {0x00, 0x00, 0x52, 0xFF},
        {0x52, 0x00, 0x00, 0xFF},
        {0x00, 0x52, 0x52, 0xFF},
        {0x00, 0x00, 0x00, 0xFF},
        {0x52, 0x52, 0x52, 0xFF},
    };

    if(inf.updated[M_SCALE])
        setTextSize(inf.scale);

    if(board.state == BS_NEW){
        drawBoardBlank(board.len, inf.scale, inf.off);
        return;
    }

    if(board.state == BS_LOOSE){
        drawBoardBlank(board.len, inf.scale, inf.off);

        if(validTilePos(board.lastClick, board.len)){
            fillSquareCoordResize(board.lastClick, inf.scale, -2);
            setColor(BLACK);
            fillCircleCoord(coordOffset(board.lastClick, coordDiv(iC(inf.scale, inf.scale),2)), inf.scale/3);
            setColor(GREY);
        }

        setColor(GREY);
        setTextColor(WHITE);
        const Length textlen = iC(strlen(" New Game ")*inf.scale, inf.scale);
        const Rect rect = rectify(
            coordOffset(inf.mid, coordDiv(textlen, -2)),
            textlen
        );
        fillRectRect(rect);
        drawTextCenteredCoord(" New Game ", inf.mid);
        return;
    }

    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            const Coord pos = tileMousePos(inf.scale, inf.off, iC(x,y));

            if(board.tile[x][y].state == S_NUM){
                setColor(GREY3);
                fillSquareCoordResize(pos, inf.scale, -1);
                if(board.tile[x][y].num > 0){
                    setTextColor(numColor[board.tile[x][y].num - 1]);
                    char txt[2] = "0";
                    txt[0] = '0'+board.tile[x][y].num;
                    drawTextCenteredCoord(txt, coordOffset(pos, iC(inf.scale/2, inf.scale/2)));
                }
            }else{
                setColor(GREY1);
                fillSquareCoordResize(pos, inf.scale, -1);
                setColor(GREY2);
                fillSquareCoordResize(pos, inf.scale, -1 -(inf.scale/8));

                if(board.tile[x][y].num > 0 && (board.cheat || board.state == BS_LOOSE)){
                    setTextColor(numColor[board.tile[x][y].num - 1]);
                    char txt[2] = "0";
                    txt[0] = '0'+board.tile[x][y].num;
                    drawTextCenteredCoord(txt, coordOffset(pos, iC(inf.scale/2, inf.scale/2)));
                }else if(board.tile[x][y].isBomb && (board.cheat || board.state == BS_LOOSE)){
                    setTextColor(BLACK);
                    drawTextCenteredCoord("B", coordOffset(pos, iC(inf.scale/2, inf.scale/2)));
                }

                if(board.tile[x][y].state == S_FLAG){
                    const uint t1 = inf.scale/3;
                    const uint t2 = t1*2;
                    const uint n1 = t1/3;
                    const uint n2 = n1*2;

                    setColor(BLACK);
                    const Coord p1 = coordShift(coordShift(pos, DIR_R, t1+n1), DIR_D, n1);
                    const Coord p2 = coordShift(coordShift(pos, DIR_R, t1+n2), DIR_D, t2+n1);
                    fillRectCoords(p1, p2);
                    const Coord b1 = coordShift(coordShift(pos, DIR_R, t1), DIR_D, t2+n1);
                    const Coord b2 = coordShift(coordShift(pos, DIR_R, t2), DIR_D, t2+n2);
                    fillRectCoords(b1, b2);
                    setColor(RED);
                    Coord f[3] = {
                        coordShift(p1, DIR_R, n1),
                        coordShift(coordShift(p1, DIR_R, t1+n1), DIR_D, n2),
                        coordShift(coordShift(p1, DIR_R, n1), DIR_D, t1+n1)
                    };
                    fillPoly(f, 3);
                }else if(board.tile[x][y].state == S_QEST){
                    setTextColor(BLACK);
                    drawTextCenteredCoord(
                        "?",
                        coordOffset(pos, (const Coord){.x=inf.scale/2,.y=inf.scale/2})
                    );
                }
            }
        }
    }

}

#endif /* end of include guard: DRAW_H */
