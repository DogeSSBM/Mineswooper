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

Coord checkNewGame(Board *board, const Coord point, const uint scale)
{
    const Rect rect = rectify(point, iC((strlen("New Game"))*scale, scale));
    if(
        mouseBtnReleased(MOUSE_L) &&
        coordInRect(mouse.pos, rectOffset(rect, coordDiv(iC(rect.w, rect.h), -2)))
    ){
        board->state = BS_NEW;
        if(!board->tile)
            *board = boardAlloc(*board);

        printf("Starting new game!\n");
    }
    return coordShift(point, DIR_D, scale+scale/2);
}

Coord checkSolver(Board *board, const Coord point, const uint scale)
{
    char buf[256] = {0};
    sprintf(buf, "Solver: %s", BoardTypeStr[board->type]);
    const Rect rect = rectify(point, iC((strlen(buf))*scale, scale));
    if(
        mouseBtnReleased(MOUSE_L) &&
        coordInRect(mouse.pos, rectOffset(rect, coordDiv(iC(rect.w, rect.h), -2)))
    ){
        board->type = (board->type+1) % B_N;
    }
    return coordShift(point, DIR_D, scale+scale/2);
}

Coord checkCheats(Board *board, const Coord point, const uint scale)
{
    char buf[256] = {0};
    sprintf(buf, "Cheats: %s", board->cheat ? "Enabled" : "Disabled");
    const Rect rect = rectify(point, iC((strlen(buf))*scale, scale));
    if(
        mouseBtnReleased(MOUSE_L) &&
        coordInRect(mouse.pos, rectOffset(rect, coordDiv(iC(rect.w, rect.h), -2)))
    ){
        board->cheat = !board->cheat;
    }
    return coordShift(point, DIR_D, scale+scale/2);
}

bool checkPlaceBombs(Board *board, const Coord pos, const Coord downPos)
{
    if(
        mouseBtnReleased(MOUSE_L) &&
        validTilePos(pos, board->len) &&
        validTilePos(downPos, board->len) &&
        coordSame(downPos, pos)
    ){
        boardPlaceBombs(board, pos);
        return true;
    }
    return false;
}

bool checkClick(Board *board, const Coord pos, const Coord downPos)
{
    if(
        validTilePos(pos, board->len) &&
        validTilePos(downPos, board->len) &&
        coordSame(downPos, pos)
    ){
        if(mouseBtnReleased(MOUSE_L) && !mouseBtnState(MOUSE_R)){
            printf(
                "MOUSE_L -\n\tpos: (%i,%i)\n\tmpos: (%i,%i)\n",
                pos.x, pos.y, mouse.pos.x, mouse.pos.y
            );
            if(board->tile[pos.x][pos.y].isBomb){
                board->state = BS_MENU;
                return 0;
            }
            if(board->tile[pos.x][pos.y].state == S_NUM)
                board->lastClickTick = getTicks();
            board->lastClick = pos;
            return floodFill(board, pos);
        }
        if(
            (
                (mouseBtnPressed(MOUSE_L) && getTicks()-board->lastClickTick < 400) ||
                (mouseBtnState(MOUSE_L) && mouseBtnPressed(MOUSE_R))
            ) &&
            board->tile[pos.x][pos.y].state == S_NUM &&
            board->tile[pos.x][pos.y].num == adjTileState(*board, pos, S_FLAG)
        ){
            printf(
                "Chord -\n\tpos: (%i,%i)\n\tmpos: (%i,%i)\n",
                pos.x, pos.y, mouse.pos.x, mouse.pos.y
            );
            board->lastClick = pos;

            return floodCoord(board, pos);
        }
    }
    return 0;
}

bool checkCtrlKey(const Scancode key)
{
    return (
        keyReleased(key) && (keyState(SDL_SCANCODE_LCTRL) || keyState(SDL_SCANCODE_RCTRL))
    ) || (
        keyState(key)  && (keyReleased(SDL_SCANCODE_LCTRL) || keyReleased(SDL_SCANCODE_RCTRL))
    );
}

bool checkRight(Board *board, const Coord pos, const Coord downPos)
{
    if(mouseBtnReleased(MOUSE_R) &&
    validTilePos(pos, board->len) &&
    validTilePos(downPos, board->len) &&
    coordSame(downPos, pos) &&
    !mouseBtnState(MOUSE_L)){
        if(board->tile[pos.x][pos.y].state != S_NUM){
            if(++board->tile[pos.x][pos.y].state >= S_QEST_SAFE)
                board->tile[pos.x][pos.y].state = S_TILE;
            if(board->tile[pos.x][pos.y].state == S_FLAG)
                printf("Flagged: %u / %u\n", boardNumState(*board, S_FLAG), board->numBombs);
        }
        return true;
    }
    return false;
}

