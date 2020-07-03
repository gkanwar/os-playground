/**
 * Reproduce some C stdlib functionality, as needed.
 */

#include "cstring"
#include <stdint.h>

namespace std {

size_t strlen(const char* str) 
{
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

void * memset(void *ptr, int value, size_t num) {
  uint8_t byte_value = (uint8_t) value;
  uint8_t *byte_ptr = (uint8_t *)ptr;
  for (; num > 0; num--, byte_ptr++) {
    *byte_ptr = byte_value;
  }
  return ptr;
}

}
