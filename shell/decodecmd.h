#ifndef DDRAIG_OS_DECODECMD_H
#define DDRAIG_OS_DECODECMD_H

typedef struct
{
    const char *name;
    const int min_args;
    const int max_args;
    void (* function)(int argc, char *argv[]);
    const char *helpme;
} cmd_entry_t;


int getline(char *line, int linesize);
void execute_cmd(char *linebuffer);

#endif
