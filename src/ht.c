#include "ht.h"

static size_t HT_INITIAL_TARGET = 1000;
static struct ht_item HT_ITEM_DELETED = {NULL, NULL};
typedef unsigned long long hash_t;

static inline void ht_resize(ht_t *, size_t);
static void ht_resize_up(ht_t *);
static void ht_resize_down(ht_t *);


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

static void ht_kill(ht_t **ht) {
    const size_t load = (*ht)->count * 100 / (*ht)->real_size;
    if(load < 10) ht_resize_down(*ht);

    for (size_t i = 0; i < (*ht)->real_size; ++i) {
        struct ht_item *it = (*ht)->items[i];
        if (it != NULL)
            ht_item_kill(it);
    }
    free((*ht)->items);
    free(*ht);
    *ht = NULL;
}

static hash_t ht_get_hash(ht_t *ht, const char *s, const size_t attempts) {
    size_t len = strlen(s);
    const hash_t hash_a = XXH64(s, len, ht->seed_a);
    const hash_t hash_b = XXH64(s, len, ht->seed_b);
    return (hash_a + (attempts * (hash_b + 1))) % ht->real_size;
}

static void ht_insert(ht_t *ht, const char *key, const char *value) {
    const size_t load = ht->count * 100 / ht->real_size;
    if(load > 70) ht_resize_up(ht);

    struct ht_item *item = ht_item_init(key, value);
    hash_t index = ht_get_hash(ht, key, 0);
    struct ht_item *current = ht->items[index];
    size_t attempts = 1;
    while (current != NULL) {
        if (current != &HT_ITEM_DELETED) {
            if (strcmp(current->key, key) == 0) {
                ht_item_kill(current);
                ht->items[index] = item;
                return;
            }
        }
        index = ht_get_hash(ht, key, attempts);
        current = ht->items[index];
        ++attempts;
    }
    ht->items[index] = item;
    ++ht->count;
}

static char *ht_search(ht_t *ht, const char *key) {
    hash_t index = ht_get_hash(ht, key, 0);
    struct ht_item *item = ht->items[index];
    size_t attempts = 1;
    while (item != NULL) {
        if (item != &HT_ITEM_DELETED) {
            if (strcmp(item->key, key) == 0) return item->value;
        }

        index = ht_get_hash(ht, key, attempts);
        item = ht->items[index];
        ++attempts;
    }
    return NULL;
}

static void ht_remove(ht_t *ht, const char *key) {
    hash_t index = ht_get_hash(ht, key, 0);
    struct ht_item *item = ht->items[index];
    size_t attempts = 1;
    while (item != NULL) {
        if (item != &HT_ITEM_DELETED) {
            if (strcmp(item->key, key) == 0) {
                ht_item_kill(item);
                ht->items[index] = &HT_ITEM_DELETED;
                --ht->count;
            }
        }
        index = ht_get_hash(ht, key, attempts);
        item = ht->items[index];
        ++attempts;
    }
}

static ht_t *ht_init_sized(const size_t capacity) {
    ht_t *ht = calloc(1, sizeof(ht_t));
    ht->target_size = capacity;
    ht->real_size = next_prime(ht->target_size);
    ht->count = 0;
    ht->items = calloc(ht->real_size, sizeof(struct ht_item *));

    seed_init(&ht->seed_a, &ht->seed_b);

    ht->insert = ht_insert;
    ht->search = ht_search;
    ht->remove = ht_remove;
    ht->kill = ht_kill;

    return ht;
}

static inline void ht_resize(ht_t *ht, const size_t target_size){
    if (target_size < HT_INITIAL_TARGET) return;

    ht_t *ht_new = ht_init_sized(target_size);
    for(size_t i = 0; i < ht->real_size; ++i){
        struct ht_item *item = ht->items[i];
        if(item != NULL && item != &HT_ITEM_DELETED) {
            ht_insert(ht_new, item->key, item->value);
        }
    }

    ht->target_size = ht_new->target_size;
    ht->count = ht_new->count;

    const size_t tmp_size = ht->real_size;
    ht->real_size = ht_new->real_size;
    ht_new->real_size= tmp_size;

    struct ht_item **tmp_items = ht->items;
    ht->items = ht_new->items;
    ht_new->items = tmp_items;

    ht_new->kill(&ht_new);
}

static void ht_resize_up(ht_t *ht){
    ht_resize(ht, ht->target_size * 2);
}

static void ht_resize_down(ht_t *ht){
    ht_resize(ht, ht->target_size / 2);
}

ht_t *ht_init() {
    return ht_init_sized(HT_INITIAL_TARGET);
}