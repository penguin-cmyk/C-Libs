//
// Created by sigma on 29/06/2025.
//

#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

typedef struct hashmap_entry {
    char* key;
    void* value;
    struct hashmap_entry *next;
} hashmap_entry;

struct hashmap {
    hashmap_entry **buckets;
    int capacity;
};

// djb2 hash function by Dan Bernstein
// It starts with an initial hash value and iteratively
// combines each character using: hash * 33 + current_char.
// This provides a good distribution for strings in hash maps.
static unsigned long hash(const char* str) {
    unsigned long hash = 5381;
    int current_char;

    while ((current_char = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + current_char; // hash * 33 + current_char
    return hash;
}

// This allocates a new hashmap with the given in capacity
// and returns the pointer to the hashmap.
hashmap* hashmap_create(int capacity) {
    hashmap* map = malloc(sizeof(hashmap));
    map->capacity = capacity;
    map->buckets = calloc(capacity, sizeof(hashmap_entry*));
    return map;
}

// This function lets you set a specific value
// to a specific key. You need to cast the value to a void pointer.
//
// This is due to a void pointer being able to hold any value,
// and since there is no generic T in C, we use that as a workaround.
void hashmap_put(hashmap* map, const char* key, void* value) {
   unsigned long hashed = hash(key) % map->capacity;
   hashmap_entry* entry = map->buckets[hashed];

   while (entry) {
       if (strcmp(entry->key,key) == 0) {
           entry->value = value; return;
       }
       entry = entry->next;
   }

   entry = malloc(sizeof(hashmap_entry));
   entry->key = strdup(key);
   entry->value = value;
   entry->next = map->buckets[hashed];
   map->buckets[hashed] = entry;
}

// From the given in key it tries to find the hashmap_entry,
// if found it will return it's void pointer (value).
// If it's not found it will return NULL;
void* hashmap_get(hashmap* map, const char* key) {
    unsigned long hashed = hash(key) % map->capacity;
    hashmap_entry* entry = map->buckets[hashed];

    while (entry) {
        if (strcmp(entry->key,key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

// From the given in key it tries to find the hashmap entry,
// If it found that specific hashmap entry it will free it's key
// And the entry itself and return 1.
//
// If it doesn't find the specific entry it returns 0.
int hashmap_remove(hashmap* map, const char* key) {
    unsigned long hashed = hash(key) % map->capacity;
    hashmap_entry* entry = map->buckets[hashed];
    hashmap_entry* prev = NULL;

    while (entry) {
        if (strcmp(entry->key,key) == 0) {
            if (prev) prev->next = entry->next;
            else map->buckets[hashed] = entry->next;

            free(entry->key);
            free(entry);
            return 1;
        }
        prev = entry;
        entry = entry->next;
    }

    return 0;
}

// This function clears every entry in the given in hashmap.
// And once it did all of that it frees the buckets and the
// hashmap itself too.
void hashmap_clear(hashmap* map) {
    for (int i = 0; i < map->capacity; i++) {
        hashmap_entry* entry = map->buckets[i];
        while (entry) {
            hashmap_entry* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }

    free(map->buckets);
    free(map);
}