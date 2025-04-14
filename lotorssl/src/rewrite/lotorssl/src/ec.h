// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#ifndef EC_H
#define EC_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#define u64 unsigned long long int
struct curve_t {
  uint32_t a[8], b[8], n[8], p[8], G[8 * 2];
};

typedef struct curve_t curve;
static curve e = { // secp256k1
  {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}, // a
  {0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}, // b
  {0xd0364141, 0xbfd25e8c, 0xaf48a03b, 0xbaaedce6, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff}, // n
  {0xfffffc2f, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}, // p
  {0xf6f81798, 0x59f2815b, 0x2dce28d9, 0x029bfcdb, 0xce870b07, 0x55a06295, 0xf9dcbbac, 0x79be667e,  // Gx
   0xfb10d4b8, 0x9c47d08f, 0xa6855419, 0xfd17b448, 0x0e1108a8, 0x5da4fbfc, 0x26a3c465, 0x483ada77}, // Gy
};

curve *getcurve(void);

void debug_printcurve(const curve *e);
#endif
