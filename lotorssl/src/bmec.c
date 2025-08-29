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

// Static variables
bint zero0 = {.wrd[0] = 0, .siz = 1, .neg = 0, .cap = 1}, one1 = {.wrd[0] = 1, .siz = 1, .neg = 0, .cap = 1};
bint two2 = {.wrd[0] = 2, .siz = 1, .neg = 0, .cap = 1}, ctmpA, ctmpB, ctmpH, ctmpN, ctmpP, ctmpX, ctmpY;

//
// Securely randomize arrays
static inline void bintrnd_array(bint *r, int len) {
  FILE *f = fopen("/dev/urandom", "r");
  fread(r->wrd, sizeof(uint32_t), len, f);
  fclose(f);
}

static inline bint *inverse_mod(bint *ret, const bint *k, const bint *p) {
  bint s = bcreate(), ols = bcreate(), t = bcreate(), olt = bcreate(), r = bcreate(), olr = bcreate();
  int16_t kz = (k->wrd[0] != 0) | (k->siz != 1);
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
  B2CPY(r, olr, *(bint*)p, *(bint*)k) // s = 0, old_s = 1, t = 1, old_t = 0, r = p, old_r = k
  bint tmp = bcreate(), q = bcreate(), qr = bcreate(), qs = bcreate(), qt = bcreate(), rr1 = bcreate(), ss1 = bcreate(), tt1 = bcreate(), kx = bcreate();
  while (cmp(&r, &zero0) > 0) {
    bdiv(&q, &tmp, &olr, &r);       // q = old_r // r
    GCDMR(&rr1, &qr, &q, &r, &olr); // qr = quot * r, r = old_r - (quot * r)
    GCDMR(&ss1, &qs, &q, &s, &ols); // qs = quot * s, s = old_s - (quot * s)
    GCDMR(&tt1, &qt, &q, &t, &olt); // qt = quot * t, t = old_t - (quot * t)

    B2CPY(olr, r, r, rr1);          // old_r, r = r, old_r - (quot * r)
    B2CPY(ols, s, s, ss1);          // old_s, s = s, old_s - (quot * s)
    B2CPY(olt, t, t, tt1);          // old_t, t = t, old_t - (quot * t)
  }                                 // gcd, x, y = old_r, old_s, old_t
  assert(olr.wrd[0] == 1 && olr.siz == 1);
  MULMOD(&kx, &tmp, k, &ols, ret, p); //bmul(&kx, k, &ols); bmod(ret, &tmp, &kx, p);
  assert(ret->wrd[0] == 1 && ret->siz == 1);
  bmod(ret, &tmp, &ols, p);
  return ret; // return x % p
}

