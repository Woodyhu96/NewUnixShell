#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/* function prototype.  It returns a pointer to a linked list for the path
   elements. */
struct pathelement *get_path();
struct pathelement {
    char *element;
    struct pathelement *next;
};
