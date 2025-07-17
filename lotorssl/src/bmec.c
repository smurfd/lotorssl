// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "lotormath/src/lotormath.h"
#include "bmec.h"

// TODO: document functions
bint *inverse_mod(bint *ret, const bint *k, const bint *p) {
  bint s = bcreate(), ols = bcreate(), t = bcreate(), olt = bcreate(), r = bcreate(), olr = bcreate(), zero = bcreate();
  int16_t kz = cmp(k, &zero);
  if (kz == 0) {
    return NULL;
  } else if (kz < 0 || k->neg == 1) {
    bint ktmp = bcreate(), rtmp = bcreate();
    BCPY(ktmp, *(bint*)k);
    ktmp.neg ^= 1;
    inverse_mod(&rtmp, &ktmp, p);
    bsub(ret, p, &rtmp);
    return ret;
  }
  wrd2bint(&s, 0);
  wrd2bint(&ols, 1);
  wrd2bint(&t, 1);
  wrd2bint(&olt, 0);
  BCPY(r, *(bint*)p);
  BCPY(olr, *(bint*)k);
  while (cmp(&r, &zero) > 0) {
    bint tmp = bcreate(), q = bcreate(), qr = bcreate(), qs = bcreate(), qt = bcreate(), rr1 = bcreate(), ss1 = bcreate(), tt1 = bcreate();
    bdiv(&q, &tmp, &olr, &r);  // q = old_r // r
    bmul(&qr, &q, &r); // qr = quot * r
    bmul(&qs, &q, &s); // qs = quot * s
    bmul(&qt, &q, &t); // qt = quot * t
    bsub(&rr1, &olr, &qr); // r = old_r - (quot * r)
    bsub(&ss1, &ols, &qs); // s = old_s - (quot * s)
    bsub(&tt1, &olt, &qt); // t = old_t - (quot * t)
    BSWP(r, olr); // r <-> old_r
    BSWP(s, ols); // s <-> old_s
    BSWP(t, olt); // t <-> old_t
    BSWP(r, rr1); // r <-> old_r - (quot * r)
    BSWP(s, ss1); // s <-> old_s - (quot * s)
    BSWP(t, tt1); // t <-> old_t - (quot * t)
  }
  bint kx = bcreate(), one = bcreate(), tmp = bcreate(), tmp3 = bcreate(), tmp4 = bcreate(), kk = bcreate();
  wrd2bint(&one, 1);
  assert(cmp(&olr, &one) == 0); // assert g == 1
  BCPY(kk, *(bint*)k);
  kk.neg = 0;
  bmul(&kx, &kk, &ols);
  bmod(&tmp3, &tmp4, &kx, p);
  assert(cmp(&tmp3, &one) == 0); // assert (k * x) % p == 1
  bmod(ret, &tmp, &ols, p);
  return ret;
}

void point_add(bint *rx, bint *ry, bint *p1x, bint *p1y, bint *p2x, bint *p2y, bint *p) {
  bint zero = bcreate(), m = bcreate();
  if (cmp(p1x, &zero) == 0 && cmp(p1y, &zero) == 0) {
    BCPY(*(bint*)rx, *(bint*)p2x);
    BCPY(*(bint*)ry, *(bint*)p2y);
    return;
  } else if (cmp(p2x, &zero) == 0 && cmp(p2y, &zero) == 0) {
    BCPY(*(bint*)rx, *(bint*)p1x);
    BCPY(*(bint*)ry, *(bint*)p1y);
    return;
  } else if (cmp(p1x, p2x) == 0 && cmp(p1y, p2y) != 0) {
    bint zx = bcreate();
    BCPY(*(bint*)rx, zx);
    BCPY(*(bint*)ry, zx);
    return;
  } else if (cmp(p1x, p2x) == 0) {
    bint xx = bcreate(), xxx = bcreate(), xxx2 = bcreate(), yy = bcreate(), iy = bcreate();
    badd(&xx, p1x, p1x);
    badd(&xxx, &xx, p1x);
    bmul(&xxx2, &xxx, p2x);
    badd(&yy, p1y, p1y);
    inverse_mod(&iy, &yy, p);
    bmul(&m, &iy, &xxx2);
  } else {
    bint xx1 = bcreate(), yy1 = bcreate(), ix = bcreate();
    bsub(&yy1, p1y, p2y);
    bsub(&xx1, p1x, p2x);
    inverse_mod(&ix, &xx1, p);
    bmul(&m, &ix, &yy1);
  }
  bint x3 = bcreate(), y3 = bcreate(), mm = bcreate(), mmx = bcreate(), xx3 = bcreate(), mmx3 = bcreate(), tmp = bcreate();
  bmul(&mm, &m, &m);
  bsub(&mmx, &mm, p1x);
  bsub(&x3, &mmx, p2x); // x3 = m * m - x1 - x2

  bsub(&xx3, &x3, p1x);
  bmul(&mmx3, &m, &xx3);
  badd(&y3, p1y, &mmx3); // y3 = y1 + m * (x3 - 1)
  bmod(rx, &tmp, &x3, p);
  y3.neg ^= 1;
  bmod(ry, &tmp, &y3, p);
}

