#ifndef EVENTEMITTER_C
#define EVENTEMITTER_C

#include <stdlib.h>
#include <string.h>
#include "EventEmitter.h"

EventEmitter *EventEmitter_new() {
    EventEmitter *emitter = malloc(sizeof(EventEmitter));
    memset(emitter, 0, sizeof(EventEmitter));
    return emitter;
}

void EventEmitter_listen(EventEmitter *emitter, EventListener listener) {
    emitter->listeners[emitter->listener_count] = listener;
    emitter->listener_count = 0;
}

void EventEmitter_emit(EventEmitter *emitter) {
    int i;
    for (i = 0; i < emitter->listener_count; i++) {
        emitter->listeners[i]();
    }
}
#endif
