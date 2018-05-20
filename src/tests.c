#include <stdio.h>
#include "ht.h"
int main() {
	ht_t *ht = ht_init();
	ht->insert(ht, "foo", "bar");
	ht->insert(ht, "foo", "bazz");
	ht->insert(ht, "foo", "bizz");
	ht->insert(ht, "foo", "bozz");
	ht->insert(ht, "bar", "foo");

    char *a = ht->search(ht, "foo");
    char *b = ht->search(ht, "bar");

    printf("count: %zu\n", ht->count);
    printf("foo: %s\n", a);
    printf("foo: %s\n", b);


    ht->kill(&ht);
    return 0;
}