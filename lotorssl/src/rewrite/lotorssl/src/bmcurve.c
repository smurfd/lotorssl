#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "bmcurve.h"

// ----- Field class -----
void field_init(field *f, bint *x, bint *p) {
  memcpy(f->p, p, sizeof(bint));
  bmod(f->x, x, p);
}

void field_initint(field *f, uint32_t x, bint *p) {
  bint x1[1];
  memcpy(f->p, p, sizeof(bint));
  bmod(f->x, wrd2bint(x1, x), p);
}

int8_t field_eq(field *a, field *b) {
  assert(cmp(a->p, b->p) == 0);
  return cmp(a->x, b->x);
}

void field_add(field *ret, field *a, field *b) {
  assert(cmp(a->p, b->p) == 0);
  bint tmp;
  tmp.siz = b->x->siz;
  b->x->neg = 0; //
  a->x->neg = 0; //
  badd(&tmp, b->x, a->x);
  bmod(ret->x, &tmp, b->p); // (self.x + data.x) % self.p
}

void field_sub(field *ret, field *a, field *b) {
  assert(cmp(a->p, b->p) == 0);
  bint tmp;
  bsub(&tmp, b->x, a->x);
  bmod(ret->x, &tmp, b->p); // (self.x - data.x) % self.p
}

void field_mul(field *ret, field *a, field *b) {
  assert(cmp(a->p, b->p) == 0);
  bint tmp;
  bmul(&tmp, b->x, a->x);
  bmod(ret->x, &tmp, b->p); // (self.x * data.x) % self.p
}

void field_div(field *ret, field *a, field *b) {
  field f, o;
  field_initint(&f, 0, b->p);
  field_initint(&o, 1, b->p);
  bdiv(f.x, o.x, a->x);
  field_mul(ret, b, &f);
}

void field_neg(field *ret, field *a, field *b) {
  bsub(ret->x, b->p, b->x); // self.p - self.x
}
//  def __truediv__(self, data): return self * data ** -1
//  def __pow__(self, data): return self.__class__(pow(self.x, data, self.p))


// ----- Order class -----
void order_init(order *o, bint *x, bint *p1) {
  field_initint(&o->f, 0, p1);
  memcpy(o->f.x, x, sizeof(bint));
  str2bint(o->N, "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"); // TODO: as arg?
}

void order_initint(order *o, uint32_t x, bint *p1) {
  bint x1[1];
  field_initint(&o->f, 0, p1);
  memcpy(o->f.x, wrd2bint(x1, x), sizeof(bint));
  str2bint(o->N, "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"); // TODO: as arg?
}


// ----- Prime class -----
void prime_init(prime *p, bint *x, bint *p1) {
  field_initint(&p->f, 0, p1);
  memcpy(p->f.x, x, sizeof(bint));
  memcpy(p->P, p1, sizeof(bint));
}

void prime_initint(prime *p, uint32_t x, bint *p1) {
  bint x1[1];
  field_initint(&p->f, 0, p1);
  memcpy(p->f.x, wrd2bint(x1, x), sizeof(bint));
  memcpy(p->P, p1, sizeof(bint));
}


// ----- Point class -----
void point_init(point *p, prime *a, prime *b, bint *p1) {
  bint y2[1], x2[1], x3[1], x37[1], x3m[1], B[1];
  prime p0a, p0b;
  prime_initint(&p0a, 0, p1);
  prime_initint(&p0b, 0, p1);
  p0a.f.x->siz = a->f.x->siz;
  p0b.f.x->siz = b->f.x->siz;
  printf("CMP: %d %d\n", cmp(a->f.x, p0a.f.x), cmp(b->f.x, p0b.f.x));
  bprint("CMP a", a->f.x);
  bprint("CMP 0", p0a.f.x);
  bprint("CMP 0", p0b.f.x);
  bprint("CMP b", b->f.x);
  if (cmp(a->f.x, p0a.f.x) == 0 || cmp(b->f.x, p0b.f.x) == 0) {
    prime_initint(&p->prime_x, 0, p1);
    prime_initint(&p->prime_y, 0, p1);
    memset(y2->wrd, 0, LEN * sizeof(uint32_t));
    memset(x2->wrd, 0, LEN * sizeof(uint32_t));
    memset(x3->wrd, 0, LEN * sizeof(uint32_t));
    memset(x37->wrd, 0, LEN * sizeof(uint32_t));
    memset(x3m->wrd, 0, LEN * sizeof(uint32_t));
    wrd2bint(B, 7);
    bmul(y2, b->f.x, b->f.x); // y*y
    bmul(x2, a->f.x, a->f.x); // x*x
    bmul(x3, x2, a->f.x); // x*x*x
    badd(x37, x3, B); // x*x*x + B
    bint xx[1], yy[1];
    bmod(xx, x37, p1);
    bmod(yy, y2, p1);
    bprint("XX", xx);
    bprint("YY", yy);
    assert(cmp(xx, yy) == 0); // y ** 2 == x ** 3 + (A * x) + B // A = 0, B = 7, (A * x) == 0
  }
  prime_init(&p->prime_x, a->f.x, p1);
  prime_init(&p->prime_y, b->f.x, p1);
}