void point_mul(bint *rx, bint *ry, const bint *p1x, const bint *p1y, const bint *p0, const bint *p) {
  bint r0x = bcreate(), r0y = bcreate(), r1x = bcreate(), r1y = bcreate(), k = bcreate(), di = bcreate(), two = bcreate(), one = bcreate(), tmp = bcreate();
  BCPY(r1x, *(bint*)p1x);
  BCPY(r1y, *(bint*)p1y);
  BCPY(k, *(bint*)p0);
  wrd2bint(&two, 2);
  wrd2bint(&one, 1);
  for (int i = bbitlen(p0) - 1; i >= 0; i--) {
    bint kt = bcreate();
    brshift(&kt, &k, i);
    bmod(&di, &tmp, &kt, &two);
    bint dii = bcreate(), diim = bcreate(), tmp = bcreate();
    badd(&dii, &di, &one);
    bmod(&diim, &tmp, &dii, &two);
    if (diim.wrd[0]) point_add(&r1x, &r1y, &r0x, &r0y, &r1x, &r1y, (bint*)p);
    else point_add(&r0x, &r0y, &r0x, &r0y, &r1x, &r1y, (bint*)p);
    if (di.wrd[0]) point_add(&r1x, &r1y, &r1x, &r1y, &r1x, &r1y, (bint*)p);
    else point_add(&r0x, &r0y, &r0x, &r0y, &r0x, &r0y, (bint*)p);
  }
  BCPY(*(bint*)rx, r0x);
  BCPY(*(bint*)ry, r0y);
}

void scalar_mul(bint *rx, bint *ry, const bint *k, const bint *p1x, const bint *p1y, const bint *p, const bint *n) {
  bint rsx = bcreate(), rsy = bcreate(), ax = bcreate(), ay = bcreate(), tw = bcreate(), one = bcreate();
  bint zero = bcreate(), q = bcreate(), tmp = bcreate(), kt = bcreate(), zx = bcreate();
  BCPY(kt, *(bint*)k);
  bmod(&q, &tmp, &kt, n);
  if (cmp(&q, &zero) == 0 || (cmp(p1x, &zero) == 0 && cmp(p1y, &zero) == 0)) {
    BCPY(*(bint*)rx, zx);
    BCPY(*(bint*)ry, zx);
    return;
  } else if (cmp(&kt, &zero) < 0) {
    kt.neg ^= 1;
    scalar_mul(rx, ry, &kt, p1x, p1y, p, n);
    return;
  }
  wrd2bint(&one, 1);
  wrd2bint(&tw, 2);
  BCPY(ax, *(bint*)p1x);
  BCPY(ay, *(bint*)p1y);
  while(cmp(&kt, &zero) != 0) {
    bint tmp1 = bcreate(), tmp2 = bcreate();
    bmod(&tmp1, &tmp2, &kt, &tw);
    if (cmp(&tmp1, &one) == 0) point_add(&rsx, &rsy, &rsx, &rsy, &ax, &ay, (bint*)p);
    point_add(&ax, &ay, &ax, &ay, &ax, &ay, (bint*)p);
    brshift(&kt, &kt, 1);
  }
  BCPY(*(bint*)rx, rsx);
  BCPY(*(bint*)ry, rsy);
}
