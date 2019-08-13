#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

int main(int argc, char **argv)
{
    log_create();

    log_write("hello\n");
    log_write("hello %d\n", 3);

    log_destroy();
    return 0;
}
