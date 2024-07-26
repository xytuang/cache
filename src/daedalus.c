#include <stdlib.h>
#include "map.h"

typedef struct Daedalus {
    int checkFrequency;
    int timeToLive;
    HashMap* store;
} Daedalus;

//returns null on error, Daedalus pointer on success
Daedalus* createDaedalus(int checkFrequency, int timeToLive) {
    Daedalus* newCache = (Daedalus*)malloc(sizeof(Daedalus));
    if (!newCache) {
        return NULL;
    }

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

int main() {
    return 0;
}