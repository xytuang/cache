#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define TABLE_SIZE 100

typedef struct Entry {
    void *key;
    void *value;
    struct Entry *next;
    int ttl;
} Entry;

typedef struct {
    Entry **table;
    unsigned int (*hashFunc)(void *key);
    int (*keyCompareFunc)(void *key1, void *key2);
    void (*keyFreeFunc)(void *key);
    void (*valueFreeFunc)(void *value);
    pthread_mutex_t mutex;
} HashMap;

// Create a new hash table
HashMap *createHashMap(
    unsigned int (*hashFunc)(void *),
    int (*keyCompareFunc)(void *, void *),
    void (*keyFreeFunc)(void *),
    void (*valueFreeFunc)(void *)
);

// Create a new hash table entry
Entry *createEntry(void *key, void *value, int ttl);

// Insert a key-value pair into the hashmap
void *insertHashMap(HashMap *hashMap, void *key, void *value, int ttl);

// Retrieve a value by key from the hashmap
void *getHashMap(HashMap *hashMap, void *key, int ttl);

// Delete a key-value pair from the hashmap
int removeHashMap(HashMap *hashMap, void *key);

// Evict key-value pairs from the hashmap
void evictHashMap(HashMap *hashMap);

// Free the memory used by the hashmap
void freeHashMap(HashMap *hashMap);

// Sample hash function for string keys
unsigned int stringHashFunc(void *key);

// Sample key comparison function for string keys
int stringKeyCompareFunc(void *key1, void *key2);

// Sample key free function for string keys
void stringKeyFreeFunc(void *key);

// Sample value free function for integer values
void intValueFreeFunc(void *value);