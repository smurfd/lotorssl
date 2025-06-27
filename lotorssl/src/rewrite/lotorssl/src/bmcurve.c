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

// TODO: fuck p=0
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
//  cc.neg = 0;
  bprint("CC1", &cc);
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
    bmul(&y2, b->f.x, b->f.x); // y*y
    bmul(&x2, a->f.x, a->f.x); // x*x
    bmul(&x3, &x2, a->f.x); // x*x*x
    badd(&x37, &x3, &B); // x*x*x + B
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
  if (field_eq(&x1.f, &x2.f) == 0) { // if x1 == x2
    //if x1 == x2: m = (3 * x1 * x2 + curve.a) * inverse_mod(2 * y1, curve.p)
    prime xx1, xxx1, xxx1x2, yy1, gg;
    prime_initint(&xx1, 0, p1);
    prime_initint(&xxx1, 0, p1);
    prime_initint(&xxx1x2, 0, p1);

    field_add(&xx1.f, &x1.f, &x1.f);
    field_add(&xxx1.f, &xx1.f, &x1.f);
    field_mul(&xxx1x2.f, &xxx1.f, &x2.f); // + A = 0, so ignore for now

    field_add(&yy1.f, &y1.f, &y1.f);
    bint g = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, p = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}; //, x37 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
    str2bint(&p, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
    gc(&g, yy1.f.x, p1);//&p);
    prime_init(&gg, &g, p1);//&p);
    field_mul(&m.f, &gg.f, &xxx1x2.f);
  } else {
    bint g = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, p = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}; //, x37 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
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
/*
  if (field_eq(&x1.f, &x2.f) == 0 && field_eq(&y1.f, &y2.f) == 0) { // y2 pos? // 
    prime x1x1, xa, yy1, x3, y3, sk;
    prime_initint(&x1x1, 0, p1);
    prime_initint(&xa, 0, p1);
    prime_initint(&yy1, 0, p1);
    prime_initint(&sk, 0, p1);
    prime_initint(&x3, 0, p1);
    prime_initint(&y3, 0, p1);
    field_mul(&x1x1.f, &x1.f, &x1.f); // x1 * x1
    field_add(&xa.f, &x1x1.f, &x1x1.f); // (x1 * x1) + (x1 * x1)
    field_add(&xa.f, &xa.f, &x1x1.f); // (x1 * x1) + (x1 * x1) + (x1 * x1) (+ 0: c.A = 0)
    field_add(&yy1.f, &y1.f, &y1.f); // y1 + y1
    field gcd;
    field_initint(&gcd, 0, p1);
    field_div(&gcd, &xa.f, &yy1.f);
    //prime_gcd(&gcd, &yy1, p1);
    field_mul(&sk.f, &xa.f, &gcd); // sk = (x1 * x1) + (x1 * x1) + (x1 * x1) * gcd(y1+y1)
    //field_div(&sk.f, &xa.f, &yy1.f); // / (y1 + y1) = sk
    //////////////field *prime_gcd(field *ret, prime *k, bint *p1)

    field_mul(&x3.f, &sk.f, &sk.f); // sk * sk
    field_sub(&x3.f, &x3.f, &x1.f); // - x1
    field_sub(&x3.f, &x3.f, &x2.f); // x3 = sk * sk - x1 - x2

    field_sub(&y3.f, &x1.f, &x3.f); // x1 - x3
    field_mul(&y3.f, &sk.f, &y3.f); // sk * (x1 - x3)
    field_sub(&y3.f, &y3.f, &y1.f); // y3 = sk * (x1 - x3) - y1
    memcpy(ret->prime_x.f.x->wrd, x3.f.x->wrd, x3.f.x->siz * sizeof(uint32_t));
    memcpy(ret->prime_y.f.x->wrd, y3.f.x->wrd, y3.f.x->siz * sizeof(uint32_t));
    return; // return point(x3, y3)
  }
  if (field_eq(&x1.f, &x2.f) == 0 && field_eq(&y1.f, &y2.f) == 0) { // y2 negative?
    memcpy(ret->prime_x.f.x->wrd, p0.f.x->wrd, p0.f.x->siz * sizeof(uint32_t));
    memcpy(ret->prime_y.f.x->wrd, p0.f.x->wrd, p0.f.x->siz * sizeof(uint32_t));
    return; // return point(0, 0)
  }
*/
  bint g = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};//, p = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}; //, x37 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  field gg;
  prime sk, x3, y3, x21, y21;
  prime_initint(&sk, 0, p1);
  prime_initint(&x3, 0, p1);
  prime_initint(&y3, 0, p1);
  prime_initint(&x21, 0, p1);
  prime_initint(&y21, 0, p1);
  field_sub(&y21.f, &y2.f, &y1.f);
  field_sub(&x21.f, &x2.f, &x1.f);
  gc(&g, x21.f.x, p1);
  field_init(&gg, &g, p1);
  field_mul(&sk.f, &y21.f, &gg); // sk = (y2 - y1) * (x2 - x1)
  
  field_mul(&x3.f, &sk.f, &sk.f);
  field_sub(&x3.f, &x3.f, &x1.f);
  field_sub(&x3.f, &x3.f, &x2.f); // x3 = sk * sk - x1 - x2
  field_sub(&y3.f, &x1.f, &x3.f);
  field_mul(&y3.f, &sk.f, &x3.f);
  field_sub(&y3.f, &y3.f, &y1.f); // y3 = sk * (x1 - x3) - y1
  memcpy(ret->prime_x.f.x->wrd, x3.f.x->wrd, x3.f.x->siz * sizeof(uint32_t));
  memcpy(ret->prime_y.f.x->wrd, y3.f.x->wrd, y3.f.x->siz * sizeof(uint32_t));
  return; // return point(x3, y3);
}
/*
def point_add(point1, point2):
  assert is_on_curve(point1)
  assert is_on_curve(point2)
  if point1 is None: return point2
  if point2 is None: return point1
  x1, y1 = point1
  x2, y2 = point2
  if x1 == x2 and y1 != y2: return None # point1 +(-point1) == 0
  if x1 == x2: m = (3 * x1 * x2 + curve.a) * inverse_mod(2 * y1, curve.p)
  else: m = (y1 - y2) * inverse_mod(x1 - x2, curve.p)
  x3 = m * m - x2 - x2
  y3 = y1 + m * (x3 - x1)
  result = (x3 % curve.p, -y3 % curve.p)
  assert is_on_curve(result)
  return result
*/


