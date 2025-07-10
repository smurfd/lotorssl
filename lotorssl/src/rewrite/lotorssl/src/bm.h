#ifndef BM_H
#define BM_H
#include <stdint.h>
#define LEN 81
typedef struct bint {
  uint32_t wrd[LEN];
  int16_t neg, siz, cap;
} bint;

bint bcreate(void);
bint *bsetbit(bint *a, const uint32_t i);
bint *blshift(bint *ret, const bint *a, const uint32_t b);
bint *brshift(bint *ret, const bint *a, const uint32_t b);
bint *badd(bint *ret, const bint *a, const bint *b);
bint *bsub(bint *ret, const bint *a, const bint *b);
bint *bmul(bint *ret, const bint *a, const bint *b);
bint *bdiv(bint *ret, bint *tmp, const bint *a, const bint *d);
bint *bmod(bint *ret, bint *tmp, const bint *a, const bint *m);
bint *bgcd(bint *ret, const bint *a, const bint *b);
bint *str2bint(bint *x, const char *str);
bint *wrd2bint(bint *x, const uint32_t w);
bint *bcpy(bint *a, const bint *b, int n);
void bprint(char *s, bint *a);
int16_t cmp(const bint *a, const bint *b);
int16_t bbitlen(const bint *a);
#endif

