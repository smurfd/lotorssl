// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "bm.h"
#include "bmec.h"

static inline bint *inverse_mod(bint *ret, const bint *k, const bint *p) {
  bint s = bcreate(), ols = bcreate(), t = bcreate(), olt = bcreate(), r = bcreate(), olr = bcreate(), zero = bcreate();
  int16_t kz = cmp(k, &zero);
  if (kz == 0) {
    return NULL;
  } else if (kz < 0 || k->neg == 1) {
    bint ktmp = bcreate(), rtmp = bcreate();
    memcpy(&ktmp, k, sizeof(bint));
    ktmp.neg ^= 1;
    inverse_mod(&rtmp, &ktmp, p);
    bsub(ret, p, &rtmp);
    return ret;
  }
  wrd2bint(&s, 0);
  wrd2bint(&ols, 1);
  wrd2bint(&t, 1);
  wrd2bint(&olt, 0);
  memcpy(&r, p, sizeof(bint));
  memcpy(&olr, k, sizeof(bint));
  while (cmp(&r, &zero) > 0) {
    bint tmp = bcreate(), q = bcreate(), tr = bcreate(), ts = bcreate(), tt = bcreate(), qr = bcreate(), qs = bcreate(), qt = bcreate();
    bint rr1 = bcreate(), ss1 = bcreate(), tt1 = bcreate();
    bdiv(&q, &tmp, &olr, &r);  // q = old_r // r
    q.neg = 0; // TODO : force always positive quotation
    r.neg = 0;
    olr.neg = 0;
    memcpy(&tr, &r, sizeof(bint));  // save r to tmp: tr = r
    memcpy(&ts, &s, sizeof(bint));  // save s to tmp: ts = s
    memcpy(&tt, &t, sizeof(bint));  // save t to tmp: tt = t
    tr.siz = r.siz;
    ts.siz = s.siz;
    tt.siz = t.siz;
    bmul(&qr, &q, &r); // qr = quot * r
    bmul(&qs, &q, &s); // qs = quot * s
    bmul(&qt, &q, &t); // qt = quot * t
    bsub(&rr1, &olr, &qr); // r = old_r - (quot * r)
    bsub(&ss1, &ols, &qs); // s = old_s - (quot * s)
    bsub(&tt1, &olt, &qt); // t = old_t - (quot * t)
    memcpy(&r, &rr1, sizeof(bint));
    memcpy(&s, &ss1, sizeof(bint));
    memcpy(&t, &tt1, sizeof(bint));
    r.siz = rr1.siz;
    s.siz = ss1.siz;
    t.siz = tt1.siz;
    memcpy(&olr, &tr, sizeof(bint));
    memcpy(&ols, &ts, sizeof(bint));
    memcpy(&olt, &tt, sizeof(bint));
  }
  bint kx = bcreate(), one = bcreate(), tmp = bcreate(), tmp3 = bcreate(), tmp4 = bcreate(), kk = bcreate();
  wrd2bint(&one, 1);
  assert(cmp(&olr, &one) == 0); // assert g == 1
  memcpy(&kk, k, sizeof(bint));
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
    memcpy(rx, p2x, sizeof(bint));
    memcpy(ry, p2y, sizeof(bint));
    return;
  } else if (cmp(p2x, &zero) == 0 && cmp(p2y, &zero) == 0) {
    memcpy(rx, p1x, sizeof(bint));
    memcpy(ry, p1y, sizeof(bint));
    return;
  } else if (cmp(p1x, p2x) == 0 && cmp(p1y, p2y) != 0) {
    bint zx = bcreate(), zy = bcreate();
    memcpy(rx, &zx, sizeof(bint));
    memcpy(ry, &zy, sizeof(bint));
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
  bsub(&x3, &mmx, p2x);

  bsub(&xx3, &x3, p1x);
  bmul(&mmx3, &m, &xx3);
  badd(&y3, p1y, &mmx3);
  bmod(rx, &tmp, &x3, p);
  y3.neg ^= 1;
  bmod(ry, &tmp, &y3, p);
  // x3 = m * m - x1 - x2
  // y3 = y1 + m * (x3 - x1)
}

void scalar_mul(bint *rx, bint *ry, bint *k, bint *p1x, bint *p1y, bint *p, bint *n) {
  bint zero = bcreate(), q = bcreate(), tmp = bcreate(), kk = bcreate();
  memcpy(&kk, k, sizeof(bint));//k->siz * sizeof(uint32_t) + (3 * sizeof(int16_t)));
  bmod(&q, &tmp, &kk, n);
  if (cmp(&q, &zero) == 0 || (cmp(p1x, &zero) == 0 && cmp(p1y, &zero) == 0)) {
    bint zx = bcreate(), zy = bcreate();
    memcpy(rx, &zx, sizeof(bint));
    memcpy(ry, &zy, sizeof(bint));
    return;
  } else if (cmp(&kk, &zero) < 0) {
    kk.neg ^= 1;
    p1x->neg ^= 1;
    p1y->neg ^= 1;
    scalar_mul(rx, ry, &kk, p1x, p1y, p, n);
    kk.neg ^= 1;
    p1x->neg ^= 1;
    p1y->neg ^= 1;
    return;
  }
  bint rsx = bcreate(), rsy = bcreate(), ax = bcreate(), ay = bcreate(), tw = bcreate(), one = bcreate();
  wrd2bint(&one, 1);
  wrd2bint(&tw, 2);
  memcpy(&ax, p1x, sizeof(bint));
  memcpy(&ay, p1y, sizeof(bint));
  while(cmp(&kk, &zero) != 0) {
    bint tmp1 = bcreate(), tmp2 = bcreate();
    bmod(&tmp1, &tmp2, &kk, &tw);
    if (cmp(&tmp1, &one) == 0) {
      point_add(&rsx, &rsy, &rsx, &rsy, &ax, &ay, p);
    }
    point_add(&ax, &ay, &ax, &ay, &ax, &ay, p);
    brshift(&kk, &kk, 1);
  }
  memcpy(rx, &rsx, sizeof(bint));
  memcpy(ry, &rsy, sizeof(bint));
}
