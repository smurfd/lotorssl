// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "bm.h"

// ----- Print functions -----
void bprint(char *s, bint *a) {
  printf("%s: %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x: %d\n",
    s, a->wrd[19], a->wrd[18], a->wrd[17], a->wrd[16], a->wrd[15], a->wrd[14], a->wrd[13], a->wrd[12], a->wrd[11], a->wrd[10], a->wrd[9], a->wrd[8],
    a->wrd[7], a->wrd[6], a->wrd[5], a->wrd[4], a->wrd[3], a->wrd[2], a->wrd[1], a->wrd[0], a->siz);
}

void bprint2(char *s, bint *a) {
  printf("%s: ", s); for (int8_t i = LEN - 1; i >= 0; i--) printf("%08x ", a->wrd[i]); printf(" : %d\n", a->siz);
}

// ----- Helper functions -----
static inline uint32_t uint32_lo(uint32_t a) {
  return ((a) & ((uint32_t) - 1) >> 32 / 2);
}

static inline uint32_t uint32_hi(uint32_t a) {
  return ((a) >> sizeof(uint32_t) * 8 / 2);
}

static inline int16_t uint32_tru(const uint32_t *a, int16_t an) {
  while (an > 0 && a[an - 1] == 0) an--;
  return an;
}

static inline int16_t uint32_abs(const uint32_t *a, const uint32_t *b, int16_t an, int16_t bn) {
  if (an > bn) return +1;
  if (an < bn) return -1;
  for (int16_t i = an - 1; i >= 0; i--) {
    if (a[i] < b[i]) return -1;
    if (a[i] > b[i]) return +1;
  }
  return 0;
}

int16_t cmp(const bint *a, const bint *b) {
  printf("GNs %d %d\n", a->siz, b->siz);
  bprint("GNcmp1", (bint*)a);
  bprint("GNcmp2", (bint*)b);
  if (a->siz != b->siz) {printf("GN s1\n"); return 1;}
  if (a->siz <= 0) {printf("GN s<\n"); return 1;}
  for (int16_t i = a->siz - 1; i >= 0; i--) {if (a->wrd[i] != b->wrd[i]) {printf("GN wrd\n");return 1;}}
  printf("equal\n");
  return 0; // Equal
}


// ----- Helper convert functions -----
bint *str2bint(bint *x, const char *str) {
  for (int i = 0; i < LEN; i++) x->wrd[i] = 0;
  for (int i = 0; i < 64; i++) { // str + 2 to skip 0x from hexstr
    uint32_t val = (uint32_t)((str + 2)[i] < 'a' ? (str + 2)[i] - '0' : (str + 2)[i] - 'a' + 10);
    x->wrd[7 - (i >> 3)] |= val << (4 * (7 - i & 0x7));
  }
  x->siz = 8;
  return x;
}

bint *wrd2bint(bint *x, const uint32_t w) {
  memset(x->wrd, 0, LEN * sizeof(uint32_t));
  x->wrd[0] = w;
  x->siz = 1;
  x->neg = 0;
  return x;
}


// ----- Math Add & Sub functions -----
static inline int16_t uint32_add(uint32_t *ret, const uint32_t *a, const uint32_t *b, int16_t an, int16_t bn) {
  uint32_t carry = 0, n = an < bn ? an : bn, i, sum, sum2;
  printf("pi: add32 %d %d %d\n", n, an, bn);
  for (i = 0; i < n; i++) {
    sum = (carry + a[i]); // add and get carry
    carry = (sum < a[i]);
    sum2 = (sum + b[i]);
    carry += (sum2 < b[i]);
    ret[i] = sum2;
  }
  for (; i < an; i++) {
    ret[i] = (a[i] + carry); // add and get carry
    carry = (ret[i] < carry);
  }
  for (; i < bn; i++) {
    ret[i] = (b[i] + carry); // add and get carry
    carry = (ret[i] < carry);
  }
  if (carry) ret[i++] = carry;
  return uint32_tru(ret, i);
}

static inline int16_t uint32_sub(uint32_t *ret, const uint32_t *a, const uint32_t *b, int16_t an, int16_t bn) {
  uint32_t carry = 0, n = an < bn ? an : bn, i, dif = 0, dif2;
  for (i = 0; i < bn; i++) {
    dif = a[i] - carry; // sub and get carry
    carry = dif > a[i];
    dif2 = dif - b[i]; // sub and get carry
    carry += (dif2 > dif);
    ret[i] = dif2;
  }
  for (; i < an; i++) {
    ret[i] = a[i] - carry; // sub and get carry
    carry = ret[i] > a[i];
  }
  return uint32_tru(ret, i);
}