void point_add(point *ret, point *a, point *b, bint *p1) {
  prime x1, x2, y1, y2, p0;
  point r;
  prime_init(&x1, b->prime_x.f.x, p1);
  prime_init(&x2, a->prime_x.f.x, p1);
  prime_init(&y1, b->prime_x.f.x, p1);
  prime_init(&y1, a->prime_x.f.x, p1);
  prime_initint(&p0, 0, p1);
  if (field_eq(&x1.f, &p0.f) == 0 && field_eq(&y1.f, &p0.f) == 0) {memcpy(ret, b, sizeof(point)); return;}
  if (field_eq(&x2.f, &p0.f) == 0 && field_eq(&y2.f, &p0.f) == 0) {memcpy(ret, a, sizeof(point)); return;}
  if (field_eq(&x1.f, &x2.f) == 0 && field_eq(&y1.f, &y2.f) == 0) { // y2 pos?
    prime x1x1, xa, yy1, x3, y3, sk;
    field_initint(&x1x1.f, 0, p1);
    field_initint(&xa, 0, p1);
    field_initint(&yy1, 0, p1);
    field_initint(&sk, 0, p1);
    field_initint(&x3, 0, p1);
    field_initint(&y3, 0, p1);
    field_mul(&x1x1.f, &x1.f, &x1.f);
    field_add(&xa.f, &x1x1.f, &x1x1.f);
    field_add(&xa.f, &xa.f, &x1x1.f);
    field_add(&yy1.f, &y1.f, &y1.f);

    field_div(&sk.f, &xa.f, &yy1.f);
    field_mul(&x3.f, &sk.f, &sk.f);
    field_sub(&x3.f, &x3.f, &x1.f);
    field_sub(&x3.f, &x3.f, &x2.f); // x3 = sk * sk - x1 - x2
    field_sub(&y3.f, &x1.f, &x3.f);
    field_mul(&y3.f, &sk.f, &x3.f);
    field_sub(&y3.f, &y3.f, &y1.f); // y3 = sk * (x1 - x3) - y1
    point_init(&r, &x3, &y3, p1);
    memcpy(ret, &r, sizeof(point));
    return; // return point(x3, y3)
  }
  if (field_eq(&x1.f, &x2.f) == 0 && field_eq(&y1.f, &y2.f) == 0) { // y2 negative?
    point_init(&r, &p0, &p0, p1);
    memcpy(ret, &r, sizeof(point));
    return; // return point(0, 0)
  }
  prime sk, x3, y3, x21, y21;
  field_initint(&sk, 0, p1);
  field_initint(&x3, 0, p1);
  field_initint(&y3, 0, p1);
  field_initint(&x21, 0, p1);
  field_initint(&y21, 0, p1);
  field_sub(&y21.f, &y2.f, &y1.f);
  field_sub(&x21.f, &x2.f, &x1.f);
  field_div(&sk.f, &y21.f, &x21.f); // sk = (y2 - y1) / (x2 - x1)
  field_mul(&x3.f, &sk.f, &sk.f);
  field_sub(&x3.f, &x3.f, &x1.f);
  field_sub(&x3.f, &x3.f, &x2.f); // x3 = sk * sk - x1 - x2
  field_sub(&y3.f, &x1.f, &x3.f);
  field_mul(&y3.f, &sk.f, &x3.f);
  field_sub(&y3.f, &y3.f, &y1.f); // y3 = sk * (x1 - x3) - y1
  point_init(&r, &x3, &y3, p1);
  memcpy(ret, &r, sizeof(point));
  return; // return point(x3, y3);
}

