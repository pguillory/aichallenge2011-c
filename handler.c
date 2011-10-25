#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

void signal_handler(int sig) {
    void *array[20];
    size_t size;

    size = backtrace(array, 20);
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, 2);
    exit(1);
}

void install_handlers() {
    signal(SIGSEGV, signal_handler);
    signal(SIGINT, signal_handler);
    // signal(SIGKILL, signal_handler);
}