static inline int16_t uint32_addsigned(uint32_t *ret, int16_t ret_neg, const uint32_t *a, const int16_t an, const int16_t a_neg,
  const uint32_t *b, const int16_t bn, const int16_t b_neg) {
  printf("pi: add add %d %d : %d %d\n", an, a_neg, bn, b_neg);
  if (a_neg) {
    if (b_neg) {
      if (an >= bn) {ret_neg = 1; return uint32_add(ret, a, b, an, bn);}
      else {ret_neg = 1; return uint32_add(ret, b, a, bn, an);}
    } else {
      if (uint32_abs(a, b, an, bn) >= 0) {ret_neg = 1; return uint32_sub(ret, a, b, an, bn);}
      else {ret_neg = 0;  return uint32_sub(ret, b, a, bn, an);}
    }
  } else {
    if (b_neg) {
      if (uint32_abs(a, b, an, bn) >= 0) {ret_neg = 0; return uint32_sub(ret, a, b, an, bn);}
      else {ret_neg = 1; return uint32_sub(ret, b, a, bn, an);}
    } else {
      if (an >= bn) {ret_neg = 0; return uint32_add(ret, a, b, an, bn);}
      else {ret_neg = 0; return uint32_add(ret, b, a, bn, an);}
    }
  }
}

bint *badd(bint *ret, const bint *a, const bint *b) {
  printf("pi: add %d %d: neg: %d %d\n", a->siz, b->siz, a->neg, b->neg);
  ret->siz = uint32_addsigned(ret->wrd, ret->neg, a->wrd, a->siz, a->neg, b->wrd, b->siz, b->neg);
  return ret;
}

bint *bsub(bint *ret, const bint *a, const bint *b) {
  printf("pi: add sub %d %d: neg: %d %d\n", a->siz, b->siz, a->neg, b->neg);
  ret->siz = uint32_addsigned(ret->wrd, ret->neg, a->wrd, a->siz, a->neg, b->wrd, b->siz, !b->neg);
  return ret;
}


// ----- Math Shift functions -----
bint *blshift(bint *ret, const bint *a, const uint32_t b) {
  int16_t ws = b / 32, bs = b % 32;
  if (bs) {
    uint32_t lo, hi = 0;
    for (int i = a->siz + ws; i > ws; i--) {
      lo = a->wrd[i - ws - 1];
      ret->wrd[i] = (hi << bs) | (lo >> (32 - bs));
      hi = lo;
    }
    for (int i = ws; i >= 0; i--) {
      ret->wrd[i] = hi << bs;
      hi = 0;
    }
    ret->neg = ret->neg ^ a->neg;
    ret->siz = uint32_tru(ret->wrd, a->siz + ws + 1);
  } else {
    for (int i = a->siz + ws - 1; i >= ws; i--) {
      ret->wrd[i] = a->wrd[i - ws];
    }
    for (int i = ws - 1; i >= 0; i--) {
      ret->wrd[i] = 0;
    }
    ret->neg = ret->neg ^ a->neg;
    ret->siz = uint32_tru(ret->wrd, a->siz + ws);
  }
  return ret;
}

bint *brshift(bint *ret, const bint *a, const uint32_t b) {
  int16_t ws = b / 32, bs = b % 32, i = 0;
  if (bs) {
    uint32_t hi, lo = a->wrd[ws];
    for (i = 0; i < a->siz - ws - 1; i++) {
      hi = a->wrd[i + ws + 1];
      ret->wrd[i] = (hi << (32 - bs)) | (lo >> bs);
      lo = hi;
    }
    ret->wrd[i++] = lo >> bs;
    ret->neg = ret->neg ^ a->neg;
    ret->siz = uint32_tru(ret->wrd, i);
  } else {
    for (i = 0; i < a->siz - ws; i++) {
      ret->wrd[i] = a->wrd[i + ws];
    }
    ret->neg = ret->neg ^ a->neg;
    ret->siz = uint32_tru(ret->wrd, i);
  }
  return ret;
}


// ----- Bit functions -----
int16_t bbitlen(const bint *a) {
  int16_t last = a->siz - 1, ret = 0;
  if (last < 0) return 0;
  for (int i = 32 - 1; i >= 0; i--) {
    if ((a->wrd[last] >> i) & 1) {ret = i + 1; break;}
  }
  return ret + (last * 32);
}

bint *bsetbit(bint *a, const uint32_t i) {
  printf("BIT %d\n", i);
  int16_t wi = i / 32, n = wi + 1;
  memset(a->wrd, 0, LEN);//a->siz * n);
  a->siz = a->siz > n ? a->siz: n;
  a->wrd[wi] |= (((uint32_t)1) << (i % 32));
  return a;
}


