#ifndef HASHTABLE_HASH_TABLE_H
#define HASHTABLE_HASH_TABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "xxhash.h"
#include "primes.h"

struct ht_item{
    char *key;
    char *value;
};

typedef struct ht{
    size_t target_size;
    size_t real_size;
    size_t count;
    unsigned long long seed_a;
    unsigned long long seed_b;
    struct ht_item **items;

    void (*insert)(struct ht*, const char *, const char *);
    char *(*search)(struct ht*, const char *);
    void (*remove)(struct ht*, const char *);
    void (*kill)(struct ht**);
}ht_t;

ht_t *ht_init();

#endif //HASHTABLE_HASH_TABLE_H
