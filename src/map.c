#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "map.h"

// Create a new hashmap
HashMap *createHashMap(
    unsigned int (*hashFunc)(void *),
    int (*keyCompareFunc)(void *, void *),
    void (*keyFreeFunc)(void *),
    void (*valueFreeFunc)(void *)
) {
    HashMap *map = malloc(sizeof(HashMap));
    if (map == NULL) {
        return NULL;
    }
    map->table = malloc(sizeof(Entry *) * TABLE_SIZE);
    if (map->table == NULL) {
        free(map);
        return NULL;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        map->table[i] = NULL;
    }
    
    map->numEntries = 0;
    map->hashFunc = hashFunc;
    map->keyCompareFunc = keyCompareFunc;
    map->keyFreeFunc = keyFreeFunc;
    map->valueFreeFunc = valueFreeFunc;
    if (pthread_mutex_init(&map->mutex, NULL) != 0) {
        free(map->table);
        free(map);
        return NULL;
    }
    return map;
}

// Create a new hash table entry
Entry *createEntry(void *key, void *value, time_t ttl) {
    Entry *entry = malloc(sizeof(Entry));
    entry->key = key;
    entry->value = value;
    entry->next = NULL;
    entry->ttl = time(NULL) + ttl;
    return entry;
}

// Insert a key-value pair into the hash table
void *insertHashMap(HashMap *hashMap, void *key, void *value, time_t ttl) {
    pthread_mutex_lock(&hashMap->mutex);

    unsigned int slot = hashMap->hashFunc(key) % TABLE_SIZE;
    Entry *entry = hashMap->table[slot];

    //case where slot is empty
    if (entry == NULL) {
        hashMap->table[slot] = createEntry(key, value, ttl);
        pthread_mutex_unlock(&hashMap->mutex);
        hashMap->numEntries++;
        return hashMap->table[slot];
    }

    //case where slot is nonempty
    Entry *prev = NULL;
    while (entry != NULL) {
        //case where found entry with matching key. update the value in this case
        if (hashMap->keyCompareFunc(entry->key, key) == 0) {
            if (hashMap->valueFreeFunc) {
                hashMap->valueFreeFunc(entry->value);
            }
            entry->value = value;
            entry->ttl = time(NULL) + ttl;
            pthread_mutex_unlock(&hashMap->mutex);
            hashMap->numEntries++;
            return entry;
        }
        prev = entry;
        entry = entry->next;
    }

    prev->next = createEntry(key, value, ttl);
    hashMap->numEntries++;
    pthread_mutex_unlock(&hashMap->mutex);

    return prev->next;
}

// Retrieve a value by key from the hashmap
void *getHashMap(HashMap *hashMap, void *key, time_t ttl) {
    pthread_mutex_lock(&hashMap->mutex);
    unsigned int slot = hashMap->hashFunc(key) % TABLE_SIZE;
    Entry *entry = hashMap->table[slot];

    while (entry != NULL) {
        if (hashMap->keyCompareFunc(entry->key, key) == 0) {
            entry->ttl = time(NULL) + ttl;
            pthread_mutex_unlock(&hashMap->mutex);
            return entry->value;
        }
        entry = entry->next;
    }
    pthread_mutex_unlock(&hashMap->mutex);
    return NULL;
}

// Delete a key-value pair from the hashmap
int removeHashMap(HashMap *hashMap, void *key) {
    pthread_mutex_lock(&hashMap->mutex);
    unsigned int slot = hashMap->hashFunc(key) % TABLE_SIZE;
    Entry *entry = hashMap->table[slot];
    Entry *prev = NULL;

    while (entry != NULL && hashMap->keyCompareFunc(entry->key, key) != 0) {
        prev = entry;
        entry = entry->next;
    }
    //key not found. invalid
    if (entry == NULL) {
        pthread_mutex_unlock(&hashMap->mutex);
        return -1;
    }

    //Found an entry and adjusting accordingly (if entry is first in the slot)
    if (prev == NULL) {
        hashMap->table[slot] = entry->next;
    } else {
        prev->next = entry->next;
    }

    if (hashMap->keyFreeFunc) {
        hashMap->keyFreeFunc(entry->key);
    }
    if (hashMap->valueFreeFunc) {
        hashMap->valueFreeFunc(entry->value);
    }
    //printf("prev is null: %d\n", prev == NULL);
    if (prev != NULL) {
        free(entry);
    }
    hashMap->numEntries--;
    pthread_mutex_unlock(&hashMap->mutex);
    return 0;
}


void evictHashMap(HashMap* hashMap) {
    pthread_mutex_lock(&hashMap->mutex);

    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry *curr = hashMap->table[i];
        while(curr) {
            if (curr->ttl < time(NULL)) {
                Entry *temp = curr->next;
                pthread_mutex_unlock(&hashMap->mutex);
                removeHashMap(hashMap, curr->key);
                pthread_mutex_lock(&hashMap->mutex);
                curr = temp;
            }
            else {
                curr = curr->next;
            }
        }
    }
    pthread_mutex_unlock(&hashMap->mutex);
}

// Free the memory used by the hashmap
void freeHashMap(HashMap *hashMap) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry *entry = hashMap->table[i];
        while (entry != NULL) {
            Entry *temp = entry;
            entry = entry->next;
            if (hashMap->keyFreeFunc) {
                hashMap->keyFreeFunc(temp->key);
            }
            if (hashMap->valueFreeFunc) {
                hashMap->valueFreeFunc(temp->value);
            }
            free(temp);
            hashMap->numEntries--;
        }
    }
    free(hashMap->table);
    pthread_mutex_destroy(&hashMap->mutex);
    printf("Entries remainining: %d\n", hashMap->numEntries);
    free(hashMap);
}

// Sample hash function for string keys
unsigned int stringHashFunc(void *key) {
    char *str = (char *)key;
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(str);

    for (; i < key_len; ++i) {
        value = value * 37 + str[i];
    }

    return value;
}

// Sample key comparison function for string keys
int stringKeyCompareFunc(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2);
}

// Sample key free function for string keys
void stringKeyFreeFunc(void *key) {
    free(key);
}

// Sample value free function for integer values
void intValueFreeFunc(void *value) {
    free(value);
}