#include "include/tetris_backend.h"

const uint16_t TETROMINO_SHAPES[7][4] = {
    {0x0F0, 0x4444, 0xF00, 0x2222},    // I
    {0x0660, 0x0660, 0x0660, 0x0660},  // O
    {0x0270, 0x0464, 0x0E40, 0x2620},  // T
    {0x0360, 0x0462, 0x06C0, 0x4620},  // S
    {0x0630, 0x0264, 0x0C60, 0x2640},  // Z
    {0x0740, 0x0622, 0x02E0, 0x4460},  // J (Corrected)
    {0x0710, 0x0226, 0x08E0, 0x6440},  // L (Corrected)
};

bool shape_has_block(tetromino_type type, rotation_t rot, int r, int c) {
  uint16_t mask = TETROMINO_SHAPES[type][rot];
  int bit = r * 4 + c;
  return (mask >> bit) & 1u;
}
