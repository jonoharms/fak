#include "key_event_queue.h"
#include "ch55x.h"

#define QUEUE_LEN KEY_EVENT_QUEUE_LEN

__xdata __at(XADDR_KEY_EVENT_QUEUE) struct {
    uint8_t head;
    uint8_t tail;
    uint8_t bhead;
    fak_key_event_t queue[QUEUE_LEN];
} key_event_queue;

inline uint8_t key_event_queue_get_size(void) {
    return (key_event_queue.head - key_event_queue.tail + QUEUE_LEN) % QUEUE_LEN;
}

inline uint8_t key_event_queue_get_bsize(void) {
    return (key_event_queue.bhead - key_event_queue.head + QUEUE_LEN) % QUEUE_LEN;
}

inline uint8_t* key_event_queue_state(void) {
    return &key_event_queue.head;
}

inline fak_key_event_t* key_event_queue_front(void) {
    return &key_event_queue.queue[key_event_queue.tail];
}

inline fak_key_event_t* key_event_queue_bfront(void) {
    return &key_event_queue.queue[key_event_queue.head];
}

void key_event_queue_push(void) {
    key_event_queue.head = (key_event_queue.head + 1) % QUEUE_LEN;
}

void key_event_queue_pop(void) {
    key_event_queue.tail = (key_event_queue.tail + 1) % QUEUE_LEN;
}

void key_event_queue_bpush(fak_key_event_t *ev) {
    key_event_queue.queue[key_event_queue.bhead] = *ev;
    key_event_queue.bhead = (key_event_queue.bhead + 1) % QUEUE_LEN;
}

void key_event_queue_bpop(void) {
    key_event_queue.bhead = (key_event_queue.bhead - 1 + QUEUE_LEN) % QUEUE_LEN;
}

void key_event_queue_breset(void) {
    key_event_queue.bhead = key_event_queue.head;
}

void key_event_queue_init(void) {
    key_event_queue.head = 0;
    key_event_queue.tail = 0;
    key_event_queue.bhead = 0;
}
