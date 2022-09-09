#ifndef ARGS_H
#define ARGS_H

void usage(void)
{
    printf("Usage -\n");
    printf("\t%s [Width,Height] [Num Bombs] [RNG|ADJ|SAT] [cheat]\n\n", PROG_NAME);
    printf("Default options are: 30,16 99 RNG\n");
    exit(EXIT_FAILURE);
}

uint parseUint(char *str, const uint len)
{
    char *end;
    const uint ret = strtoul(str, &end, 0);
    if(end-str > len)
        usage();
    return ret;
}

ArgType parseArgType(char *arg)
{
    if(!arg || strlen(arg) == 0)
        usage();
    if(isdigit(*arg)){
        if(strchr(arg, ','))
            return A_LEN;
        return A_MIN;
    }
    return strlen(arg) == strlen("cheat") && strcmp(arg, "cheat") == 0 ? A_CHEAT : A_TYP;
}

Length parseLen(char *arg)
{
    char *comma = strchr(arg, ',');
    return iC(
        parseUint(arg, comma-arg),
        parseUint(comma+1, strlen(comma+1))
    );
}

uint parseBombs(char *arg)
{
    return parseUint(arg, strlen(arg));
}

BoardType parseType(char *arg)
{
    if(!arg || strlen(arg) != 3)
        usage();
    if(strcmp(arg, "RNG") == 0)
        return B_RNG;
    if(strcmp(arg, "ADJ") == 0)
        return B_ADJ;
    if(strcmp(arg, "SAT") == 0)
        return B_SAT;
    usage();
    return B_RNG;
}

Board boardArgs(int argc, char **argv)
{
    Board board = {
        .lastClick = iC(-1,-1),
        .len = iC(30, 16),
        .numBombs = 99,
        .state = BS_LOOSE
    };

    bool lenDone = false;
    bool bombsDone = false;
    bool typeDone = false;
    bool cheatDone = false;
    for(int i = 1; i < argc; i++){
        switch(parseArgType(argv[i])){
            case A_LEN:
                if(lenDone)
                    usage();
                board.len = parseLen(argv[i]);
                lenDone = true;
                break;
            case A_MIN:
                if(bombsDone)
                    usage();
                board.numBombs = parseBombs(argv[i]);
                bombsDone = true;
                break;
            case A_TYP:
                if(typeDone)
                    usage();
                board.type = parseType(argv[i]);
                typeDone = true;
                break;
            case A_CHEAT:
                if(cheatDone)
                    usage();
                board.cheat = true;
                cheatDone = true;
                break;
            default:
                usage();
                break;
        }
    }

    return board;
}

#endif /* end of include guard: ARGS_H */