bool checkBombNum(Board *board)
{
    if(mouseScrolled(MW_U)){
        board->numBombs = imax(1, board->numBombs-1);
        return true;
    }
    if(mouseScrolled(MW_D)){
        board->numBombs++;
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
        case BS_MENU:
            if(checkCtrlKey(SDL_SCANCODE_O)){
                board->state = BS_OPEN;
                break;
            }
            if(checkCtrlKey(SDL_SCANCODE_S)){
                boardSave(board);
                printf("Saved board\n");
            }
            if(checkLen(board) || windowResized()){
                win = getWindowLen();
                mid = coordDiv(win, 2);
                scale = tileScale(win, board->len);
                off = tileOffset(win, board->len, scale);
            }

            if(checkBombNum(board))
                printf("Setting numBombs: %u\n", board->numBombs);
            checkCheats(board, checkSolver(board, checkNewGame(board, iC(mid.x, mid.y/2), scale), scale), scale);

            break;
        case BS_OPEN:;
            const int prvLvl = board->lvl;
            board->lvl = imax(0, board->lvl-keyPressed(SDL_SCANCODE_MINUS)) + keyPressed(SDL_SCANCODE_EQUALS);
            if(prvLvl != board->lvl)
                printf("lvl: %i\n", board->lvl);

            if(keyPressed(SDL_SCANCODE_RETURN) && boardLoad(board)){
                printf("Loaded\n");
                board->state = BS_PLAY;
                break;
            }

            break;
        case BS_NEW:
            if(checkCtrlKey(SDL_SCANCODE_O)){
                board->state = BS_OPEN;
                break;
            }
            if(checkPlaceBombs(board, pos, down[0]))
                printf(
                    "Placed %u bombs\nIn %i,%i tiles\n",
                    board->numBombs, board->len.x, board->len.y
                );
            break;
        case BS_PLAY:
            if(checkCtrlKey(SDL_SCANCODE_O)){
                board->state = BS_OPEN;
                break;
            }
            if(keyPressed(SDL_SCANCODE_L)){
                board->state = BS_MENU;
                break;
            }
            if(keyPressed(SDL_SCANCODE_T)){
                *board = boardReset(*board);
                printf("Reset tiles.\n");
            }
            if(checkCtrlKey(SDL_SCANCODE_R)){
                board = boardRestart(board);
                printf("Restarting.\n");
            }

            uint left = 0;
            if(printCleared(checkClick(board, pos, down[0]))){
                if((left = boardRemaining(*board))){
                    printf("Tiles remaining: %u\n", left);
                }else{
                    board->state = BS_WIN;
                    break;
                }
            }
            if(board->state == BS_MENU)
                printf("Exiting to main menu\n");
            if(board->state != BS_PLAY)
                break;

            if(checkRight(board, pos, down[1]))
                printf(
                    "MOUSE_R -\n\tpos: (%i,%i)\n\tmpos: (%i,%i)\n",
                    pos.x, pos.y, mouse.pos.x, mouse.pos.y
                );
            break;
        default:
            break;
    }
    return scale;
}

uint boardUpdateMenu(Board *board)
{
    (void)board;
    return 0;
}

bool boardUpdateWindow(const Length len, Length *win, Coord *mid, Coord *off, uint *scale)
{
    if(windowResized() || (scale == 0 && coordMax(len) > 0)){
        *win = getWindowLen();
        *mid = coordDiv(*win, 2);
        *scale = tileScale(*win, len);
        *off = tileOffset(*win, len, *scale);
        return true;
    }
    return false;
}

uint boardUpdateNew(Board *board)
{
    checkQuit(board);

    static Length win = {.x=-1,.y=-1};
    static Coord mid = {.x=-1,.y=-1};
    static Coord off = {.x=-1,.y=-1};
    static uint scale = 0;
    boardUpdateWindow(board->len, &win, &mid, &off, &scale);

    switch(board->state){
        case BS_NEW:

            break;
        case BS_WIN:

            break;
        case BS_MENU:

            checkCheats(board, checkSolver(board, checkNewGame(board, iC(mid.x, mid.y/2), scale), scale), scale);
            break;
        case BS_OPEN:

            break;
        case BS_PLAY:

            break;
        default:

            break;
    }
    return scale;
}

#endif /* end of include guard: UPDATE_H */
