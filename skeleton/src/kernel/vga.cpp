#include "vga.h" // TODO: rename to TTY? I don't really understand what TTY means...

#include <stddef.h>
#include <cstring>

using namespace std;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
  return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
  return (uint16_t) uc | (uint16_t) color << 8;
}

size_t terminal_row;
size_t terminal_column;
size_t terminal_indent;
uint8_t terminal_color;
uint16_t* terminal_buffer;

uint8_t terminal_getcolor() {
  return terminal_color;
}
void terminal_setcolor(uint8_t color) {
  terminal_color = color;
}
void terminal_setcolor(enum vga_color fg, enum vga_color bg) {
  terminal_setcolor(vga_entry_color(fg, bg));
}

void terminal_setindent(size_t indent) {
  terminal_indent = indent;
}

void terminal_clear() {
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

void terminal_initialize() {
  terminal_row = 0;
  terminal_column = 0;
  terminal_indent = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_buffer = (uint16_t*) VIRT_TERM_BUFFER;
  terminal_clear();
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void terminal_advance_row() {
  if (++terminal_row == VGA_HEIGHT) {
    terminal_row = 0;
  }
}

static void terminal_cr() {
  terminal_column = terminal_indent;
}
static void terminal_crlf() {
  terminal_cr();
  terminal_advance_row();
}

void terminal_advance_char() {
  if (++terminal_column == VGA_WIDTH) {
    terminal_crlf();
  }
}

void terminal_putchar(char c) {
  if (c == '\n') {
    terminal_crlf();
  }
  else if (c == '\r') {
    terminal_cr();
  }
  /* TODO: other special chars? */
  else {
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    terminal_advance_char();
  }
}

void terminal_write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++)
    terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
  terminal_write(data, strlen(data));
}
