#include "lib/user.h"
#include <stdio.h>
#include <time.h>

int main()
{
    char *str = (char *)"holaaaa";
    struct colors lesbian_flag[3] = {{245, 169, 184}, {255, 255, 255}, {91, 206, 250}};
    char *a = multicolor_string(lesbian_flag, str, 3);
    printf("%s%s", a, RESET);
    free(a);
}