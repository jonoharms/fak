#ifndef __KEY_EVENT_QUEUE_H__
#define __KEY_EVENT_QUEUE_H__

#include <stdint.h>

typedef struct {
    uint8_t pressed;
    uint8_t mapped; // TODO: Wasteful. Merge with pressed as bit flags
    uint8_t key_idx;
    uint16_t timestamp;
} fak_key_event_t;

inline uint8_t key_event_queue_get_size(void);
inline uint8_t key_event_queue_get_bsize(void);
inline uint8_t* key_event_queue_state(void);
inline fak_key_event_t* key_event_queue_front(void);
inline fak_key_event_t* key_event_queue_bfront(void);

void key_event_queue_push(void);
void key_event_queue_pop(void);
void key_event_queue_bpush(fak_key_event_t *ev);
void key_event_queue_bpop(void);
void key_event_queue_breset(void);

void key_event_queue_init(void);

#endif // __KEY_EVENT_QUEUE_H__
