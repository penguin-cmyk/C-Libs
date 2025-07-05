//
// Created by sigma on 29/06/2025.
//

#include <stdlib.h>
#include <stdint.h>

#ifndef LIBS_VECTOR_H
#define LIBS_VECTOR_H

typedef struct {
    void** data;
    size_t size;
    size_t capacity;
} Vector;

Vector* vector_new(void);
void vector_free(Vector* vec);
int vector_push(Vector* vec, void* value);
void* vector_get(Vector* vec, int index);
void vector_remove(Vector* vec, int index);
int vector_search(Vector* vec, void* value);

#endif