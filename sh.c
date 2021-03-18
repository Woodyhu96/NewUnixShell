#include "sh.h"

struct Node *flag = NULL;
struct passwd *psw;
struct pathelement *path_list;
int uid, i,csource,length,breaker =1;
char *home_dir;
char *command, *arg, *commandpath, *p, *pwd, *owd, *cwd;
char *prefix = NULL;
char BUFFER[BUF_SIZE];

typedef enum inputs {
        EXIT,
        WHICH,
        WHERE,
        CD,
        GET_CWD,
        PWD,
        LIST,
        LS,
        PID,
        KILL,
        PROMPT,
        PRINT_ENV,
        SET_ENV,
        input_ct
    } inputs;

void handle_sigchild(int sig) {
    while (waitpid((pid_t) (-1), 0, WNOHANG) > 0) {}
}

int sh(int argc, char **argv, char **environment) {

    prefix = (char *) malloc(0);
    uid = getuid();
    psw = getpwuid(uid);
    home_dir = psw->pw_dir;
    char *prompt = calloc(32, sizeof(char));
    char **args = calloc(MAX_ARG, sizeof(char *));
    pwd = getcwd(BUFFER, BUF_SIZE + 1);
    cwd = calloc(strlen(pwd) + 1, sizeof(char));
    owd = calloc(strlen(pwd) + 1, sizeof(char));
    memcpy(owd, pwd, strlen(pwd));
    memcpy(cwd, pwd, strlen(pwd));
    path_list = get_path();
    int length;
    char *string_input;

    sigignore(SIGINT);
    sigignore(SIGTERM);
    sigignore(SIGTSTP);
    signal(SIGCHLD, handle_sigchild);
    char *input_command[] = {
            "exit",
            "which",
            "where",
            "cd",
            "getcwd",
            "pwd",
            "list",
            "ls",
            "pid",
            "kill",
            "prompt",
            "printenv",
            "setenv",
    };

    while (breaker) {
        for (int n = 0; n < MAX_ARG; n++) {
            args[n] = NULL;
        } //initialize

        printf("%s[%s]>", prefix, cwd);//print default cmd line
        fgets(BUFFER, BUF_SIZE, stdin);
        length = (int) strlen(BUFFER);

        if (length >= 2) {
            int num_args = 0;
            BUFFER[length - 1] = '\0';
            string_input = (char *) malloc(length);
            strcpy(string_input, BUFFER);

            char *find_string = (char *) malloc(length);
            strcpy(find_string, BUFFER);
            char *token = strtok(find_string, " ");
            char *find_result = find(flag, token);

            if (find_result != NULL) {
                           char *flagtok = strtok(find_result, " ");
                           token = strtok(NULL, " ");
                           while (flagtok) {
                               length = (int) strlen(flagtok);
                               args[num_args] = (char *) malloc(length);
                               strcpy(BUFFER, flagtok);
                               strcpy(args[num_args], BUFFER);
                               flagtok = strtok(NULL, " ");
                               num_args++;
                           }

                           free(flagtok);
                       }
                       free(find_result);
                       token = strtok(string_input, " "); //reset


            while (token) { //wildcard setting
                if (strstr(token, "*") != NULL || strstr(token, "?") != NULL) {
                    char **p;
                    glob_t t_paths;
                    csource = glob(token, 0, NULL, &t_paths);

                    if (csource == 0) {
                        for (p = t_paths.gl_pathv; *p != NULL; ++p) {
                            length = (int) strlen(*p);
                            args[num_args] = (char *) malloc(length);
                            strcpy(args[num_args], *p);
                            num_args++;
                        }
                        free(&t_paths);
                    }
                }
                else {
                    length = (int) strlen(token);
                    args[num_args] = (char *) malloc(length);
                    strcpy(args[num_args], token);
                }
                token = strtok(NULL, " ");
                num_args = num_args + 1;
            }
            char** piped_args = NULL;
            int piped_arg_nums = 0;
            int pfds[2], first = -1, second = -1, input_index = 0;
            int flag = 0, piped = 0, p_index = -1, p_error =0;

            for (input_index = 0; input_index < input_ct; ++input_index) {
                if (strcmp(args[0], input_command[input_index]) == 0) {
                    break;
                }
            }
            main_run(input_index, args, path_list, num_args, environment, 1);
            free(token);
//clean up
            for (int j = 0; j < MAX_ARG; j++) {
                if(args[j] != NULL){
                    free(args[j]);
                    args[j] = NULL;
                }
            }
            free(string_input);
          }
        }
    free(prompt);
    free(owd);
    free(cwd);
    free(args);
    free(prefix);

    struct pathelement *current;
    current = path_list;
    free(current->element);

    while (current != NULL) {
        free(current);
        current = current->next;
    }
    return 0;
}

