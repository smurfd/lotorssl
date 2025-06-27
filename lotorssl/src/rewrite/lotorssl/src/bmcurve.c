#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "bmcurve.h"

// ----- Field class -----
void field_init(field *f, bint *x, bint *p) {
  bint tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  memcpy(f->p, p, sizeof(bint));
  bmod(f->x, &tmp, x, p);
  f->x->siz = x->siz;
  f->p->siz = p->siz;
}

void field_initint(field *f, uint32_t x, bint *p) {
  bint tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, tmp2 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  memcpy(f->p, p, sizeof(bint));
  bmod(f->x, &tmp, wrd2bint(&tmp2, x), p);
  f->x->siz = 1;
  f->p->siz = p->siz;
}

int8_t field_eq(field *a, field *b) {
  bprint("P1", a->p);
  bprint("P2", b->p);
  assert(cmp(a->p, b->p) == 0);
  return cmp(a->x, b->x);
}

void field_add(field *ret, field *a, field *b) {
  bint tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, tmp2 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  assert(cmp(a->p, b->p) == 0);
  tmp.siz = b->x->siz;
  badd(&tmp, b->x, a->x);
  if (cmp(b->p, wrd2bint(&tmp2, 0)) == 0) {
    memcpy(ret->x->wrd, tmp.wrd, tmp.siz * sizeof(uint32_t));
  } else bmod(ret->x, &tmp2, &tmp, b->p); // (self.x + data.x) % self.p
  ret->x->siz = a->x->siz;// > b->x->siz ? a->x->siz : b->x->siz;
  ret->p->siz = a->p->siz;// > b->p->siz ? a->p->siz : b->p->siz;
}

void field_sub(field *ret, const field *a, const field *b) {
  bint tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, tmp2 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  field aa, bb;
  memcpy(&aa, a, sizeof(field));
  memcpy(&bb, b, sizeof(field));
  assert(cmp(aa.p, bb.p) == 0);
  if (cmp(aa.x, bb.x) < 0) {
    memcpy(aa.x->wrd, bb.p->wrd, 8 * sizeof(uint32_t));
    aa.x->siz = 8;
    bsub(&tmp, aa.x, bb.x);
    tmp.siz = aa.x->siz;
    bmod(ret->x, &tmp2, &tmp, bb.p);
    ret->x->siz = aa.x->siz;
    return;
  }
  bb.x->neg = 1;
  aa.x->neg = 0;
  bsub(&tmp, aa.x, bb.x);//b->x, a->x);
  bb.x->neg = 0;
  bmod(ret->x, &tmp2, &tmp, bb.p); // (self.x - data.x) % self.p
  ret->x->siz = aa.x->siz;
  ret->p->siz = aa.p->siz;
}

void field_mul(field *ret, field *a, field *b) {
  bint tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, tmp2 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bprint("AP", a->p);
  bprint("BP", b->p);
  assert(cmp(a->p, b->p) == 0);;
  bmul(&tmp, b->x, a->x);
  bmod(ret->x, &tmp2, &tmp, b->p); // (self.x * data.x) % self.p
  ret->x->siz = a->x->siz;
  ret->p->siz = a->p->siz;
}

void field_div(field *ret, field *a, field *b) {
  // TODO: field div does not work! because (self * data ** -1 == self * 1/(data*data)) and bdiv can't handle that.
  // TODO: look into GCD
  bint tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bdiv(ret->x, &tmp, a->x, b->x);
  ret->x->siz = a->x->siz;
  ret->p->siz = a->p->siz;
}

void field_neg(field *ret, field *a, field *b) {
  bsub(ret->x, b->p, b->x); // self.p - self.x
  ret->x->siz = a->x->siz;
  ret->p->siz = a->p->siz;
}
//  def __truediv__(self, data): return self * data ** -1
//  def __pow__(self, data): return self.__class__(pow(self.x, data, self.p))


// ----- Order class -----
void order_init(order *o, bint *x, bint *p1) {
  field_initint(&o->f, 0, p1);
  memcpy(o->f.x, x, sizeof(bint));
  str2bint(o->N, "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"); // TODO: as arg?
  o->f.x->siz = x->siz;
  o->f.p->siz = p1->siz;
}

void order_initint(order *o, uint32_t x, bint *p1) {
  bint tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  field_initint(&o->f, 0, p1);
  memcpy(o->f.x, wrd2bint(&tmp, x), sizeof(bint));
  str2bint(o->N, "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"); // TODO: as arg?
  o->f.x->siz = 1;
  o->f.p->siz = p1->siz;
}


// ----- Prime class -----
void prime_init(prime *p, bint *x, bint *p1) {
  field_initint(&p->f, 0, p1);
  memcpy(p->f.x, x, sizeof(bint));
  memcpy(p->f.p, p1, sizeof(bint));
  memcpy(p->P, p1, sizeof(bint));
  p->f.x->siz = x->siz;
  p->f.x->neg = x->neg;
  p->f.x->cap = x->cap;
}