void point_mul(point *ret, point *a, field *k, bint *p1) { // TODO: does this work?!
  bint b = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, n = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, o = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint t = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, z = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  prime pz; // Point multiplication: Double-and-add https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication
  point result, addend, aa;
  wrd2bint(&o, 1);
  wrd2bint(&t, 2);
  prime_initint(&pz, 0, p1);
  point_init(&result, &pz, &pz, p1);
  point_init(&aa, &pz, &pz, p1);
  point_init(&addend, &a->prime_x, &a->prime_y, p1);
//  addend.prime_x.f.x->siz = //8;
//  addend.prime_y.f.x->siz = //8;
  memset(n.wrd, 0, LEN * sizeof(uint32_t));
  memcpy(n.wrd, k->x->wrd, k->x->siz * sizeof(uint32_t));
//  n.siz = 8;
  while (cmp(&n, &z) == 1 && n.siz > 0) {
    bint tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
    bmod(&b, &tmp, &n, &t);
    if (cmp(&b, &o) == 0) {
      printf("PA 1\n");
      point_add(&result, &result, &addend, p1);
    }
    printf("PA 2\n");
    point_add(&addend, &addend, &addend, p1);
    brshift(&n, &n, 1);  
  }
  memcpy(ret->prime_x.f.x->wrd, result.prime_x.f.x->wrd, result.prime_x.f.x->siz * sizeof(uint32_t));
  memcpy(ret->prime_y.f.x->wrd, result.prime_y.f.x->wrd, result.prime_y.f.x->siz * sizeof(uint32_t));
}

