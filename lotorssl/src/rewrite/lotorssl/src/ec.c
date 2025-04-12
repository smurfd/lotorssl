// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "ec.h"

// turning the rosetta code ecdsa example to use arrays. possible?

// Helpers
static inline void str2uint32(uint32_t *r, const char *s) {
  for (uint8_t i = 0; i < 64; i++) {
    uint32_t val = (uint32_t)(s[i] < 'a' ? s[i] - '0' : s[i] - 'a' + 10);
    r[(i >> 3)] |= val << (4 * (7 - i & 0x7)); // big endian
  }
}

static inline void printuint32(const char *s, const uint32_t *a) {
  printf("%s: %.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x\n", s, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
}

static inline void getrandom(uint32_t *r, int len) {
  FILE *f = fopen("/dev/urandom", "rb");
  fread(r, sizeof(uint32_t), len, f);
  fclose(f);
}

static inline uint32_t empty(const uint32_t *a) {
  uint32_t zero = 0;
  for (uint8_t i = 0; i < 8; i++) {zero |= a[i];}
  return (zero == 0);
}

static inline uint32_t checkbit(const uint32_t *a, const int16_t b) {
  return (a[b >> 5] & ((uint32_t)1 << (b & 0x01f)));
}

// Math
// return mod(a, N)
static inline void mod_n(uint32_t *r, const uint32_t *a, const struct curve_t *c) {
  uint64_t t[8] = {0};
  for (uint8_t i = 0; i < 8; i++) {
    t[i] = a[i] % c->n[i];
    if (t[i] < 0) printf("tn small\n");
    r[i] = t[i];
  }
}

// return mod(a, p)
static inline void mod_p(uint32_t *r, const uint32_t *a, const struct curve_t *c) {
  uint64_t t[8] = {0};
  for (uint8_t i = 0; i < 8; i++) {
    t[i] = a[i] % c->p[i];
    if (t[i] < 0) printf("tr small\n");
    r[i] = t[i];
  }
}

// Public
struct curve_t *getcurve(void) {
  return &c;
}

// Public debug
void debug_printcurve(const struct curve_t *c) {
  printuint32("a ", c->a);
  printuint32("b ", c->b);
  printuint32("n ", c->n);
  printuint32("p ", c->p);
  printuint32("Gx", c->G);
  printuint32("Gy", c->G+8);
  uint32_t r[8] = {0};
  mod_p(r, c->b, c);
  printuint32("mod_p", r);
  mod_n(r, c->G, c);
  printuint32("mod_n", r);
}
