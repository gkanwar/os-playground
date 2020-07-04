#ifndef KERNEL_H
#define KERNEL_H

// FORNOW: check id mapping works
#define KERNEL_BASE 0x0 //0xc0000000
#define KERNEL_VIRT_TO_PHYS(addr) ((addr) - KERNEL_BASE)

#ifndef ASM_FILE // C++ specific stuff

#include <stdint.h>

// Just memory locations but we want void pointer semantics so mark as uint8_t
extern uint8_t _kernel_start;
extern uint8_t _kernel_end;

#endif

#endif
