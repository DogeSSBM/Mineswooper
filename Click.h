#ifndef CLICK_H
#define CLICK_H

Coord mouseTilePos(const uint scale, const Offset boardOff)
{
    return coordDiv(coordSub(mouse.pos, boardOff), scale);
}

Coord tileMousePos(const uint scale, const Offset boardOff, const Coord tpos)
{
    return coordOffset(coordMul(tpos, scale), boardOff);
}

Board boardUpdate(Board board)
{
    static Coord down[2] = {0};

    const Length win = getWindowLen();
    const Coord mid = getWindowMid();
    const uint scale = tileScale(win, board.len);
    const Offset off = tileOffset(win, board.len, scale);
    const Coord pos = mouseTilePos(scale, off)

    if(mouseBtnPressed(MOUSE_L))
        down[0] = pos;
    if(mouseBtnPressed(MOUSE_R))
        down[1] = pos;

    if(board.state == BS_LOOSE){
        const Length txtlen = (strlen(" New Game ")*scale, scale);
        const Rect rect = rectify(coordSub(mid, coordDiv(txtlen,2)), txtlen);
        if(
            mouseBtnReleased(MOUSE_L) &&
            coordInRect(mouse.pos, rect)
        ){
            const Length len = board.len;
            boardFree(board);
            board.len = len;
            board = boardAlloc(board);
            board.state = BS_NEW;
            return board;
        }
        board.len = iC(
            imax(4, board.len.x+keyPressed(SDL_SCANCODE_RIGHT)-keyPressed(SDL_SCANCODE_LEFT)),
            imax(4, board.len.y+keyPressed(SDL_SCANCODE_DOWN)-keyPressed(SDL_SCANCODE_UP))
        );;
    }

    if(board.state == BS_NEW){
        if(
            mouseBtnReleased(MOUSE_L) &&
            validTilePos(pos, board.len) &&
            validTilePos(down[0], board.len) &&
            coordSame(down[0], pos)
        )
            return boardPlaceBombs(board, firstClick);
    }

    if(board.state = BS_PLAY){
        if(
            mouseBtnReleased(MOUSE_L) &&
            validTilePos(pos, board.len) &&
            validTilePos(down[0], board.len) &&
            coordSame(down[0], pos) &&
            !mouseBtnState(MOUSE_R)
        ){
            printf("MOUSE_L -\n\tpos: (%i,%i)\n\tmpos: (%i,%i)\n", pos.x, pos.y, mouse.pos.x, mouse.pos.y);
            board.lastClick = pos;
            if(board.tile[pos.x][pos.y].isBomb){
                board.state = BS_LOOSE;
                return board;
            }else{
                return prop(board, pos);
            }
        }

        if(
            mouseBtnReleased(MOUSE_R) &&
            validTilePos(pos, board.len) &&
            validTilePos(down[0], board.len) &&
            coordSame(down[0], pos) &&
            !mouseBtnState(MOUSE_L)
        ){
            printf("MOUSE_R -\n\tpos: (%i,%i)\n\tmpos: (%i,%i)\n", pos.x, pos.y, mouse.pos.x, mouse.pos.y);
            if(board.tile[pos.x][pos.y].state != S_NUM){
                if(++board.tile[pos.x][pos.y].state == S_N)
                    board.tile[pos.x][pos.y].state = S_TILE;
                return board;
            }
        }
    }
    return board;
}

Board boardResize(Board board)
{


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
