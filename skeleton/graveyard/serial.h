#ifndef SERIAL_H
#define SERIAL_H

#define COM1_PORT 0x3f8
#define COM2_PORT 0x2f8
#define COM3_PORT 0x3e8
#define COM4_PORT 0x2e8

extern Serial* debug_serial;

class Serial {
 public:
  enum baud_rate {
    Baud300 = 384,
    Baud600 = 192,
    Baud1200 = 96,
    Baud2400 = 48,
    Baud4800 = 24,
    Baud9600 = 12,
    Baud14400 = 8,
    Baud19200 = 6,
    Baud38400 = 3,
    Baud57600 = 2,
    Baud115200 = 1
  };
  Serial(uint16_t port, baud_rate baud) : port(port), baud(baud) {
    initialize();
  }
  // WARNING: this hard blocks, so only use for debug-level stuff
  void blocking_putc(uint8_t c) const;

 private:
  void initialize() const;
  uint8_t get_line_status() const;
  
  uint16_t port;
  baud_rate baud;
};


#endif
