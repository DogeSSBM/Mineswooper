#ifndef ARGS_H
#define ARGS_H

void usage(void)
{
    printf("Usage -\n");
    printf("\t%s [Width,Height] [Num Bombs] [RNG|ADJ|SAT]\n\n", PROG_NAME);
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
    return A_TYP;
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

#endif /* end of include guard: ARGS_H */
