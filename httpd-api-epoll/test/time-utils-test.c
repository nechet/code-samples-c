#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "../src/time-utils.h"

void testTimestamp() {
    char* time = malloc(sizeof(char) * 50);
    memset(time, 0, sizeof(char) * 50);
    
    timestamp(time);
    printf("%s\n", time);

    free(time);
}

int main(void* args) {
    testTimestamp();
}