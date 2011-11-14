#ifndef EVENTEMITTER_H
#define EVENTEMITTER_H

#define EVENTEMITTER_MAX_LISTENERS 20

typedef void (*EventListener)();

typedef struct
{
    EventListener listeners[EVENTEMITTER_MAX_LISTENERS];
    int listener_count;
} EventEmitter;

EventEmitter *EventEmitter_new();
void EventEmitter_listen(EventEmitter *emitter, EventListener listener);
void EventEmitter_emit(EventEmitter *emitter);

#endif