void main_run(int input_index, char** args, char* path_list, int num_args, char** environment, int supress_output){
    signal(SIGINT, INThandler); //capture ctrl-c
    
    switch (input_index) {
        case EXIT:
            breaker = 0;
            break;

        case WHICH:
            if (args[1] == NULL) {
                printf("%s", "input error.\n");
            } else {
                for (int i = 1; i < MAX_ARG; i++) {
                    if (args[i] != NULL) {
                        char *result = which(args[i], path_list);
                        if (result != NULL) {
                            printf("%s\n", result);
                            free(result);
                        } else {
                            printf("%s is not found\n", args[i]);
                        }
                    }
                    else {
                        break;
                    }
                }
            }
            break;
        case WHERE:
            if (args[1] == NULL) {
                printf("%s", "input error.\n");
            } else {
                for (int i = 1; i < MAX_ARG; i++) {
                    if (args[i] != NULL) {
                        char *result = where(args[i], &path_list);
                        if (result != NULL) {
                            printf("%s\n", result);
                            free(result);
                        } else {
                            printf("%s is not found\n", args[i]);
                        }
                    } else {
                        break;
                    }
                }
            }
            break;
        case CD:
            printf("");
            char *directory = args[1];
            if (num_args > 2) {
                perror("input incorrect");
            }
            else {
                if (num_args == 1) {
                    directory = home_dir;
                }
                else if (num_args == 2) {
                    directory = args[1];
                }
                if ((pwd = getcwd(BUFFER, BUF_SIZE + 1)) == NULL) {
                    perror("getcwd");
                    exit(2);
                }

                if (directory[0] == '-') {
                    if (chdir(owd) < 0) {
                        printf("Directory is wrong: %d\n", errno);
                    }
                    else {
                        free(cwd);
                        cwd = malloc((int) strlen(owd));
                        strcpy(cwd, owd);
                        free(owd);
                        owd = malloc((int) strlen(BUFFER));
                        strcpy(owd, BUFFER);
                    }
                }
                else {
                    if (chdir(directory) < 0) {
                        printf("Directory is wrong: %d\n", errno);
                    }
                    else {
                        free(owd);
                        owd = malloc((int) strlen(BUFFER));
                        strcpy(owd, BUFFER);
                        pwd = getcwd(BUFFER, BUF_SIZE + 1);
                        free(cwd);
                        cwd = malloc((int) strlen(BUFFER));
                        strcpy(cwd, BUFFER);
                    }
                }
            }
            break;
        case PWD:
            printf("working directory: %s\n", cwd);
            break;
        case LIST:
            if (num_args == 1) {
                list(cwd);
            } else {
                for (int i = 1; i < MAX_ARG; i++) {
                    if (args[i] != NULL) {
                        printf("[%s]:\n", args[i]);
                        list(args[i]);
                    }
                }
            }
            break;
        case LS:
            if (num_args == 1) {
                list(cwd);
            } else {
                for (int i = 1; i < MAX_ARG; i++) {
                    if (args[i] != NULL) {
                        printf("[%s]:\n", args[i]);
                        list(args[i]);
                    }
                }
            }
            break;
        case PID:
            printf("");
            int pid = getpid();
            printf("pid is: %d\n", pid);
            break;
        case KILL:
           if (num_args == 3) {
               char *pid_seq = args[2];
               char *signalfeed = args[1];
               char *last;
               long pidNum;
               long signalNum;
               pidNum = strtol(pid_seq, &last, 10);
               signalfeed[0] = ' ';
               signalNum = strtol(signalfeed, &last, 10);
               if ((pid_seq == last) ||(signalfeed == last)) {
                   printf("Cannot complete request\n");
               }
               int id = (int) pidNum;
               int sig = (int) signalNum;
               kill(id, signalNum);
           }
           else if (num_args == 2){
               char *pid_seq = args[1];
               char *last;
               long temp;
               temp = strtol(pid_seq, &last, 10);
               if ( pid_seq == last ) {
                   printf("Cannot complete request\n");
               }
               int id = (int) temp;
               kill(id, SIGTERM);
           }
           else {
                printf("numargs is %d\n",num_args);
                printf("%s\n", "input is incorrect");
         }
           break;
        case PROMPT:
            free(prefix);
            if (num_args == 1) {
                fgets(BUFFER, BUF_SIZE, stdin);
                length = (int) strlen(BUFFER);
                BUFFER[length - 1] = '\0';
                prefix = (char *) malloc(length);
                strcpy(prefix, BUFFER);
            }
            if (num_args == 2) {
                prefix = (char *) malloc(strlen(args[1]));
                strcpy(prefix, args[1]);
            }
            break;
        case PRINT_ENV:
            printenv(num_args, environment, args);
            break;
        case SET_ENV:
            if (num_args == 1) {
                printenv(num_args, environment, args);
            }
            if (num_args == 2) {
                setenv(args[1], "", 1);
            }
            if (num_args == 3) {
                setenv(args[1], args[2], 1);

                if (strcmp(args[1], "DEFAULT") == 0) {
                    home_dir = getenv("DEFAULT");
                }
                else if (strcmp(args[1], "PATH") == 0) {
                    home_dir = get_path();
                }
                else if (strcmp(args[1], "HOME") == 0) {
                    path_list = getenv("HOME");
                }
            } else {
                printf("%s\n", "arguments number incorrect");
            }
            break;
    }
}

