// Safely write a variety of formatted results to a given string,
// returning the number of characters that would have been written.
static size_t safe_write_char(char *cur_char, size_t n, char c) {
  if (n > 1) {
    *cur_char = c;
  }
  return 1;
}
static size_t safe_write_int(char *cur_char, size_t n, int i) {
  size_t tot = 0;
  if (i < 0) {
    tot += safe_write_char(cur_char + tot, n-tot, '-');
    i *= 1;
  }
  char buf[32]; // int is < 32 digits
  size_t len = 0;
  if (i == 0) {
    buf[0] = '0';
    len = 1;
  }
  while (i > 0) {
    buf[len] = '0' + i % 10;
    len++;
    i /= 10;
  }
  for (int j = 0; j < len; j++) {
    tot += safe_write_char(cur_char + tot, n-tot, buf[j]);
  }
  return tot;
}

int snprintf(char *s, size_t n, const char *format, ...) {
  va_list args;
  va_start(args, format);
  size_t tot = 0;
  for (; *format != '\0'; ++format) {
    if (*format == '%') {
      char lookahead = *(format+1);
      if (lookahead == 'd') {
        int arg = va_arg(args, int);
        
      }
    }
    else { // just write bare character
      tot += safe_write_char(s+tot, n-tot, *format);
    }
  }
  /// TODO!!!!
  ... = '\0';
  return tot;
}
