// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "bm.h"
#include "bmec.h"

bint *inverse_mod(bint *ret, const bint *k, const bint *p) {
  bint s = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ols = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint t = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, olt = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint r = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, olr = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint zero = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  int16_t kz = cmp(k, &zero);
  if (kz == 0) {
    return NULL;
  } else if (kz < 0 || k->neg == 1) {
    bint ktmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bint rtmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
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
    bint tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bint q = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bdiv(&q, &tmp, &olr, &r);  // q = old_r // r
    q.neg = 0; // TODO : force always positive quotation
    r.neg = 0;
    olr.neg = 0;
    bint tr = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ts = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tt = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    memcpy(&tr, &r, sizeof(bint));  // save r to tmp: tr = r
    memcpy(&ts, &s, sizeof(bint));  // save s to tmp: ts = s
    memcpy(&tt, &t, sizeof(bint));  // save t to tmp: tt = t
    tr.siz = r.siz;
    ts.siz = s.siz;
    tt.siz = t.siz;
    bint qr = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, qs = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, qt = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bmul(&qr, &q, &r); // qr = quot * r
    bmul(&qs, &q, &s); // qs = quot * s
    bmul(&qt, &q, &t); // qt = quot * t
    bint rr1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ss1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tt1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bsub(&rr1, &olr, &qr); // r = old_r - (quot * r)
    bsub(&ss1, &ols, &qs); // s = old_s - (quot * s)
    bsub(&tt1, &olt, &qt); // t = old_t - (quot * t)
    memset(r.wrd, 0, LEN * sizeof(uint32_t));
    memset(s.wrd, 0, LEN * sizeof(uint32_t));
    memset(t.wrd, 0, LEN * sizeof(uint32_t));
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
  bint kx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, one = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tmp3 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp4 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint pn = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint kk = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  str2bint(&pn, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2e");
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
  bint zero = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, m = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  if (cmp(p1x, &zero) == 0 && cmp(p1y, &zero) == 0) {
    memcpy(rx, p2x, sizeof(bint));
    memcpy(ry, p2y, sizeof(bint));
    return;
  }
  if (cmp(p2x, &zero) == 0 && cmp(p2y, &zero) == 0) {
    memcpy(rx, p1x, sizeof(bint));
    memcpy(ry, p1y, sizeof(bint));
    return;
  }
  if (cmp(p1x, p2x) == 0 && cmp(p1y, p2y) != 0) {
    bint zx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, zy = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    memcpy(rx, &zx, sizeof(bint));
    memcpy(ry, &zy, sizeof(bint));
    return;
  }
  if (cmp(p1x, p2x) == 0) {
    bint xx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, xxx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bint xxx2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, yy = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bint iy = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    badd(&xx, p1x, p1x);
    badd(&xxx, &xx, p1x);
    bmul(&xxx2, &xxx, p2x);
    badd(&yy, p1y, p1y);
    inverse_mod(&iy, &yy, p);
    bmul(&m, &iy, &xxx2);
  } else {
    bint xx1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, yy1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ix = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bsub(&yy1, p1y, p2y);
    bsub(&xx1, p1x, p2x);
    inverse_mod(&ix, &xx1, p);
    memset(&m.wrd, 0, LEN * sizeof(uint32_t));
    bmul(&m, &ix, &yy1);
    // m = (y1 - y2) * inverse_mod(x1 - x2, curve.p)
  }
  bint x3 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, y3 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint mm = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, mmx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint xx3 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, mmx3 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
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
  bint zero = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, q = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint kk = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  memcpy(&kk, k, sizeof(bint));
  bmod(&q, &tmp, &kk, n);
  if (cmp(&q, &zero) == 0 || (cmp(p1x, &zero) == 0 && cmp(p1y, &zero) == 0)) {
    bint zx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, zy = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    memcpy(rx, &zx, sizeof(bint));
    memcpy(ry, &zy, sizeof(bint));
    return;
  }
  if (cmp(&kk, &zero) < 0) {
    kk.neg ^= 1;
    p1x->neg ^= 1;
    p1y->neg ^= 1;
    scalar_mul(rx, ry, &kk, p1x, p1y, p, n);
    kk.neg ^= 1;
    p1x->neg ^= 1;
    p1y->neg ^= 1;
    return;
  }
  bint rsx = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, rsy = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint ax = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ay = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tw = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, one = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  memcpy(&ax, p1x, sizeof(bint));
  memcpy(&ay, p1y, sizeof(bint));
  wrd2bint(&one, 1);
  wrd2bint(&tw, 2);
  while(cmp(&kk, &zero) != 0) {
    bint tmp1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tmp2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
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
