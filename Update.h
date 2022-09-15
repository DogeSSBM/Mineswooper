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

void checkQuit(Board *board)
{
    bool quit = false;
    if((quit = keyPressed(SDL_SCANCODE_ESCAPE)))
        printf("Escape pressed, freeing board and exiting\n");
    else if((quit =
        keyPressed(SDL_SCANCODE_Q) &&
        (keyState(SDL_SCANCODE_LCTRL) || keyState(SDL_SCANCODE_RCTRL))
    ))
        printf("Ctrl+Q pressed, freeing board and exiting\n");
    if(quit){
        if(board->tile)
            *board = boardFree(*board);
        exit(EXIT_SUCCESS);
    }
}

bool checkCheat(const bool cheat)
{
    if(keyReleased(SDL_SCANCODE_SPACE)){
        printf("board->cheat = %s\n", !cheat?"true":"false");
        return !cheat;
    }
    return cheat;
}

bool checkLen(Board* board)
{
    const Scancode arowkey[4] = {
        SDL_SCANCODE_UP,
        SDL_SCANCODE_RIGHT,
        SDL_SCANCODE_DOWN,
        SDL_SCANCODE_LEFT
    };
    const Scancode wasdkey[4] = {
        SDL_SCANCODE_W,
        SDL_SCANCODE_D,
        SDL_SCANCODE_S,
        SDL_SCANCODE_A
    };
    Length newLen = board->len;
    for(Direction d = 0; d < 4; d++)
            if(keyPressed(wasdkey[d]) || keyPressed(arowkey[d]))
                newLen = coordMost(iC(4,4), coordShift(newLen, d, 1));
    if(!coordSame(newLen, board->len)){
        if(board->tile)
            *board = boardFree(*board);
        board->len = newLen;
        *board = boardAlloc(*board);
        return true;
    }
    return false;
}

bool checkNewGame(Board *board, const Coord mid, const uint scale)
{
    const Rect rect = rectify(mid, iC(strlen(" New Game ")*scale, scale));
    if(
        mouseBtnReleased(MOUSE_L) &&
        coordInRect(mouse.pos, rectOffset(rect, iC(-rect.w/2, -rect.h/2)))
    ){
        board->state = BS_NEW;
        if(!board->tile)
            *board = boardAlloc(*board);
        return true;
    }
    return false;
}

uint boardUpdate(Board *board)
{
    static Coord down[2] = {0};
    static Length win = {.x=-1,.y=-1};
    static Coord mid = {.x=-1,.y=-1};
    static Coord off = {.x=-1,.y=-1};
    static uint scale = 0;
    checkQuit(board);

    if(scale == 0 || windowResized()){
        win = getWindowLen();
        mid = coordDiv(win, 2);
        scale = tileScale(win, board->len);
        off = tileOffset(win, board->len, scale);
    }

    Coord pos = mouseTilePos(scale, off);
    if(mouseBtnPressed(MOUSE_L))
        down[0] = pos;
    if(mouseBtnPressed(MOUSE_R))
        down[1] = pos;

    switch(board->state){
        case BS_WIN:
        case BS_LOOSE:
            board->cheat = checkCheat(board->cheat);
            if(checkLen(board) || windowResized()){
                win = getWindowLen();
                mid = coordDiv(win, 2);
                scale = tileScale(win, board->len);
                off = tileOffset(win, board->len, scale);
            }
            if(checkNewGame(board, mid, scale))
                printf("Starting new game!\n");
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
                const uint cleared = floodFill(board, pos);
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
    return scale;
}

#endif /* end of include guard: UPDATE_H */
