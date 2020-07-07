#ifndef PANIC_H
#define PANIC_H

/**
 * If things go wrong we might need to hang with a panic message.
 */

// "Pretty"-print our panic message and halt
void panic(const char* msg);

#endif
