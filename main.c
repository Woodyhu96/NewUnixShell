#include "sh.h"
void sig_handler(int signal);
int main(int argc, char **argv, char **envp) {
    return sh(argc, argv, envp);
}
void sig_handler(int signal) {
}
