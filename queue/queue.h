//
// Created by sigma on 03/07/2025.
//

#ifndef LIBS_QUEUE_H
#define LIBS_QUEUE_H

typedef struct Queue  {
    void* current;
    struct Queue* next;
} Queue;

Queue* new_queue();
void* current_queue(Queue* queue);
void queue_pop(Queue* queue);
void queue_push(Queue* queue, void* value);
void queue_free(Queue* queue);

#endif //LIBS_QUEUE_H
