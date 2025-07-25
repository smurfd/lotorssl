// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "lotormath/src/lotormath.h"
#include "bmec.h"
#include "hash.h"

// TODO: document functions
// TODO: struct point bint x, bint y?

//
// Securely randomize arrays
static inline void bintrnd_array(bint *r, int len) {
  FILE *f = fopen("/dev/urandom", "r");
  fread(r->wrd, sizeof(uint32_t), len, f);
  fclose(f);
}

static inline bint *inverse_mod(bint *ret, const bint *k, const bint *p) {
  bint s = bcreate(), ols = bcreate(), t = bcreate(), olt = bcreate(), r = bcreate(), olr = bcreate();
  int16_t kz = cmp(k, &zero0);
  if (kz == 0) {
    return NULL;
  } else if (kz < 0 || k->neg == 1) {
    bint ktmp = bcreate(), rtmp = bcreate(); // k ** -1 = p - (-k) ** -1  (mod p)
    BCPY(ktmp, *(bint*)k);
    ktmp.neg ^= 1;
    inverse_mod(&rtmp, &ktmp, p);
    bsub(ret, p, &rtmp);
    return ret; // p - inverse_mod(-k, p)
  }
  wrd2bint(&s, 0);
  wrd2bint(&ols, 1);
  wrd2bint(&t, 1);
  wrd2bint(&olt, 0);
  BCPY(r, *(bint*)p);
  BCPY(olr, *(bint*)k); // s = 0, old_s = 1, t = 1, old_t = 0, r = p, old_r = k
  bint tmp = bcreate(), q = bcreate(), qr = bcreate(), qs = bcreate(), qt = bcreate(), rr1 = bcreate(), ss1 = bcreate(), tt1 = bcreate(), kx = bcreate();
  while (cmp(&r, &zero0) > 0) {
    bdiv(&q, &tmp, &olr, &r); // q = old_r // r
    bmul(&qr, &q, &r);        // qr = quot * r
    bmul(&qs, &q, &s);        // qs = quot * s
    bmul(&qt, &q, &t);        // qt = quot * t
    bsub(&rr1, &olr, &qr);    // r = old_r - (quot * r)
    bsub(&ss1, &ols, &qs);    // s = old_s - (quot * s)
    bsub(&tt1, &olt, &qt);    // t = old_t - (quot * t)

    BCPY(olr, r);             // old_r = r
    BCPY(ols, s);             // old_s = s
    BCPY(olt, t);             // old_t = t
    BCPY(r, rr1);             // r = old_r - (quot * r)
    BCPY(s, ss1);             // s = old_s - (quot * s)
    BCPY(t, tt1);             // t = old_t - (quot * t)
  } //  gcd, x, y = old_r, old_s, old_t
  assert(cmp(&olr, &one1) == 0); // assert g == 1
  bmul(&kx, k, &ols);
  bmod(ret, &tmp, &kx, p);
  assert(cmp(ret, &one1) == 0); // assert (k * x) % p == 1
  bmod(ret, &tmp, &ols, p);
  return ret; // return x % p
}

