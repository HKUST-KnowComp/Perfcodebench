#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const char* parse_int(const char* str, int* value) {
    int num = 0;
    while (*str >= '0' && *str <= '9') {
        num = num * 10 + (*str - '0');
        str++;
    }
    *value = num;
    return str;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* ptr = rows[i];

        // Skip 'r' and first comma
        ptr++;
        ptr++;

        // Parse id (ignore)
        int id;
        ptr = parse_int(ptr, &id);
        ptr++;

        // Parse a (ignore)
        int a;
        ptr = parse_int(ptr, &a);
        ptr++;

        // Parse b
        int b;
        ptr = parse_int(ptr, &b);
        ptr++;

        // Parse c
        int c;
        ptr = parse_int(ptr, &c);
        ptr++;

        // Parse flag
        char flag[8];
        int j = 0;
        while (j < 7 && *ptr != '\n' && *ptr != '\0') {
            flag[j++] = *ptr++;
        }
        flag[j] = '\0';

        // Compute v
        uint64_t v = (uint64_t)(b + c + 7);
        if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0') {
            v += 29;
        }

        // Update hash
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}