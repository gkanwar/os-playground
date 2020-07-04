#include "serial.h"

#define DLAB_BIT 1 << 7
#define DATA_BITS   0b00000011
#define STOP_BIT    0b00000100
#define PARITY_BITS 0b00111000
#define DATA_8BIT 0x3
#define STOP_1BIT 0x0
#define PARITY_NONE 0x0
#define LINE_STATUS_THRE 1 << 5

Serial* debug_serial;

void Serial::blocking_putc(uint8_t c) {
  while(!(get_line_status() & LINE_STATUS_THRE));
  io::out8(port, c);
}

void Serial::initialize() {
  io::set_bits8(port + 3, DLAB_BIT);
  io::out8(port + 0, (uint8_t) baud & 0xff);
  io::out8(port + 1, (uint8_t) (baud >> 8) & 0xff);
  io::unset_bits8(port + 3, DLAB_BIT);
  // 8N1 mode
  io::write_bits8(port + 3, DATA_8BIT, DATA_BITS);
  io::write_bits8(port + 3, STOP_1BIT, STOP_BIT);
  io::write_bits8(port + 3, PARITY_NONE, PARITY_BITS);
}

uint8_t Serial::get_line_status() {
  return io::get8(port + 5);
}
