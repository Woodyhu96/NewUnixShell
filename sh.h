#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <unistd.h>
#include "get_path.h"
#define MAX_ARG 128
#define BUF_SIZE 1024

int pid;
int sh(int argc, char **argv, char **envp);
void printenv(int num_args, char **envp, char **args);
void main_run(int command_index, char** args, char* pathlist, int num_args, char** envp, int supress_output);
char *which(char *command, struct pathelement *pathlist);
char *where(char *command, struct pathelement *pathlist);
void list(char *dir);
void  INThandler(int sig);
