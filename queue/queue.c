//
// Created by sigma on 03/07/2025.
//

#include "queue.h"
#include <stdlib.h>

Queue* new_queue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->current = NULL;
    queue->next = NULL;

    return queue;
}

void* current_queue(Queue* queue) {
    if (!queue) return NULL;
    return queue->current;
}

void queue_pop(Queue* queue) {
    if (!queue) return;

    if (queue->next) {
        queue->current = queue->next->current;
        queue->next = queue->next->next;
    } else {
        queue->current = NULL;
    }
}

void queue_push(Queue* queue, void* value) {
    if (!queue) return;

    Queue* newNode = new_queue();
    newNode->current = value;
    newNode->next = queue->next;

    queue->next = newNode;
}

void queue_free(Queue* queue) {
    while (queue) {
        Queue* next = queue->next;
        free(queue);
        queue = next;
    }
}


