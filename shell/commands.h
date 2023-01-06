#ifndef DDRAIG_OS_COMMANDS_H
#define DDRAIG_OS_COMMANDS_H

void do_help(int argc, char *argv[]);
void do_dump(int argc, char *argv[]);
void do_srecord(int argc, char *argv[]);
void do_binfile(int argc, char *argv[]);
void do_binmem(int argc, char *argv[]);
void do_run(int argc, char *argv[]);
void do_writemem(int argc, char *argv[]);

void do_dir(int argc, char *argv[]);
void do_delete(int argc, char *argv[]);
void do_mkdir(int argc, char *argv[]);
void do_rename(int argc, char *argv[]);
void do_cd(int argc, char *argv[]);
void do_loadmem(int argc, char *argv[]);
void do_copyfile(int argc, char *argv[]);
void do_cat(int argc, char *argv[]);

void do_time(int argc, char *argv[]);
void do_uptime(int argc, char *argv[]);

void do_traptest(int argc, char *argv[]);

void do_notimplemented(int argc, char *argv[]);

void do_ehbasic(int argc, char *argv[]);

#endif
