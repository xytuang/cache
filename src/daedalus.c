#include <stdlib.h>
#include <unistd.h>
#include "map.h"

typedef struct Daedalus {
    int checkFrequency;
    int timeToLive;
    HashMap* store;
    pthread_t evictThread;
} Daedalus;

void* evict(void* cache) {
    Daedalus* currCache = (Daedalus*) cache;
    while(1) {
        evictHashMap(currCache->store);
        sleep(currCache->checkFrequency);
    }
    
    return NULL;
}

//returns null on error, pointer to Dadealus on success
Daedalus* createDaedalus(int checkFrequency, int timeToLive, int keyType, int valueType) {
    Daedalus* newCache = (Daedalus*)malloc(sizeof(Daedalus));
    if (!newCache) {
        return NULL;
    }
    newCache->store = createHashMap(stringHashFunc, stringKeyCompareFunc, stringKeyFreeFunc, intValueFreeFunc);

    newCache->checkFrequency = checkFrequency;
    newCache->timeToLive = timeToLive;
    if (pthread_create(&newCache->evictThread, NULL, evict, &checkFrequency) != 0) {
        perror("Failed to create evict thread\n");
        freeHashMap(newCache->store);
        return NULL;
    }
    return newCache;
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
    Daedalus* cache = createDaedalus(5, 64, 1, 1);
    
    // Insert some key-value pairs
    int* val = malloc(sizeof(int));
    *val = 1;
    add(cache, strdup("key1"), val);


    val = malloc(sizeof(int));
    *val = 2;
    add(cache, strdup("key2"), val);

    val = malloc(sizeof(int));
    *val = 3;
    add(cache, strdup("key3"), val);
    
    // Retrieve values
    printf("Value for key1: %d\n", *(int *)get(cache, "key1"));
    
    printf("Value for key2: %d\n", *(int *)get(cache, "key2"));
    printf("Value for key3: %d\n", *(int *)get(cache, "key3"));
    
    // Delete a key-value pair
    destroy(cache, "key2");
    printf("key2 is null: %d\n", get(cache, "key2") == NULL);

    // Free the hash map
    freeDaedalus(cache);
    return 0;
}
