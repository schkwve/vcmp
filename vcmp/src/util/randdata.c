#include <stdlib.h>
#include <vcmp/util/randdata.h>

void rand_array(char *arr, int len)
{
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static int charset_size = sizeof(charset) - 1;

    for (int i = 0; i < len; i++) {
        int index = rand() % charset_size;
        arr[i] = charset[index];
    }
}