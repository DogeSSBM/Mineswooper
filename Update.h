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
        case BS_WIN:
        case BS_LOOSE:
            ;
            const Rect rect = rectify(
                inf.mid,
                iC(strlen(" New Game ")*inf.scale, inf.scale)
            );
            if(
                mouseBtnReleased(MOUSE_L) &&
                coordInRect(mouse.pos, rectOffset(rect, iC(-rect.w/2, -rect.h/2)))
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
            if(keyReleased(SDL_SCANCODE_R)){
                if(keyState(SDL_SCANCODE_LCTRL) || keyState(SDL_SCANCODE_RCTRL))
                    *board = boardReset(*board);
                else
                    board = boardRestart(board);
                break;
            }

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
                    break;
                }
                const uint cleared = prop(board, pos);
                if(cleared){
                    printf("cleared %u tiles\n", cleared);
                    const uint left = boardRemaining(*board);
                    printf("Tiles remaining: %u\n", left);
                    if(left == 0){
                        board->state = BS_WIN;
                        break;
                    }
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
                    if(++board->tile[pos.x][pos.y].state >= S_QEST_SAFE)
                        board->tile[pos.x][pos.y].state = S_TILE;
                    if(board->tile[pos.x][pos.y].state == S_FLAG)
                        printf("Flagged: %u / %u\n", boardNumState(*board, S_FLAG), board->numBombs);
                }
            }
            break;
        default:
            break;
    }
    return updateInf(inf);
}

#endif /* end of include guard: UPDATE_H */