void prime_initint(prime *p, uint32_t x, bint *p1) {
  bint tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  field_initint(&p->f, 0, p1);
  memcpy(p->f.x, wrd2bint(&tmp, x), sizeof(bint));
  memcpy(p->f.p, p1, sizeof(bint));
  memcpy(p->P, p1, sizeof(bint));
  p->f.x->siz = 1;
  p->f.x->neg = 0;
  p->f.x->cap = 0;
}

#define SWAP(a, b) {bint tmp = a; a = b; b = tmp;}
bint *gc(bint *ret, bint *k, const bint *p) {
  bint s = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, os = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, t = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint ot = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, r = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, or = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint quot = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, P = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint qr = {.wrd = {0}, .siz = 0, .cap = 0, .neg = 0}, oqr = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, rtmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint aa  = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, bb = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, cc = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint qs = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, qt = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, oqs = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint oqt = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, zero = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  wrd2bint(ret, 0);
  wrd2bint(&os, 1);
  wrd2bint(&t, 1);
  str2bint(&P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  bprint("K", k);
  printf("P %d %d %d\n", p->neg, p->cap, p->siz);
  printf("K %d %d %d\n", k->neg, k->cap, k->siz);
  memset(&r.wrd, 0, LEN * sizeof(uint32_t));
  memset(&or.wrd, 0, LEN * sizeof(uint32_t));
  memcpy(&r.wrd, p->wrd, p->siz * sizeof(uint32_t));
  memcpy(&or.wrd, k->wrd, k->siz * sizeof(uint32_t));
  or.cap = k->cap;
  or.neg = k->neg;
  or.siz = k->siz;
  r.cap = p->cap;
  r.neg = p->neg;
  r.siz = p->siz;
  if (p->siz == 0) r.siz = 1;
  printf("rrrr %d\n", r.siz);
//  if (r.siz == 0) r.siz = 1;
  bprint("R1", &r);
  bprint("OR1", &or);
  //str2bint(&or, "0x9075b4ee4d4788cabb49f7f81c221151fa2f68914d0aa833388fa11ff621a970");
  int16_t kz = cmp(k, &zero);
  printf("KZ %d\n", kz);
  if (kz == 0) {
    printf("Divide by zero, not good\n");
    return NULL; // should never happen
  } else if (kz < 0) {
    k->neg = 1;
    printf("Kneg\n");
    gc(ret, k, p);
    bsub(ret, p, ret);
    return ret; // p - inverse_mod(-k, p)
  }
  qr.siz = 1;
  while (cmp(&r, &zero) != 0 && r.siz != 0) {
    bdiv(&quot, &tmp, &or, &r); // q = or / r
    bmul(&qr, &quot, &r); // (q * r)
    bmul(&qs, &quot, &s); // (q * s)
    bmul(&qt, &quot, &t); // (q * t)
    bsub(&oqr, &or, &qr); // or = or - (q * r)
    bsub(&oqs, &os, &qs); // os = os - (q * s)
    bsub(&oqt, &ot, &qt); // ot = ot - (q * t)
    bprint("quot", &quot);
    bprint("R2", &r);
    bprint("OR2", &or);
    bprint("QR2", &qr);
    bprint("OQR2", &oqr);
    SWAP(r, or); // or = r
    SWAP(s, os); // os = s
    SWAP(t, ot); // ot = t
    bprint("R3", &r);
    bprint("OR3", &or);
    bprint("QR3", &qr);
    bprint("OQR3", &oqr);
    SWAP(r, oqr); // r = or
    SWAP(s, oqs); // s = os
    SWAP(t, oqt); // t = ot
    printf("siz %d %d %d: %d %d\n", qr.siz, or.siz, oqr.siz, quot.siz, r.siz);
    bprint("R4", &r);
    bprint("OR4", &or);
    bprint("QR4", &qr);
    bprint("OQR4", &oqr);
  }
  bprint("OR", &or);
  int16_t c1 = cmp(&or, wrd2bint(&tmp, 1)), c2 = cmp(&or, p);
  printf("    %d   : %d %d\n", c1 & c2, c1, c2);
  assert((c1 & c2) == 0); // either g == 1 or g == P TODO: hack for now, should just be g == 1
  //assert(cmp(&or, wrd2bint(&tmp, 1)) == 0 || cmp(&or, p1) == 0); // assert g == 1 // g = or
  bmul(&aa, &os, k); // x = os, (x * k)
  bmod(&cc, &bb, &aa, p); // (x * k) % P
  if (cc.neg && cmp(&cc, wrd2bint(&rtmp, 1)) != 0) badd(&cc, p, &cc); // TODO: hack?!
  bprint("CC2", &cc);
  bprint("P", &P);
  assert(cmp(&cc, wrd2bint(&rtmp, 1)) == 0); // assert (k * x) % p == 1
  bmod(ret, &tmp, &os, p);
  return ret; // return x % p
}

// ----- Point class -----
void point_init(point *p, prime *a, prime *b, bint *p1) {
  bint x2 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, x3 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, x37 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint x3m = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, xx = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, y2 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint yy = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, B = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  prime p0a, p0b;
  prime_initint(&p0a, 0, p1);
  prime_initint(&p0b, 0, p1);
  p0a.f.x->siz = a->f.x->siz;
  p0b.f.x->siz = b->f.x->siz;
  if (cmp(a->f.x, p0a.f.x) != 0 || cmp(b->f.x, p0b.f.x) != 0) {
    prime_initint(&p->prime_x, 0, p1);
    prime_initint(&p->prime_y, 0, p1);
    memset(&y2.wrd, 0, LEN * sizeof(uint32_t));
    memset(&x2.wrd, 0, LEN * sizeof(uint32_t));
    memset(&x3.wrd, 0, LEN * sizeof(uint32_t));
    memset(&x37.wrd, 0, LEN * sizeof(uint32_t));
    memset(&x3m.wrd, 0, LEN * sizeof(uint32_t));
    wrd2bint(&B, 7);
    bmul(&y2, b->f.x, b->f.x); // y * y
    bmul(&x2, a->f.x, a->f.x); // x * x
    bmul(&x3, &x2, a->f.x); // x * x *x
    badd(&x37, &x3, &B); // x * x * x + B
    bmod(&xx, &tmp, &x37, p1);
    bmod(&yy, &tmp, &y2, p1);
    assert(cmp(&xx, &yy) == 0); // y ** 2 == x ** 3 + (A * x) + B // A = 0, B = 7, (A * x) == 0
  }
  prime_init(&p->prime_x, a->f.x, p1);
  prime_init(&p->prime_y, b->f.x, p1);
  p->prime_x.f.x->siz = a->f.x->siz;
  p->prime_y.f.x->siz = b->f.x->siz;
}

void point_add(point *ret, point *a, point *b, bint *p1) {
  prime x1, x2, y1, y2, p0, m;
  prime_init(&x2, b->prime_x.f.x, p1);
  prime_init(&x1, a->prime_x.f.x, p1);
  prime_init(&y2, b->prime_y.f.x, p1);
  prime_init(&y1, a->prime_y.f.x, p1);
  prime_initint(&p0, 0, p1);
  prime_initint(&m, 0, p1);
  if (field_eq(&x1.f, &p0.f) == 0 && field_eq(&y1.f, &p0.f) == 0) { // if point1 is None, return point2
    memcpy(ret->prime_x.f.x->wrd, b->prime_x.f.x->wrd, b->prime_x.f.x->siz * sizeof(uint32_t));
    memcpy(ret->prime_y.f.x->wrd, b->prime_y.f.x->wrd, b->prime_y.f.x->siz * sizeof(uint32_t));
    return;
  }
  if (field_eq(&x2.f, &p0.f) == 0 && field_eq(&y2.f, &p0.f) == 0) { // if point2 is None, return point1
    memcpy(ret->prime_x.f.x->wrd, a->prime_x.f.x->wrd, a->prime_x.f.x->siz * sizeof(uint32_t));
    memcpy(ret->prime_y.f.x->wrd, a->prime_y.f.x->wrd, a->prime_y.f.x->siz * sizeof(uint32_t));
    return;
  }
  if (field_eq(&x1.f, &x2.f) == 0 && field_eq(&y1.f, &y2.f) != 0) { // if x1 == x2 and y1 != y2: return None
    memcpy(ret->prime_x.f.x->wrd, 0, LEN * sizeof(uint32_t));
    memcpy(ret->prime_y.f.x->wrd, 0, LEN * sizeof(uint32_t));
    return;
  }
  if (field_eq(&x1.f, &x2.f) == 0) { // if x1 == x2: m = (3 * x1 * x2 + curve.a) * inverse_mod(2 * y1, curve.p)
    prime xx1, xxx1, xxx1x2, yy1, gg;
    prime_initint(&xx1, 0, p1);
    prime_initint(&xxx1, 0, p1);
    prime_initint(&xxx1x2, 0, p1);

    field_add(&xx1.f, &x1.f, &x1.f); // x1 + x1
    field_add(&xxx1.f, &xx1.f, &x1.f); // x1 + x1 + x1
    field_mul(&xxx1x2.f, &xxx1.f, &x2.f); // (x1 + x1 + x1) * x2 + A, (A = 0, so ignore for now)

    field_add(&yy1.f, &y1.f, &y1.f);
    bint g = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, p = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
    str2bint(&p, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
    gc(&g, yy1.f.x, p1);
    prime_init(&gg, &g, p1);
    field_mul(&m.f, &gg.f, &xxx1x2.f);
  } else { // m = (y1 - y2) * inverse_mod(x1 - x2, curve.p)
    bint g = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, p = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
    prime y1y2, x1x2;
    field gg;
    str2bint(&p, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
    prime_initint(&y1y2, 0, p1);
    prime_initint(&x1x2, 0, p1);
    //SWAP(*(bint*)x1.f.x, *(bint*)x2.f.x);
    field_sub(&y1y2.f, &y1.f, &y2.f);
    field_sub(&x1x2.f, &x1.f, &x2.f);
    bprint("x1x2", x1x2.f.x);
    printf("gc2 start -----\n");
    gc(&g, x1x2.f.x, p1);
    printf("------- gc2 end\n");
    field_init(&gg, &g, p1);//&p);
    field_mul(&m.f, &gg, &y1y2.f);
  }
  prime sk, x3, y3, x21, y21;
  prime_initint(&sk, 0, p1);
  prime_initint(&x3, 0, p1);
  prime_initint(&y3, 0, p1);
  prime_initint(&x21, 0, p1);
  prime_initint(&y21, 0, p1);
  field_sub(&y21.f, &y2.f, &y1.f);
  field_sub(&x21.f, &x2.f, &x1.f);

  field_mul(&x3.f, &m.f, &m.f); // m * m
  field_sub(&x3.f, &x3.f, &x1.f); // m * m - x1
  field_sub(&x3.f, &x3.f, &x2.f); // x3 = m * m - x1 - x2

  field_sub(&y3.f, &x3.f, &x1.f); // x3 - x1
  field_mul(&y3.f, &m.f, &y3.f); // (x3 - x1) * m
  field_add(&y3.f, &y3.f, &y1.f); // y3 = y1 + m * (x3 - x1)
  bprint("XXX", x3.f.x);
  bprint("YYY", y3.f.x);
  memcpy(ret->prime_x.f.x->wrd, x3.f.x->wrd, x3.f.x->siz * sizeof(uint32_t));
  memcpy(ret->prime_y.f.x->wrd, y3.f.x->wrd, y3.f.x->siz * sizeof(uint32_t));
  // x3 = m * m - x1 - x2
  // y3 = y1 + m * (x3 - x1)
  // return (x3 % curve.p, -y3 % curve.p)
  return; // return point(x3, y3);
}

void point_mul(point *ret, point *a, field *k, bint *p1) { // TODO: does this work?!
  bint b = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, n = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, o = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint t = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, z = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  prime pz; // Point multiplication: Double-and-add https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication
  point result, addend, aa;
  wrd2bint(&z, 0);
  wrd2bint(&o, 1);
  wrd2bint(&t, 2);
  prime_initint(&pz, 0, p1);
  point_init(&result, &pz, &pz, p1);
  point_init(&aa, &pz, &pz, p1);
  point_init(&addend, &a->prime_x, &a->prime_y, p1);
  memset(n.wrd, 0, LEN * sizeof(uint32_t));
  memcpy(n.wrd, k->x->wrd, k->x->siz * sizeof(uint32_t));
  n.neg = k->x->neg;
  n.cap = k->x->cap;
  n.siz = k->x->siz;
  printf("MULK %d\n", k->x->neg);
  bprint("N", &n);
  bprint("Z", &z);
  while (cmp(&n, &z) == 1 && n.siz > 0) {
    bint tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
    bmod(&b, &tmp, &n, &t);
    bprint("TMP", &tmp);
    bprint("TMP N", &n);
    bprint("TMP T", &t);
    bprint("TMP B", &b);
    bprint("TMP O", &o);
    memcpy(n.wrd, b.wrd, b.siz * sizeof(uint32_t));
    n.siz = b.siz;
    n.cap = b.cap;
    n.neg = b.neg;
    printf("BO %d\n", cmp(&b, &o));
    if (cmp(&b, &o) == 0) {
      printf("PA 1\n");
      point_add(&result, &result, &addend, p1);
    }
    printf("PA 2\n");
    point_add(&addend, &addend, &addend, p1);
    brshift(&n, &n, 1);  
  }
  printf("PA 3\n");
  memcpy(ret->prime_x.f.x->wrd, result.prime_x.f.x->wrd, result.prime_x.f.x->siz * sizeof(uint32_t));
  memcpy(ret->prime_y.f.x->wrd, result.prime_y.f.x->wrd, result.prime_y.f.x->siz * sizeof(uint32_t));
}

//
// Translated to C from https://github.com/mohanson/cryptography-python

