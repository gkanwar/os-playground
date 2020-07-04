/**
 * Reproduce some C stdlib functionality, as needed.
 */

#include "cstring"
#include <stdarg.h>
#include <stdint.h>

namespace std {

size_t strlen(const char* str) 
{
  size_t len = 0;
  while (str[len]) len++;
  return len;
}

void* memset(void* ptr, int value, size_t num) {
  uint8_t byte_value = (uint8_t) value;
  uint8_t* byte_ptr = (uint8_t*)ptr;
  for (; num > 0; num--, byte_ptr++) {
    *byte_ptr = byte_value;
  }
  return ptr;
}

static size_t fprintf_format_int(void putc(char), int arg) {
  char buf[32];
  size_t len = 0;
  size_t tot = 0;
  if (arg == 0) {
    putc('0');
    return 1;
  }
  else if (arg < 0) {
    putc('-');
    arg *= -1;
    tot = 1;
  }
  while (arg > 0) {
    buf[len] = '0' + arg % 10;
    arg /= 10;
    len++;
  }
  for (size_t i = len; i > 0; --i) {
    putc(buf[i-1]);
    tot++;
  }
  return tot;
}

static size_t vfprintf_format_arg(void putc(char), const char** pfmt, va_list& args) {
  const char* fmt = *pfmt;
  const char* fmt_spec = fmt+1;
  if (*fmt_spec == 'd') {
    *pfmt += 2;
    int arg = va_arg(args, int);
    return fprintf_format_int(putc, arg);
  }
  else { // unknown format spec, just print '%' and move to next char
    putc(*fmt);
    *pfmt += 1;
    return 1;
  }
}

size_t vfprintf(void putc(char), const char* fmt, va_list& args) {
  const char* s = fmt;
  size_t tot = 0;
  for (char next = *s; next != '\0'; next = *s) {
    if (next != '%') {
      putc(next);
      tot++;
      s++;
      continue;
    }
    tot += vfprintf_format_arg(putc, &s, args);
  }
  return tot;
}

}