// ----- Math Mul functions -----
static inline uint32_t uint32_mul_lo(const uint32_t a, const uint32_t b) {
  return a * b;
}

static inline uint32_t uint32_mul_hi(const uint32_t a, const uint32_t b) {
  uint32_t r0 = uint32_lo(a) * uint32_lo(b);
  uint32_t r1 = uint32_lo(a) * uint32_hi(b);
  uint32_t r2 = uint32_hi(a) * uint32_lo(b);
  uint32_t r3 = uint32_hi(a) * uint32_hi(b);
  uint32_t r4 = uint32_hi(r0) + uint32_lo(r1) + uint32_lo(r2);
  return uint32_hi(r4) + uint32_hi(r1) + uint32_hi(r2) + r3;
}

static inline int16_t uint32_mul_add(uint32_t *ret, const uint32_t *a, const uint32_t *b, int16_t an, int16_t bn) {
  if (an == 0 || bn == 0) return 0;
  for (int16_t j = 0; j < bn; j++) {
    uint32_t carry = 0, i, n = an, f = b[j], r[LEN] = {0};
    memcpy(r, ret+j, LEN * sizeof(uint32_t));
    for (i = 0; i < n; i++){
      uint32_t src_word = a[i];
      uint32_t dst_word = uint32_mul_lo(src_word, f);
      dst_word = dst_word + carry; // add and get carry
      carry = (dst_word < carry);
      carry += uint32_mul_hi(src_word, f);
      r[i] = r[i] + dst_word; // add and get carry
      carry += (r[i] < dst_word);
    }
    for (; carry; i++){
      r[i] = r[i] + carry; // add and get carry
      carry = r[i] < carry;
    }
    memcpy(ret+j, r, LEN * sizeof(uint32_t));
  }
  return uint32_tru(ret, an + bn);
}

// https://en.wikipedia.org/wiki/Karatsuba_algorithm
static inline int16_t uint32_mul_karatsuba(uint32_t *ret, const uint32_t *a, const uint32_t *b, int16_t an, int16_t bn, uint32_t *tmp) {
  if (an < LEN && bn < LEN) return uint32_mul_add(ret, a, b, an, bn);
  int16_t m = an > bn? an : bn, m2 = m / 2, m3 = m2 + 2, nlo1hi1, nlo2hi2, nz0, nz1, nz2;
  uint32_t *lo1 = (uint32_t*)a, *lo2 = (uint32_t*)b, *hi1 = (uint32_t*)a + m2, *hi2 = (uint32_t*)b + m2, *lo1hi1, *lo2hi2, *z0, *z1, *z2;
  int16_t nlo1 = uint32_tru(lo1, m2 < an? m2 : an), nlo2 = uint32_tru(lo2, m2 < bn? m2 : bn);
  int16_t nhi1 = uint32_tru(hi1, (an-m2) > 0? (an-m2) : 0), nhi2 = uint32_tru(hi2, (bn-m2) > 0? (bn-m2) : 0);
  lo1hi1 = tmp; tmp += m3; lo2hi2 = tmp; tmp += m3;
  z0 = tmp; tmp += m3 * 2; z1 = tmp; tmp += m3 * 2; z2 = tmp; tmp += m3 * 2;
  nlo1hi1 = uint32_add(lo1hi1, lo1, hi1, nlo1, nhi1); nlo2hi2 = uint32_add(lo2hi2, lo2, hi2, nlo2, nhi2);
  nz0 = uint32_mul_karatsuba(z0, lo1, lo2, nlo1, nlo2, tmp);
  nz1 = uint32_mul_karatsuba(z1, lo1hi1, lo2hi2, nlo1hi1, nlo2hi2, tmp);
  nz2 = uint32_mul_karatsuba(z2, hi1, hi2, nhi1, nhi2, tmp);
  nz1 = uint32_sub(z1, z1, z0, nz1, nz0);
  nz1 = uint32_sub(z1, z1, z2, nz1, nz2);
  memcpy(ret, z0, nz0 * sizeof(uint32_t));
  nz0 = uint32_add(ret+m2*1, ret+m2*1, z1, (nz0-m2) > 0? (nz0-m2) : 0, nz1);
  nz0 = uint32_add(ret+m2*2, ret+m2*2, z2, (nz0-m2) > 0? (nz0-m2) : 0, nz2);
  return uint32_tru(ret, nz0+m2*2);
}

