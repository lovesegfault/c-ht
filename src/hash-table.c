#include "hash-table.h"

static struct ht_item HT_ITEM_DELETED = {NULL, NULL};
static typedef unsigned long long hash_t;

static struct ht_item *ht_item_init(const char *key, const char *value) {
    struct ht_item *it = calloc(1, sizeof(struct ht_item));
    it->key = strdup(key);
    it->value = strdup(value);
    return it;
}

static struct ht_item *ht_item_kill(struct ht_item *it) {
    free(it->value);
    free(it->key);
    free(it);
    return NULL;
}

static inline void seed_init(unsigned long long *a, unsigned long long *b) {
    // Try to open the random generator device
    FILE *f = fopen("/dev/urandom", "re");
    if (f == NULL) {
        printf("Could not open /dev/urandom - Are you on POSIX?\n");
        exit(EXIT_FAILURE);
    }
    // If we have urandom, just read from it and cast to ulonglong
    fread(a, sizeof(*a), 1, f);
    fread(b, sizeof(*b), 1, f);

    if (ferror(f) != 0) {
        printf("Failure reading /dev/urandom: %i\n", feof(f));
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fclose(f);
}

static void ht_kill(hashtable **ht) {
    for (size_t i = 0; i < (*ht)->size; ++i) {
        struct ht_item *it = (*ht)->items[i];
        if (it != NULL)
            ht_item_kill(it);
    }
    free((*ht)->items);
    free(*ht);
    *ht = NULL;
}

static hash_t ht_get_hash(hashtable *ht, const char *s, const size_t attempts) {
    size_t len = strlen(s);
    const hash_t hash_a = XXH64(s, len, ht->seed_a);
    const hash_t hash_b = XXH64(s, len, ht->seed_b);
    return (hash_a + (attempts * (hash_b + 1))) % ht->size;
}

static void ht_insert(hashtable *ht, const char *key, const char *value) {
    struct ht_item *item = ht_item_init(key, value);
    hash_t index = ht_get_hash(ht, key, 0);
    struct ht_item *current = ht->items[index];
    size_t attempts = 1;
    while (current != NULL) {
        index = ht_get_hash(ht, key, attempts);
        current = ht->items[index];
        ++attempts;
    }
    ht->items[index] = item;
    ++ht->count;
}

static char *ht_search(hashtable *ht, const char *key) {
    hash_t index = ht_get_hash(ht, key, 0);
    struct ht_item *item = ht->items[index];
    size_t attempts = 1;
    while (item != NULL) {
        if (strcmp(item->key, key) == 0) return item->value;

        index = ht_get_hash(ht, key, attempts);
        item = ht->items[index];
        ++attempts;
    }
    return NULL;
}

static void ht_remove(hashtable *h, const char *key) {
    hash_t index = ht_get_hash(ht, key, 0);
    struct ht_item *item = ht->items[index];
    size_t attempts = 1;
    while (item != NULL){

    }
}


hashtable *ht_init() {
    hashtable *ht = calloc(1, sizeof(hashtable));
    // TODO: Resizing!
    ht->size = 53;
    ht->count = 0;
    ht->items = calloc(ht->size, sizeof(struct ht_item *));

    seed_init(&ht->seed_a, &ht->seed_b);

    ht->insert = ht_insert;
    ht->search = ht_search;
    ht->remove = ht_remove;
    ht->kill = ht_kill;
    return ht;
}