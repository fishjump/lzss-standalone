#ifndef __LZSS_STANDALONE_CIRCULAR_QUEUE_H__
#define __LZSS_STANDALONE_CIRCULAR_QUEUE_H__

#include <stddef.h>
#include <stdint.h>

typedef struct lzss_circular_queue lzss_circular_queue_t;

struct lzss_circular_queue {
  uint8_t *buffer;
  size_t capacity;
  size_t head;
  size_t tail;
};

// initializor
void make_lcq(lzss_circular_queue_t *queue, uint8_t *buffer, size_t capacity);

// element access
int lcq_front(const lzss_circular_queue_t *queue, uint8_t *value);
int lcq_back(const lzss_circular_queue_t *queue, uint8_t *value);
int lcq_at(const lzss_circular_queue_t *queue, size_t index, uint8_t *value);

// status access
int lcq_empty(const lzss_circular_queue_t *queue);
int lcq_full(const lzss_circular_queue_t *queue);
size_t lcq_size(const lzss_circular_queue_t *queue);
size_t lcq_capacity(const lzss_circular_queue_t *queue);

// modifiers
int lcq_push(lzss_circular_queue_t *queue, uint8_t value);
int lcq_pop(lzss_circular_queue_t *queue);
int lcq_push_and_pop(lzss_circular_queue_t *queue, uint8_t value);

#endif // __LZSS_STANDALONE_CIRCULAR_QUEUE_H__