bint *bmul(bint *ret, const bint *a, const bint *b) {
  int16_t an = a->siz, bn = b->siz, n = an+bn;
  memset(ret->wrd, 0, LEN * sizeof(uint32_t));
  if (ret->wrd != a->wrd && ret->wrd != b->wrd && an < LEN && bn < LEN) {
    ret->siz = uint32_mul_add(ret->wrd, a->wrd, b->wrd, an, bn);
  } else {
    uint32_t tmp[65535] = {0};
    ret->siz = uint32_mul_karatsuba(tmp, a->wrd, b->wrd, an, bn, tmp+n);
    memcpy(ret->wrd, tmp, ret->siz * sizeof(uint32_t));
  }
  ret->neg = a->neg ^ b->neg;
  return ret;
}


// ----- Div functions -----
static inline bint *bdivmod_hw(bint *ret, uint32_t *rem, uint32_t a) {
  uint32_t part[2] = {0}, remaind = 0, dw, mw;
  for (int i = ret->siz - 1; i >= 0; i--) {
    uint32_t rw = 0, aw = ret->wrd[i];
    part[1] = uint32_lo(aw); part[0] = uint32_hi(aw);
    for (int j = 0; j < 2; j++) {
      remaind <<= 16; // 32 / 2
      remaind |= part[j];
      dw = remaind / a;
      mw = remaind % a;
      remaind = mw;
      rw <<= 16; // 32 / 2
      rw |= dw;
    }
    ret->wrd[i] = rw;
  }
  *rem = remaind;
  ret->siz = uint32_tru(ret->wrd, ret->siz);
  return ret;
}

static inline bint *bdivmod(bint *ret, bint *rem, const bint *a, const bint *d) {
  bint *quot = ret, den, *rema = rem;
  rema->siz = rem->siz;
  quot->siz = ret->siz;
  rema->neg = rem->neg;
  quot->neg = ret->neg;
  if (d->siz == 0) {
    printf("RUHRO\n"); return NULL; // this should never happen
  } else if (a->siz == 1 && d->siz == 1) {
    printf("d1\n");
    uint32_t aw = a->wrd[0], bw = d->wrd[0];
    wrd2bint(quot, aw / bw);
    wrd2bint(rema, aw % bw);
    ret->neg = a->neg;
    rem->neg = a->neg ^ d->neg;
    memcpy(rem->wrd, rema->wrd, rem->siz * sizeof(uint32_t));
    memcpy(ret->wrd, quot->wrd, ret->siz * sizeof(uint32_t));
    return ret;
  } else if (d->siz == 1 && d->wrd[0] <= (UINT32_MAX) / 2) {
    uint32_t rm;
    memcpy(quot->wrd, a->wrd, a->siz * sizeof(uint32_t));
    bdivmod_hw(quot, &rm, d->wrd[0]);
    wrd2bint(rema, rm);
    ret->neg = a->neg;
    rem->neg = a->neg ^ d->neg;
    memcpy(rem->wrd, rema->wrd, rem->siz * sizeof(uint32_t));
    memcpy(ret->wrd, quot->wrd, ret->siz * sizeof(uint32_t));
    return ret;
  }
  memset(rema->wrd, 0, LEN * sizeof(uint32_t));
  memset(quot->wrd, 0, LEN * sizeof(uint32_t));
  memcpy(rema->wrd, a->wrd, a->siz * sizeof(uint32_t));
  rema->neg = 0;
  quot->siz = 0;
  if (uint32_abs(rema->wrd, d->wrd, rema->siz, d->siz) >= 0) {
    int32_t sh = bbitlen(rema) - bbitlen(d);
    blshift(&den, (bint*)d, sh);
    den.neg = 0;
    for (int32_t s = sh; s >= 0; s--) {
      if (uint32_abs(rema->wrd, den.wrd, rema->siz, den.siz) >= 0) {
        bsub(rema, rema, &den);
        bsetbit(quot, s);
      }
      brshift(&den, &den, 1);
    }
  }
  ret->neg = a->neg;
  rem->neg = a->neg ^ d->neg;
  rem->siz = rema->siz;
  ret->siz = quot->siz;
  return ret;
}

bint *bdiv(bint *ret, const bint *a, const bint *d) {
  bint tmp = {.wrd={0}};
  memset(ret->wrd, 0, LEN * sizeof(uint32_t));
  ret->siz = a->siz;
  bdivmod(ret, &tmp, a, d);
  return ret;
}

bint *bmod(bint *ret, const bint *a, const bint *m) {
  bint tmp = {.wrd={0}};
  memset(ret->wrd, 0, LEN * sizeof(uint32_t));
  ret->siz = a->siz;
  bdivmod(&tmp, ret, a, m);
  bprint("bmod tmp", &tmp);
  bprint("bmod ret", ret);
  bprint("bmod a", a);
  bprint("bmod m", m);
  return ret;
}

