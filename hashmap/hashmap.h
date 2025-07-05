//
// Created by sigma on 29/06/2025.
//

#ifndef LIBS_HASHMAP_H
#define LIBS_HASHMAP_H

typedef struct hashmap hashmap;
hashmap *hashmap_create(int initial_capacity);
void hashmap_put(hashmap *map, const char *key, void* value);
void*  hashmap_get(hashmap *map, const char *key);
int  hashmap_remove(hashmap *map, const char *key);
void hashmap_clear(hashmap *map);

#endif
