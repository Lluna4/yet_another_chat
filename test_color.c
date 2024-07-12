#include "lib/user.h"
#include <stdio.h>
#include <time.h>

int main()
{
    char *str = (char *)"holaaaa";
    struct colors lesbian_flag[3] = {{245, 169, 184}, {255, 255, 255}, {91, 206, 250}};
    clock_t start = clock();
    char *a = multicolor_string(lesbian_flag, str, 3);
    clock_t end = clock();
    printf("%s time: %fms", a, ((float)(end - start) / CLOCKS_PER_SEC) * 3600);
}