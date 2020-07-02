/**
 * Reproduce some C stdlib functionality, as needed.
 */

#include "cstring"

namespace std {

size_t strlen(const char* str) 
{
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

}
