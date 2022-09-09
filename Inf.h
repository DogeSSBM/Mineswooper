#ifndef INF_H
#define INF_H

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


#endif /* end of include guard: INF_H */
