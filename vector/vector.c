//
// Created by sigma on 30/06/2025.
//
#include "vector.h"
#include "stdio.h"
// This function allocates and returns a pointer to a new Vector
Vector* vector_new(void) {
    Vector* vec = (Vector*)malloc(sizeof(Vector));
    if (!vec) return NULL;
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
    return vec;
}

// This function free's the given in Vector by first
// freeing all the data within in the vector and then
// freeing the Vector itself.
void vector_free(Vector* vec) {
    if (vec) {
        free(vec->data);
        free(vec);
    }
}

// For pushing a value to the specific vector,
// you need to cast it as a void*.
//
// This is due to a void pointer being able to hold any value,
// and since there is no generic T in C, we use that as a workaround.
int vector_push(Vector* vec, void* value) {
    if (!vec) return 0;

    if (vec->size >= vec->capacity) {
        size_t new_capacity = vec->capacity == 0 ? 4 : vec->capacity * 2;
        void** new_data = realloc(vec->data, new_capacity * sizeof(void*));
        if (!new_data) return 0;
        vec->data = new_data;
        vec->capacity = new_capacity;
    }

    vec->data[vec->size++] = value;
    return 1;
}

// This function return a void pointer from
// the given in index.
//
// If not found it will return NULL;
void* vector_get(Vector* vec, int index) {
    if (index < 0 || (size_t)index >= vec->size) NULL;

    return vec->data[index];
}

// This searches for a specific value and returns the index for it
// If the data was not found it will return -1
int vector_search(Vector* vec, void* value) {
    for (int i = 0; i < vec->size; i++) {
        if (vec->data[i] == value) return i;
    }
    return -1;
}

// This function removes a specific value by its index
// in the given in Vector by setting it to NULL
void vector_remove(Vector* vec, int index) {
    if (index < 0 || (size_t)index >= vec->size) return;

    if (vec->data[index]) {
        vec->data[index] = NULL;
    }
}