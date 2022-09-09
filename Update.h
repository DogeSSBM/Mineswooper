#ifndef UPDATE_H
#define UPDATE_H

Coord mouseTilePos(const uint scale, const Offset boardOff)
{
    return coordDiv(coordSub(mouse.pos, boardOff), scale);
}

Coord tileMousePos(const uint scale, const Offset boardOff, const Coord tpos)
{
    return coordOffset(coordMul(tpos, scale), boardOff);
}

MetaInf updateInf(MetaInf inf)
{
    for(MetaInfType i = 0; i < M_N; i++){
        if(inf.updated[i])
            continue;
        switch(i){
            case M_LEN:
                inf.prv.len = inf.len;
                break;
            case M_WIN:
                inf.prv.win = inf.win;
                break;
            case M_MID:
                inf.prv.mid = inf.mid;
                break;
            case M_SCALE:
                inf.prv.scale = inf.scale;
                break;
            case M_OFF:
                inf.prv.off = inf.off;
                break;
            default:
                panic("Unknown MetaInfType int: %i", i);
                break;
        }
    }
    return inf;
}

MetaInf infBoard(const Board board)
{
    const Length win = getWindowLen();
    return updateInf(
        (const MetaInf){
            .len = board.len,
            .win = win,
            .mid = coordDiv(win,2),
            .scale = tileScale(win, board.len),
            .off = tileOffset(win, board.len, tileScale(win, board.len))
        }
    );
}

MetaInf boardUpdate(Board *board, MetaInf inf)
{
    if(keyPressed(SDL_SCANCODE_ESCAPE)){
        printf("Escape pressed, freeing board and exiting\n");
        *board = boardFree(*board);
        exit(0);
    }

    static Coord down[2] = {0};
    memset(inf.updated, false, sizeof(bool)*M_N);

    if(board->state == BS_LOOSE){
        if(keyReleased(SDL_SCANCODE_SPACE)){
            board->cheat = !board->cheat;
            printf("board->cheat = %s\n", board->cheat?"true":"false");
        }

        inf.prv.len = inf.len;
        inf.len = iC(
            imax(4, board->len.x+keyPressed(SDL_SCANCODE_RIGHT)-keyPressed(SDL_SCANCODE_LEFT)),
            imax(4, board->len.y+keyPressed(SDL_SCANCODE_DOWN)-keyPressed(SDL_SCANCODE_UP))
        );
        if((inf.updated[M_LEN] = !coordSame(inf.prv.len, inf.len)))
            board->len = inf.len;
    }

    if(windowResized()){
        inf.prv.win = inf.win;
        inf.prv.mid = inf.mid;
        inf.updated[M_WIN] = true;
        inf.updated[M_MID] = true;
        inf.win = getWindowLen();
        inf.mid = coordDiv(inf.win, 2);
    }

    if(inf.updated[M_WIN] || inf.updated[M_LEN]){
        inf.prv.scale = inf.scale;
        inf.prv.off = inf.off;
        inf.updated[M_SCALE] = true;
        inf.updated[M_OFF] = true;
        inf.scale = tileScale(inf.win, inf.len);
        inf.off = tileOffset(inf.win, inf.len, inf.scale);
    }

    const Coord pos = mouseTilePos(inf.scale, inf.off);
    if(mouseBtnPressed(MOUSE_L))
        down[0] = pos;
    if(mouseBtnPressed(MOUSE_R))
        down[1] = pos;

    switch(board->state){
        case BS_LOOSE:
            ;
            const Length txtlen = iC(strlen(" New Game ")*inf.scale, inf.scale);
            const Rect rect = rectify(coordSub(inf.mid, coordDiv(txtlen,2)), txtlen);
            if(
                mouseBtnReleased(MOUSE_L) &&
                coordInRect(mouse.pos, rect)
            ){
                *board = boardAlloc(*board);
                board->state = BS_NEW;
            }
            break;
        case BS_NEW:
            if(
                mouseBtnReleased(MOUSE_L) &&
                validTilePos(pos, board->len) &&
                validTilePos(down[0], board->len) &&
                coordSame(down[0], pos)
            )
                boardPlaceBombs(board, pos);
            break;
        case BS_PLAY:
            if(
                mouseBtnReleased(MOUSE_L) &&
                validTilePos(pos, board->len) &&
                validTilePos(down[0], board->len) &&
                coordSame(down[0], pos) &&
                !mouseBtnState(MOUSE_R)
            ){
                printf(
                    "MOUSE_L -\n\tpos: (%i,%i)\n\tmpos: (%i,%i)\n",
                    pos.x, pos.y, mouse.pos.x, mouse.pos.y
                );
                board->lastClick = pos;
                if(board->tile[pos.x][pos.y].isBomb){
                    board->state = BS_LOOSE;
                    *board = boardFree(*board);
                }else{
                    *board =  prop(*board, pos);
                }
            }

            if(
                mouseBtnReleased(MOUSE_R) &&
                validTilePos(pos, board->len) &&
                validTilePos(down[1], board->len) &&
                coordSame(down[1], pos) &&
                !mouseBtnState(MOUSE_L)
            ){
                printf(
                    "MOUSE_R -\n\tpos: (%i,%i)\n\tmpos: (%i,%i)\n",
                    pos.x, pos.y, mouse.pos.x, mouse.pos.y
                );
                if(board->tile[pos.x][pos.y].state != S_NUM){
                    if(++board->tile[pos.x][pos.y].state == S_N)
                        board->tile[pos.x][pos.y].state = S_TILE;
                }
            }
            break;
        default:
            break;
    }
    return updateInf(inf);
}

#endif /* end of include guard: UPDATE_H */
