#include <stdlib.h>
#include "map.h"

typedef struct Daedalus {
    int checkFrequency;
    int timeToLive;
    HashMap* store;
} Daedalus;

//returns null on error, Daedalus pointer on success
Daedalus* createDaedalus(int checkFrequency, int timeToLive, int keyType, int valueType) {
    Daedalus* newCache = (Daedalus*)malloc(sizeof(Daedalus));
    if (!newCache) {
        return NULL;
    }
    newCache->store = createHashMap(stringHashFunc, stringKeyCompareFunc, stringKeyFreeFunc, intValueFreeFunc);

    newCache->checkFrequency = checkFrequency;
    newCache->timeToLive = timeToLive;
    return newCache;
}

void evict(Daedalus* cache) {
    evictHashMap(cache->store);
}

// Returns NULL on error
void* add(Daedalus* cache, void* key, void* value) {
    return insertHashMap(cache->store, key, value, cache->timeToLive);
}

//returns NULL on error
void* get(Daedalus* cache, void* key) {
    void* res = getHashMap(cache->store, key, cache->timeToLive);
    if (!res) {
        return NULL;
    }
    return res;
}

void destroy(Daedalus *cache, void *key) {
    removeHashMap(cache->store, key);
}

void freeDaedalus(Daedalus* cache) {
    freeHashMap(cache->store);
    free(cache);
}

int main() {
    // Create a hash map with string keys and integer values
    Daedalus* cache = createDaedalus(5, 5, 1, 1);

    // Insert some key-value pairs
    add(cache, strdup("key1"), (void*)1);

    add(cache, strdup("key2"), (void*)2);

    add(cache, strdup("key3"), (void*)3);

    // Retrieve values
    printf("Value for key1: %d\n", *(int *)get(cache, "key1"));
    printf("Value for key2: %d\n", *(int *)get(cache, "key2"));
    printf("Value for key3: %d\n", *(int *)get(cache, "key3"));

    // Delete a key-value pair
    destroy(cache, "key2");
    printf("Value for key2 after deletion: %p\n", get(cache, "key2"));

    // Free the hash map
    freeDaedalus(cache);

    return 0;
}