static inline void point_add(bint *rx, bint *ry, bint *p1x, bint *p1y, bint *p2x, bint *p2y, bint *p) {
  bint m = bcreate();
  if (cmp(p1x, &zero0) == 0 && cmp(p1y, &zero0) == 0) { // if point1 == 0, return point2
    BCPY(*(bint*)rx, *(bint*)p2x);
    BCPY(*(bint*)ry, *(bint*)p2y);
    return;
  } else if (cmp(p2x, &zero0) == 0 && cmp(p2y, &zero0) == 0) { // if point2 == 0, return point1
    BCPY(*(bint*)rx, *(bint*)p1x);
    BCPY(*(bint*)ry, *(bint*)p1y);
    return;
  } else if (cmp(p1x, p2x) == 0 && cmp(p1y, p2y) != 0) { // if x1 == x2 and y1 != y2, return 0, 0
    BCPY(*(bint*)rx, zero0);
    BCPY(*(bint*)ry, zero0);
    return;
  } else if (cmp(p1x, p2x) == 0) { // if x1 == x2, m = (3 * x1 * x2 + curve.a) * inverse_mod(2 * y1, curve.p)
    bint xx = bcreate(), xxx = bcreate(), xxx2 = bcreate(), yy = bcreate(), iy = bcreate();
    badd(&xx, p1x, p1x);
    badd(&xxx, &xx, p1x);
    bmul(&xxx2, &xxx, p2x);
    badd(&yy, p1y, p1y);
    inverse_mod(&iy, &yy, p);
    bmul(&m, &iy, &xxx2);
  } else { // m = (y1 - y2) * inverse_mod(x1 - x2, curve.p)
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
  bmod(ry, &tmp, &y3, p); // result = (x3 % curve.p, -y3 % curve.p)
}

static inline void point_mul(bint *rx, bint *ry, const bint *p1x, const bint *p1y, const bint *p0, const bint *p) {
  bint r0x = bcreate(), r0y = bcreate(), r1x = bcreate(), r1y = bcreate(), k = bcreate(), di = bcreate(), tmp = bcreate();
  bint dii = bcreate(), diim = bcreate(), kt = bcreate();
  BCPY(r1x, *(bint*)p1x);
  BCPY(r1y, *(bint*)p1y);
  BCPY(k, *(bint*)p0);
  for (int i = bbitlen(p0) - 1; i >= 0; i--) {
    brshift(&kt, &k, i);
    bmod(&di, &tmp, &kt, &two2);
    badd(&dii, &di, &one1);
    bmod(&diim, &tmp, &dii, &two2);
    if (diim.wrd[0]) point_add(&r1x, &r1y, &r0x, &r0y, &r1x, &r1y, (bint*)p);
    else point_add(&r0x, &r0y, &r0x, &r0y, &r1x, &r1y, (bint*)p);
    if (di.wrd[0]) point_add(&r1x, &r1y, &r1x, &r1y, &r1x, &r1y, (bint*)p);
    else point_add(&r0x, &r0y, &r0x, &r0y, &r0x, &r0y, (bint*)p);
  }
  BCPY(*(bint*)rx, r0x);
  BCPY(*(bint*)ry, r0y);
}

static inline void scalar_mul(bint *rx, bint *ry, const bint *k, const bint *p1x, const bint *p1y, const bint *p, const bint *n) {
  bint rsx = bcreate(), rsy = bcreate(), ax = bcreate(), ay = bcreate(), q = bcreate(), kt = bcreate(), zx = bcreate();
  bint tmp = bcreate(), tmp1 = bcreate(), tmp2 = bcreate();
  BCPY(kt, *(bint*)k);
  bmod(&q, &tmp, &kt, n);
  if ((q.wrd[0] == 0 && q.siz == 1) || (p1x->wrd[0] == 0 && p1x->siz == 1) || (p1y->wrd[0] == 0 && p1y->siz == 1)) {
    BCPY(*(bint*)rx, zx);
    BCPY(*(bint*)ry, zx);
    return;
  } else if (kt.wrd[0] == 0 && kt.siz == 1) {
    kt.neg ^= 1;
    scalar_mul(rx, ry, &kt, p1x, p1y, p, n);
    return;
  }
  BCPY(ax, *(bint*)p1x);
  BCPY(ay, *(bint*)p1y);
  while(cmp(&kt, &zero0) != 0) {
    bmod(&tmp1, &tmp2, &kt, &two2); // k & 1 is the same as k % 2
    if (tmp1.wrd[0] == 1 && tmp.siz == 1) point_add(&rsx, &rsy, &rsx, &rsy, &ax, &ay, (bint*)p); // add to result
    point_add(&ax, &ay, &ax, &ay, &ax, &ay, (bint*)p); // double
    brshift(&kt, &kt, 1);
  }
  BCPY(*(bint*)rx, rsx);
  BCPY(*(bint*)ry, rsy);
}

void sign(bint *sigx, bint *sigy, bint *pri, char *msg) {
  bint Vx = bcreate(), Vy = bcreate(), c = bcreate(), mi = bcreate(), hash1 = bcreate(), h = bcreate(), mc = bcreate(), d = bcreate(), hc = bcreate();
  bint u = bcreate(), tmp = bcreate();
  uint8_t in1[1024] = {0}, out1[512] = {0};
  memcpy(in1, msg, strlen(msg) * sizeof(uint8_t));
  hash_shake_new(out1, 64, in1, strlen(msg));
  memcpy(hash1.wrd, out1, 64 * sizeof(uint8_t));
  bintrnd_array(&u, 8);
  while (true) {
    point_mul(&Vx, &Vy, curveX, curveY, &u, curveP);
    bmod(&c, &tmp, &Vx, curveN);
    if (cmp(&c, &zero0) == 0) continue;
    inverse_mod(&mi, &u, curveN);
    bmul(&hc, pri, &c); // pri * c (pri == secret key)
    badd(&h, &hash1, &hc); // hash_msg + pri * c
    bmul(&mc, &mi, &h);
    bmod(&d, &tmp, &mc, curveN);
    if (cmp(&d, &zero0) == 0) continue;
    break;
  }
  BCPY(*(bint*)sigx, c);
  BCPY(*(bint*)sigy, d);
}

int16_t verify(bint *pubx, bint *puby, char *msg, bint *sigx, bint *sigy) {
  bint hash1 = bcreate(), tmp = bcreate(), h = bcreate(), h1 = bcreate(), h2 = bcreate(), h1x = bcreate(), h1y = bcreate(), h2x = bcreate(), h2y = bcreate();
  bint Px = bcreate(), Py = bcreate(), c1 = bcreate(), tmp2 = bcreate();
  uint8_t in1[1024] = {0}, out1[512] = {0};
  memcpy(in1, msg, strlen(msg) * sizeof(uint8_t));
  hash_shake_new(out1, 64, in1, strlen(msg));
  memcpy(hash1.wrd, out1, 64 * sizeof(uint8_t));
  inverse_mod(&h, sigy, curveN);
  bmul(&tmp, &hash1, &h);
  bmod(&h1, &tmp2, &tmp, curveN);
  bmul(&tmp, sigx, &h);
  bmod(&h2, &tmp2, &tmp, curveN);
  scalar_mul(&h1x, &h1y, &h1, curveX, curveY, curveP, curveN);
  scalar_mul(&h2x, &h2y, &h2, pubx, puby, curveP, curveN);
  point_add(&Px, &Py, &h1x, &h1y, &h2x, &h2y, curveP);
  bmod(&c1, &tmp2, &Px, curveN);
  return cmp(&c1, sigx) == 0; // c1 == c?
}

void genkeypair(bint *pubx, bint *puby, bint *sec) {
  bintrnd_array(sec, 8); // private key
  scalar_mul(pubx, puby, sec, curveX, curveY, curveP, curveN); // public key
}

void gensharedsecret(bint *shrx, bint *shry, bint *sec, bint *pubx, bint *puby) {
  scalar_mul(shrx, shry, sec, pubx, puby, curveP, curveN);
}

void verifysharedsecret(bint *alshrx, bint *alshry, bint *boshrx, bint *boshry, bint *alsk, bint *bosk) {
  bint tmp1 = bcreate(), tmp2 = bcreate(), res1 = bcreate(), r1 = bcreate(), r2 = bcreate();
  assert(cmp(alshrx, boshrx) == 0 && cmp(alshry, boshry) == 0); // assert alices shared secret is the same as bobs shared secret
  bmul(&res1, alsk, bosk); // alice's and bob's secret
  bmod(&tmp1, &tmp2, &res1, curveN);
  scalar_mul(&r1, &r2, &tmp1, curveX, curveY, curveP, curveN); // scale with curve G
  assert(cmp(&r1, alshrx) == 0); // assert alices shared x is same as ((alicesecret * bobsecret) % N) scalar mult by curve G
}