void printenv(int num_args, char **environment, char **args) {
    if (num_args == 1) {
        int n = 0;
        while (environment[n] != NULL) {
            printf("%s \n", environment[n]);
            n++;
        }
    } else if (num_args == 2) {
        char *tmp = getenv(args[1]);
        if (tmp != NULL) {
            printf("%s \n", tmp);
        }
    }
}
char *which(char *command, struct pathelement *path_list) {
    struct pathelement *current = path_list;
    struct dirent *Directory_var;
    char DIR_BUF[BUF_SIZE];
    DIR *Directory_tmp;
    while (current != NULL) {
        char *route = current->element;
        Directory_tmp = opendir(route);
        if(Directory_tmp){
            while ((Directory_var = readdir(Directory_tmp)) != NULL) {
                if (strcmp(Directory_var->d_name, command) == 0) {
                    int length = (int) strlen(DIR_BUF);
                    char *output = (char *) malloc(length);
                    strcpy(output, DIR_BUF);
                    strcpy(DIR_BUF, route);
                    strcat(DIR_BUF, "/");
                    strcat(DIR_BUF, Directory_var->d_name);
                    closedir(Directory_tmp);
                    return output;
                }
            }
        }
        current = current->next;
        closedir(Directory_tmp);
    }
    return NULL;
}

char *where(char *command, struct pathelement *path_list) {
    struct pathelement *current = path_list;
    struct dirent *Directory_var;
    char DIR_BUF[BUF_SIZE];
    strcpy(DIR_BUF, "");
    DIR *Directory_tmp;

    while (current != NULL) {
        char *route = current->element;
        Directory_tmp = opendir(route);
        if(Directory_tmp){
            while ((Directory_var = readdir(Directory_tmp)) != NULL) {
                if (strcmp(Directory_var->d_name, command) == 0) {
                    strcat(DIR_BUF, route);
                    strcat(DIR_BUF, "/");
                    strcat(DIR_BUF, Directory_var->d_name);
                    strcat(DIR_BUF, "\n");
                }
            }
        }
        current = current->next;
        closedir(Directory_tmp);
    }
    int length = (int) strlen(DIR_BUF);
    char *output = (char *) malloc(length);
    DIR_BUF[length - 1] = '\0';
    strcpy(output, DIR_BUF);
    return output;
}
void list(char *directory) {
    DIR *dir;
    struct dirent *dir_val;
    dir = opendir(directory);
    if (dir == NULL) {
        printf("directory is empty: %s\n", directory);
    } else {
        while ((dir_val = readdir(dir)) != NULL) {
            printf("%s\n", dir_val->d_name);
        }
    }
    closedir(dir);
}
void  INThandler(int sig)//catch ctrl+c
{
     signal(sig, SIG_IGN);
     printf("\n");
     exit(0);

}
