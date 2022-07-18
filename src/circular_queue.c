#include "circular_queue.h"
#include "helper.h"

// initializor
void make_lcq(lzss_circular_queue_t *queue, uint8_t *buffer, size_t capacity) {
  queue->buffer = buffer;
  queue->capacity = capacity;
  queue->head = 0;
  queue->tail = 0;
}

// element access
int lcq_front(const lzss_circular_queue_t *queue, uint8_t *value) {
  if (lcq_empty(queue)) {
    return -1;
  }
  *value = queue->buffer[queue->head];
  return 0;
}

int lcq_back(const lzss_circular_queue_t *queue, uint8_t *value) {
  if (lcq_empty(queue)) {
    return -1;
  }
  *value = queue->buffer[queue->tail];
  return 0;
}

int lcq_at(const lzss_circular_queue_t *queue, size_t index, uint8_t *value) {
  if (index >= lcq_size(queue)) {
    return -1;
  }
  *value = queue->buffer[(queue->head + index) % queue->capacity];
  return 0;
}

// status access
int lcq_empty(const lzss_circular_queue_t *queue) {
  return queue->head == queue->tail;
}

int lcq_full(const lzss_circular_queue_t *queue) {
  return (queue->tail + 1) % queue->capacity == queue->head;
}

size_t lcq_size(const lzss_circular_queue_t *queue) {
  return (queue->tail - queue->head + queue->capacity) % queue->capacity;
}

size_t lcq_capacity(const lzss_circular_queue_t *queue) {
  return queue->capacity;
}

// modifiers
int lcq_push(lzss_circular_queue_t *queue, uint8_t value) {
  if (lcq_full(queue)) {
    return -1;
  }
  queue->buffer[queue->tail] = value;
  queue->tail = (queue->tail + 1) % queue->capacity;
  return 0;
}

int lcq_pop(lzss_circular_queue_t *queue) {
  if (lcq_empty(queue)) {
    return -1;
  }
  queue->head = (queue->head + 1) % queue->capacity;
  return 0;
}

int lcq_push_and_pop(lzss_circular_queue_t *queue, uint8_t value) {
  if (lcq_full(queue)) {
    ASSERT_EQ(lcq_pop(queue), 0);
  }

  ASSERT_EQ(lcq_push(queue, value), 0);
  return 0;
}