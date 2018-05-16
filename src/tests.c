#include <stdio.h>
#include "hash-table.h"
int main() {
	hash_table *ht = ht_new();
	ht_insert(ht, "foo", "bar");
	ht_insert(ht, "foo", "bazz");
	ht_insert(ht, "foo", "bizz");
	ht_insert(ht, "foo", "bozz");
	ht_insert(ht, "bar", "foo");
    printf("Hello, World!\n");
    char *a = ht_search(ht, "foo");
    char *b = ht_search(ht, "bar");

    printf("count: %zu\n", ht->count);
    printf("foo: %s\n", a);
    printf("foo: %s\n", b);


    ht_del(ht);
    return 0;
}