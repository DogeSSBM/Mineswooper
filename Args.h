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

bool sameStrLower(char *a, char *b)
{
    const uint len = strlen(a);
    if(strlen(b) != len)
        return false;
    for(uint i = 0; i < len; i++)
        if(tolower(a[i]) != tolower(b[i]))
            return false;

    return true;
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
    if(sameStrLower("cheat", arg))
        return A_CHEAT;
    else if(strlen(arg) == 3)
        return A_TYP;
    panic("Unknown arg type! arg: %s", arg);
    return A_N;
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
    if(sameStrLower(arg, "rng"))
        return B_RNG;
    if(sameStrLower(arg, "adj"))
        return B_ADJ;
    if(sameStrLower(arg, "sat"))
        return B_SAT;
    panic("Unknown BoardType! arg: %s", arg);
    return B_N;
}

Board boardArgs(int argc, char **argv)
{
    Board board = {
        .lastClick = iC(-1,-1),
        .len = iC(30, 16),
        .numBombs = 99,
        .state = BS_MENU
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