void Ftester(void) {
  bint p1 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, p2 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, p3 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint x = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, y = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  prime px, py, pz, po1, po2, po3;
  point G1, G2, G3, infinity;
  field f0, f1, f2, f3;
  order o1, o2, o3;
  wrd2bint(&p1, 23);
  wrd2bint(&p2, 0);
  field_initint(&f0, 0, &p1);
  field_initint(&f1, 12, &p1);
  field_initint(&f2, 20, &p1);
  field_initint(&f3, 9, &p1);
  field_add(&f0, &f1, &f2);
  assert(field_eq(&f0, &f3) == 0); // Field(12) + Field(20) == Field(9)

  field_initint(&f0, 0, &p1);
  field_initint(&f1, 8, &p1);
  field_initint(&f2, 9, &p1);
  field_initint(&f3, 3, &p1);
  field_mul(&f0, &f1, &f2);
  assert(field_eq(&f0, &f3) == 0); // Field(8) * Field(9) == Field(3)

  order_initint(&o1, 42, &p2);
  order_initint(&o2, 24, &p2);
  order_initint(&o3, 66, &p2);

  str2bint(&p3, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  prime_init(&px, str2bint(&x, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"), &p2); // point gx
  prime_init(&py, str2bint(&y, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"), &p2); // point gy
  prime_initint(&pz, 0, &p2);

  prime_init(&po1, o1.f.x, &p2);
  prime_init(&po2, o2.f.x, &p2);
  prime_init(&po3, o3.f.x, &p2);

  point_init(&infinity, &pz, &pz, &p2);
  point_init(&G1, &pz, &pz, &p3);

  memcpy(G1.prime_x.f.x->wrd, px.f.x->wrd, px.f.x->siz * sizeof(uint32_t));
  memcpy(G1.prime_y.f.x->wrd, py.f.x->wrd, py.f.x->siz * sizeof(uint32_t));

  memcpy(G1.prime_x.f.p->wrd, p2.wrd, p2.siz * sizeof(uint32_t));
  memcpy(G1.prime_y.f.p->wrd, p2.wrd, p2.siz * sizeof(uint32_t));
  G1.prime_x.f.x->siz = px.f.x->siz;
  G1.prime_x.f.x->neg = 0;

  G1.prime_y.f.x->siz = py.f.x->siz;
  G1.prime_y.f.x->neg = 0;

  point_init(&G2, &pz, &pz, &p3);
  point_init(&G3, &pz, &pz, &p3);
  point_add(&G3, &G1, &G1, &p3);
  point_mul(&G2, &G1, &o1.f, &p3);
  
  bprint("G3X", G3.prime_x.f.x);
  bprint("G3Y", G3.prime_y.f.x);
  bprint("G2X", G2.prime_x.f.x);
  bprint("G2Y", G2.prime_y.f.x);
}

void Ftester_sanity(void) {
  bint aaa = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, bbb = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, ccc = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint ddd = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, hhh = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, jjj = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint r1 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, r2 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  wrd2bint(&aaa, 666);
  wrd2bint(&bbb, 999);
  wrd2bint(&r1, 333);
  bmod(&ccc, &tmp, &bbb, &aaa);
  assert(cmp(&ccc, &r1) == 0);
  str2bint(&ddd, "0x6666666666666666666666666666666666666666666666666666666666666666");
  str2bint(&jjj, "0x9999999999999999999999999999999999999999999999999999999999999999");
  str2bint(&r2, "0x3333333333333333333333333333333333333333333333333333333333333333");
  bmod(&hhh, &tmp, &jjj, &ddd);
  assert(cmp(&hhh, &r2) == 0);
}

void Ftester_math_sanity(void) {
  field f1, f2, f3, fr;
  bint a = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, b = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, p = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  bint r1 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, r2 = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  str2bint(&a, "0x3b6859c358bb6fa30b3f11ff6c29164dc21b2abaf4c2027ea8e6701e99dd5b7c");
  str2bint(&b, "0xd7dab791a8647ac88695e20e29960a6fd41707258cc88cc0480ea7e5bb3f132f");
  str2bint(&p, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&r1, "0x13431155011fea6b91d4f40d95bf20bd963231e0818a8f3ef0f51805551c727c");
  str2bint(&r2, "0xa0cc141790812f1cc95dd3da756f3d4930f1adbeef8929265e0a74480f6eb392");
  field_initint(&fr, 0, &p);
  field_init(&f1, &a, &p);
  field_init(&f2, &b, &p);
  field_add(&fr, &f1, &f2);
  assert(cmp((bint*)fr.x, &r1) == 0);
  field_mul(&f3, &f1, &f2);
  assert(cmp((bint*)f3.x, &r2) == 0);
}

void Ftester_PK(void) { // test public & private key creation and verification
  bint P = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0}, tmp = {.wrd = {0}, .siz = 0, .neg = 0, .cap = 0};
  prime px, py, pz;
  order priv;
  point publ, G;
  printf("--------- Public key -------------------------------------------\n");
  str2bint(&P, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  order_init(&priv, str2bint(&tmp, "0x5f6717883bef25f45a129c11fcac1567d74bda5a9ad4cbffc8203c0da2a1473c"), &P); // priv key, TODO: randomize 
  prime_initint(&pz, 0, &P);
  prime_init(&px, str2bint(&tmp, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"), &P); // point gx
  prime_init(&py, str2bint(&tmp, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"), &P); // point gy

  point_init(&publ, &pz, &pz, &P);
  point_init(&G, &px, &py, &P);
  point_mul(&publ, &G, &priv.f, &P);
  bprint("GX", G.prime_x.f.x);
  bprint("GY", G.prime_y.f.x);
  bprint("PX", publ.prime_x.f.x);
  bprint("PY", publ.prime_y.f.x);

/*
  Field.p = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f
  printf("--------- Public key -------------------------------------------\n");
  // public & private key
  prikey = Order(0x5f6717883bef25f45a129c11fcac1567d74bda5a9ad4cbffc8203c0da2a1473c) // TODO: randomize
  pubkey = c.G * prikey
  assert pubkey.x.x == 0xfb95541bf75e809625f860758a1bc38ac3c1cf120d899096194b94a5e700e891 // TODO: no assert
  assert pubkey.y.x == 0xc7b6277d32c52266ab94af215556316e31a9acde79a8b39643c6887544fdf58c // TODO: no assert
  printf("--------- Sign & Verify ----------------------------------------\n");
  // sign & verify
  prikey = Order(0x5f6717883bef25f45a129c11fcac1567d74bda5a9ad4cbffc8203c0da2a1473c)
  pubkey = c.G * prikey
  // Hash of messages. Generated by "sha256sum secp256k1.py"
  m = Order(0x72a963cdfb01bc37cd283106875ff1f07f02bc9ad6121b75c3d17629df128d4e)
  printf("hash={m}\n");
  // Sign
  while True:
    k = Order(random.randint(0, c.N - 1))
    R = c.G * k
    r = Order(R.x.x)
    if r.x == 0: continue
    s = (m + prikey * r) / k
    if s.x == 0: continue
    printf("sigr={r}\nsigs={s}\n");
    break
  // Verify
  u1 = m / s
  u2 = r / s
  x = c.G * u1 + pubkey * u2
  assert x != I
  v = Order(x.x.x)
  printf("V={v}\nR={r}\n");
  assert v == r
  printf("--------- Pass -------------------------------------------------\n");
*/

}

//
// Translated to C from https://github.com/mohanson/cryptography-python

