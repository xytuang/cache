#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

typedef struct Entry {
    void *key;
    void *value;
    struct Entry *next;
} Entry;

typedef struct {
    Entry **table;
    unsigned int (*hashFunc)(void *key);
    int (*keyCompareFunc)(void *key1, void *key2);
    void (*keyFreeFunc)(void *key);
    void (*valueFreeFunc)(void *value);
} HashMap;

// Create a new hash table
HashMap *createHashMap(
    unsigned int (*hashFunc)(void *),
    int (*keyCompareFunc)(void *, void *),
    void (*keyFreeFunc)(void *),
    void (*valueFreeFunc)(void *)
) {
    HashMap *map = malloc(sizeof(map));
    map->table = malloc(sizeof(Entry *) * TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++) {
        map->table[i] = NULL;
    }
    map->hashFunc = hashFunc;
    map->keyCompareFunc = keyCompareFunc;
    map->keyFreeFunc = keyFreeFunc;
    map->valueFreeFunc = valueFreeFunc;
    return map;
}

// Create a new hash table entry
Entry *createEntry(void *key, void *value) {
    Entry *entry = malloc(sizeof(Entry));
    entry->key = key;
    entry->value = value;
    entry->next = NULL;
    return entry;
}

// Insert a key-value pair into the hash table
void insert(HashMap *hashMap, void *key, void *value) {
    unsigned int slot = hashMap->hashFunc(key) % TABLE_SIZE;
    Entry *entry = hashMap->table[slot];

    if (entry == NULL) {
        hashMap->table[slot] = createEntry(key, value);
        return;
    }

    Entry *prev = NULL;
    while (entry != NULL) {
        if (hashMap->keyCompareFunc(entry->key, key) == 0) {
            if (hashMap->valueFreeFunc) {
                hashMap->valueFreeFunc(entry->value);
            }
            entry->value = value;
            return;
        }
        prev = entry;
        entry = entry->next;
    }

    prev->next = createEntry(key, value);
}

// Retrieve a value by key from the hash table
void *get(HashMap *hashMap, void *key) {
    unsigned int slot = hashMap->hashFunc(key) % TABLE_SIZE;
    Entry *entry = hashMap->table[slot];

    while (entry != NULL) {
        if (hashMap->keyCompareFunc(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

// Delete a key-value pair from the hash table
void delete(HashMap *hashMap, void *key) {
    unsigned int slot = hashMap->hashFunc(key) % TABLE_SIZE;
    Entry *entry = hashMap->table[slot];
    Entry *prev = NULL;

    while (entry != NULL && hashMap->keyCompareFunc(entry->key, key) != 0) {
        prev = entry;
        entry = entry->next;
    }

    if (entry == NULL) {
        return;
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

    free(entry);
}

// Free the memory used by the hash table
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
        }
    }
    free(hashMap->table);
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

int main() {
    // Create a hash map with string keys and integer values
    HashMap *map = createHashMap(
        stringHashFunc,
        stringKeyCompareFunc,
        stringKeyFreeFunc,
        intValueFreeFunc
    );

    // Insert some key-value pairs
    insert(map, strdup("key1"), malloc(sizeof(int)));
    *(int *)get(map, "key1") = 1;

    insert(map, strdup("key2"), malloc(sizeof(int)));
    *(int *)get(map, "key2") = 2;

    insert(map, strdup("key3"), malloc(sizeof(int)));
    *(int *)get(map, "key3") = 3;


    // Retrieve values
    printf("Value for key1: %d\n", *(int *)get(map, "key1"));
    printf("Value for key2: %d\n", *(int *)get(map, "key2"));
    printf("Value for key3: %d\n", *(int *)get(map, "key3"));

    // Delete a key-value pair
    delete(map, "key2");
    printf("Value for key2 after deletion: %p\n", get(map, "key2"));

    // Free the hash map
    freeHashMap(map);

    return 0;
}