static inline void point_add(bint *rx, bint *ry, bint *p1x, bint *p1y, bint *p2x, bint *p2y, bint *p) {
  bint m = bcreate();
  if (p1x->wrd[0] == 0 && p1x->siz == 1 && p1y->wrd[0] == 0 && p1y->siz == 1) { // if point1 == 0, return point2
    B2CPY(*(bint*)rx, *(bint*)ry, *(bint*)p2x, *(bint*)p2y);
    return;
  } else if (p2x->wrd[0] == 0 && p2x->siz == 1 && p2y->wrd[0] == 0 && p2y->siz == 1) { // if point2 == 0, return point1
    B2CPY(*(bint*)rx, *(bint*)ry, *(bint*)p1x, *(bint*)p1y);
    return;
  }
  if (memcmp(p1x, p2x, sizeof(bint)) == 0 && memcmp(p1y, p2y, sizeof(bint)) != 0) { // if x1 == x2 and y1 != y2, return 0, 0
    B2CPY(*(bint*)rx, *(bint*)ry, zero0, zero0);
    return;
  } else if (memcmp(p1x, p2x, sizeof(bint)) == 0) { // if x1 == x2, m = (3 * x1 * x2 + curve.a) * inverse_mod(2 * y1, curve.p)
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
  B2CPY(r1x, r1y, *(bint*)p1x, *(bint*)p1y);
  BCPY(k, *(bint*)p0);
  int16_t bl = bbitlen(p0);
  for (int i = bl - 1; i > bl - (bl % 4) - 1; i--) {
    brshift(&kt, &k, i);
    MAM(&di, &dii, &diim, &tmp, &kt, &one1, &two2);
    PA(diim.wrd[0], di.wrd[0], &r0x, &r0y, &r1x, &r1y, (bint*)p);
  }
  for (int i = bl - (bl % 4) - 1; i >= 0; i-=4) {
    brshift(&kt, &k, i);
    MAM(&di, &dii, &diim, &tmp, &kt, &one1, &two2);
    PA(diim.wrd[0], di.wrd[0], &r0x, &r0y, &r1x, &r1y, (bint*)p);

    brshift(&kt, &k, i-1);
    MAM(&di, &dii, &diim, &tmp, &kt, &one1, &two2);
    PA(diim.wrd[0], di.wrd[0], &r0x, &r0y, &r1x, &r1y, (bint*)p);

    brshift(&kt, &k, i-2);
    MAM(&di, &dii, &diim, &tmp, &kt, &one1, &two2);
    PA(diim.wrd[0], di.wrd[0], &r0x, &r0y, &r1x, &r1y, (bint*)p);

    brshift(&kt, &k, i-3);
    MAM(&di, &dii, &diim, &tmp, &kt, &one1, &two2);
    PA(diim.wrd[0], di.wrd[0], &r0x, &r0y, &r1x, &r1y, (bint*)p);
  }
  B2CPY(*(bint*)rx, *(bint*)ry, r0x, r0y);
}

// k & 1 is the same as k % 2. If (k & 1) add to result, otherwise double
static inline void scalar_mul(bint *rx, bint *ry, const bint *k, const bint *p1x, const bint *p1y, const bint *p, const bint *n) {
  bint rsx = bcreate(), rsy = bcreate(), ax = bcreate(), ay = bcreate(), q = bcreate(), kt = bcreate(), zx = bcreate();
  bint tmp = bcreate(), tmp1 = bcreate();
  BCPY(kt, *(bint*)k);
  bmod(&q, &tmp, &kt, n);
  if ((q.wrd[0] == 0 && q.siz == 1) || (p1x->wrd[0] == 0 && p1x->siz == 1) || (p1y->wrd[0] == 0 && p1y->siz == 1)) {
    B2CPY(*(bint*)rx, *(bint*)ry, zx, zx);
    return;
  } else if (kt.wrd[0] == 0 && kt.siz == 1) {
    kt.neg ^= 1;
    scalar_mul(rx, ry, &kt, p1x, p1y, p, n);
    return;
  }
  B2CPY(ax, ay, *(bint*)p1x, *(bint*)p1y);
  for (int i = bbitlen(k) - 1; i >= 0; i--) {
    MPAPA(&tmp1, &tmp1, &kt, &two2, &rsx, &rsy, &ax, &ay, (bint*)p, tmp1.wrd[0], tmp.siz);
    brshift1(&kt, &kt);
  }
  B2CPY(*(bint*)rx, *(bint*)ry, rsx, rsy);
}

void sign(bint *sigx, bint *sigy, bint *pri, char *msg) {
  bint Vx = bcreate(), Vy = bcreate(), c = bcreate(), mi = bcreate(), hash1 = bcreate(), h = bcreate(), mc = bcreate(), d = bcreate(), hc = bcreate();
  bint u = bcreate(), tmp = bcreate();
  hash_shake_new((uint8_t*)hash1.wrd, 64, (uint8_t*)msg, strlen(msg));
  bintrnd_array(&u, 8);
  while (true) {
    point_mul(&Vx, &Vy, curveX, curveY, &u, curveP);
    bmod(&c, &tmp, &Vx, curveN);
    if (memcmp(&c, &zero0, sizeof(bint)) == 0) continue;
    inverse_mod(&mi, &u, curveN);
    bmul(&hc, pri, &c); // pri * c (pri == secret key)
    badd(&h, &hash1, &hc); // hash_msg + pri * c
    MULMOD(&mc, &tmp, &mi, &h, &d, curveN); // bmul(&mc, &mi, &h); bmod(&d, &tmp, &mc, curveN);
    if (memcmp(&d, &zero0, sizeof(bint)) == 0) continue;
    break;
  }
  B2CPY(*(bint*)sigx, *(bint*)sigy, c, d);
}

int16_t verify(bint *pubx, bint *puby, char *msg, bint *sigx, bint *sigy) {
  bint hash1 = bcreate(), tmp = bcreate(), h = bcreate(), h1 = bcreate(), h2 = bcreate(), h1x = bcreate(), h1y = bcreate(), h2x = bcreate(), h2y = bcreate();
  bint Px = bcreate(), Py = bcreate(), c1 = bcreate(), tmp2 = bcreate();
  hash_shake_new((uint8_t*)hash1.wrd, 64, (uint8_t*)msg, strlen(msg));
  inverse_mod(&h, sigy, curveN);
  MULMOD(&tmp, &tmp2, &hash1, &h, &h1, curveN); // bmul(&tmp, &hash1, &h); bmod(&h1, &tmp2, &tmp, curveN);
  MULMOD(&tmp, &tmp2, sigx, &h, &h2, curveN); // bmul(&tmp, sigx, &h); bmod(&h2, &tmp2, &tmp, curveN);
  SMSM(&h1x, &h1y, &h2x, &h2y, &h1, &h2, curveX, curveY, pubx, puby, curveP, curveN); // scalar_mul(&h1x, &h1y, &h1, curveX, curveY, curveP, curveN); scalar_mul(&h2x, &h2y, &h2, pubx, puby, curveP, curveN);
  point_add(&Px, &Py, &h1x, &h1y, &h2x, &h2y, curveP);
  bmod(&c1, &tmp2, &Px, curveN);
  return memcmp(&c1, sigx, sizeof(bint)) == 0; // c1 == c?
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
  assert(memcmp(alshrx, boshrx, sizeof(bint)) == 0 && memcmp(alshry, boshry, sizeof(bint)) == 0); // assert alices shared secret is the same as bobs shared secret
  bmul(&res1, alsk, bosk); // alice's and bob's secret
  bmod(&tmp1, &tmp2, &res1, curveN);
  scalar_mul(&r1, &r2, &tmp1, curveX, curveY, curveP, curveN); // scale with curve G
  assert(memcmp(&r1, alshrx, sizeof(bint)) == 0);
}

// ECDSA
// https://en.wikipedia.org/wiki/Elliptic_Curve_Digital_Signature_Algorithm
// https://www.rfc-editor.org/rfc/rfc6979
// https://www.rfc-editor.org/rfc/rfc4050

// http://www.secg.org/sec2-v2.pdf
// http://csrc.nist.gov/publications/fips/fips186-3/fips_186-3.pdf
// https://www.ietf.org/rfc/rfc4492.txt

// https://csrc.nist.gov/csrc/media/publications/fips/186/3/archive/2009-06-25/documents/fips_186-3.pdf
// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.186-4.pdf
// https://www.ietf.org/rfc/rfc4492.txt