void point_mul(point *ret, point *a, point *k, bint *p1) { // TODO: does this work?!
  bint n[1], z[1], o[1], t[1], b[1];
  prime pz; // Point multiplication: Double-and-add https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication
  point result, addend, aa;
  wrd2bint(z, 0);
  wrd2bint(o, 1);
  wrd2bint(t, 2);
  prime_initint(&pz, 0, p1);
  point_init(&result, &pz, &pz, p1);
  point_init(&addend, &a->prime_x, &a->prime_y, p1);
  memset(n->wrd, 0, LEN * sizeof(uint32_t));
  memcpy(n->wrd, k->prime_x.f.x->wrd, k->prime_x.f.x->siz * sizeof(uint32_t));
  while (cmp(n, z) == 1 && n->siz > 0) {
    bmod(b, n, t);
    if (cmp(b, o) == 0) {
      point_add(&result, &result, &addend, p1);
    }
    point_init(&aa, &pz, &pz, p1);
    point_add(&aa, &addend, &addend, &p1); // TODO??
    brshift(n, n, 1);
  }
  memcpy(ret, &result, sizeof(point));
}

void Ftester(void) {
  prime px, py, pz, po1, po2, po3;
  field f0, f1, f2, f3;
  bint p1[1], p2[1], x[1], y[1];
  order o1, o2, o3;
  wrd2bint(p1, 23);
  wrd2bint(p2, 0);
  field_initint(&f0, 0, p1);
  field_initint(&f1, 12, p1);
  field_initint(&f2, 20, p1);
  field_initint(&f3, 9, p1);
  field_add(&f0, &f1, &f2);
  assert(field_eq(&f0, &f3) == 0); // Field(12) + Field(20) == Field(9)

  field_initint(&f0, 0, p1);
  field_initint(&f1, 8, p1);
  field_initint(&f2, 9, p1);
  field_initint(&f3, 3, p1);
  field_mul(&f0, &f1, &f2);
  assert(field_eq(&f0, &f3) == 0); // Field(8) * Field(9) == Field(3)

  order_initint(&o1, 42, p2);
  order_initint(&o2, 24, p2);
  order_initint(&o3, 66, p2);

  prime_init(&px, str2bint(x, "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"), p2); // point gx
  prime_init(&py, str2bint(y, "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"), p2); // point gy
  prime_initint(&pz, 0, p2);
  prime_init(&po1, o1.f.x, p2);
  prime_init(&po2, o2.f.x, p2);
  prime_init(&po3, o3.f.x, p2);

  point G1, G2, G3, G4, Gp;
  point_init(&G1, &px, &py, p2);
  point_init(&G2, &po1, &po1, p2); // G * Order(42)
  point_init(&G3, &po2, &po2, p2); // G * Order(24)
  point_init(&G4, &po3, &po3, p2); // G * Order(66)
  //point_init(&Gp, &pz, &pz, &p2);
  bprint("PX", px.f.x);
  bprint("PY", py.f.x);

  bprint("PO1", po1.f.x);
  bprint("PO2", po2.f.x);
  // TODO: Problem is that multipy Point to Order does not work

}

void Ftester_sanity(void) {
  bint aaa, bbb, ccc, r1, r2, ddd, hhh, jjj;
  wrd2bint(&aaa, 666);
  wrd2bint(&bbb, 999);
  wrd2bint(&r1, 333);
  bmod(&ccc, &bbb, &aaa);
  assert(cmp(&ccc, &r1) == 0);
  str2bint(&ddd, "0x6666666666666666666666666666666666666666666666666666666666666666");
  str2bint(&jjj, "0x9999999999999999999999999999999999999999999999999999999999999999");
  str2bint(&r2, "0x3333333333333333333333333333333333333333333333333333333333333333");
  bmod(&hhh, &jjj, &ddd);
  assert(cmp(&hhh, &r2) == 0);
}

void Ftester_math_sanity(void) {
  field f1, f2, f3, fr;
  bint a, b, p, r1, r2;
  str2bint(&a, "0x3b6859c358bb6fa30b3f11ff6c29164dc21b2abaf4c2027ea8e6701e99dd5b7c");
  str2bint(&b, "0xd7dab791a8647ac88695e20e29960a6fd41707258cc88cc0480ea7e5bb3f132f");
  str2bint(&p, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&r1, "0x13431155011fea6b91d4f40d95bf20bd963231e0818a8f3ef0f51805551c727c");
  str2bint(&r2, "0xa0cc141790812f1cc95dd3da756f3d4930f1adbeef8929265e0a74480f6eb392");
  field_initint(&fr, 0, &p);
  field_init(&f1, &a, &p);
  field_init(&f2, &b, &p);
  fr.x->siz = 8;
  field_add(&fr, &f1, &f2);
  assert(cmp((bint*)fr.x, &r1) == 0);
  field_mul(&f3, &f1, &f2);
  assert(cmp((bint*)f3.x, &r2) == 0);
}

