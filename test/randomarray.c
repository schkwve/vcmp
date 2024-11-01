#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void generate_random_char_array(char *array, int length)
{
    const char charset[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int charset_size = sizeof(charset) - 1;

    for (int i = 0; i < length - 1; i++) {
        int index = rand() % charset_size;
        array[i] = charset[index];
    }
    array[length - 1] = '\0'; // Null-terminate the string
}

int main()
{
    int length = 10; // Set the desired length of the character array

    // High-resolution time seeding
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((unsigned int)(ts.tv_nsec ^ ts.tv_sec ^
                         getpid())); // XOR with seconds, nanoseconds, and PID

    for (int i = 0; i < 10; i++) {
        char array[length];
        generate_random_char_array(array, length);

        printf("Random character array: %s\n", array);
    }

    return 0;
}
