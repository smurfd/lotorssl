// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "ec.h"

// turning the rosetta code ecdsa example to use arrays. possible?
// ugg not uint32, but long ie +-

// ----- Helpers -----

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

// ----- Math -----

// add a and b
static inline uint32_t add(uint32_t *r, const uint32_t *a, const uint32_t *b) {
  uint32_t ovr = 0;
  for (uint8_t i = 0; i < 8; ++i) {
    uint32_t s = a[i] + b[i] + ovr;
    if (s != a[i]) ovr = (s < a[i]);
    r[i] = s;
  }
  return ovr;
}

// sub a and b
static inline uint32_t sub(uint32_t *r, const uint32_t *a, const uint32_t *b) {
  uint32_t ovr = 0;
  for (uint8_t i = 0; i < 8; ++i) {
    uint32_t s = a[i] - b[i] - ovr;
    if (s != a[i]) ovr = (s > a[i]);
    r[i] = s;
  }
  return ovr;
}

// multiply a and b
static inline void mul(uint32_t *r, const uint32_t *a, const uint32_t* b) {
  memset(r, 0, sizeof(uint32_t) * 8);
  for (uint8_t i = 0; i < 8; i++) {
    uint32_t carry = 0;
    for (uint8_t j = 0; j < 8; j++) {
      uint64_t product = (uint64_t)a[j] * b[i] + r[i + j] + carry;
      r[i + j] = product & 0xffffffff;
      carry = product >> 32;
    }
    r[i + 8] = carry;
  }
}

// return mod(a, n)
static inline void mod_n(uint32_t *r, const uint32_t *a, const curve *e) {
  uint32_t t0 = a[0], t1 = a[1], t2 = a[2], t3 = a[3], t4 = a[4], t5 = a[5], t6 = a[6], t7 = a[7], r0 = 0;
  r0 = t0 > e->n[0];
  t0 %= e->n[0];
  if (r0) {
    t0 -= 1;
    t1 = (u64)(t1 + 0xffffffff % e->n[1]);
    t2 = (u64)(t2 + 0xffffffff % e->n[2]);
    t3 = (u64)(t3 + 0xffffffff % e->n[3]);
    t4 = (u64)(t4 + 0xffffffff % e->n[4]);
    t5 = (u64)(t5 + 0xffffffff % e->n[5]);
    t6 = (u64)(t6 + 0xffffffff % e->n[6]);
    t7 = (u64)(t7 + 0xffffffff % e->n[7]);
    t7 += 1;
  }
  r[0] = t0; r[1] = t1; r[2] = t2; r[3] = t3; r[4] = t4; r[5] = t5; r[6] = t6; r[7] = t7;
}

// return mod(a, p)
static inline void mod_p(uint32_t *r, const uint32_t *a, const curve *e) {
  uint32_t t0 = a[0], t1 = a[1], t2 = a[2], t3 = a[3], t4 = a[4], t5 = a[5], t6 = a[6], t7 = a[7], r0 = 0;
  r0 = t0 > e->p[0];
  t0 %= e->p[0];
  if (r0) {
    t0 -= 1;
    t1 = (u64)(t1 + 0xffffffff % e->p[1]);
    t2 = (u64)(t2 + 0xffffffff % e->p[2]);
    t3 = (u64)(t3 + 0xffffffff % e->p[3]);
    t4 = (u64)(t4 + 0xffffffff % e->p[4]);
    t5 = (u64)(t5 + 0xffffffff % e->p[5]);
    t6 = (u64)(t6 + 0xffffffff % e->p[6]);
    t7 = (u64)(t7 + 0xffffffff % e->p[7]);
    t7 += 1;
  }
  r[0] = t0; r[1] = t1; r[2] = t2; r[3] = t3; r[4] = t4; r[5] = t5; r[6] = t6; r[7] = t7;
}

// return the discriminant of the curve
static inline void discriminant(uint32_t *r, const curve *e) {
  uint32_t a[8], b[8], r1[8] = {0}, aa[8] = {0}, aar[8] = {0}, aarr[8] = {0}, aarrr[8] = {0}, four[8] = {4, 0, 0, 0, 0, 0, 0, 0};
  memcpy(a, e->a, 8 * sizeof(uint32_t));
  memcpy(b, e->b, 8 * sizeof(uint32_t));
  mul(aa, a, a);
  mod_n(aar, aa, e);
  mul(aarr, aar, a);
  mod_n(aarrr, aarr, e);
  mul(r1, four, aarrr);
  // return mod_n(-16 * (c + 27 * mod_n(b * b, e)), e);
}


// ----- Public -----

curve *getcurve(void) {
  return &e;
}

// Public debug
void debug_printcurve(const curve *e) {
  printuint32("a ", e->a);
  printuint32("b ", e->b);
  printuint32("n ", e->n);
  printuint32("p ", e->p);
  printuint32("Gx", e->G);
  printuint32("Gy", e->G + 8);
  uint32_t r1[8] = {0}, correctn[8] = {0}, r2[8] = {0}, correctp[8] = {0};
  mod_n(r1, e->G, e);
  mod_p(r2, e->G, e);
  str2uint32(correctn, "26c1d6569a2022ce7e85889d47ed1ff4ce870b0855a06295f9dcbbac79be667f");
  str2uint32(correctp, "f6f8179859f2815b2dce28d9029bfcdbce870b0755a06295f9dcbbac79be667e");
  printuint32("mod_n", r1);
  printuint32("corkn", correctn);
  printuint32("mod_p", r2);
  printuint32("corkp", correctp);
}
