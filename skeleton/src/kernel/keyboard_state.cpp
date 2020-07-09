#include "assert.h"
#include "debug_serial.h"
#include "keyboard_state.h"

static KeyboardState* inst;
KeyboardState::KeyboardState() {
  inst = this;
  debug::serial_printf("KeyboardState() scan_state = %08x\n", static_cast<unsigned>(scan_state));
  debug::serial_printf("KeyboardState() &scan_state = %08x\n", (uint32_t)&scan_state);
  debug::serial_printf("KeyboardState() &key_states = %08x\n", (uint32_t)key_states);
  debug::serial_printf("KeyboardState() &lock_states = %08x\n", (uint32_t)&lock_states);
  debug::serial_printf("KeyboardState() &scan_buf = %08x\n", (uint32_t)scan_buf);
}
KeyboardState& KeyboardState::get() { return *inst; }

void KeyboardState::key_set_state(KeyCode code) {
  uint8_t state_bitfield = key_states[code / 8];
  state_bitfield |= 1 << (code % 8);
  key_states[code / 8] = state_bitfield;
}
void KeyboardState::key_unset_state(KeyCode code) {
  uint8_t state_bitfield = key_states[code / 8];
  state_bitfield &= ~(1 << (code % 8));
  key_states[code / 8] = state_bitfield;
}
void KeyboardState::key_update_state(KeyCode code, bool make) {
  if (make) key_set_state(code);
  else key_unset_state(code);
}


static inline uint8_t break_bit_set(uint8_t c) {
  return (c >> 7) & 0x1;
}
static inline uint8_t unset_break_bit(uint8_t c) {
  return c & ~(1 << 7);
}

void KeyboardState::handle_scan_code(uint8_t c) {
  debug::serial_printf("scan_state = %08x\n", static_cast<unsigned>(scan_state));
  debug::serial_printf("&scan_state = %08x\n", (uint32_t)&scan_state);
  switch (scan_state) {
    case ready: {
      if (c == 0xe0) {
        scan_state = e0_b1;
      }
      else if (c == 0xe1) {
        // FORNOW: skip
        scan_state = ready;
      }
      else if (break_bit_set(c)) {
        uint8_t code = unset_break_bit(c);
        if (code <= KeyCode::_last) {
          key_unset_state(static_cast<KeyCode>(code));
        }
      }
      else {
        uint8_t code = c;
        if (c <= KeyCode::_last) {
          key_set_state(static_cast<KeyCode>(code));
        }
      }
      break;
    }
    case e0_b1: {
      bool break_bit = break_bit_set(c);
      uint8_t code;
      if (break_bit) code = unset_break_bit(c);
      else code = c;
      switch (code) {
        // FORNOW: these codes just map to the corresponding keypad keys
        case KeyCode::enter:
        case KeyCode::left_ctrl:
        case KeyCode::left_shift:
        case KeyCode::fwd_slash:
        case KeyCode::right_shift:
        case KeyCode::left_alt:
        case KeyCode::scroll_lock:
        case KeyCode::keypad_print_screen:
        case KeyCode::keypad_home:
        case KeyCode::keypad_up:
        case KeyCode::keypad_page_up:
        case KeyCode::keypad_left:
        case KeyCode::keypad_right:
        case KeyCode::keypad_end:
        case KeyCode::keypad_down:
        case KeyCode::keypad_page_down:
        case KeyCode::keypad_insert:
        case KeyCode::keypad_del:
          key_update_state(static_cast<KeyCode>(code), !break_bit);
          scan_state = ready;
          break;
        // TODO: Microsoft keyboard (left window, right window, menu)
        // case 0x5b:
        // case 0x5c:
        // case 0x5d:
      }
      break;
    }
    default: panic("bad keyboard scan state\n");
  }
}
