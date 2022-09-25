#ifndef SAVE_H
#define SAVE_H

Length boardFileLength(File *file)
{
    if(file == NULL)
        panic("Cant get length of NULL file");
    Length len = {0};
    int c;
    while((c = fgetc(file)) != EOF && c != '\n')
        len.x++;
    printf("x: %i\n", len.x);
    rewind(file);

    while((c = fgetc(file)) != EOF){
        len.y += c == '\n';
    }

    rewind(file);
    printf("y: %i\n", len.y);

    for(int y = 0; y < len.y; y++){
        for(int x = 0; x < len.x; x++){
            if((c = fgetc(file))=='\n'){
                fclose(file);
                panic("unexpected '\\n'. char pos: %i,%i", x, len.y);
            }else if(c == EOF){
                fclose(file);
                panic("unexpected EOF. char pos: %i,%i", x, len.y);
            }
        }
        if((c = fgetc(file)) != '\n'){
            fclose(file);
            panic("expected '\\n' at char pos: %i,%i", len.x, y);
        }
    }
    if((c = fgetc(file)) != EOF){
        fclose(file);
        panic("Expected EOF at pos: %i,%i", len.x, len.y);
    }

    rewind(file);
    printf("Board file len: %i,%i\n", len.x, len.y);
    return len;
}

Tile tileUnpack(char c)
{
    Tile tile = {0};
    tile.isBomb = 1 & (c >> 7);
    tile.num = 15 & (c >> 3);
    tile.state = 7 & c;
    return tile;
}

char tilePack(const Tile tile)
{
    return tile.state | (tile.num << 3) | (tile.isBomb << 7);
}

bool boardSave(Board *board)
{

    for(uint i = 0; i < ~0u; i++){
        char path[64] = {0};
        sprintf(path, "./Saves/%u.swoop", i);
        File *file = fopen(path, "r");
        if(file){
            fclose(file);
        }else{
            printf("Saving board in file \"%s\"\n", path);
            file = fopen(path, "w");
            for(int y = 0; y < board->len.y; y++){
                for(int x = 0; x < board->len.x; x++)
                    fputc(tilePack(board->tile[x][y]), file);
                if(fputc('\n', file) == EOF){
                    fclose(file);
                    return false;
                }
            }
            fclose(file);
            return true;
        }
    }
    return false;
}

bool boardLoad(Board *board)
{
    char buf[64] = {0};
    sprintf(buf, "./Saves/%i.swoop", board->lvl);
    if((board->file = fopen(buf, "r")) == NULL){
        printf("Board at \"%s\" does not exitst\n", buf);
        return false;
    }
    board->len = boardFileLength(board->file);
    printf("len.x: %i\nlen.y: %i\n", board->len.x, board->len.y);
    if(board->tile)
        *board = boardFree(*board);

    *board = boardAlloc(*board);
    int c;
    for(int y = 0; y < board->len.y; y++){
        for(int x = 0; x < board->len.x; x++){
            if((c = fgetc(board->file))=='\n'){
                fclose(board->file);
                panic("unexpected '\\n'. char pos: %i,%i", x, y);
                printf("Board file len: %i,%i\n", board->len.x, board->len.y);
            }else if(c == EOF){
                fclose(board->file);
                panic("unexpected EOF. char pos: %i,%i", x, y);
                printf("Board file len: %i,%i\n", board->len.x, board->len.y);
            }
            board->tile[x][y] = tileUnpack(c);
        }
        if((c = fgetc(board->file)) != '\n'){
            fclose(board->file);
            panic("Expected newline at end of line %i\n", y);
        }
    }
    if((c = fgetc(board->file)) != EOF){
        fclose(board->file);
        panic("Expected EOF at end of line %i\n", board->len.y);
    }

    return true;
}

#endif /* end of include guard: SAVE_H */
